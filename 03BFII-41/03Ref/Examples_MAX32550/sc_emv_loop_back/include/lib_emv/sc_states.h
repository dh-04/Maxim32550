/*
 * @file sc_states.h
 *
 * Smartcard states
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

#ifndef _SC_STATES_H_
#define _SC_STATES_H_
#include <stdint.h>
#include "sc_types.h"

/** @defgroup ICC_STATES   Smartcard states
 *
 * @ingroup ICC_ABSTRACTION
 *
 * This is not intended to be directly used by the user.
 *
 * @{
 */


/** @enum    CardState_t Card Event/states enum
 */
typedef enum {
    ICC_INSERTION,      /**< a card has been inserted */
    ICC_REMOVAL,        /**< the card has been removed */
    ICC_FAULT,          /**< card fault (over current, short-circuit...)*/
} CardState_t;


/** @enum    CardPowerState_t Icc power states enum.
 */
typedef enum {
    POWER_DOWN = 0, /**< Power off the card */
    POWER_UP,       /**< turn on with the voltage defined in the context */
    RESET_DO,       /**< Put the card in RESET (RST pin low) */
    RESET_RELEASE,  /**< Release the card reset signal (RST pin high) */
} CardPowerState_t;


/** @} */ /*defgroup*/
/** @} */ /*file*/
#endif /* _SC_DATA_H_ */
