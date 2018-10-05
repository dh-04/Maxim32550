
/*
 * @file mml_sflc.h MML SFLC Driver Header File
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

#ifndef _MML_SFLC_H_
#define _MML_SFLC_H_

/* Global includes */
#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_SFLC SFLC Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_SFLC_VERS_MAJOR	4
#define MML_SFLC_VERS_MINOR	0
#define MML_SFLC_VERS_PATCH	4
#define MML_SFLC_VERSION_STRING	"v" xstr(MML_SFLC_VERS_MAJOR) "." xstr(MML_SFLC_VERS_MINOR) "." xstr(MML_SFLC_VERS_PATCH)

/* Defines ********************************************************************/
#define	K_MML_SFLC_BASE_ERROR					( COBRA_OTP << COBRA_ERR_PREFIX_OFFSET )

/** Flash characteristics *
 */
#define	MML_MEM_FLASH_BASE						0x10000000
#define	MML_MEM_FLASH_SIZE						2*512*1024

/* Mask */
#define	FC_FLSH_ALL_CMDS_Msk				( FC_FLSH_CN_WR_Msk | \
													FC_FLSH_CN_ME_Msk | \
													FC_FLSH_CN_PGE_Msk )
#define	MML_SFLC_CN_WIDTH_MASK_NOOFST			0x1

/* Internal Flash */
#define	K_MML_SFLC_QUANTUM_SIZE_IN_BYTES		sizeof(unsigned int)
#define	K_MML_SFLC_BURST_SIZE_IN_BYTES			( 4 * K_MML_SFLC_QUANTUM_SIZE_IN_BYTES )

#define	K_MML_SFLC_PGM_32BITS_OFST				2
#define	K_MML_SFLC_PGM_128BITS_OFST				4

#define	K_MML_SFLC_PGM_32BITS_LENGHT			4
#define	K_MML_SFLC_PGM_128BITS_LENGHT			16

#define	K_MML_SFLC_ERASE_PAGE_SIZE				( 4 * 1024 )

#define	K_MML_SFLC_PGM_32BITS_SPECIAL_MASK		0xfffffffc
#define	K_MML_SFLC_PGM_128BITS_SPECIAL_MASK		0xfffffff0
#define K_MML_SFLC_QUANTUM_SPECIAL_MASK			( 0xffffffff ^ ( K_MML_SFLC_QUANTUM_SIZE_IN_BYTES - 1 ) )
#define	K_MML_SFLC_ERASE_PAGE_SPECIAL_MASK		( 0xffffffff ^ ( K_MML_SFLC_ERASE_PAGE_SIZE - 1 ) )

/* OTP */
/* Size of OTP line in bytes */
#define	K_MML_SFLC_OTP_LINE_SIZE				0x08

#define MML_MEM_OTP_MXIM_BASE					0x10100000
#define MML_MEM_OTP_MXIM_SIZE					( 4 * 1024 )
#define MML_MEM_OTP_BASE						MML_MEM_OTP_MXIM_BASE

#define	K_MML_SFLC_OTP_BASE						MML_MEM_OTP_BASE
#define	K_MML_SFLC_OTP_BASE_OFFSET				0
#define	K_MML_SFLC_OTP_MAXIM_BASE				MML_MEM_OTP_MXIM_BASE
#define	K_MML_SFLC_OTP_MAXIM_BASE_OFFSET		( MML_MEM_OTP_MXIM_BASE - MML_MEM_OTP_BASE )
#define	K_MML_SFLC_OTP_MAXIM_SIZE				MML_MEM_OTP_MXIM_SIZE /* 4 KiB */

#define	MML_MEM_OTP_USER_BASE					0x10101000
#define MML_MEM_OTP_USER_SIZE					( 4 * 1024 )
#define	K_MML_SFLC_OTP_USER_BASE				MML_MEM_OTP_USER_BASE
#define	K_MML_SFLC_OTP_USER_BASE_OFFSET			( MML_MEM_OTP_USER_BASE - MML_MEM_OTP_BASE )
#define	K_MML_SFLC_OTP_USER_SIZE				MML_MEM_OTP_USER_SIZE /* 4 KiB */

