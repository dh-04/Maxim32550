/*
 * @file security_monitor/src/main.c
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
 * trademarks, mask works rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

/**
 * @defgroup EX_SECURITY_MONITOR Security Monitor Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure SECMON to get NMI when event occurs on an external sensor.
 *
 * Launch a first time the example to enabled NMI.
 * 		- if during the first run of the example you used jumper to create an event on one of the jumper configured an NMI DRS will
 * 		  appear in the terminal saying if the key wipe has been performed correctly. Reset the board. Then re-launch the example, the program
 * 		  will say you that the key was successfully erased.
 * 		- if  during the first run of the example you don't used jumper to create an event on one of the jumper configured, at the second
 * 		  launch of the program will say you that Key was not erased.
 *
 * @note To reset RTC correctly make sure to disconnect VBAT (with the appropriate JUMPER), to disconnect your board with the switch button ON/OFF and disconnect JTAG.
 * @note This example does not handle RAM and SCPA Target.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

/* Global includes */
#include <MAX325xx.h>
#include <errors.h>
#include <stdio.h>
/* Local includes */
#include <extscn.h>


#define EX_SECURITY_MONITOR_VERS_MAJOR	<VERSMAJ>
#define EX_SECURITY_MONITOR_VERS_MINOR	<VERSMIN>
#define EX_SECURITY_MONITOR_VERS_PATCH	<VERSPAT>
#define EX_SECURITY_MONITOR_VERSION_STRING	"v" xstr(EX_SECURITY_MONITOR_VERS_MAJOR) "." xstr(EX_SECURITY_MONITOR_VERS_MINOR) "." xstr(EX_SECURITY_MONITOR_VERS_PATCH)


extern int secure_application(void);

int main(void)
{
	int					result = COMMON_ERR_UNKNOWN;
	mml_uart_config_t 	uart_config;

	/* Initialize UATR0 port with default configurations */
	uart_config.baudrate = 115200;
	uart_config.data_bits = UARTn_CTRL_SIZE_bits8;
	uart_config.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	uart_config.parity = MML_UART_PARITY_NONE;
	uart_config.parity_mode = MML_UART_PARITY_MODE_ONES;
	uart_config.rts_ctl = UARTn_PIN_RTS_lo;
	uart_config.stop_bits = UARTn_CTRL_STOP_stop1;
	result = mml_uart_init(MML_UART_DEV0,  uart_config);
	if ( result ) {
		return result;
	}
	/* Welcome message */
	printf("\nWelcome to security monitor application !\n");
	/* Launch tests */
	result = secure_application();
	return result;
}

#ifdef __RAM_FWK__
#error "This example does not handle RAM Target"
#endif //__SCPA_FWK__

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/* EOF */
