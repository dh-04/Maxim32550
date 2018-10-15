/*
 * @file usb_hid_kbd/src/main.c
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
 * @defgroup EX_USB USB_HID_KBD Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure a the USB device controller as a HID keyboard class device
 * @details The EvKit should enumerate as a HID Keyboard device.
 *          1. Open a text editor on the PC host and place cursor in edit box.
 *          2. Pressing button of secure keyboard will cause a message to be typed in on a virtual keyboard one character at a time.
 * @note The HID keyboard emulate a qwerty keyboard. If you are using another type of keyboard it should lead to some error. See details in kid_kbd.c file.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <descriptors.h>
#include <enumerate.h>
#include <usb_event.h>
#include <stdio.h>
#include <stddef.h>
#include "MAX325xx.h"
#include "mml_usb.h"
#include "hid_kbd.h"


#define EX_USB_VERS_MAJOR	<VERSMAJ>
#define EX_USB_VERS_MINOR	<VERSMIN>
#define EX_USB_VERS_PATCH	<VERSPAT>
#define EX_USB_VERSION_STRING	"v" xstr(EX_USB_VERS_MAJOR) "." xstr(EX_USB_VERS_MINOR) "." xstr(EX_USB_VERS_PATCH)


/***** Definitions *****/
#define EVENT_ENUM_COMP     MAXUSB_NUM_EVENTS
#define EVENT_REMOTE_WAKE   (EVENT_ENUM_COMP + 1)

#define WORKAROUND_TIMER    0           // Timer used for USB wakeup workaround
#define TMRn_IRQHandler     TMR0_IRQHandler
#define MXC_TMRn            MXC_TMR_GET_TMR(WORKAROUND_TIMER)
#define TMRn_IRQn           MXC_TMR_GET_IRQ_32(WORKAROUND_TIMER)

/***** Global Data *****/
volatile int configured;
volatile int suspended;
volatile unsigned int event_flags;
int remote_wake_en;

mml_skbd_keys_t keys = { 0, 0, 0, 0 };
volatile int key_pressed = 0;
/* keys mapping on the keyboard */
unsigned char keyboard_map[16] =
{
    'F', 'E', 'D', 'C',
    '3', '6', '9', 'B',
    '2', '5', '8', '0',
    '1', '4', '7', 'A'
};

/***** Function Prototypes *****/
static int setconfig_callback(usb_setup_pkt *sud, void *cbdata);
static int setfeature_callback(usb_setup_pkt *sud, void *cbdata);
static int clrfeature_callback(usb_setup_pkt *sud, void *cbdata);
static int event_callback(maxusb_event_t evt, void *data);
void usecase_keypad_handler(void);

