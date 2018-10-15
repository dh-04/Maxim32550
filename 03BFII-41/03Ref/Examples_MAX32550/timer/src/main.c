/*
 * timer/src/main.c Timer
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
 * @defgroup EX_TIMER Timer Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example shows you how to configure timer to get an interrupt.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

/* Global includes */
#include <MAX325xx.h>
#include <stdio.h>
#include <stdlib.h>
/* Local include */

#include "timer.h"


#define EX_TIMER_VERS_MAJOR	<VERSMAJ>
#define EX_TIMER_VERS_MINOR	<VERSMIN>
#define EX_TIMER_VERS_PATCH	<VERSPAT>
#define EX_TIMER_VERSION_STRING	"v" xstr(EX_TIMER_VERS_MAJOR) "." xstr(EX_TIMER_VERS_MINOR) "." xstr(EX_TIMER_VERS_PATCH)


/* Local defines */
volatile unsigned int usecase_loop;
volatile unsigned int usecase_sema;
cobra_frequencies_t frequencies;

void usecase_timeout_handler(void) {
	/* Increment loop counter */
	usecase_loop++;
	/* Release semaphore */
	usecase_sema = 0;
	/* Clear interruption */
	mml_tmr_interrupt_clear(MML_TMR_DEV0);
	/* Disable timer */
	mml_tmr_disable(MML_TMR_DEV0);
	return;
}

int main(void) {
	int											result = COMMON_ERR_UNKNOWN;
	mml_uart_config_t 							uart_conf;
	mml_tmr_config_t							timer_conf;

	/* Retrieve all frequencies has been set */
	result = mml_get_system_frequency(&frequencies.system);
	if ( result )
		return result;

	result = mml_get_ahb_frequency(&frequencies.ahb);
	if ( result )
		return result;

	result = mml_get_apb_frequency(&frequencies.apb);
	if ( result )
		return result;

	/* Initialize UATR0 port with default configurations */
	uart_conf.baudrate = 115200;
	uart_conf.data_bits = UARTn_CTRL_SIZE_bits8;
	uart_conf.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	uart_conf.parity = MML_UART_PARITY_NONE;
	uart_conf.parity_mode = MML_UART_PARITY_MODE_ONES;
	uart_conf.rts_ctl = UARTn_PIN_RTS_lo;
	uart_conf.stop_bits = UARTn_CTRL_STOP_stop1;
	result = mml_uart_init(MML_UART_DEV0, uart_conf);
	if ( result )
		return result;

	/* Welcome message */
	printf("Welcome to Usecase Timer Application !\n");
	/* Display frequencies */
	printf("System Frequency : %d Hz\n",frequencies.system);
	printf("AHB Frequency : %d Hz\n",frequencies.ahb);
	printf("APB Frequency : %d Hz\n\n",frequencies.apb);
	/* Initialize timer0 */
	timer_conf.count = 1;
	timer_conf.pwm_value = 0;
	timer_conf.clock = TMRn_CN_PRES_div4;
	timer_conf.mode = TMRn_CN_TMODE_oneShot;
	timer_conf.polarity = TMRn_CN_TPOL_activeLo;
	/* Base count in milliseconds */
	timer_conf.handler = usecase_timeout_handler;
	result = mml_tmr_init(MML_TMR_DEV0, &timer_conf);
	if ( result ) {
		printf("Error while initializing timer0 : error 0x%08x\n", result);
		printf("\nExiting the usecase now ...\n");
		return result;
	}
	else {
		usecase_loop = 0;
		usecase_sema = 1;
		printf("\t -> Timer0 initialized\n");
	}
	/* Once initialized, enable timer's interruption ... */
	result = mml_tmr_interrupt_enable(MML_TMR_DEV0);
	if ( result ) {
		printf("Error when enabling timer0 IRQ : error 0x%08x\n", result);
		printf("\nExiting the usecase now ...\n");
		return result;
	}
	else
		printf("\t\t -> Timer0 interruption enabled\n");

	/* Set timeout to be 5seconds */
	mml_tmr_set_timeout_us(MML_TMR_DEV0, 5000000);

	/* ... then enable timer itself. */
	result = mml_tmr_enable(MML_TMR_DEV0);
	if ( result ) {
		printf("Error for the very first timer0 enabling : error 0x%08x\n", result);
		printf("\nExiting the usecase now ...\n");
		return result;
	}
	else
		printf("\t\t\t -> Timer0 activated\n");

	/* Endless loop */
	while( 1 ) {
		/* Freeze semaphore */
		while( 1 == usecase_sema );
		/* Display message */
		printf("Timer hits %d time(s)\n", usecase_loop);
		/* Lock again semaphore */
		usecase_sema = 1;
		/* Relaunch timer */
		result = mml_tmr_enable(MML_TMR_DEV0);
		if ( result ) {
			printf("Error while enabling timer0 : error 0x%08x\n", result);
			printf("\nExiting the usecase now ...\n");
			return result;
		}
	}
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/******************************************************************************/
/* EOF */
