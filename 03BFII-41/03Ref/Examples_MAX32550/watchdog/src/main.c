/*
 * @file main.c
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
 * @file main.c
 * @brief Watchdog example
 *
 * This example show you how to configure WDT.
 * NOTE: - RTC is used to give indication on time passed before reaching interrupt.
 * 		 - Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <MAX325xx.h>

#define EX_WDT_VERS_MAJOR	<VERSMAJ>
#define EX_WDT_VERS_MINOR	<VERSMIN>
#define EX_WDT_VERS_PATCH	<VERSPAT>
#define EX_WDT_VERSION_STRING	"v" xstr(EX_WDT_VERS_MAJOR) "." xstr(EX_WDT_VERS_MINOR) "." xstr(EX_WDT_VERS_PATCH)
 
void WDG_IRQHandler() {
	printf("\nYou have reached Watchdog interrupt  after %f seconds.\n", (float) (RTC->SEC + (float) (0.0039063 * RTC->SSEC ) ) );
	/* Disable RTC */
	mml_rtc_disable();
	/* Disable interrupt */
	mml_wdt_disable_int();
	/* Disable Watchdog interrupt */
	NVIC_DisableIRQ(Watchdog_Timer_IRQn);

}

void rtc_start(void) {
	/* First Disable/Reset RTC */
	mml_rtc_disable();
	mml_rtc_reset();
	/* Then start rtc */
	mml_rtc_start_up();
}

int calculate_period(WDT_CTRL_INT_PERIOD_Type int_period, float *interrupt_time) {
	float		 	pclk_period;
	unsigned int	system_freq;			/* Variable to stored system_frequency											  */
	unsigned int 	puissance;				/* Variable to calculate power of 2 set in INT_PERIOD bits of WDT_CTRL register . */
	unsigned int 	number_clock_cycles;	/* Number of clock cycles		 												  */

	puissance = ~int_period & 0x1F;

	/* Calculate number of clock cycles defines by wdt_config.int_period */
	number_clock_cycles = 1;
	for (unsigned int i = 0 ; i < puissance ; i++) {
		number_clock_cycles = 2*number_clock_cycles;
	}

	mml_get_system_frequency(&system_freq);
	/* PCLK_freq = ( system_freq / 2 ) */
	pclk_period = ( 1 / ( (float) (system_freq / 2) ) );
	*interrupt_time = pclk_period * number_clock_cycles;

	return NO_ERROR;
}

void uart_init(void) {
	mml_uart_config_t uart_conf;								/* Structure to initialize uart. */

	/* Uart configuration and initialization */
	uart_conf.baudrate		  = 115200;
	uart_conf.data_bits		  = UARTn_CTRL_SIZE_bits8;
	uart_conf.flwctrl		  = UARTn_CTRL_RTSCTSF_disable;
	uart_conf.parity		  = MML_UART_PARITY_NONE;
	uart_conf.parity_mode	  = MML_UART_PARITY_MODE_ONES;
	uart_conf.rts_ctl		  = UARTn_PIN_RTS_lo;
	uart_conf.stop_bits		  = UARTn_CTRL_STOP_stop1;
	/* Initialize UART */
	mml_uart_init(MML_UART_DEV0, uart_conf);
}

int main()
{
	unsigned int 				result = COMMON_ERR_UNKNOWN;
	float						interrupt_time;					/* Time to wait before interrupt */
	mml_wdt_config				wdt_conf;						/* Structure to initialize wdt.  */

	/* Uart initialization */
	uart_init();

	printf("\n********************************\n");
	printf("\nWelcome to WDT example\n\n");

	/* Configure watchdog in order to have interrupt after 2^27 clock cycles */
	wdt_conf.int_en 	= WDT_CTRL_INT_EN_enable;
	wdt_conf.int_period = WDT_CTRL_INT_PERIOD_wdt2pow27;
	/* Watchdog reset disable */
	wdt_conf.rst_ien 	= WDT_CTRL_RST_EN_disable;
	wdt_conf.rst_period = WDT_CTRL_RST_PERIOD_wdt2pow31;

	/* Calculate time before interrupt */
	result = calculate_period(wdt_conf.int_period, &interrupt_time);
	if (result) {
		printf("Error during calculation of time before interrupt.\n");
		return result;
	}
	printf("You will wait %f seconds approximatively before reached Watchdog interrupt.\n", interrupt_time);

	/* Check if reset is configured */
	if (wdt_conf.rst_ien) {
		/* Calculate time before reset */
		result = calculate_period(wdt_conf.rst_period, &interrupt_time);
		if (result) {
			printf("Error during calculation of time before reset.\n");
			return result;
		}
		printf("You will wait %f seconds approximatively before reset of Watchdog counter.\n", interrupt_time);
	}

	result = mml_wdt_set_configuration(wdt_conf);
	if (result)
	{
		printf("Error during configuration.\n");
		printf("Error code = %d\n", result);
		return result;
	}
	printf("Configuration of Watchdog ended with no error.\n");

	/* Enable Watchdog timer */
	mml_wdt_enable();
	printf("Watchdog timer enabled.\n");

	/* Start RTC in order to measure time passed before reaching interrupt */
	rtc_start();

	/* Wait until we reached interrupt */
	while (!(WDT->CTRL & WDT_CTRL_INT_FLAG_Msk));

	/* Disable Watchdog */
	mml_wdt_disable();

	/* Clear interrupt flag */
	mml_wdt_clear_flag(MML_WDT_INTERRUPT_FLAG_INT_FLAG);

	printf("\nEnd of example\n");
	printf("\n********************************\n");

	return NO_ERROR;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/******************************************************************************/
/* EOF */