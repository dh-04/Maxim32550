/*
 * @file mml_usb.c MML USB Driver C File
 *
 ******************************************************************************
 * Copyright (C) 2016-2018 Maxim Integrated Products, Inc., All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include <MAX325xx.h>

#ifdef _MML_USB_H_

#define _USB_RESET_AT_INIT_

#define CHECK_EP(ep)  if ((ep) >= MML_USB_NUM_EP) { return -1;}
#define CHECK_NULL(x)  if ((x) == NULL) { return -1;}

typedef struct {
    volatile uint32_t buf0_desc;
    volatile uint32_t buf0_address;
    volatile uint32_t buf1_desc;
    volatile uint32_t buf1_address;
} ep_buffer_t;

typedef struct {
    ep_buffer_t out_buffer;
    ep_buffer_t in_buffer;
} ep0_buffer_t;

typedef struct {
    ep0_buffer_t ep0;
    ep_buffer_t ep[MML_USB_NUM_EP - 1];
} ep_buffer_descriptor_t;

/* static storage for endpoint buffer descriptor table, must be 512 byte aligned for DMA */
#ifdef __IAR_SYSTEMS_ICC__
#pragma data_alignment = 512
#else
__attribute__ ((aligned (512)))
#endif
ep_buffer_descriptor_t ep_buffer_descriptor;

/* storage for active endpoint data request objects */
static usb_req_t *usb_request[MML_USB_NUM_EP];

static uint8_t ep_size[MML_USB_NUM_EP];

/** DMA channel context info */
__attribute__((section(".bss")))                                         mml_usb_context_t mml_usb_context;

int mml_usb_init(usb_cfg_options_t *options)
{
    int i;

	(void) options;


    /* Check if first init is done */
    if (!(mml_usb_context.first_init))
    {

    	/* enable PLL1 */
    	mml_gcr_enable_pll1();

		/* Enable USB clock */
		GCR->PERCKCN &= ~(GCR_PERCKCN_USBD_Msk);
    }

#ifdef _USB_RESET_AT_INIT_
		/* Reset USB interface */
		GCR->RSTR |= GCR_RSTR_USB_Msk;
		/* Wait until USB reset completes */
		while( GCR_RSTR_USB_Msk & GCR->RSTR );
#endif /* _USB_RESET_AT_INIT_ */

    for (i = 0; i < MML_USB_NUM_EP; i++) {
        usb_request[i] = NULL;
        ep_size[i] = 0;
    }

    /* set the EP0 size */
    ep_size[0] = MML_USB_MAX_PACKET;

    /* reset the device */
    USB->CN = 0;
    USB->CN = USB_CN_USB_EN_Msk;
    USB->DEV_INT_EN = 0;
    USB->DEV_CN = 0;
    USB->DEV_CN = USB_DEV_CN_URST_Msk;
    USB->DEV_CN = 0;

    /* set the descriptor location */
    USB->EP_BASE = (uint32_t)&ep_buffer_descriptor;

    return 0;
}

int mml_usb_shutdown(void)
{
    USB->DEV_CN = USB_DEV_CN_URST_Msk;
    USB->DEV_CN = 0;
    USB->CN = 0;

    /* Disable PLL1 */
    mml_gcr_disable_pll1();

    return 0;
}

int mml_usb_connect(void)
{
    /* enable interrupts */
    USB->DEV_INT_EN |= (USB_DEV_INT_EN_SETUP_Msk | USB_DEV_INT_EN_EP_IN_Msk | USB_DEV_INT_EN_EP_OUT_Msk | USB_DEV_INT_EN_DMA_ERR_Msk);

    /* allow interrupts on EP0 */
    USB->EP0 |= USB_EP_EP_INT_EN_Msk;

    /* pull-up enable */
    USB->DEV_CN |= (USB_DEV_CN_CONNECT_Msk | USB_DEV_CN_FIFO_MODE_Msk);

    return 0;
}

int mml_usb_disconnect(void)
{
    USB->DEV_CN &= ~USB_DEV_CN_CONNECT_Msk;
    return 0;
}

int mml_usb_config_ep(unsigned int ep, USB_EP_EP0_EP_DIR_Type type, unsigned int size)
{
    uint32_t ep_ctrl;

    CHECK_EP(ep);

    if (size > MML_USB_MAX_PACKET) {
        return -1;
    }

    ep_ctrl = ( (type << USB_EP_EP_DIR_Pos) & USB_EP_EP_DIR_Msk);    /* input 'type' matches USB spec for direction */
    ep_ctrl |= USB_EP_EP_DT_Msk;

    if (type == USB_EP_EP0_EP_DIR_disable) {
        ep_ctrl &= ~USB_EP_EP_INT_EN_Msk;
    } else {
        ep_ctrl |= USB_EP_EP_INT_EN_Msk;
    }

    ep_size[ep] = size;

    *(unsigned int *)(MML_EP0_BASE_ADDRESS + ep*4) = ep_ctrl;
    return 0;
}

