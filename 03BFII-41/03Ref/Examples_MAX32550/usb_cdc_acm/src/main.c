/*
 * @file usb_cdc_acm/src/main.c
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

 /**
 * @defgroup EX_USB USB_CDC_ACM Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief   USB CDC-ACM example
 * @details Configures the device as an USB-UART adaptor.
 * 			For a Windows PC the driver is the .inf file in the example folder.
 * 			This example configures UART0 and USP device to create an USB-UART adaptor. Open two terminal one
 * 			connected to the COMi associated to UART0 and the second connected to the USB device " Maxim USB-to-UART Adapter(COMj).
 * 			When you will write data on the terminal connected to the COMi the data wrote will be printed on the terminal connected
 * 			to the COMj and vice versa.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <cdc_acm.h>
#include <enumerate.h>
#include <stdio.h>
#include <stddef.h>
#include <usb_event.h>
#include "MAX325xx.h"
#include "mml_usb.h"
#include "descriptors.h"


#define EX_USB_VERS_MAJOR	<VERSMAJ>
#define EX_USB_VERS_MINOR	<VERSMIN>
#define EX_USB_VERS_PATCH	<VERSPAT>
#define EX_USB_VERSION_STRING	"v" xstr(EX_USB_VERS_MAJOR) "." xstr(EX_USB_VERS_MINOR) "." xstr(EX_USB_VERS_PATCH)


/* Definitions */
#define MXC_UART_FIFO_DEPTH	8
#define EVENT_REMOTE_WAKE   (USB_NUM_EVENTS + 1)

#define WORKAROUND_TIMER    0           // Timer used for USB wakeup workaround

/* Global Data */
volatile int configured;
volatile unsigned int event_flags;
int remote_wake_en;

/* Function Prototypes */
static int setconfig_callback(usb_setup_pkt *sud, void *cbdata);
static int setfeature_callback(usb_setup_pkt *sud, void *cbdata);
static int clrfeature_callback(usb_setup_pkt *sud, void *cbdata);
static int event_callback(maxusb_event_t evt, void *data);
static int configure_uart(void);
static int usb_read_callback(void);
static void read_from_usb(void);
void uart_handler(void);

/* File Scope Variables *****/

/* This EP assignment must match the Configuration Descriptor */
static const acm_cfg_t acm_cfg = {
  1,                  /* EP OUT */
  MML_USB_MAX_PACKET, /* OUT max packet size */
  2,                  /* EP IN */
  MML_USB_MAX_PACKET, /* IN max packet size */
  3,                  /* EP Notify */
  MML_USB_MAX_PACKET, /* Notify max packet size */
};

static uint8_t uart_rx_data[MXC_UART_FIFO_DEPTH + 1];
static uint8_t uart_tx_data[MXC_UART_FIFO_DEPTH];
static volatile int usb_read_complete;
static volatile int uart_read_complete;

