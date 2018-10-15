/*
 * @file keypad.c
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

/* Global includes */
#include <MAX325xx.h>
#include <errors.h>
#include <stdio.h>
/* Local includes */
#include "keypad.h"


static volatile int is_pressed = 0;

/* keys mapping on the keyboard */
static unsigned char keyboard_map[16] =
{
    KEY_F, KEY_E, KEY_D, KEY_C,
	KEY_3, KEY_6, KEY_9, KEY_B,
	KEY_2, KEY_5, KEY_8, KEY_0,
	KEY_1, KEY_4, KEY_7, KEY_A
};

static void usecase_keypad_handler(void)
{
	unsigned int status;

	/* Do what has to be done */
	mml_keypad_interrupt_status(&status);
	if (MML_SKBD_INTERRUPT_STATUS_OVERIS & status) {
		mml_keypad_clear_interrupt_status(MML_SKBD_INTERRUPT_STATUS_OVERIS);
	}
	if (MML_SKBD_INTERRUPT_STATUS_PUSHIS & status) {
		is_pressed = 1;
		/* Clear interruption */
		mml_keypad_clear_interrupt_status(MML_SKBD_INTERRUPT_STATUS_PUSHIS);
	}
	if (MML_SKBD_INTERRUPT_STATUS_RELEASEIS & status) {
		mml_keypad_clear_interrupt_status(MML_SKBD_INTERRUPT_STATUS_RELEASEIS);
	}
	return;
}

int skbd_init(void)
{
	int rv = MML_SKBD_ERR_UNKNOWN;
	mml_skbd_config_t config;

	config.debounce = SKBD_CR1_DBTM_time10ms;
	config.inputs = (MML_SKBD_KBDIO4 | MML_SKBD_KBDIO5 | MML_SKBD_KBDIO6 | MML_SKBD_KBDIO7);
	config.outputs = (MML_SKBD_KBDIO0 | MML_SKBD_KBDIO1 | MML_SKBD_KBDIO2 | MML_SKBD_KBDIO3);
	config.reg_erase = 1;
	config.irq_handler = usecase_keypad_handler;
	rv = mml_keypad_init(config);
	if (rv) {
		/* Failed to initialize keypad */
		return -1;
	}

	rv = mml_keypad_enable_interrupt_events( MML_SKBD_EVENT_PUSH | MML_SKBD_EVENT_OVERRUN);
	if (rv) {
		/* Failed to enable the keypad interrupts */
		return -1;
	}
	return NO_ERROR;
}

int skbd_getkey(void)
{
	volatile unsigned int in;
	volatile unsigned int out;
	volatile unsigned int i;
	unsigned short *key;
	mml_skbd_keys_t keys = { 0, 0, 0, 0 };
	int pressed_key = 0;

	if (1 == is_pressed) {
		mml_keypad_read_keys(&keys);
		key = &keys.key0;
		for (i = 0; i < 4; i++) {
			in = 0x0f & *key;
			out = (0xf0 & *key) >> 4;
			if ( MML_SKBD_FLAG_KEY_PRESS & *key) {
				pressed_key = keyboard_map[(in - 4) * 4 + out];

				printf("Key pressed %c \n", keyboard_map[(in - 4) * 4 + out]);
			}
			*key = 0;
			key++;
		}
		is_pressed = 0;
	}

	return pressed_key;
}
