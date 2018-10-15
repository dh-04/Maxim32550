/*
 * @file iflash_dump/src/main.c
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
 * @defgroup EX_IFLASH_DUMP IFLASH_DUMP Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to write or erase Flash and dumps it by UART via a terminal.
 * @note Define _WITH_PROGRAM_PATTERN_ will write a pattern in internal flash.
 * @note Define _WITH_ERASE_ will erase page in internal flash.
 * @note Define _WITH_MASS_ERASE_ will mass erase internal flash.
 * @note If your code is store in FLASH MEMORY define _WITH_ERASE_ and _WITH_MASS_ERASE_ will have no effect.
 * 		 If you want to erase, mass erase or write a pattern you must build this example in RAM.
 * 		 To write a pattern you must first erase the pages you wanted to write.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

/* Global includes */
#include <stdio.h>
#include <string.h>
#include <MAX325xx.h>
#include <errors.h>
/* Local includes */
#include <iflash_dump.h>


#define EX_IFLASH_DUMP_VERS_MAJOR	<VERSMAJ>
#define EX_IFLASH_DUMP_VERS_MINOR	<VERSMIN>
#define EX_IFLASH_DUMP_VERS_PATCH	<VERSPAT>
#define EX_IFLASH_DUMP_VERSION_STRING	"v" xstr(EX_IFLASH_DUMP_VERS_MAJOR) "." xstr(EX_IFLASH_DUMP_VERS_MINOR) "." xstr(EX_IFLASH_DUMP_VERS_PATCH)


#ifdef _WITH_PROGRAM_PATTERN_
unsigned int pattern[K_MML_IFLASH_PATTERN_SIZE_INT];
#endif /* _WITH_PROGRAM_PATTERN_ */

#define _WITH_FORCED_EXIT_

/* UART configuration */
#define	K_LITE_MAX_SYST_FREQ_FOR_UART		192000000
#define	K_LITE_UART0_DEFAULT_BAUDRATE		115200
#define LITE_UART_BAUD_RATE_MIN				1200
#define LITE_UART_BAUD_RATE_MAX				921600

unsigned char work_buffer[K_IFLASH_DUMP_WORKBUFFER_SIZE_BYTE];

int iflash_dump_init(void) {
	/* Call SFLC initialization*/
	mml_sflc_init();
	/* Initialize UART0 port with default configurations */
	return config_uart();
}

int config_uart(void) {
	mml_uart_config_t uart_conf;
	uart_conf.baudrate = K_LITE_UART0_DEFAULT_BAUDRATE;
	uart_conf.data_bits = UARTn_CTRL_SIZE_bits8;
	uart_conf.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	uart_conf.parity = MML_UART_PARITY_NONE;
	uart_conf.parity_mode = MML_UART_PARITY_MODE_ONES;
	uart_conf.rts_ctl = UARTn_PIN_RTS_lo;
	uart_conf.stop_bits = UARTn_CTRL_STOP_stop1;
	return mml_uart_init(MML_UART_DEV0, uart_conf);
}

int iflash_dump_mass_erase(void) {
	return mml_sflc_mass_erase();
}

int iflash_dump_flash_erase(unsigned int offset, unsigned int length) {
	return mml_sflc_erase(( MML_MEM_FLASH_BASE + offset ), length);
}

int iflash_dump_flash_program(unsigned int offset, unsigned char *p_src, unsigned int length) {
	return mml_sflc_write(( MML_MEM_FLASH_BASE + offset ), p_src, length);
}

int iflash_dump_display(unsigned int address, unsigned int length) {
	unsigned int last_size = length;
	unsigned int i;
	volatile unsigned char *p_src = (volatile unsigned char*) ( MML_MEM_FLASH_BASE + address);

	/* Looping */
	while (last_size) {
		unsigned int size;

		if (last_size < sizeof(work_buffer)) {
			size = last_size;
		} else if (last_size >= sizeof(work_buffer)) {
			size = sizeof(work_buffer);
		}
		else
			return COMMON_ERR_INVAL;
		for (i = 0; i < size; i++) {
			if (!(i % 64)) {
				fflush(0);
				printf("\n0x%08x: ", (unsigned int) p_src + i);
				fflush(0);
			}
			printf("%02x", (unsigned int) (p_src[i]) );
			fflush(0);
		}
		/* Update lasting size */
		last_size -= size;
		/* Update work pointer */
		p_src += size;
	}
	printf("\n");
	return NO_ERROR;
}

