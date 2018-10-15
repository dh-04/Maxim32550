/*
 * @file print_echo/src/main.c
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
 * @defgroup EX_PRINT_USN Print USN Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure UART to print USN in terminal via UART.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diag/Trace.h"
#include <MAX325xx.h>


#define EX_PRINT_USN_VERS_MAJOR	<VERSMAJ>
#define EX_PRINT_USN_VERS_MINOR	<VERSMIN>
#define EX_PRINT_USN_VERS_PATCH	<VERSPAT>
#define EX_PRINT_USN_VERSION_STRING	"v" xstr(EX_PRINT_USN_VERS_MAJOR) "." xstr(EX_PRINT_USN_VERS_MINOR) "." xstr(EX_PRINT_USN_VERS_PATCH)


int init_uart(void);
void compute_USN64bit(unsigned int USNR1,unsigned int USNR2);
void print_USN(void);

int init_uart(void) {
	int result = COMMON_ERR_UNKNOWN;
	mml_uart_config_t config;

	config.baudrate = 115200;
	config.data_bits = UARTn_CTRL_SIZE_bits8;
	config.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	config.parity = MML_UART_PARITY_NONE;
	config.parity_mode = MML_UART_PARITY_MODE_ONES;
	config.rts_ctl = UARTn_PIN_RTS_lo;
	config.stop_bits = UARTn_CTRL_STOP_stop1;
	result = mml_uart_init(MML_UART_DEV0, config);
	return result;
}

void compute_USN64bit(unsigned int USNR1,unsigned int USNR2) {

	unsigned int USN1 = ( USNR1 >> 15 );
	unsigned int USN2 = ( USNR2 & 0x7FFFFFFF );

	unsigned char US1 = USN1 & 0xFF ;
	unsigned char US2 = ((USN1 >> 8 ) & 0xFF );
	unsigned char US3 = ((USN2 << 0x1 ) & 0xFE ) + (( USN1 >> 16 ) & 0x1 );
	unsigned char US4 = ((USN2 >> 0x7 ) & 0xFF );
	unsigned char US5 = ((USN2 >> 15 ) & 0xFF );
	unsigned char US6 = ((USN2 >> 23 ) & 0xFF );

	printf("%02x %02x %02x %02x %02x %02x ", US1, US2, US3, US4, US5, US6);
	fflush(0);

}

void print_USN(void) {

	unsigned int otpdata[8];
	mml_sflc_otp_read(N_MML_SFLC_OTP_ID_MAXIM, MML_MEM_OTP_MXIM_BASE, (unsigned char*)otpdata, 32);
	printf("\nUSN : ");
	fflush(0);
	compute_USN64bit(otpdata[0],otpdata[1]);
	compute_USN64bit(otpdata[2],otpdata[3]);
	unsigned char US13 = ( otpdata[4] >> 15 ) & 0xFF ;
	printf("%02x\n", US13);
	fflush(0);
}

int main(void)
{
    int result = COMMON_ERR_UNKNOWN;

    /* Initialize UART0 port with default configurations */
    result =  init_uart();
    if ( NO_ERROR == result )
    {
        /* Welcome message */
        printf("Welcome to USN Display Application !\n\n");
        fflush(0);
        mml_sflc_init();
        print_USN();
    }
    return result;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

#pragma GCC diagnostic pop