/* OTP patterns */
#define	K_MML_SFLC_OTP_VIRGIN_PATTERN_BYTE		0xff
#define	K_MML_SFLC_OTP_VIRGIN_PATTERN_SHORT		0xffff
#define	K_MML_SFLC_OTP_VIRGIN_PATTERN_INT		0xffffffff
#define	K_MML_SFLC_OTP_VIRGIN_PATTERN			0xffffffffffffffff
#define	K_MML_SFLC_OTP_LOCK_PATTERN_BYTE		0x00
#define	K_MML_SFLC_OTP_LOCK_PATTERN_SHORT		0x0000
#define	K_MML_SFLC_OTP_LOCK_PATTERN_INT			0x00000000
#define	K_MML_SFLC_OTP_LOCK_PATTERN				0x0000000000000000

/* Magic words */
#define	FL_ACNTL_MAGIC_WORD1				0x3a7f5ca3
#define	FL_ACNTL_MAGIC_WORD2				0xa1e34f20
#define	FL_ACNTL_MAGIC_WORD3				0x9608b2c1

/* Erase Code */
#define	MML_SFLC_CN_ERASE_CODE_NOP				0x00
#define	MML_SFLC_CN_ERASE_CODE_PAGE				0x55
#define	MML_SFLC_CN_ERASE_CODE_MASS				0xaa

/* Macros *********************************************************************/

/* Enumerations ***************************************************************/
/** MML_SLFC errors list */
typedef enum {
	N_MML_SFLC_ERR_MIN = K_MML_SFLC_BASE_ERROR, N_MML_SFLC_ERR_NOT_INITIALIZED, /**< Error Code: SFLC module not initialized */
	N_MML_SFLC_ERR_LENGTH_NOT_ALIGNED, /**< Error Code: Error Code: Data is not aligned according to memory specification */
	N_MML_SFLC_ERR_ADDRESS_NOT_ALIGNED, /**< Error Code: Address is not aligned according to memory specification */
	N_MML_SFLC_ERR_PACKET_NOT_ALIGNED, /**< Error Code: Data packet is not aligned according to memory specification */
	N_MML_SFLC_ERR_NOT_ALLOWED, /**< Error Code: Memory area is not allowed */
	N_MML_SFLC_ERR_NOT_ACCESSIBLE, /**< Error Code: Memory is not accessible */
	N_MML_SFLC_ERR_OVERFLOW, /**< Error Code: Data length is out of memory capacity */
	N_MML_SFLC_ERR_32BITS_PGM_FAILED, /**< Error Code: 32bits programming failed */
	N_MML_SFLC_ERR_128BITS_PGM_FAILED, /**< Error Code: 128bits programming failed */
	N_MML_SFLC_ERR_BURST_PGM_FAILED, /**< Error Code: Burst programming failed */
	N_MML_SFLC_ERR_PAGE_ERASE_FAILED, /**< Error Code: Page erase failed */
	N_MML_SFLC_ERR_MASS_ERASE_FAILED, /**< Error Code: Mass erase failed */
	N_MML_SFLC_ERR_FAILURE, /**< Error Code: Internal failure */
	N_MML_SFLC_ERR_UNKNOWN, N_MML_SFLC_ERR_MAX = N_MML_SFLC_ERR_UNKNOWN /**< Error Code: Generic error for unknown behavior */

} mml_sflc_error_t;

/** Number of errors for this module */
#define	K_MML_SFLC_ERR_COUNT				( N_MML_SFLC_ERR_MAX - N_MML_SFLC_ERR_MIN )

/** OTP area identifier */
typedef enum {
	N_MML_SFLC_OTP_ID_MIN = 0, N_MML_SFLC_OTP_ID_MAXIM = N_MML_SFLC_OTP_ID_MIN, /**< Maxim Area OTP ID */
	N_MML_SFLC_OTP_ID_USER, /**< User Area OTP ID */
	N_MML_SFLC_OTP_ID_MAX = N_MML_SFLC_OTP_ID_USER,
	N_MML_SFLC_OTP_ID_COUNT,
} mml_sflc_otp_id_t;

/**
 * Return the version of the SFLC driver.
 * 
 */
const char * mml_sflc_get_version(void);

/**
 * This function is used to initialize Flash controller
 */
void mml_sflc_init(void);

/**
 * This function is used to shutdown Flash controller
 */
void mml_sflc_shutdown(void);

/** This function unlocks OTP for programming process */
void mml_sflc_otp_unlock(void);

/** This function locks OTP to go back to read process */
void mml_sflc_otp_lock(void);

/**
 * This function is uses to erase data in Flash
 * @param[in] 	address								Address of the data in Flash memory.
 * @param[in] 	length								Number of data bytes to erase.
 * @retval		NO_ERROR							No Error
 * @retval		COMMON_ERR_UNKNOWN					Unknown Error
 * @retval		COMMON_ERR_IN_PROGRESS				Forced exit not finished
 * @retval		N_MML_SFLC_ERR_OVERFLOW				Data length is out of memory capacity
 * @retval		N_MML_SFLC_ERR_PAGE_ERASE_FAILED	Page erase failed
 */