int main(void) {
	int result = COMMON_ERR_UNKNOWN;

#ifdef __RAM_FWK__
#ifdef _WITH_PROGRAM_PATTERN_
	unsigned int i;
	unsigned int last_size;
	unsigned int dest;
#endif /* _WITH_PROGRAM_PATTERN_ */
#endif /* __RAM_FWK__ */

	/* Reset interfaces */
	result = iflash_dump_init();
	if (result) {
		return result;
	}
	/* Welcome message */
	printf("\nWelcome to Internal Flash Dump Application !\n");
	fflush(0);

#ifdef __RAM_FWK__
#ifdef _WITH_ERASE_
#ifdef _WITH_MASS_ERASE_
	result = iflash_dump_mass_erase();
#else
	result = iflash_dump_flash_erase(K_IFLASH_DUMP_BASE_OFFSET, K_IFLASH_DUMP_WORK_SIZE_BYTE);
#endif /* _WITH_MASS_ERASE_ */

	if ( result ) {
		printf("\n\t[iFlash Dump] Erasure failed - 0x%08x\n", result);
		return result;
	}
#endif /* _WITH_ERASE_*/
#endif /* __RAM_FWK__ */

/* The pattern is 0xdeadbeef. If you are in little endian  the pattern (K_IFLASH_DUMP_PATTERN_INT)
 * must be 0xefbeadde but if you are in big endian the pattern must be 0xdeadbeef
 * Note: if you're not sure of your endianess write 0xdeadbeef in flash. If the printf in terminal gives you 0xEFBEADDE
 *		 your endianess is little endian
 */

#ifdef __RAM_FWK__
#ifdef _WITH_PROGRAM_PATTERN_

	/* First erase flash before programming */
	result = iflash_dump_flash_erase(K_IFLASH_DUMP_BASE_OFFSET, K_IFLASH_DUMP_WORK_SIZE_BYTE);

	/* Fill pattern array */
	for(i = 0;i < K_MML_IFLASH_PATTERN_SIZE_INT;i++ ) {
		pattern[i] = K_IFLASH_DUMP_PATTERN_INT;
	}
	/* Initialize index */
	last_size = K_IFLASH_DUMP_WORK_SIZE_BYTE;
	dest = K_IFLASH_DUMP_BASE_OFFSET;
	/* Program pattern in chosen memory area */
	while( last_size ) {
		int size;
		if (last_size < sizeof(work_buffer)) {
			size = last_size;
		}
		else if(last_size >= sizeof(work_buffer)) {
			size = sizeof(work_buffer);
		}
		else
			return COMMON_ERR_INVAL;
		result = iflash_dump_flash_program(dest, (unsigned char*)pattern, size);
		if ( result ) {
			/* Programming issue */
			printf("\n\t[iFlash Dump] Programming failed - 0x%08x\n", result);
			return result;
		}
		/* Update lasting size */
		last_size -= size;
		dest += size;
	}
	/* Now display programmed pattern */
	result = iflash_dump_display(K_IFLASH_DUMP_BASE_OFFSET, K_IFLASH_DUMP_WORK_SIZE_BYTE);
	if ( result ) {
		/* Error while dumping internal flash */
		printf("\n\t[iFlash Dump] Dumping internal flash failed - 0x%08x\n", result);
	}
	else {
		printf("\n\t[iFlash Dump] Dumping done.\n");
	}
	printf("\n\t[iFlash Dump] ... Now erasing ...\n");
	result = iflash_dump_flash_erase(K_IFLASH_DUMP_BASE_OFFSET, K_IFLASH_DUMP_WORK_SIZE_BYTE);
	if (result)
		return result;


#endif /* _WITH_PROGRAM_PATTERN_ */
#endif /* __RAM_FWK__ */

	/* Now display programmed pattern */
	result = iflash_dump_display(K_IFLASH_DUMP_BASE_OFFSET, K_IFLASH_DUMP_WORK_SIZE_BYTE);
	if (result) {
		/* Error while dumping internal flash */
		printf("\n\t[iFlash Dump] Dumping internal flash failed - 0x%08x\n",
				result);
	} else {
		printf("\n\t[iFlash Dump] Dumping done.\n");
	}
	return result;
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/* EOF */