int mml_usb_is_configured(unsigned int ep)
{
	CHECK_EP(ep);

    return (((*(unsigned int *)(MML_EP0_BASE_ADDRESS + ep*4) & USB_EP_EP_DIR_Msk) >> USB_EP_EP_DIR_Pos) != USB_EP_EP0_EP_DIR_disable);
}

int mml_usb_stall(unsigned int ep)
{
    usb_req_t *req;

    CHECK_EP(ep);

    if (ep == 0) {
        *(unsigned int *)(MML_EP0_BASE_ADDRESS + ep*4) |= USB_EP_EP_ST_STALL_Msk;
    }

    *(unsigned int *)(MML_EP0_BASE_ADDRESS + ep*4) |= USB_EP_EP_STALL_Msk;

    /* clear pending requests */
    req = usb_request[ep];
    usb_request[ep] = NULL;

    if (req) {
        /* complete pending requests with error */
        req->error_code = -1;
        if (req->callback) {
            req->callback(req->cbdata);
        }
    }

    return 0;
}

int mml_usb_unstall(unsigned int ep)
{
	CHECK_EP(ep);

    /* clear the data toggle */
    *(unsigned int *)(MML_EP0_BASE_ADDRESS + ep*4) |= USB_EP_EP_DT_Msk;

    *(unsigned int *)(MML_EP0_BASE_ADDRESS + ep*4) &= ~USB_EP_EP_STALL_Msk;

    return 0;
}

int mml_usb_is_stalled(unsigned int ep)
{
	CHECK_EP(ep);

    return !!(*(unsigned int *)(MML_EP0_BASE_ADDRESS + ep*4) & USB_EP_EP_STALL_Msk);
}

int mml_usb_reset_ep(unsigned int ep)
{
    usb_req_t *req;

    CHECK_EP(ep);

    /* clear pending requests */
    req = usb_request[ep];
    usb_request[ep] = NULL;

    /* disable the EP */
    *(unsigned int *)(MML_EP0_BASE_ADDRESS + ep*4) &= ~USB_EP_EP_DIR_Msk;

    /* clear the data toggle */
    *(unsigned int *)(MML_EP0_BASE_ADDRESS + ep*4) |= USB_EP_EP_DT_Msk;

    if (req) {
        /* complete pending requests with error */
        req->error_code = -1;
        if (req->callback) {
            req->callback(req->cbdata);
        }
    }

    return 0;
}

int mml_usb_ackstat(unsigned int ep)
{
	CHECK_EP(ep);

    *(unsigned int *)(MML_EP0_BASE_ADDRESS + ep*4) |= USB_EP_EP_ST_ACK_Msk;
    return 0;
}

/* sent packet done handler*/
static void mml_event_in_data(uint32_t irqs)
{
    uint32_t epnum, buffer_bit, data_left;
    usb_req_t *req;
    ep_buffer_t *buf_desc;

    /* Loop for each data endpoint */
    for (epnum = 0; epnum < MML_USB_NUM_EP; epnum++) {

        buffer_bit = (1 << epnum);
        if ((irqs & buffer_bit) == 0) { /* Not set, next Endpoint */
            continue;
        }

        /* not sure how this could happen, safe anyway */
        if (!usb_request[epnum]) {
            continue;
        }

        req = usb_request[epnum];
        data_left = req->reqlen - req->actlen;

        if (epnum == 0) {
            buf_desc = &ep_buffer_descriptor.ep0.in_buffer;
        } else {
            buf_desc = &ep_buffer_descriptor.ep[epnum-1];
        }

        if (buf_desc->buf0_desc == 0) {
            /* free request first, the callback may re-issue a request */
            usb_request[epnum] = NULL;

            /* must have sent the ZLP, mark done */
            if (req->callback) {
                req->callback(req->cbdata);
            }
            continue;
        }

        if (data_left) {   /* more data to send */
            if (data_left >= ep_size[epnum]) {
                buf_desc->buf0_desc = ep_size[epnum];
            } else {
                buf_desc->buf0_desc = data_left;
            }

            req->actlen += buf_desc->buf0_desc;

            /* update the pointer */
            buf_desc->buf0_address += ep_size[epnum];

            /* start the DMA to send it */
            USB->IN_OWNER = buffer_bit;
        }
        else {
            /* all done sending data, either send ZLP or done here */
            if ((req->reqlen & (ep_size[epnum]-1)) == 0) {
                /* send ZLP per spec, last packet was full sized and nothing left to send */
                buf_desc->buf0_desc = 0;
                USB->IN_OWNER = buffer_bit;
            }
            else {
                /* free request */
                usb_request[epnum] = NULL;

                /* set done return value */
                if (req->callback) {
                    req->callback(req->cbdata);
                }
            }
        }
    }
}

