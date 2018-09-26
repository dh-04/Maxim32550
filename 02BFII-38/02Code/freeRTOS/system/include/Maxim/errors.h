/*
 * @file errors.h
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

#ifndef _ERRORS_H_
#define _ERRORS_H_

/** @file errors.h MML common error codes */

/** @addtogroup MML_SYS MML System 
 *	@ingroup MaximCMSIS
 */

/** @defgroup MML_ERRORS MML common error codes
 *
 * @ingroup MML_SYS
 *
 * @{
 */

#ifndef __ASSEMBLER__

/* -------------------------------------------------------------------------- */
/* Common error codes */
#define COBRA_COMMON         		0x70
/**  */
#define COBRA_UART            		0x71
#define COBRA_I2C             		0x72
#define COBRA_DMA             		0x73
#define COBRA_GPIO             		0x74
#define COBRA_USB	            	0x75
#define COBRA_KEYPAD             	0x76
#define COBRA_SPI	            	0x77
#define COBRA_SMART_CARD            0x78
#define COBRA_TIMER	             	0x79
#define COBRA_TFT	            	0x7a
#define	COBRA_SMON					0x7b
#define	COBRA_TRNG					0x7c
#define	COBRA_ADC					0x7d
#define	COBRA_ETH					0x7e
#define	COBRA_GCR					0x7f
#define	COBRA_INIT					0x81
#define	COBRA_INTC					0x82
#define	COBRA_MLCD					0x83
#define	COBRA_RTC					0x84
#define	COBRA_UCI					0x85
#define	COBRA_WATCHDOG				0x86
#define	COBRA_OTP					0x87
#define	COBRA_WUT					0x88


/** Error code prefix offset */
#define COBRA_ERR_PREFIX_OFFSET		24

/** Common errors list */
typedef enum
{
	/** Specific No Error code */
	NO_ERROR = 0,
	/** Generic errors */
	COMMON_ERR_MIN = (COBRA_COMMON << COBRA_ERR_PREFIX_OFFSET),
	/** Error Code: No such device */
	COMMON_ERR_NO_DEV,
	/** Error Code: Value is not appropriate */
	COMMON_ERR_INVAL,
	/** Error Code: Pointer is null */
	COMMON_ERR_NULL_PTR,
	/** Error Code: Value is out of expected range */
	COMMON_ERR_OUT_OF_RANGE,
	/** Error Code: Module not initialized */
	COMMON_ERR_NOT_INITIALIZED,
	/** Error Code: Critical error */
	COMMON_ERR_FATAL_ERROR,
	/** Error Code: Still processing */
	COMMON_ERR_RUNNING,
	/** Error Code: Action not allowed in this state */
	COMMON_ERR_BAD_STATE,
	/** Error Code: Data does not match */
	COMMON_ERR_NO_MATCH,
	/** Error Code: Action already done */
	COMMON_ERR_ALREADY,
	/** Error Code: Action not finished yet, still in progress */
	COMMON_ERR_IN_PROGRESS,
	/** Error Code: Operation is not permitted */
	COMMON_ERR_NOT_PERMITTED,
	/** Error Code: Generic error for unknown behavior */
	COMMON_ERR_UNKNOWN,
	COMMON_ERR_MAX = COMMON_ERR_UNKNOWN,

} cobra_common_errors_t;

/** peripheral modules base errors  */
#define COBRA_COMMON_BASE_ERR       		(COBRA_COMMON << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_UART_BASE_ERR					(COBRA_UART << COBRA_ERR_PREFIX_OFFSET)
#define COBRA_I2C_BASE_ERR       			(COBRA_I2C << COBRA_ERR_PREFIX_OFFSET)
#define COBRA_DMA_BASE_ERR      			(COBRA_DMA << COBRA_ERR_PREFIX_OFFSET)
#define COBRA_GPIO_BASE_ERR     			(COBRA_GPIO << COBRA_ERR_PREFIX_OFFSET)
#define COBRA_USB_BASE_ERR       			(COBRA_USB << COBRA_ERR_PREFIX_OFFSET)
#define COBRA_KEYPAD_BASE_ERR    			(COBRA_KEYPAD << COBRA_ERR_PREFIX_OFFSET)
#define COBRA_SPI_BASE_ERR	    			(COBRA_SPI << COBRA_ERR_PREFIX_OFFSET)
#define COBRA_SMART_CARD_BASE_ERR    		(COBRA_SMART_CARD << COBRA_ERR_PREFIX_OFFSET)
#define COBRA_TIMER_BASE_ERR	        	(COBRA_TIMER << COBRA_ERR_PREFIX_OFFSET)
#define COBRA_TFT_BASE_ERR	        		(COBRA_TFT << COBRA_ERR_PREFIX_OFFSET)
#define COBRA_SMON_BASE_ERR       			(COBRA_SMON << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_TRNG_BASE_ERR					(COBRA_TRNG << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_ADC_BASE_ERR					(COBRA_ADC << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_ETH_BASE_ERR					(COBRA_ETH << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_GCR_BASE_ERR					(COBRA_GCR << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_INIT_BASE_ERR					(COBRA_INIT << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_INTC_BASE_ERR					(COBRA_INTC << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_MLCD_BASE_ERR					(COBRA_MLCD << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_RTC_BASE_ERR					(COBRA_RTC << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_UCI_BASE_ERR					(COBRA_UCI << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_WATCHDOG_BASE_ERR				(COBRA_WATCHDOG << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_OTP_BASE_ERR					(COBRA_OTP << COBRA_ERR_PREFIX_OFFSET)
#define	COBRA_WUT_BASE_ERR					(COBRA_WUT << COBRA_ERR_PREFIX_OFFSET)


#endif /* #ifndef __ASSEMBLER__ */

/** @} */ /* @defgroup MML_ERRORS */

#endif /* _ERRORS_H_ */

/******************************************************************************/
/* EOF */
