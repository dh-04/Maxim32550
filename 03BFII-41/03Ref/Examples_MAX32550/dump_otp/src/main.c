/*
 * @file dump_otp/src/main.c
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
 * @defgroup EX_Dump_OTP OTP Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example is used to dump OTP memory. The output will be send over UART.
 * 		  So please open relevant comport on PC side to see OTP memory content.
 *
 * If you build this example in SCPA you must use scpa_erase(unsigned int dest, unsigned int length) function to dump OTP memory.
 * @note In SLA (or RAM) build to dump MAXIM area define _OTP_DUMP_MAXIM_, by default the example dump USER are.
 * @note In SCPA build to dump MAXIM area of OTP length must be equal to 16.
 * @note In SCPA build to dump USER  area of OTP length must be equal to 32.
 * @note In SCPA build to dump entire are of OTP length must be equal to 48.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diag/Trace.h"
#include <MAX325xx.h>
#include <otp.h>


#define EX_Dump_OTP_VERS_MAJOR	<VERSMAJ>
#define EX_Dump_OTP_VERS_MINOR	<VERSMIN>
#define EX_Dump_OTP_VERS_PATCH	<VERSPAT>
#define EX_Dump_OTP_VERSION_STRING	"v" xstr(EX_Dump_OTP_VERS_MAJOR) "." xstr(EX_Dump_OTP_VERS_MINOR) "." xstr(EX_Dump_OTP_VERS_PATCH)


unsigned char work_buffer[K_OTP_DUMP_WORKBUFFER_SIZE_BYTE];

int init_uart(void);
void otp_dump_realign_data(unsigned char *p_src, unsigned int length);
int iflash_dump_display(unsigned int address, unsigned int length, mml_sflc_otp_id_t otp_id);
int iflash_dump_init(void) ;

int init_uart(void)
{
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


void otp_dump_realign_data(unsigned char *p_src, unsigned int length)
{
	register unsigned int						j;
	unsigned int								i;
	unsigned char								*p_recover = p_src;
	unsigned char								*p_tmp = p_src;

	/* Loop on full length OTP line number */
	for( i = 0;i < ( length / K_OTP_DUMP_OTP_LINE_SIZE );i++ ) {
		/* Shift data - CVs removed, bit lock removed */
		for( j = 0;j < ( K_OTP_DUMP_OTP_LINE_SIZE - K_OTP_DUMP_OTP_CV_LINE_SIZE );j++ ) {
			p_recover[j] = ( ( p_tmp[j + K_OTP_DUMP_OTP_CV_LINE_SIZE] << 1 ) & 0xfe ) | ( ( p_tmp[j + ( K_OTP_DUMP_OTP_CV_LINE_SIZE - 1 )] >> 7 ) & 0x01 );
		}
		/* Increment pointer */
		p_tmp += K_OTP_DUMP_OTP_LINE_SIZE;
		p_recover += ( K_OTP_DUMP_OTP_LINE_SIZE - K_OTP_DUMP_OTP_CV_LINE_SIZE );
	}
	return;
}

int iflash_dump_display(unsigned int address, unsigned int length, mml_sflc_otp_id_t otp_id) {
	unsigned int							last_size = length;
	unsigned int							i;
	volatile unsigned int					addr_src = (volatile unsigned int)address;
	volatile unsigned char					*p_dst = (volatile unsigned char*)work_buffer;

	while( last_size )	{
		unsigned int						size = M_COBRA_MIN(last_size,sizeof(work_buffer));

		/* Initialize work buffer */
		memset(work_buffer, 0x00, sizeof(work_buffer));
		/* Copy buffer to display into work buffer */
		mml_sflc_otp_read(otp_id, addr_src, (unsigned char*)p_dst, size);
		if (otp_id == N_MML_SFLC_OTP_ID_MAXIM) {
			/* Realign data */
			otp_dump_realign_data((unsigned char*)p_dst, size);
		}
		/* Display work buffer */
		for( i = 0;i < size;i++ ) {
			if ( !( i % 8 ) ) printf("\n0x%08x:", ( addr_src + i ));

			printf(" %02x", p_dst[i]);
		}
		/* Update lasting size */
		last_size -= size;
		/* Update work pointer */
		addr_src += size;
	}
	printf("\n");

	return NO_ERROR;
}

int iflash_dump_init(void) {
       int                                                                       result = COMMON_ERR_UNKNOWN;
 
    /* Initialize UART0 port with default configurations */
#ifndef __SCPA_FWK__
    result =  init_uart();
    if ( NO_ERROR == result )
        mml_sflc_init();
       return result;
#else
    result =  mml_uart_init_context_scpa(MML_UART_DEV0);
    if ( NO_ERROR == result )
        mml_sflc_init();
       return result;
#endif//__SCPA_FWK__
}

int main(void) {
    int                                         result = COMMON_ERR_UNKNOWN;

    /* Initialize */
    result =  iflash_dump_init();
    if ( NO_ERROR == result ) {
        printf("Welcome to OTP Dump Application !\n\n");
        result = iflash_dump_display(K_OTP_DUMP_BASE_ADDRESS, K_OTP_DUMP_MAX_SIZE_BYTE, N_MML_SFLC_OTP_ID_USER);
        printf("\n\tEnding execution with result 0x%08x ...\n",result);
    }
    return result;
}

#ifdef __SCPA_FWK__
int scpa_erase(unsigned int dest, unsigned int length);
int scpa_erase(unsigned int dest, unsigned int length)
{
	(void)dest;
	switch(length ){
	case 16:
		iflash_dump_init();
		printf("\n====  MAXIM AREA OTP DUMP ====");
		iflash_dump_display(MML_MEM_OTP_MXIM_BASE, MML_MEM_OTP_MXIM_SIZE,N_MML_SFLC_OTP_ID_MAXIM);
		printf("\n====  END ====\n");
		/* Character ASCII for end transmission */
		mml_uart_write_char(MML_UART_DEV0, 0x04);
		break;
	case 32:
		iflash_dump_init();
		printf("\n====  USER AREA OTP DUMP ====");
		iflash_dump_display(MML_MEM_OTP_USER_BASE , MML_MEM_OTP_USER_SIZE,N_MML_SFLC_OTP_ID_USER);
		printf("\n====  END ====\n");
		/* Character ASCII for end transmission */
		mml_uart_write_char(MML_UART_DEV0, 0x04);
		break;
	case 48:
		iflash_dump_init();
		printf("\n====  OTP DUMP ====");
		iflash_dump_display(MML_MEM_OTP_MXIM_BASE, MML_MEM_OTP_MXIM_SIZE,N_MML_SFLC_OTP_ID_MAXIM);
		iflash_dump_display(MML_MEM_OTP_USER_BASE , MML_MEM_OTP_USER_SIZE,N_MML_SFLC_OTP_ID_USER);
		printf("\n====  END ====\n");
		/* Character ASCII for end transmission */
		mml_uart_write_char(MML_UART_DEV0, 0x04);
		break;
	}
	return 0;
}
#endif //__SCPA_FWK__


#pragma GCC diagnostic pop