/* received packet */
static void mml_event_out_data(uint32_t irqs)
{
    uint32_t epnum, buffer_bit, reqsize, rxsize;
    usb_req_t *req;
    ep_buffer_t *buf_desc;

    /* Loop for each data endpoint */
    for (epnum = 0; epnum < MML_USB_NUM_EP; epnum++) {

        buffer_bit = (1 << epnum);
        if ((irqs & buffer_bit) == 0) {
            continue;
        }

        /* this can happen if the callback was called then ZLP received */
        if (!usb_request[epnum]) {
            continue; /* ignored, because callback must have been called */
        }

        if (epnum == 0) {
            buf_desc = &ep_buffer_descriptor.ep0.out_buffer;
        } else {
            buf_desc = &ep_buffer_descriptor.ep[epnum-1];
        }

        req = usb_request[epnum];

        /* what was the last request size? */
        if ((req->reqlen - req->actlen) >= ep_size[epnum]) {
            reqsize = ep_size[epnum];
        } else {
            reqsize = (req->reqlen - req->actlen);
        }

        /* the actual size of data written to buffer will be the lesser of the packet size and the requested size */
        if (reqsize < buf_desc->buf0_desc) {
            rxsize = reqsize;
        } else {
            rxsize = buf_desc->buf0_desc;
        }

        req->actlen += rxsize;

        /* less than a full packet or zero length packet)  */
        if ((req->type == MAXUSB_TYPE_PKT) || (rxsize < ep_size[epnum]) || (rxsize == 0)) {
            /* free request */
            usb_request[epnum] = NULL;

            /* call it done */
            if (req->callback) {
                req->callback(req->cbdata);
            }
        }
        else {
            /* not done yet, push pointers, update descriptor */
            buf_desc->buf0_address += ep_size[epnum];

            /* don't overflow */
            if ((req->reqlen - req->actlen) >= ep_size[epnum]) {
                buf_desc->buf0_desc = ep_size[epnum];
            } else {
                buf_desc->buf0_desc = (req->reqlen - req->actlen);
            }

            /* transfer buffer back to controller */
            USB->OUT_OWNER = buffer_bit;
        }
    }
}

void mml_usb_irq_handler(maxusb_usbio_events_t *evt)
{
    uint32_t in_irqs, out_irqs, irq_flags;

    /* get and clear enabled irqs */
    irq_flags = USB->DEV_INT & USB->DEV_INT_EN;
    USB->DEV_INT = irq_flags;

    /* copy all the flags over to the "other" struct */
    evt->dpact  = ((irq_flags & USB_DEV_INT_DPACT_Msk) >> USB_DEV_INT_DPACT_Pos);
    evt->rwudn  = ((irq_flags & USB_DEV_INT_RWU_DN_Msk) >> USB_DEV_INT_RWU_DN_Pos);
    evt->bact   = ((irq_flags & USB_DEV_INT_BACT_Msk) >> USB_DEV_INT_BACT_Pos);
    evt->brst   = ((irq_flags & USB_DEV_INT_BRST_Msk) >> USB_DEV_INT_BRST_Pos);
    evt->susp   = ((irq_flags & USB_DEV_INT_SUSP_Msk) >> USB_DEV_INT_SUSP_Pos);
    evt->novbus = ((irq_flags & USB_DEV_INT_NO_VBUS_Msk) >> USB_DEV_INT_NO_VBUS_Pos);
    evt->vbus   = ((irq_flags & USB_DEV_INT_VBUS_Msk) >> USB_DEV_INT_VBUS_Pos);
    evt->brstdn = ((irq_flags & USB_DEV_INT_BRST_DN_Msk) >> USB_DEV_INT_BRST_DN_Pos);
    evt->sudav  = ((irq_flags & USB_DEV_INT_SETUP_Msk) >> USB_DEV_INT_SETUP_Pos);

    /* do cleanup in cases of bus reset */
    if (irq_flags & USB_DEV_INT_BRST_Msk) {
        int i;

        /* kill any pending requests */
        for (i = 0; i < MML_USB_NUM_EP; i++) {
            mml_usb_reset_ep(i);
        }
        /* no need to process events after reset */
        return;
    }

    if (irq_flags & USB_DEV_INT_EP_IN_Msk) {
        /* get and clear IN irqs */
        in_irqs = USB->IN_INT;
        USB->IN_INT = in_irqs;
        mml_event_in_data(in_irqs);
    }

    if (irq_flags & USB_DEV_INT_EP_OUT_Msk) {
        /* get and clear OUT irqs */
        out_irqs = USB->OUT_INT;
        USB->OUT_INT = out_irqs;
        mml_event_out_data(out_irqs);
    }
}

