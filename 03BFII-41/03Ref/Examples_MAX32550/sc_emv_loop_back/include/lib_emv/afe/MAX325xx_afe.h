/*
 * @file MAX3255x_afe.h
 *
 * MAX32550, MAX32552, MAX32555, MAX32560 & MAX32565 on-SoC Analog Front End - driver private functions
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

#ifndef _AFE_MAX3255x_H_
#define _AFE_MAX3255x_H_

#include <stdint.h>
#include "sc_errors.h"
#include "slot.h"
#include "sc_states.h"
#include "sc_config.h"

/** @defgroup AFE_MAX325xx Analog Front End drivers for the MAX32550, MAX32552, MAX32555, MAX32560, MAX32565
 *
 * @ingroup ICC_ABSTRACTION
 *
 * This file defines AFE access functions for calls from the abstraction layer.\n
 * This is not intended to be directly used by the user.
 *
 * @{
 */

/** @typedef MAX325xxSlots_t
 *  @brief   Smartcard slots definition for MAX325xx
 *
 *  by definition, the smartcard slot is the PHY output and the SAM slot
 *  is the RAW output.
 */

typedef enum {
    SCI_0_INTERNAL_AFE = 0,
	SCI_0_BYPASS_SLOT,
    SCI_1_INTERNAL_AFE,
	SCI_1_BYPASS_SLOT,
	MAX325xx_SLOT_NUMBER_MAX
} MAX325xxSlots_t;


#if defined(__MAX32590)
# error The MAX32590 has no on-chip PHY (Analog Front End)
#elif defined(__MAX32550)
# define MAX325xx_SLOT_NUMBER       1
#elif defined(MAX32550_B1) || defined(__MAX32555)
# define MAX325xx_SLOT_NUMBER       2
#elif  defined(__MAX32552) ||defined (__MAX32560) || defined(__MAX32565)
# define MAX325xx_SLOT_NUMBER		4
#else
#error	Unkown chip. (you must define either __MAX32550, MAX32550_B1, __MAX32555, __MAX32552, __MAX32560 or __MAX32565)
#endif

/** @def MAX325xx_AFE_INTERRUPT_MASK
 *  @brief Smartcard AFE interrupt mask
 */
#define MAX325xx_AFE_INTERRUPT_MASK      (0xE00)

/** @fn                     AfeInit
 *  @brief                  Initialize the Analog Front End
 *  @param [in] SlotId      Interface number, cf #UartId_t
 *
 *  @return                 return an #IccReturn_t error code
 *  @retval ICC_OK          if the AFE is now selected (enabled)
 *
 *  @note   The AFE init must be done *AFTER* the UART init.
 *  @note   As this driver is only for bare-metal, we can directly access to
 *          the GPIOs to configure/drive/
 */
IccReturn_t AfeInit(UartId_t UarttId);

/** @} */ /*defgroup*/
/** @} */ /*file*/


#endif /* _AFE_MAX3255x_H_*/
