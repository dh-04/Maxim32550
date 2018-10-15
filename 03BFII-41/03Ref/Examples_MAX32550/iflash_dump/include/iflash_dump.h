/*
 * @file iflash_dump/include/iflash_dump.h
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

#define K_COBRA_ONE_KB							1024
#define	K_MML_IFLASH_PATTERN_SIZE_BYTE			K_COBRA_ONE_KB
#define	K_MML_IFLASH_PATTERN_SIZE_INT			( K_MML_IFLASH_PATTERN_SIZE_BYTE / sizeof(unsigned int) )

#define	K_IFLASH_DUMP_BASE_OFFSET				0x00000000
#define	K_IFLASH_DUMP_WORK_SIZE_BYTE			( 10 * K_COBRA_ONE_KB )
#define	K_IFLASH_DUMP_WORK_SIZE_INT				( K_IFLASH_DUMP_WORK_SIZE_BYTE / sizeof(unsigned int) )

#define	K_IFLASH_DUMP_WORKBUFFER_SIZE_BYTE		( 1 * K_COBRA_ONE_KB )
#define	K_IFLASH_DUMP_WORKBUFFER_SIZE_INT		( K_IFLASH_DUMP_WORKBUFFER_SIZE_BYTE / sizeof(unsigned int) )

#define	K_IFLASH_DUMP_PATTERN_INT				0xefbeadde /* Considering Little endian, if your endianess is big endian the pattern must be 0xdeadbeef */

int config_uart(void);

#ifdef _WITH_ERASE_
#ifdef _WITH_MASS_ERASE_

/**
 * @brief This function is used to mass erase internal flash.
 * @retval		NO_ERROR							No Error.
 * @retval      COMMON_ERR_UNKNOWN					Unknown Error.
 * @retval		N_MML_SFLC_ERR_LENGTH_NOT_ALIGNED	Data is not aligned according to memory specification.
 */
int iflash_dump_mass_erase(void);

#endif /* _WITH_MASS_ERASE_ */

/**
 * @brief This function is uses to erase Flash page.
 * @param[in] 	offset								Offset of the data in flash memory to start erase(0 means MML_MEM_FLASH_BASE).
 * @param[in] 	length								Number of data bits (must be a multiple of K_MML_SFLC_QUANTUM_SIZE_IN_BYTES).
 * @retval		NO_ERROR							No Error.
 * @retval		COMMON_ERR_UNKNOWN					Unknown Error.
 * @retval		N_MML_SFLC_ERR_OVERFLOW				Data length is out of memory capacity.
 * @retval		N_MML_SFLC_ERR_ADDRESS_NOT_ALIGNED	Address is not aligned according to memory specification.
 * @retval 		N_MML_SFLC_ERR_32BITS_PGM_FAILED	32 bits programming failed.
 * @retval 		N_MML_SFLC_ERR_128BITS_PGM_FAILED	128 bits programming failed.
 * @retval		N_MML_SFLC_ERR_LENGTH_NOT_ALIGNED	Data is not aligned according to memory specification.
 * @retval		N_MML_SFLC_ERR_BURST_PGM_FAILED		Burst programming failed.
 * @retval		N_MML_SFLC_ERR_PAGE_ERASE_FAILED	Page erase failed.
 */
int iflash_dump_flash_erase(unsigned int offset, unsigned int length);
#endif /* _WITH_ERASE_ */
#ifdef _WITH_PROGRAM_PATTERN_

/**
 * @brief This function is used to write data
 * @param[in] 	address								Offset of the data in flash memory to start dump and program(0 means MML_MEM_FLASH_BASE).
 * @param[in] 	*p_src								Pointer on data source.
 * @param[in] 	length								Number of bits for data (must be a multiple of K_MML_SFLC_QUANTUM_SIZE_IN_BYTES).
 * @retval		NO_ERROR							No error.
 * @retval		COMMON_ERR_UNKNOWN					Unknown Error.
 * @retval		COMMON_ERR_NULL_PTR					Pointer is null.
 * @retval		N_MML_SFLC_ERR_ADDRESS_NOT_ALIGNED	Address is not aligned according to memory specification.
 * @retval		N_MML_SFLC_ERR_PACKET_NOT_ALIGNED	Data packet is not aligned according to memory specification.
 * @retval		N_MML_SFLC_ERR_OVERFLOW				Data length is out of memory capacity.
 * @retval		COMMON_ERR_FATAL_ERROR				Critical error.
 */
int iflash_dump_flash_program(unsigned int offset, unsigned char *p_src, unsigned int length);

#endif /* _WITH_PROGRAM_PATTERN_ */

/**
 * @brief This function is used to initialize flash controller and UART.
 * @retval NO_ERROR							No error.
 * @retval COMMON_ERR_UNKNOWN				Unknown error.
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number.
 * @retval COMMON_ERR_NULL_PTR				NULL pointer parameter passed.
 * @retval COMMON_ERR_BAD_STATE				UART port is not initialized.
 * @retval COMMON_ERR_INVAL					Invalid configuration parameter.
 * @retval COMMON_ERR_RUNNING				UART is currently busy with transmitting or receiving the data.
 * @retval MML_UART_ERR_IRQ_SET				Interrupt can not be initialized.
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier.
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid parameter.
 */
int iflash_dump_init(void);

/**
 * @brief This function is used to dump on display OTP data.
 * @param[in] address						Relative start address (0 means MML_MEM_FLASH_BASE).
 * @param[in] length						length to dump.
 * @retval NO_ERROR							No error.
 */
int iflash_dump_display(unsigned int address, unsigned int length);