int mml_usb_irq_enable(maxusb_event_t event)
{
    uint32_t event_bit = 0;

    if (event >= MAXUSB_NUM_EVENTS) {
        return -1;
    }

    /* Note: the enum value is the same as the bit number */
    event_bit =  1 << event;
    USB->DEV_INT_EN |= event_bit;

    return 0;
}

int mml_usb_irq_disable(maxusb_event_t event)
{
    uint32_t event_bit = 0;

    if (event >= MAXUSB_NUM_EVENTS) {
        return -1;
    }

    /* Note: the enum value is the same as the bit number */
    event_bit =  1 << event;
    USB->DEV_INT_EN &= ~event_bit;

    return 0;
}

int mml_usb_irq_clear(maxusb_event_t event)
{
    uint32_t event_bit = 0;

    if (event >= MAXUSB_NUM_EVENTS) {
        return -1;
    }

    /* Note: the enum value is the same as the bit number */
    event_bit =  1 << event;
    USB->DEV_INT = event_bit;

    return 0;
}

int mml_usb_get_setup(usb_setup_pkt *sud)
{
	unsigned int *pSrc;
	unsigned int *pDst;

	CHECK_NULL(sud);

	pSrc = (unsigned int *)&USB->SETUP0;
	pDst = (unsigned int *)sud;

	/* setup packet is fixed at 8 bytes */
	pDst[0] = pSrc[0];
	pDst[1] = pSrc[1];

    return 0;
}

int mml_usb_write_endpoint(usb_req_t *req)
{
    unsigned int ep  = req->ep;
    uint8_t *data    = req->data;
    unsigned int len = req->reqlen;
    ep_buffer_t *buf_desc;
    uint32_t buffer_bit = (1 << ep);

    CHECK_EP(ep);
	CHECK_NULL(req);

    /* data buffer must be 32-bit aligned */
    if ((unsigned int)data & 0x3) {
        return -1;
    }

    /* EP must be enabled (configured) */
    if ( mml_usb_is_configured(ep) <= 0 ) {
        return -1;
    }

    /* if pending request; error */
    if (usb_request[ep] || (USB->IN_OWNER & buffer_bit)) {
        return -1;
    }

    /* assign req object */
    usb_request[ep] = req;

    /* clear errors */
    req->error_code = 0;

    if (ep == 0) {
        buf_desc = &ep_buffer_descriptor.ep0.in_buffer;
    } else {
        buf_desc = &ep_buffer_descriptor.ep[ep-1];
    }

    if (len > ep_size[ep]) {
        buf_desc->buf0_desc = ep_size[ep];
        usb_request[ep]->actlen = ep_size[ep];
    }
    else {
        buf_desc->buf0_desc = len;
        usb_request[ep]->actlen = len;
    }

    /* set pointer, force single buffered */
    buf_desc->buf0_address = (uint32_t)data;

    /* start the DMA */
    USB->IN_OWNER = buffer_bit;

    return 0;
}

int mml_usb_read_endpoint(usb_req_t *req)
{
    unsigned int ep  = req->ep;
    ep_buffer_t *buf_desc;
    uint32_t buffer_bit = (1 << ep);

    CHECK_NULL(req);
    CHECK_EP(ep);

    /* data buffer must be 32-bit aligned */
    if ((unsigned int)req->data & 0x3) {
        return -1;
    }

    /* EP must be enabled (configured) and not stalled */
    if (!mml_usb_is_configured(ep) || mml_usb_is_stalled(ep)) {
        return -1;
    }

    if (ep == 0) {
        buf_desc = &ep_buffer_descriptor.ep0.out_buffer;
    } else {
        buf_desc = &ep_buffer_descriptor.ep[ep-1];
    }

    /* if pending request; error */
    if (usb_request[ep] || (USB->OUT_OWNER & buffer_bit)) {
        return -1;
    }

    /* assign the req object */
    usb_request[ep] = req;

    /* clear errors */
    req->error_code = 0;

    /* reset length */
    req->actlen = 0;

    if (req->reqlen < ep_size[ep]) {
        buf_desc->buf0_desc = req->reqlen;
    } else {
        buf_desc->buf0_desc = ep_size[ep];
    }
    buf_desc->buf0_address = (uint32_t)req->data;

    USB->OUT_OWNER = buffer_bit;

    return 0;
}

#endif // _MML_USB_H_
