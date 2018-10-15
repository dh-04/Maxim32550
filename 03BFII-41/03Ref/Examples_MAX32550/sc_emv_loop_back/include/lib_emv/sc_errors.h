/*
 * @file sc_errors.h
 *
 * Smartcard errors definition
 *
 ******************************************************************************
 * Copyright (C) 2015-2017 Maxim Integrated Products, Inc., All rights Reserved.
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

#ifndef _SC_ERRORS_H_
#define _SC_ERRORS_H_
#include <stdint.h>

/** @defgroup DRIVER_ERRORS    Smartcard error codes definitions
 *
 * @ingroup SMARTCARD_DRIVER
 *
 * @brief    This file defines all the drivers/stacks error codes.
 *
 * @note
 * @{
 */

/** @typedef    IccReturn_t
 *  @brief      contains all the UART / AFE drivers & Stacks error codes \n
 *              Failure value is negative, \n
 *              Success is 0.
*/
typedef enum {
    ICC_ERR_BAD_PARAMETER       = -127,   /**< Inconsistent param from user */
    ICC_ERR_OVERCURRENT,                  /**< Smartcard overcurrent detected */
    ICC_ERR_UNKOWN,                       /**< all unknow error sources */
    ICC_ERR_NOTPERMITTED,                 /**< ICC already powered when trying to change the voltage */
    ICC_ERR_POWERED,                      /**< ICC already powered when trying to change the voltage */
	ICC_ERR_VOLTAGE,					  /**< Voltage can't be set	*/
    ICC_ERR_NULL_PTR,                     /**< NULL pointer received */
    ICC_ERR_BAD_BLOCK,                    /**< Wrong block on T=1 procotocol*/
    ICC_ERR_BAD_EDC,                      /**< Bad LRC or CRC */
    ICC_ERR_TRANSMISSION,                 /**< Error when Tx on T=1 */
    ICC_ERR_ABORT_OCCURED,                /**< ABORT Request during a transaction */
    ICC_ERR_PTS_NEGOTIATION,              /**< PTS negociation failed */
    ICC_ERR_BAD_TCK,                      /**< Bad TCK byte */
    ICC_ERR_BAD_TS,                       /**< TS is neither 3B or 3F */
    ICC_ERR_BAD_ATR_VALUE,                /**< Inconsistent ATR received */
    ICC_ERR_BAD_PROCEDURE_BYTE,           /**< Wrong PCB value */
    ICC_ERR_WRONG_LEN,                    /**< Inconsitent command len */
    ICC_ERR_ATR_TIMEOUT,                  /**< Card Mute or parity issue */
    ICC_ERR_BAD_SLOT,                     /**< Unkown or unitialized Slot */
	ICC_ERR_BAD_INTERFACE,				  /**< Incorrect smart card interface */
    ICC_ERR_REMOVED,                      /**< Card absent or removed */
    ICC_ERR_PRESENT_INACTIVE,             /**< Card not powered */
    ICC_ERR_PARITY,                       /**< Parity check error */
    ICC_ERR_TIMEOUT,                      /**< timeout during a Rx */
    ICC_ERR_TX_UNDERRUN,                  /**< Tx FIFO underrun (error !) */
    ICC_ERR_RX_OVERRUN,                   /**< overflow on the Rx FIFO */
    ICC_NEED_WARMRESET,                   /**< the Icc needs a warm reset */

    ICC_RX_PENDING,                       /**< Rx is in progress */
    ICC_OK = 0,                           /**< Operation succeed */
} IccReturn_t;


/** @} */ /* @defgroup SMARCARD_ERRORS */
/** @} */ /* @file    sc_errors.h*/
#endif /*_SC_ERRORS_H_*/
