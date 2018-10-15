/*
 * @file rtc/src/main.c
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
 * @defgroup EX_RTC RTC Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure RTC alarms, once alarm reached an interrupt will be triggered.
 * @note Defines SUB_SECOND_ALARM will configure RTC sub-second alarm instead of time-of-day alarm.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <MAX325xx.h>


#define EX_RTC_VERS_MAJOR	<VERSMAJ>
#define EX_RTC_VERS_MINOR	<VERSMIN>
#define EX_RTC_VERS_PATCH	<VERSPAT>
#define EX_RTC_VERSION_STRING	"v" xstr(EX_RTC_VERS_MAJOR) "." xstr(EX_RTC_VERS_MINOR) "." xstr(EX_RTC_VERS_PATCH)

unsigned int interrupt_status;
void RTC_IRQHandler ()
{
	printf("\nInterrupt reached after %lu seconds and %lu sub-seconds\n", RTC->SEC, RTC->SSEC);
	/* Check if we have reached time of day alarm interrupt*/
	if(RTC->CN & RTC_CN_ALDF_Msk) {
		printf("Time of day Alarm interrupt\n");
		/* Disable time of day alarm */
		mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_ADE_DISABLE);
	}

	/* Check if we have reached sub-second alarm interrupt */
	else if(RTC->CN & RTC_CN_ALSF_Msk) {
		printf("Sub-second alarm\n");
		/* Disable sub-second alarm */
		mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_ASE_DISABLE);
	}

	/* Clear RTC interrupt */
	NVIC_ClearPendingIRQ(RTC_IRQn);
	/* Disable RTC Interrupt */
	NVIC_DisableIRQ(RTC_IRQn);
}

int main()
{
	unsigned int result = COMMON_ERR_UNKNOWN;
#ifndef SUB_SECOND_ALARM
	unsigned int days 		 = 0;							/* Number of day to wait before alarm triggered     	*/
	unsigned int hours 		 = 0;							/* Number of hours to wait before alarm triggered   	*/
	unsigned int minutes 	 = 0;							/* Number of minutes to wait before alarm triggered 	*/
	unsigned int seconds 	 = 5;							/* Number of seconds to wait before alarm triggered 	*/
#else
	unsigned int sub_seconds = 2;							/* Number of sub-seconds to wait before alarm triggered */
	unsigned int seconds 	 = 5;							/* Number of seconds to wait before alarm triggered 	*/
#endif

	mml_uart_config_t			uart_conf;		/* Structure to initialize uart.                    	*/

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

	printf("\n********************************\n");
	printf("\nWelcome to RTC example\n\n");

	/* First Disable/Reset RTC */
	mml_rtc_disable();
	mml_rtc_reset();

#ifndef SUB_SECOND_ALARM
	printf("Time of day Alarm \n");
	/* Set time of day Alarm */
	result = mml_rtc_set_time_of_day_alrm(days, hours, minutes, seconds);
	if (result) {
		printf("Error in set time of day alarm\n");
		printf("Error code = %d\n", result);
		return result;
	}
	printf("Time of day alarm will occur after %ld seconds\n", RTC->RAS);

	/* Set time of day Alarm interrupt */
	result = mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_ADE_ENABLE);
	if (result) {
		printf("Error in interrupt configuration\n");
		printf("Error code = %d\n", result);
		return result;
	}

	/* Start-Up RTC */
	printf("Configuration of RTC ended, now start RTC\n");
	mml_rtc_start_up();

	/* Wait until we reached time of day alarm interrupt */
	while(!(RTC->CN & RTC_CN_ALDF_Msk));

	/* Clear ALDF flag */
	mml_rtc_clear_interrupt(MML_RTC_INTERRUPT_FLAG_ALDF);

	/* Disable RTC */
	mml_rtc_disable();

	printf("\nEnd of example\n");
	printf("\n********************************\n");
#else
	printf("Sub-seconds Alarm \n");
	/* Set sub-second Alarm */
	result = mml_rtc_set_sub_second_alarm(seconds, sub_seconds);
	if (result) {
		printf("Error in set sub-second alarm\n");
		printf("Error code = %d\n", result);
		return result;
	}

	/* Set sub-second Alarm interrupt */
	result = mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_ASE_ENABLE);
	if (result) {
		printf("Error in interrupt configuration\n");
		printf("Error code = %d\n", result);
		return result;
	}

	/* Start-Up RTC */
	printf("Configuration of RTC ended, now start RTC\n");
	mml_rtc_start_up();

	/* Wait until we reached sub-second alarm interrupt */
	while(!(RTC->CN & RTC_CN_ALSF_Msk));

	/* Clear ALDF flag */
	mml_rtc_clear_interrupt(MML_RTC_INTERRUPT_FLAG_ALSF);

	/* Disable RTC */
	mml_rtc_disable();

	printf("\nEnd of example\n");
	printf("\n********************************\n");
#endif /* TIME_DAY_ALARM */

	return NO_ERROR;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/******************************************************************************/
/* EOF */
