/*
 * @file ATR.h
 *
 * Smartcard stack Anwser To Reset Management.
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

#ifndef _ATR_H_
#define _ATR_H_
#include <stdint.h>
#include "sc_errors.h"
#include "sc_types.h"

/** @defgroup ATR    Answer To Reset (ATR) management
 *
 * @ingroup SMARTCARD_DRIVER
 *
 * This file defines the Answer To Reset functions & variables.
 *
 * @note
 * @{
 */


/** @defgroup ATR_DEFINES    Answer To Reset (ATR) constants definition
 *
 * @ingroup ATR
 * @{
 */
#define TA_BIT          (1<<4)  /**< TAx presence bit (bit 4, 0x10) */
#define TB_BIT          (1<<5)  /**< TBx presence bit (bit 5, 0x20) */
#define TC_BIT          (1<<6)  /**< TCx presence bit (bit 6, 0x40) */
#define TD_BIT          (1<<7)  /**< TDx presence bit (bit 7, 0x80) */

#define BGTT0           16      /**< EMV set the BGT to 16 ETU for T=0 */
#define BGTT1           22      /**< EMV set the BGT to 22 ETU for T=1 */

/**  @var NB_BITS
 * @brief Use to get the number of bytes following a TDi byte\n
 *
 * The number of bytes to get is the number of bits set in the
 * most significant nibble of the TDx byte.
 *  */
extern const uint8_t NB_BITS[];

/** @var wDi
 * @brief Di constants lookup array
 */
extern const uint8_t wDI[];


/** @} */ /* @defgroup ATR_DEFINES */
/** @} */ /* 2nd closing brace, I do not understand why I do need it ! */

/**@defgroup ATR_FUNCTIONS    Answer To Reset API
 * @ingroup ATR
 *
 * @func     GetATR(...)
 * @param [in] slotid                 Slot Id to activate
 * @param [out] pRxBuffer             pointer on the output buffer (where the ATR will be stored)
 * @param [out] pATRLength            address of the ATR length word
 * @param [in]  ATRDeactivateOnError  if bTRUE the card will be deactivated on error.
 * @param [in]  ActivationParams      card activation parameters (cf #ActivationParams_t)
 *
 * @return it returns ICC_OK on success
 * @return an #IccReturn_t return code on fail
 */
IccReturn_t GetATR(uint8_t slotid, uint8_t *pRxBuffer,
                   uint32_t *pATRLength,
                   boolean_t ATRDeactivateOnError,
                   ActivationParams_t   *ActivationParams);

/** @} */ /* @defgroup ATR_FUNCTIONS */
/** @} */ /* @defgroup SMARCARD_ATR */
/** @} */ /* @file    atr.h*/
#endif /*_ATR_H_*/