int main(void)
{
	int result = MML_SKBD_ERR_UNKNOWN;
	mml_uart_config_t uart_config;
	mml_gpio_config_t vbus_det;
	mml_skbd_config_t config;
	volatile unsigned int in;
	volatile unsigned int out;
	volatile unsigned int i;
	unsigned short *key;

	/* Configure VBUS_DET P2.0 */
	vbus_det.gpio_direction = MML_GPIO_DIR_IN;
	vbus_det.gpio_function = MML_GPIO_SECONDARY_ALT_FUNCTION;
	vbus_det.gpio_pad_config = MML_GPIO_PAD_PULLUP;
	vbus_det.gpio_intr_mode = MML_GPIO_INT_MODE_EDGE_TRIGGERED;
	vbus_det.gpio_intr_polarity = MML_GPIO_INT_POL_FALLING;
	/* Initialize VBUS */
	mml_gpio_init(MML_GPIO_DEV2, 0, 1, vbus_det);

	uart_config.baudrate = 115200;
	uart_config.data_bits = UARTn_CTRL_SIZE_bits8;
	uart_config.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	uart_config.parity = MML_UART_PARITY_NONE;
	uart_config.parity_mode = MML_UART_PARITY_MODE_ONES;
	uart_config.rts_ctl = UARTn_PIN_RTS_lo;
	uart_config.stop_bits = UARTn_CTRL_STOP_stop1;
	mml_uart_init(MML_UART_DEV0, uart_config);

    printf("\n\n***** MAX325xx USB HID Keyboard Example *****\n");
    printf("Waiting for VBUS...\n");

    /* Initialize Secure Keypad */
	config.debounce = SKBD_CR1_DBTM_time10ms;
	config.inputs = (MML_SKBD_KBDIO4 | MML_SKBD_KBDIO5 | MML_SKBD_KBDIO6
			| MML_SKBD_KBDIO7);
	config.outputs = (MML_SKBD_KBDIO0 | MML_SKBD_KBDIO1 | MML_SKBD_KBDIO2
			| MML_SKBD_KBDIO3);
	config.reg_erase = 1;
	config.irq_handler = usecase_keypad_handler;

	result = mml_keypad_init(config);
	if (result) {
		/* Failed to initialize keypad */
		return result;
	}
	result = mml_keypad_enable_interrupt_events(
			MML_SKBD_EVENT_PUSH | MML_SKBD_EVENT_OVERRUN);
	if (result) {
		/* Failed to enable the keypad interrupts */
		return result;
	}
	printf("Press the keys on EvKit Keypad \n");

    /* Initialize state */
    configured = 0;
    suspended = 0;
    event_flags = 0;
    remote_wake_en = 0;

    /* Initialize the usb module */
    if (mml_usb_init(NULL) != 0) {
        printf("usb_init() failed\n");
        while (1);
    }

    /* Initialize the enumeration module */
    if (enum_init() != 0) {
        printf("mml_usb_enum_init() failed\n");
        while (1);
    }

    /* Register enumeration data */
    enum_register_descriptor(ENUM_DESC_DEVICE, (uint8_t*)&device_descriptor, 0);
    enum_register_descriptor(ENUM_DESC_CONFIG, (uint8_t*)&config_descriptor, 0);
    enum_register_descriptor(ENUM_DESC_STRING, lang_id_desc, 0);
    enum_register_descriptor(ENUM_DESC_STRING, mfg_id_desc, 1);
    enum_register_descriptor(ENUM_DESC_STRING, prod_id_desc, 2);
    enum_register_descriptor(ENUM_DESC_STRING, serial_id_desc, 3);

    /* Handle configuration */
    enum_register_callback(ENUM_SETCONFIG, setconfig_callback, NULL);

    /* Handle feature set/clear */
    enum_register_callback(ENUM_SETFEATURE, setfeature_callback, NULL);
    enum_register_callback(ENUM_CLRFEATURE, clrfeature_callback, NULL);

    /* Initialize the class driver */
    if (hidkbd_init(&config_descriptor.hid_descriptor, report_descriptor) != 0) {
        printf("hidkbd_init() failed\n");
        while (1);
    }

    /* Register callbacks */
    usb_event_enable(MAXUSB_EVENT_NOVBUS, event_callback, NULL);
    usb_event_enable(MAXUSB_EVENT_VBUS, event_callback, NULL);

    NVIC_EnableIRQ(USB_IRQn);

    /* Wait for events */
    while (1) {

		if (1 == key_pressed) {
			key = &keys.key0;
			for (i = 0; i < 4; i++) {
				in = 0x0f & *key;
				out = (0xf0 & *key) >> 4;
				if ( MML_SKBD_FLAG_KEY_PRESS & *key) {
					hidkbd_keypress(keyboard_map[(in - 4) * 4 + out]);
				}
				*key = 0;
				key++;
			}
			key_pressed = 0;
		}

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
            } else if (MXC_GETBIT(&event_flags, EVENT_ENUM_COMP)) {
                MXC_CLRBIT(&event_flags, EVENT_ENUM_COMP);
                printf("Enumeration complete.\n");
            }
        }
    }
}

static int setconfig_callback(usb_setup_pkt *sud, void *cbdata)
{
	(void) cbdata;

    /* Confirm the configuration value */
    if (sud->wValue == config_descriptor.config_descriptor.bConfigurationValue) {
        configured = 1;
        MXC_SETBIT(&event_flags, EVENT_ENUM_COMP);
        return hidkbd_configure(config_descriptor.endpoint_descriptor.bEndpointAddress & USB_EP_NUM_MASK);
    } else if (sud->wValue == 0) {
        configured = 0;
        return hidkbd_deconfigure();
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
            hidkbd_deconfigure();
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
            hidkbd_deconfigure();
            configured = 0;
            suspended = 0;
            break;
        default:
            break;
    }

    return 0;
}

void usecase_keypad_handler(void) {
	unsigned int status;

	/* Do what has to be done */
	mml_keypad_interrupt_status(&status);
	if (MML_SKBD_INTERRUPT_STATUS_OVERIS & status) {
		mml_keypad_clear_interrupt_status(MML_SKBD_INTERRUPT_STATUS_OVERIS);
	}
	if (MML_SKBD_INTERRUPT_STATUS_PUSHIS & status) {
		mml_keypad_read_keys(&keys);
		key_pressed = 1;
		/* Clear interruption */
		mml_keypad_clear_interrupt_status(MML_SKBD_INTERRUPT_STATUS_PUSHIS);
	}
	if (MML_SKBD_INTERRUPT_STATUS_RELEASEIS & status) {
		mml_keypad_clear_interrupt_status(MML_SKBD_INTERRUPT_STATUS_RELEASEIS);
	}
	NVIC_ClearPendingIRQ(Secure_Keypad_IRQn);
	return;
}

/******************************************************************************/
void USB_IRQHandler(void)
{
    usb_event_handler();
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__
