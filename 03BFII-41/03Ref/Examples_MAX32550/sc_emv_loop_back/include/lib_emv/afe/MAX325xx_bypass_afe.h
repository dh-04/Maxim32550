/*
 * @file MAX325xx_bypass_afe.h Teridian 73S8009r AFE driver
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

#ifndef _MAX325XX_BYPASS_AFE_H_
#define _MAX325XX_BYPASS_AFE_H_

#include <stdint.h>
#include <MAX325xx_afe.h>
#include "sc_errors.h"
#include "slot.h"
#include "sc_states.h"
#include "sc_config.h"

/** @defgroup _MAX325XX_BYPASS_AFE_H_ Analog Front End drivers for the MAX325xx bypass mode
 *
 * @ingroup ICC_ABSTRACTION
 *
 * This file defines AFE access functions for calls from the abstraction layer.\n
 * This is not intended to be directly used by the user.
 *
 * @{
 */

/** @fn                     bypassSelect
 *  @brief                  Select (enable) an AFE (Analog Front End)
 *  @param [in] SlotCtx     Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in] Selected     if #bTRUE, we select the AFE (we enable)
 *
 *  @return                         return an #IccReturn_t error code
 *  @retval ICC_ERR_BAD_PARAMETER   if is the slot number is out of range
 *  @retval ICC_ERR_NULL_PTR        if the SlotCtx is #NULL
 *  @retval ICC_OK                  if the AFE is now selected (enabled)
 */
IccReturn_t __attribute__ ((weak)) bypassSelect(SlotContext_t  *SlotCtx, boolean_t Selected);

/** @fn                     bypassSetVoltage
 *  @brief                  Set the slot voltage
 *  @param [in] SlotCtx     Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in] Voltage      the requested voltage (cf #IccVoltage_t)
 *
 *  @return                 return an #IccReturn_t error code
 *  @retval ICC_ERR_POWERED if the card session is already active
 *  @retval ICC_OK          if the AFE is now selected (enabled)
 *
 *  Change a card voltage.
 *  @note   The card must be powered off before changing the voltage.
 */
IccReturn_t __attribute__ ((weak)) bypassSetVoltage(SlotContext_t  *SlotCtx, IccVoltage_t Voltage);

/** @fn                     bypassApplyVoltage
 *  @brief                  Apply the voltage setting on the slot
 *
 *  @param [in] SlotCtx     Slot configuration context pointer (cf #SlotContext_t)
 *
 *  @return                 return an #IccReturn_t error code
 *  @retval ICC_ERR_POWERED if the card session is already active
 *  @retval ICC_OK          if the AFE is now selected (enabled)
 *
 *  Change a card voltage.
 *  @note   The card must be powered off before changing the voltage.
 *  @note 	This function must be updated with the specific external PHY used on bypass mode (depending on the GPIO pin used).
 */
IccReturn_t __attribute__ ((weak)) bypassApplyVoltage(SlotContext_t  *SlotCtx);

/** @fn                     bypassPower
 *  @brief                  Turn On/Off the card
 *  @param [in] SlotCtx     Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in] PowerUp     The requested state (ON/OFF/WARM_RESET, cf #CardPowerState_t)
 *
 *  @return                 return an #IccReturn_t error code
 *  @retval ICC_ERR_POWERED if the card session is already active
 *  @retval ICC_OK          if the AFE is now selected (enabled)
 *  @note	This function must be updated in the case of PowerUp = POWER_UP with the speciic external PHY used.
 *
 */
IccReturn_t __attribute__ ((weak)) bypassPower(SlotContext_t  *SlotCtx, CardPowerState_t PowerUp);

/** @fn     bypassInterrupt_Handler
 *  @brief  Manage PHY Interrupts
 *
 */
void __attribute__ ((weak)) bypassInterrupt_Handler(void *param);

/** @fn                     bypassEnableIrq
 *  @brief                  enable or disable AFE interrupts
 *  @param [in] SlotId      slot numbre, cf #MAX3255xSlots_t
 *
 *  @return                 return an #IccReturn_t error code
 *  @retval ICC_OK          if the AFE is now selected (enabled)
 *
 */
IccReturn_t __attribute__ ((weak)) bypassEnableIrq(SlotContext_t  *SlotCtx, int32_t enable);

/** @fn                     bypassGetCardStatus
 *  @brief                  Return the slot state
 *  @param [in] SlotCtx     AFE slot configuration context pointer (cf #SlotContext_t)
 *
 *  @return                     return an #IccReturn_t error code
 *  @retval    ICC_OK                    if the card is inserted and powered.
 *  @retval    ICC_ERR_REMOVED           if the card is not present.
 *  @retval    ICC_ERR_PRESENT_INACTIVE  if the card is present but not powered.
 *
 */
IccReturn_t __attribute__ ((weak)) bypassGetCardStatus(SlotContext_t  *SlotCtx);

/** @fn                     bypassInit
 *  @brief                  Initialize the Analog Front End for the SAM slot
 *
 *  @return                 return an #IccReturn_t error code
 *  @retval ICC_OK          if the AFE is now selected (enabled)
 *
 *  @note   The AFE init must be done *AFTER* the UART init.
 *  @note   As this driver is only for bare-metal, we can directly access to
 *          the GPIOs to configure/drive/
 */
IccReturn_t __attribute__ ((weak)) bypassInit(UartId_t UartId, MAX325xxSlots_t SlotId);

/** @} */ /*defgroup*/
/** @} */ /*file*/


#endif /* _MAX325XX_BYPASS_AFE_H_*/