int mml_sflc_erase(unsigned int address, unsigned int length);

/**
 * This function is uses to erase data in sector Flash (data erased fill be part of the sector)
 * @param[in] 	address								Address of the data in Flash memory.
 * @param[in] 	length								Number of data bytes to erase.
 * @retval		NO_ERROR							No Error
 * @retval		COMMON_ERR_UNKNOWN					Unknown Error
 * @retval		COMMON_ERR_IN_PROGRESS				Forced exit not finished
 * @retval		N_MML_SFLC_ERR_OVERFLOW				Data length is out of memory capacity
 * @retval		N_MML_SFLC_ERR_PAGE_ERASE_FAILED	Page erase failed
 */
int mml_sflc_erase_sector(unsigned int address, unsigned int length);

/**
 * This function is uses to erase all data in sector Flash
 * @param[in] 	address								Address of the data in Flash memory. Address must be sector Aligned
 * @retval		NO_ERROR							No Error
 * @retval		COMMON_ERR_UNKNOWN					Unknown Error
 * @retval		COMMON_ERR_IN_PROGRESS				Forced exit not finished
 * @retval		N_MML_SFLC_ERR_OVERFLOW				Data length is out of memory capacity
 * @retval		N_MML_SFLC_ERR_PAGE_ERASE_FAILED	Page erase failed
 */
int mml_sflc_erase_full_sector(unsigned int address);

int mml_sflc_mass_erase(void);
int mml_sflc_write(unsigned int address, unsigned char* tx_buf, unsigned int length);
int mml_sflc_write_32bits(unsigned int address, unsigned int *p_src);
int mml_sflc_write_128bits(unsigned int address, unsigned int *p_src);
#ifdef __SFLC_BURST
int mml_sflc_write_128bits_burst_start(unsigned int address, unsigned int *p_src);
int mml_sflc_write_128bits_burst_continue(unsigned int address, unsigned int *p_src);
#endif
void mml_sflc_lock_flash();

/**
 * This function is used to write data in OTP memory
 * @param[in] 	id									OTP area identifier (MAXIM or User)
 * @param[in] 	address								OTP address where the data will be put
 * @param[in] 	*p_src								Pointer on data source
 * @param[in] 	length								Data length
 * @retval		NO_ERROR							No Error
 * @retval		COMMON_ERR_INVAL					OTP id is not in appropriate range
 * @retval		COMMON_ERR_UNKNOWN					Unknown Error
 * @retval		COMMON_ERR_NULL_PTR					Pointer is null
 * @retval		N_MML_SFLC_ERR_ADDRESS_NOT_ALIGNED	Address is not aligned according to memory specification
 * @retval		N_MML_SFLC_ERR_LENGTH_NOT_ALIGNED	Data is not aligned according to memory specification
 * @retval		N_MML_SFLC_ERR_OVERFLOW				Data length is out of memory capacity
 */
int mml_sflc_otp_write(mml_sflc_otp_id_t id, unsigned int address,
		unsigned char *p_src, unsigned int length);

/**
 * This function is used to read data from OTP memory
 * @param[in] 	id									OTP area identifier (MAXIM or User)
 * @param[in] 	address								Address of the data
 * @param[out] 	*p_dst								Pointer on data destination
 * @param[in]	length								Length of data to read
 * @retval		NO_ERROR							No Error
 * @retval		COMMON_ERR_INVAL					OTP id is not in appropriate range
 * @retval		COMMON_ERR_UNKNOWN					Unknown Error
 * @retval		COMMON_ERR_NULL_PTR					Pointer is null
 * @retval		COMMON_ERR_OUT_OF_RANGE				Address is out of expected range
 * @retval		N_MML_SFLC_ERR_ADDRESS_NOT_ALIGNED	Address is not aligned according to memory specification
 * @retval		N_MML_SFLC_ERR_LENGTH_NOT_ALIGNED	Data is not aligned according to memory specification
 * @retval		N_MML_SFLC_ERR_OVERFLOW				Data length is out of memory capacity
 */
int mml_sflc_otp_read(mml_sflc_otp_id_t id, unsigned int address,
		unsigned char *p_dst, unsigned int length);

/** @} *//* @defgroup MML_SFLC */

#ifdef __cplusplus
}
#endif

#endif /* _MML_SFLC_H_ */

/******************************************************************************/
/* EOF */
