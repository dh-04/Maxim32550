/*
 * @file sc_types.h
 *
 * Smartcard data types & structures definitions
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
#ifndef _SC_TYPES_H_
#define _SC_TYPES_H_
#include <stdint.h>
#include "sc_errors.h"
#include "sc_states.h"
#include "OS_types.h"

/** @var Di Di constants lookup array
 */
extern const uint8_t Di[16];

/** @var Fi Frequency divider constants lookup array
 */
extern const uint16_t Fi[16];

/** @defgroup SMARTCARD_TYPES   Smartcard Types & Structures definitions
 *
 * @ingroup SMARTCARD_DRIVER
 *
 * This file defines all the drivers/stacks data types & structures.
 *
 * @note
 * @{
 */

/** @def NULL NULL pointer definition
 *  @brief In case of NULL is not define, set NULL as a pointer on 0
 */
#ifndef NULL
# define NULL     (0)
#endif

/** @typedef    boolean_t defines a boolean type & the bTRUE and bFALSE values.
 */
typedef enum {
    bFALSE = 0,
    bTRUE = 1,
} boolean_t;

/** @enum    IccVoltage_t declares the configurable voltages.
 *
 *   This declares the configurable voltages, some may not be applicable.
 *   It depends on the Analog Front End and on the Smartcard itself
*/
typedef enum
{
    VCC_1V8 = 0,        /**< turn on with VCC=1.8v (Class C) */
    VCC_3V  ,           /**< turn on with VCC=3v (Class B) */
    VCC_5V  ,           /**< turn on with VCC=5v (Class A) */
    LAST_ALLOWED_VOLTAGE = VCC_5V,
} IccVoltage_t;


/** @typedef    ActivationParams_t contains all the Slot activation parameters
 *
 *   This struct declares all the configurable slot activation parameters
*/
typedef struct
{
    IccVoltage_t    IccVCC;                 /**< Card voltage setting */
    uint8_t         IccResetDuration;       /**< Reset duration in ETU */
    uint16_t        IccATR_Timeout;         /**< ATR Timeout value in ETU */
    uint8_t         IccTS_Timeout;          /**< TS Byte timeout in ETU */
    uint8_t         IccWarmReset;           /**< If not 0, we operate a warm reset */
} ActivationParams_t;




/** @typedef    IccRequest_t defines a data exchange between the reader a the card
*/
typedef struct
{
    uint8_t        *IccData;            /**< pointer on the receive buffer*/
    uint16_t       IccLen;              /**< length to receive */
    uint16_t       IccReceived;         /**< number of received bytes */
    boolean_t      IccLastByte;         /**< is it the last byte to receive */
    uint16_t       IccEDC;              /**< current LRC/CRC value */
    IccReturn_t    IccStatus;           /**< card status */
} IccRequest_t;


/** @} */ /* @defgroup SMARTCARD_TYPES */

/** @defgroup SMARTCARD_LOW_LEVEL_OPS Smartcard low level driver operations
 * @ingroup ICC_ABSTRACTION
 *
 * @note
 * @{
 */

 /** @typedef UartId_t Defines  UART interfaces
  */
typedef enum {
    SCI_0 = 0,      /**< Smartcard interface 0 */
    SCI_1,          /**< Smartcard interface 1 */
} UartId_t;




 /** @} */

/** @} */ /* @file    sc_types.h*/

#endif /*_SC_TYPES_H_*/
