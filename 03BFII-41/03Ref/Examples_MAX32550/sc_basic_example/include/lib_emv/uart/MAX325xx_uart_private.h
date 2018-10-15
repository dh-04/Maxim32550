/*
 * @file MAX325xx_uart_private.h
 *
 * MAX32550, MAX32552, MAX32555, MAX32560 & MAX32565 Smartcard UART private driver functions
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

#ifndef _UART_MAX325xx_PRIVATE_H_
#define _UART_MAX325xx_PRIVATE_H_

#include <stdint.h>
#include "sc_errors.h"
#include "slot.h"
#include "sc_config.h"

/** @defgroup UART_MAX325xx_PRIVATE MAX32590, MAX32550, MAX32552, MAX32555, MAX32560, MAX32565 Smartcard UART driver - private functions
 *
 * @ingroup ICC_ABSTRACTION
 *
 * This file defines UART access functions for calls from the abstraction layer.\n
 * This is not intended to be directly used by the user.
 *
 * @{
 */

/** @fn UartOnCardStateChanged
 *  @brief  Notify the driver that the card status changed
 *  @param [in] SlotCtx     Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in] CardState    New card state (cf #CardState_t)
 *
 *  Notify the driver that the card status changed, \n
 *  if the card has been removed or is no longer powered, the UART driver
 *  will deactivate the card and return an error code.
 */
static void  UartOnCardStateChanged(SlotContext_t  *SlotCtx, CardState_t CardState);


/** @fn     UartSetConfig
 *  @brief  set the timing parameters according to the actual protocol
 *  @param [in] SlotCtx     Slot configuration context pointer (cf #SlotContext_t)
 *
 *  set the ETU, EGT, WT according to the #SlotContext_t and the current protocol.
 */
static IccReturn_t UartSetConfig(SlotContext_t  *SlotCtx);

/** @fn                             UartActiveWait
 *  @brief                          Wait for a number of ETUs
 *  @param [in]  SlotCtx             Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in]  etus                number of ETUs to wait
 *
 *  Waits for a defined number of ETUs.
 */
static void UartActiveWait(SlotContext_t  *SlotCtx, uint32_t etus);

/** @fn     UartReceive
 *  @brief  Set the UART driver in Rx state
 *  @param [in] SlotCtx     Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in] RxRequest    Rx parameters (pointer on the Rx buffer, expected length... cf #IccRequest_t)
 *  @return                 return an #IccReturn_t error code
 *  @reval     ICC_ERR_NULL_PTR if either the RxRequest pointer or the Rx buffer pointer is null
 *  @retval    ICC_OK           if everything went well.
 *
 *  Change the UART Driver state to Rx mode and set the Rx parameters according to the #IccRequest_t RxRequest
 */
static IccReturn_t UartReceive(SlotContext_t  *SlotCtx, IccRequest_t *RxRequest);

/** @fn                     UartGetRxLen
 *  @brief                  return the number of received bytes
 *  @param [in] SlotCtx      Slot configuration context pointer (cf #SlotContext_t)
 *  @param [out] Status      contains an #IccReturn_t error code
 *
 *  @return                 the number of bytes received.
 */
static uint16_t    UartGetRxLen(SlotContext_t  *SlotCtx, IccReturn_t *Status);

/** @fn     UartSend
 *  @brief  Send a buffer to the card
 *  @param [in] SlotCtx     Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in] pData        Pointer on the Tx buffer
 *  @param [in] datalen      Length of the data to send (excluding the EDC in T=1)
 *
 *  @return                 return an #IccReturn_t error code
 *  @reval     ICC_ERR_NULL_PTR     if pData pointer is null
 *  @retval    ICC_ERR_TX_UNDERRUN  if we were interrupted during the Tx and we got an underrun.
 *  @retval    ICC_OK               if everything went well.
 *
 * Send a buffer to the card.
 * For T=1 protocol, it also append the EDC (CRC16 or LRC8) to the frame.
 */
static IccReturn_t UartSend(SlotContext_t  *SlotCtx, uint8_t *pData, uint16_t datalen);


/** @fn     UartStop
 *  @brief  Stop the UART Rx process and set the IccStatus value
 *  @param [in] SlotCtx     Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in] status       The Rx status
 *
 */
static void UartStop(SlotContext_t  *SlotCtx, IccReturn_t status);


/** @fn                             UartActivate
 *  @brief                          Start the card activation
 *  @param [in]  SlotCtx            Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in]  ActivationParams    pointer on the activation timings (cf #ActivationParams_t)
 *  @param [in] RxRequest    Rx parameters (pointer on the Rx buffer, expected length... cf #IccRequest_t)
 *
 *  @return                 return an #IccReturn_t error code
 *  @reval     ICC_ERR_NULL_PTR     if pData pointer is null
 *  @retval    ICC_OK               if everything went well.
 *
 */
static IccReturn_t UartActivate(SlotContext_t  *SlotCtx, ActivationParams_t *ActivationParams, IccRequest_t *RxRequest);

/** @fn                     UartInterruptEnable IRQ
 *  @brief                  enable or disable UART interrupts
 *  @param [in] SlotId      slot numbre, cf #MAX3255xSlots_t
 *
 *  @return                 return an #IccReturn_t error code
 *  @retval ICC_OK          if the AFE is now selected (enabled)
 *
 */
IccReturn_t UartInterruptEnable(SlotContext_t  *SlotCtx, int32_t enable);
/** @} */ /*defgroup*/
/** @} */ /*file*/


#endif /* _UART_MAX325xx_PRIVATE_H_*/
