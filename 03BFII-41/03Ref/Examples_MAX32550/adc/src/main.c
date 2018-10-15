/*
 * @file adc/src/main.c
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
 * @defgroup EX_ADC ADC Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example measures ANO pin voltage value one time.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <MAX325xx.h>


#define EX_ADC_VERS_MAJOR	<VERSMAJ>
#define EX_ADC_VERS_MINOR	<VERSMIN>
#define EX_ADC_VERS_PATCH	<VERSPAT>
#define EX_ADC_VERSION_STRING	"v" xstr(EX_ADC_VERS_MAJOR) "." xstr(EX_ADC_VERS_MINOR) "." xstr(EX_ADC_VERS_PATCH)


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

void calculate_voltage(unsigned int data, float *voltage)
{
	/* Formula from AVDD = 3.3V / 2^10 = 0.003223 */
	*voltage = ((float) (data) * (float) 0.003223);
}

int main(void)
{
	unsigned int 	 result 		= COMMON_ERR_UNKNOWN;
	float 			 voltage		= 0;					/* Variable to save Voltage Value 					*/
	unsigned short	 data			= 0;					/* Variable to save data from ADC_DATA registers	*/
	unsigned int	 freq_system	= 0;					/* Variable to store system frequency 				*/
	unsigned int	 freq_adc		= 0;					/* Variable to store ADC frequency					*/
	mml_adc_config_t adc_config;							/* Structure to configure ADC						*/

	/* Uart initialization */
	uart_init();

	printf("\n********************************\n");
	printf("\nWelcome to ADC example\n\n");

	adc_config.alignment	= ADC_CN_ADALGN_rightAlign;
	adc_config.channel		= ADC_CN_ADCH_an0;
	adc_config.clock		= ADC_CN_ADCCLK_div16;
	adc_config.reference	= ADC_CN_ADREF_avdd;
	adc_config.scale		= ADC_CN_ADSCL_disable;

	/* Initialize ADC */
	result = mml_adc_init(adc_config);
	if (result) {
		printf("Error during ADC initialization\n");
		return result;
	}

	mml_get_system_frequency(&freq_system);
	freq_adc = freq_system / (2*16);
	printf("freq_sytem = %dHz\n", freq_system);
	printf("freq_adc = %dHz\n", freq_adc);
	printf("The fastest ADC rate = %dHz\n", freq_adc/1025);

	/* Enable ADC */
	result =  mml_adc_operation_config(ADC_CN_ADEN_enable);
	if (result) {
		printf("Error during ADC operation configuration\n");
		return result;
	}

	/* Start conversion */
	result = mml_adc_start_conversion();
	if (result) {
		printf("Error during start conversion\n");
		return result;
	}
	/* Wait until conversion ended */
	while ( (ADC->ST & ADC_ST_ADCONV_Msk) );
	/* Conversion ended */
	data = ADC->DATA;
	calculate_voltage(data, &voltage );
	printf("data = %d\n", data);
	printf("Voltage Value at AN0 Pin is %fV\n", voltage);

	printf("\nEnd of example\n");
	printf("\n********************************\n");

	return 0;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

