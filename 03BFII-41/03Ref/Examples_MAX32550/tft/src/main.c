/*
 * @file tft/src/main.c
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
 * @defgroup EX_TFT  Color LCD Example
 * @{
 *
 *
 * @ingroup MML_Examples
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "MAX325xx.h"
#include "framebuffer.h"
#include "NHD-3.5-320240MF.h"
#include "keypad.h"
#include "state.h"
#include "utils.h"
#include "touch_screen.h"


#define EX_TFT_VERS_MAJOR	<VERSMAJ>
#define EX_TFT_VERS_MINOR	<VERSMIN>
#define EX_TFT_VERS_PATCH	<VERSPAT>
#define EX_TFT_VERSION_STRING	"v" xstr(EX_TFT_VERS_MAJOR) "." xstr(EX_TFT_VERS_MINOR) "." xstr(EX_TFT_VERS_PATCH)


extern tft_timings_t panel;

static int config_uart(void)
{
	mml_uart_config_t uart_conf;
	uart_conf.baudrate = 115200;
	uart_conf.data_bits = UARTn_CTRL_SIZE_bits8;
	uart_conf.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	uart_conf.parity = MML_UART_PARITY_NONE;
	uart_conf.parity_mode = MML_UART_PARITY_MODE_ONES;
	uart_conf.rts_ctl = UARTn_PIN_RTS_lo;
	uart_conf.stop_bits = UARTn_CTRL_STOP_stop1;
	return mml_uart_init(MML_UART_DEV0, uart_conf);
}

static int system_init(void)
{
	config_uart();

	/** Initialize TFT port with default configurations and enable backlight */
    tft_extra_config();
    tft_config();
    skbd_init();
    set_display(8, panel.width, panel.height);
    //screen_splashboot();

    /** Enable TFT Display */
    tft_poweron_only();

    ts_init();
    ts_start();

    return 0;
}

int main(void)
{
	int key;
	unsigned int start_time;
	State *state;

	system_init();

	//
    state_init();

	/* Infinite loop */
    start_time = utils_get_time_as_ms();
	while (1) {

		state = state_get_current();

		// check keyboard key
		key = skbd_getkey();
		if (key > 0) {
			state->prcss_key(key);
			start_time = utils_get_time_as_ms();
		}
		// check touch screen key
		key = ts_get_key();
		if (key > 0) {
			state->prcss_key(key);
			start_time = utils_get_time_as_ms();
		}
		// check tick
		if (utils_get_time_as_ms() >= (start_time + state->timeout) ) {
			if (state->tick) {
				state->tick();
				start_time = utils_get_time_as_ms();
			}
		}
	}
}

#ifdef __RAM_FWK__
#error "This example does not handle RAM Target"
#endif //__RAM_FWK__

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__