int main(void)
{
	mml_uart_config_t config;
	mml_gpio_config_t vbus_det;

	/* Configure VBUS_DET P2.0 */
	vbus_det.gpio_direction = MML_GPIO_DIR_IN;
	vbus_det.gpio_function = MML_GPIO_SECONDARY_ALT_FUNCTION;
	vbus_det.gpio_pad_config = MML_GPIO_PAD_PULLUP;
	vbus_det.gpio_intr_mode = MML_GPIO_INT_MODE_EDGE_TRIGGERED;
	vbus_det.gpio_intr_polarity = MML_GPIO_INT_POL_FALLING;
	/* Initialize VBUS */
	mml_gpio_init(MML_GPIO_DEV2, 0, 1, vbus_det);

	config.baudrate = 9600;
	config.data_bits = UARTn_CTRL_SIZE_bits8;
	config.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	config.parity = MML_UART_PARITY_NONE;
	config.parity_mode = MML_UART_PARITY_MODE_ONES;
	config.rts_ctl = UARTn_PIN_RTS_lo;
	config.stop_bits = UARTn_CTRL_STOP_stop1;
	config.handler = uart_handler;
	mml_uart_init(MML_UART_DEV0, config);

    printf("\n\n***** USB CDC-ACM Example *****\n");
    printf("Waiting for VBUS...\n");

    /* Initialize state */
    configured = 0;
    event_flags = 0;
    remote_wake_en = 0;

    /* Initialize the usb module */
    if (mml_usb_init(NULL) != 0) {
        printf("mml_usb_init() failed\n");
        while (1);
    }

    /* Initialize the enumeration module */
    if (enum_init() != 0) {
        printf("enum_init() failed\n");
        while (1);
    }

    /* Register enumeration data */
    enum_register_descriptor(ENUM_DESC_DEVICE, (uint8_t*)&device_descriptor, 0);
    enum_register_descriptor(ENUM_DESC_CONFIG, (uint8_t*)&config_descriptor, 0);
    enum_register_descriptor(ENUM_DESC_STRING, lang_id_desc, 0);
    enum_register_descriptor(ENUM_DESC_STRING, mfg_id_desc, 1);
    enum_register_descriptor(ENUM_DESC_STRING, prod_id_desc, 2);

    /* Handle configuration */
    enum_register_callback(ENUM_SETCONFIG, setconfig_callback, NULL);

    /* Handle feature set/clear */
    enum_register_callback(ENUM_SETFEATURE, setfeature_callback, NULL);
    enum_register_callback(ENUM_CLRFEATURE, clrfeature_callback, NULL);

    /* Initialize the class driver */
    if (acm_init() != 0) {
        printf("acm_init() failed\n");
        while (1);
    }

    /* Register callbacks */
    usb_event_enable(MAXUSB_EVENT_NOVBUS, event_callback, NULL);
    usb_event_enable(MAXUSB_EVENT_VBUS, event_callback, NULL);
    acm_register_callback(ACM_CB_SET_LINE_CODING, configure_uart);
    acm_register_callback(ACM_CB_READ_READY, usb_read_callback);
    usb_read_complete = 0;

    if (configure_uart() != 0) {
        printf("configure_uart() failed\n");
        while (1);
    }

    /* Start with USB in low power mode */
    NVIC_SetPriority(USB_IRQn, MML_INTC_PRIO_2);
    NVIC_EnableIRQ(USB_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);

    /* Wait for events */
    while (1) {
        read_from_usb();

        if (event_flags) {
            /* Display events */
            if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_NOVBUS)) {
                MXC_CLRBIT(&event_flags, MAXUSB_EVENT_NOVBUS);
                printf("VBUS Disconnect\n");
            } else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_VBUS)) {
                MXC_CLRBIT(&event_flags, MAXUSB_EVENT_VBUS);
                printf("VBUS Connect\n");
            } else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_BRST)) {
                MXC_CLRBIT(&event_flags, MAXUSB_EVENT_BRST);
                printf("Bus Reset\n");
            } else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_SUSP)) {
                MXC_CLRBIT(&event_flags, MAXUSB_EVENT_SUSP);
                printf("Suspended\n");
            } else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_DPACT)) {
                MXC_CLRBIT(&event_flags, MAXUSB_EVENT_DPACT);
                printf("Resume\n");
            } else if (MXC_GETBIT(&event_flags, MAXUSB_NUM_EVENTS)) {
                MXC_CLRBIT(&event_flags, MAXUSB_NUM_EVENTS);
                printf("Enumeration complete. Waiting for characters...\n");
            }
        }
    }
}

static void read_from_usb(void)
{
	int i=0;
	int chars;

  if ((chars = acm_canread()) > 0) {

    if (chars > (int)(sizeof(uart_tx_data))) {
      chars = sizeof(uart_tx_data);
    }

    if (acm_read(uart_tx_data, chars) != chars) {
      printf("acm_read() failed\n");
    } else {
    	while(chars) {
    		mml_uart_write_char(MML_UART_DEV0, uart_tx_data[i]);
    		if(uart_tx_data[i] == '\r') {
    			mml_uart_write_char(MML_UART_DEV0, '\n');
    		}
    		chars --;
    		i++;
    	}
    }
  }
}

static int configure_uart(void)
{
    int err;
    const acm_line_t *params;
    mml_uart_config_t uart_cfg;

    /* get the current line coding parameters */
    params = acm_line_coding();

    /* some settings are fixed for this implementation */
    if ((params->stopbits != ACM_STOP_1) || (params->databits != 8)) {
        return -1;
    }

    uart_cfg.flwctrl = UARTn_CTRL_RTSCTSF_disable;
    uart_cfg.rts_ctl = UARTn_PIN_RTS_lo;
    uart_cfg.baudrate = params->speed;
    uart_cfg.parity_mode = MML_UART_PARITY_MODE_ONES;

    if (params->parity == ACM_PARITY_NONE) {
        uart_cfg.parity = MML_UART_PARITY_NONE;
    } else if (params->parity == ACM_PARITY_ODD) {
        uart_cfg.parity = MML_UART_PARITY_ODD;
    } else if (params->parity == ACM_PARITY_EVEN) {
        uart_cfg.parity = MML_UART_PARITY_EVEN;
    } else {
        return -1;
    }

    if (params->databits == 5) {
        uart_cfg.data_bits = UARTn_CTRL_SIZE_bits5;
    } else if (params->databits == 6) {
        uart_cfg.data_bits = UARTn_CTRL_SIZE_bits6;
    } else if (params->databits == 7) {
        uart_cfg.data_bits = UARTn_CTRL_SIZE_bits7;
    } else if (params->databits == 8) {
        uart_cfg.data_bits = UARTn_CTRL_SIZE_bits8;
    } else {
        return -1;
    }

    uart_cfg.stop_bits = (params->stopbits == ACM_STOP_1) ? 0 : 1;
    uart_cfg.handler = (mml_uart_handler_t)uart_handler;

    if ((err = mml_uart_init(MML_UART_DEV0, uart_cfg)) != 0) {
        return err;
    }

    UART0->CTRL |= (UARTn_CTRL_RXTHD_bytes1 << UARTn_CTRL_RXTHD_Pos) & UARTn_CTRL_RXTHD_Msk;

    mml_uart_interrupt_set(MML_UART_DEV0, UARTn_INT_EN_FFRXIE_Msk);

    /* Read one bytes from UART1 */
    err = mml_uart_read_char(MML_UART_DEV0, (unsigned char*)(&uart_rx_data));
    uart_read_complete = 1;

    return 0;
}

