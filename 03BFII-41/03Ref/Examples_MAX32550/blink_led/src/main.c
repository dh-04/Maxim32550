/*
 * @file blink_led/src/main.c
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
 * @defgroup EX_BLINK_LED Blink LED Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure GPIO in order to blink LED1.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */


#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include <MAX325xx.h>


#define EX_BLINK_LED_VERS_MAJOR	<VERSMAJ>
#define EX_BLINK_LED_VERS_MINOR	<VERSMIN>
#define EX_BLINK_LED_VERS_PATCH	<VERSPAT>
#define EX_BLINK_LED_VERSION_STRING	"v" xstr(EX_BLINK_LED_VERS_MAJOR) "." xstr(EX_BLINK_LED_VERS_MINOR) "." xstr(EX_BLINK_LED_VERS_PATCH)


#define LED1_GPIO_PORT	MML_GPIO_DEV0
#define LED1_GPIO_PIN	21
#define DELAY_TIME		1000

void __attribute__ ((noinline))  __attribute__((optimize("-O0"))) 
loop_delay(unsigned int ms)
{
     unsigned int i = ms*5000;
     unsigned int systemfreq = 0;
     mml_get_system_frequency(&systemfreq);

     i /= (MML_SYSTEM_FREQUENCY_108_MHZ/systemfreq);

     for (; i != 0 ; i--)
          __asm volatile ("nop\n");

}

int main(void)
{
	mml_gpio_config_t led_config;

	led_config.gpio_direction = MML_GPIO_DIR_OUT;
	led_config.gpio_function = MML_GPIO_NORMAL_FUNCTION;
	led_config.gpio_pad_config = MML_GPIO_PAD_NORMAL;
	mml_gpio_init(LED1_GPIO_PORT,LED1_GPIO_PIN,1,led_config);
	/* Infinite loop */
	while (1) {
		  mml_gpio_pin_output(LED1_GPIO_PORT,LED1_GPIO_PIN,MML_GPIO_OUT_LOGIC_ONE);
		  loop_delay(DELAY_TIME);
		  mml_gpio_pin_output(LED1_GPIO_PORT,LED1_GPIO_PIN,MML_GPIO_OUT_LOGIC_ZERO);
		  loop_delay(DELAY_TIME);
	}
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__


