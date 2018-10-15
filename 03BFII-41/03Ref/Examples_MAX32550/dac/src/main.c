/*
 * @file dac/src/main.c
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
 * @defgroup EX_DAC DAC Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to use DAC.
 * @note Define A440 will send a sinusoidal signal of frequency 440Hz.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <MAX325xx.h>

#define EX_DAC_VERS_MAJOR	<VERSMAJ>
#define EX_DAC_VERS_MINOR	<VERSMIN>
#define EX_DAC_VERS_PATCH	<VERSPAT>
#define EX_DAC_VERSION_STRING	"v" xstr(EX_DAC_VERS_MAJOR) "." xstr(EX_DAC_VERS_MINOR) "." xstr(EX_DAC_VERS_PATCH)

void uart_init(void)
{
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
	unsigned int 				result 			  = COMMON_ERR_UNKNOWN;
	unsigned int				nbr_sampl_convert = 8;									/* Number of sample to convert.*/
	mml_dac_config				dac_config;

	/* Setup configuration for DAC */
	/* First configure control register */
	dac_config.dac_ctrl_config.dacfaecnt 		  = 0xFFFF;								/* DAC FIFO Almost Empty level.						*/
	dac_config.dac_ctrl_config.interpolation_mode = DAC_CTRL_DACINTERPMODE_disabled;	/* DAC interpolation mode.							*/
	dac_config.dac_ctrl_config.dacffcnt 		  = 0xFFFF;								/* DAC FIFO Almost Full level.						*/
	dac_config.dac_ctrl_config.dacstmode		  = 0;									/* DAC Output Start Mode. 							*/
	dac_config.dac_ctrl_config.dacmode			  = DAC_CTRL_DACMODE_perDacRateCnt;		/* DAC Operation Mode
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	   Output DacSmplCnt[15:0] Data points from FIFO.
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	   as Ts defined by DacRateCnt[15:0]. 				*/
	dac_config.dac_ctrl_config.dacrst			  = 0;									/* DAC Timing Generator Reset. 						*/
	/* Then configure rate register*/
#ifndef A440
	dac_config.dac_rate_config.dacratecnt		  = 0xFFFF;								/* Configuration to have 2.73ms of sampling rate.	*/
#else
	dac_config.dac_rate_config.dacratecnt		  = 6818;								/* Configuration to have 284us of sampling rate:
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	   284us * 8 = 2.27ms and freq = 1/2.27ms = 440Hz.	*/
#endif
	dac_config.dac_rate_config.dacsamplecnt		  = 1;

	/* Uart initialization */
	uart_init();

	printf("\n********************************\n");
	printf("\nWelcome to DAC example\n\n");

#ifndef A440
	unsigned char				sample[8];												/* Number of Sample */

	/* Set sample to convert */

	for (unsigned int i = 0 ; i < nbr_sampl_convert ; i++ ) {
		sample[i] = (unsigned char) (i * (255/nbr_sampl_convert));
		printf("sample[%d] = %d\n", i , sample[i]);
	}

	/* Initialization of DAC */
	mml_dac_init();
	printf("Now DAC is ready for conversion\n");

	result = mml_dac_config_f(dac_config);
	if (result) {
		printf(" Error during configuration of DAC.\n");
		return result;
	}

	/* Infinite loop */
	while(1) {
		for (unsigned int i=0 ; i < nbr_sampl_convert ; i++)
		{
			DAC_FIFO->REG =  sample[i];
		}
	}
#else
	volatile unsigned char test[8];

	/* Initialization of DAC */
	mml_dac_init();
	printf("Now DAC is ready for conversion\n");

	result = mml_dac_config_f(dac_config);
	if (result) {
		printf(" Error during configuration of DAC.\n");
		return result;
	}

	/* Set samples for a LA 440 */
	test[0] = 0x80;
	test[1] = 0xe5;
	test[2] = 0xfb;
	test[3] = 0xb2;
	test[4] = 0x42;
	test[5] = 0x01;
	test[6] = 0x22;
	test[7] = 0x78;

	/* Infinite loop */
	while(1) {
		for (unsigned int i=0 ; i < nbr_sampl_convert ; i++)
		{
			DAC_FIFO->REG = test[i];
		}
	}
#endif
	printf("\nEnd of example\n");
	printf("\n********************************\n");

	return NO_ERROR;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