static int setconfig_callback(usb_setup_pkt *sud, void *cbdata)
{
	(void) cbdata;

    /* Confirm the configuration value */
    if (sud->wValue == config_descriptor.config_descriptor.bConfigurationValue) {
        configured = 1;
        printf("Enumeration complete. Waiting for characters...\n");
        return acm_configure(&acm_cfg); /* Configure the device class */
    } else if (sud->wValue == 0) {
        configured = 0;
        return acm_deconfigure();
    }

    return -1;
}

static int setfeature_callback(usb_setup_pkt *sud, void *cbdata)
{
	(void) cbdata;

    if(sud->wValue == FEAT_REMOTE_WAKE) {
        remote_wake_en = 1;
    } else {
        // Unknown callback
        return -1;
    }

    return 0;
}

static int clrfeature_callback(usb_setup_pkt *sud, void *cbdata)
{
	(void) cbdata;

    if(sud->wValue == FEAT_REMOTE_WAKE) {
        remote_wake_en = 0;
    } else {
        // Unknown callback
        return -1;
    }

    return 0;
}

static int event_callback(maxusb_event_t evt, void *data)
{
	(void) data;

    /* Set event flag */
    MXC_SETBIT(&event_flags, evt);

    switch (evt) {
        case MAXUSB_EVENT_NOVBUS:
            usb_event_disable(MAXUSB_EVENT_BRST);
            usb_event_disable(MAXUSB_EVENT_SUSP);
            usb_event_disable(MAXUSB_EVENT_DPACT);
            mml_usb_disconnect();
            configured = 0;
            enum_clearconfig();
            acm_deconfigure();
            break;
        case MAXUSB_EVENT_VBUS:
            usb_event_clear(MAXUSB_EVENT_BRST);
            usb_event_enable(MAXUSB_EVENT_BRST, event_callback, NULL);
            usb_event_clear(MAXUSB_EVENT_SUSP);
            usb_event_enable(MAXUSB_EVENT_SUSP, event_callback, NULL);
            mml_usb_connect();
            break;
        case MAXUSB_EVENT_BRST:
            enum_clearconfig();
            acm_deconfigure();
            configured = 0;
            break;
        default:
            break;
    }

    return 0;
}

static int usb_read_callback(void)
{
    usb_read_complete = 1;
    return 0;
}

void USB_IRQHandler(void)
{
    usb_event_handler();
}

void uart_handler(void) {
	int available = UART0->CTRL & UARTn_CTRL_RXTHD_Msk;
	int i=0;
	int err;
	unsigned int isr;
	isr = UART0->INT_STAT;

	if(isr & UARTn_INT_STAT_FFRXIS_Msk) {
    	while(available)
    	{
    		err = mml_uart_read_char(MML_UART_DEV0, &uart_rx_data[1+i]);
    		if (uart_rx_data[1+i] =='\r') {
    			uart_rx_data[1+i] = '\n';
    			i++;
    			uart_rx_data[1+i] = '\r';
    		}
    		if(err) {
    	        printf("uart_read() failed\n");
    	        return;
    		}
    		available--;
    		i++;
    	}
    	/* Write data read from uart via USB */
        if (acm_present()) {
        	i++;
          if (acm_write(uart_rx_data, i) != i) {
            printf("acm_write() failed\n");
          }
        }

        uart_read_complete = 0;

        /* Read one bytes from UART1 */
        err = mml_uart_read_char(MML_UART_DEV0, (unsigned char*)(&uart_rx_data));
        uart_read_complete = 1;
        /* CLear status bytes of interrupt */
        mml_uart_interrupt_clear(MML_UART_DEV0, UARTn_INT_STAT_FFRXIS_Msk);
	}
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__
