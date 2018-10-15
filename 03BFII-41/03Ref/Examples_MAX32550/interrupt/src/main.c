/*
 * @file interrupt/src/main.c
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
 * @defgroup EX_INTERRUPT INTERRUPT Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure interrupt on button1 and button2 of GPIO2.
 *
 * When a button is pushed it changes state of associated LED. Button1 is associated to LED1 and button2 to LED2.
 * @note This example doesn't take count of the debouncing of the two buttons.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <MAX325xx.h>


#define EX_INTERRUPT_VERS_MAJOR	<VERSMAJ>
#define EX_INTERRUPT_VERS_MINOR	<VERSMIN>
#define EX_INTERRUPT_VERS_PATCH	<VERSPAT>
#define EX_INTERRUPT_VERSION_STRING	"v" xstr(EX_INTERRUPT_VERS_MAJOR) "." xstr(EX_INTERRUPT_VERS_MINOR) "." xstr(EX_INTERRUPT_VERS_PATCH)


/* LED1 */
#define LED1_GPIO_PORT	MML_GPIO_DEV0
#define LED1_GPIO_PIN	21
/* LED2 */
#define LED2_GPIO_PORT	MML_GPIO_DEV0
#define LED2_GPIO_PIN	22
/* Button 1 */
#define BUT1_GPIO_PORT	MML_GPIO_DEV0
#define BUT1_GPIO_PIN	10
/* Button 2 */
#define BUT2_GPIO_PORT	MML_GPIO_DEV0
#define BUT2_GPIO_PIN	11

unsigned int data;

void handler_but_1(void *params) {

	(void) params;
	unsigned int port_value;

	mml_gpio_read_bit_pattern(LED1_GPIO_PORT, LED1_GPIO_PIN, 1, &data);
	//Check if LED is ON
	if (data == 0) {
		/* LED is ON we need to turn OFF */
		mml_gpio_pin_output(LED1_GPIO_PORT, LED1_GPIO_PIN, MML_GPIO_OUT_LOGIC_ONE);
	} else {
		/* LED is OFF we need to turn ON */
		mml_gpio_pin_output(LED1_GPIO_PORT, LED1_GPIO_PIN, MML_GPIO_OUT_LOGIC_ZERO);
	}

	/* Wait until button1 is released */
	do {
		port_value = 0xffffffff;
		mml_gpio_read_datain(BUT1_GPIO_PORT, &port_value);
	} while (port_value & (1 << BUT1_GPIO_PIN));
}

void handler_but_2(void *params) {

	(void) params;
	unsigned int port_value;

	mml_gpio_read_bit_pattern(LED2_GPIO_PORT, LED2_GPIO_PIN, 1, &data);
	/* Check if LED is ON */
	if (data == 0) {
		/* LED is ON we need to turn OFF */
		mml_gpio_pin_output(LED2_GPIO_PORT, LED2_GPIO_PIN, MML_GPIO_OUT_LOGIC_ONE);
	} else {
		/* LED is OFF we need to turn ON */
		mml_gpio_pin_output(LED2_GPIO_PORT, LED2_GPIO_PIN, MML_GPIO_OUT_LOGIC_ZERO);
	}

	/* Wait until button2 is released */
	do {
		port_value = 0xffffffff;
		mml_gpio_read_datain(BUT2_GPIO_PORT, &port_value);
	} while (port_value & (1 << BUT2_GPIO_PIN));
}

int main(void) {
	int result;
	mml_gpio_config_t led_config;
	mml_gpio_config_t button_config;
	result = mml_gpio_reset_interface();
	if (result)
		return result;

	/* Configure LED GPIO */
	led_config.gpio_direction = MML_GPIO_DIR_OUT;
	led_config.gpio_function = MML_GPIO_NORMAL_FUNCTION;
	led_config.gpio_pad_config = MML_GPIO_PAD_NORMAL;
	/* Initialize LEDs */
	result = mml_gpio_init(LED1_GPIO_PORT, LED1_GPIO_PIN, 1, led_config);
	if (result)
		return result;

	result = mml_gpio_init(LED2_GPIO_PORT, LED2_GPIO_PIN, 1, led_config);
	if (result)
		return result;

	/* Configure Button GPIO */
	button_config.gpio_direction = MML_GPIO_DIR_IN;
	button_config.gpio_function = MML_GPIO_NORMAL_FUNCTION;
	button_config.gpio_pad_config = MML_GPIO_PAD_PULLUP;
	button_config.gpio_intr_mode = MML_GPIO_INT_MODE_EDGE_TRIGGERED;
	button_config.gpio_intr_polarity = MML_GPIO_INT_POL_FALLING;
	/* Initialize BUTTONs */
	result = mml_gpio_init(BUT1_GPIO_PORT, BUT1_GPIO_PIN, 1, button_config);
	if (result)
		return result;
	result = mml_gpio_init(BUT2_GPIO_PORT, BUT2_GPIO_PIN, 1, button_config);
	if (result)
		return result;

	/* Enable Interrupt on Buttons and attach specific handler */
	/* BUTTON 1 */
	result = mml_gpio_enable_interrupt(BUT1_GPIO_PORT, BUT1_GPIO_PIN, 1);
	if(result)
		return result;
	result = mml_gpio_attach_handler(BUT1_GPIO_PORT, BUT1_GPIO_PIN, handler_but_1, 0);
	if(result)
		return result;
	/* BUTTON 2 */
	result = mml_gpio_enable_interrupt(BUT2_GPIO_PORT, BUT2_GPIO_PIN, 1);
	if(result)
		return result;
	result = mml_gpio_attach_handler(BUT2_GPIO_PORT, BUT2_GPIO_PIN, handler_but_2, 0);
	if(result)
		return result;

	while(1);
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

#pragma GCC diagnostic pop

