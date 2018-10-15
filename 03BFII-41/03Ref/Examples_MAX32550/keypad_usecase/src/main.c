/*
 * @file keypad/src/main.c
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
 * @defgroup EX_KEYPAD Keypad Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure keypad and print key press in a termianl via UART
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

/* Global includes */
#include <MAX325xx.h>
#include <errors.h>
#include <stdio.h>
/* Local includes */
#include <keypad.h>


#define EX_KEYPAD_VERS_MAJOR	<VERSMAJ>
#define EX_KEYPAD_VERS_MINOR	<VERSMIN>
#define EX_KEYPAD_VERS_PATCH	<VERSPAT>
#define EX_KEYPAD_VERSION_STRING	"v" xstr(EX_KEYPAD_VERS_MAJOR) "." xstr(EX_KEYPAD_VERS_MINOR) "." xstr(EX_KEYPAD_VERS_PATCH)


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

int keypad_usecase(void) {
	int result = MML_SKBD_ERR_UNKNOWN;
	volatile unsigned int in;
	volatile unsigned int out;
	volatile unsigned int i;
	unsigned short *key;
	mml_skbd_config_t config;

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
		goto keypad_usecase_out;
	}
	result = mml_keypad_enable_interrupt_events(
			MML_SKBD_EVENT_PUSH | MML_SKBD_EVENT_OVERRUN);
	if (result) {
		/* Failed to enable the keypad interrupts */
		goto keypad_usecase_out;
	}
	printf("Press the keys on EvKit Keypad \n");
	while (1) {
		if (1 == key_pressed) {
			key = &keys.key0;
			for (i = 0; i < 4; i++) {
				in = 0x0f & *key;
				out = (0xf0 & *key) >> 4;
				if ( MML_SKBD_FLAG_KEY_PRESS & *key) {
					printf("Key pressed %c \n", keyboard_map[(in - 4) * 4 + out]);
				}
				*key = 0;
				key++;
			}
			key_pressed = 0;
		}
	}
	result = NO_ERROR;
	keypad_usecase_out: return result;
}

int main(void) {
	int result = COMMON_ERR_UNKNOWN;
	mml_uart_config_t uart_conf;
	cobra_frequencies_t freq;

	result = mml_get_system_frequency(&freq.system);
	if (result) {
		return result;
	}
	result = mml_get_ahb_frequency(&freq.ahb);
	if (result) {
		return result;
	}
	result = mml_get_apb_frequency(&freq.apb);
	if (result) {
		return result;
	}
	/* Initialize UATR0 port with default configurations */
	uart_conf.baudrate = 115200;
	uart_conf.data_bits = UARTn_CTRL_SIZE_bits8;
	uart_conf.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	uart_conf.parity = MML_UART_PARITY_NONE;
	uart_conf.parity_mode = MML_UART_PARITY_MODE_ONES;
	uart_conf.rts_ctl = UARTn_PIN_RTS_lo;
	uart_conf.stop_bits = UARTn_CTRL_STOP_stop1;
	result = mml_uart_init(MML_UART_DEV0, uart_conf);
	if (result) {
		return result;
	}
	/* Welcome message */
	printf("Welcome to keypad usecase application !\n");
	/* Display frequencies */
	printf("System Frequency : %d Hz\n", freq.system);
	printf("AHB Frequency : %d Hz\n", freq.ahb);
	printf("APB Frequency : %d Hz\n\n", freq.apb);
	/* Call your procedures here for instance ... */
	result = keypad_usecase();
	if (result) {
		printf("Keypad usecase : error 0x%08x\n", result);
		printf("\nExiting the usecase now ...\n");
		return result;
	} else {
		printf("\t -> Keypad usecase succeeds\n");
	}
	
	return result;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/******************************************************************************/
/* EOF */
