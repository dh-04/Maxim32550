/*
 * @file ProtocolIT1.h
 *
 * Send/Receive APDU using T=1 protocol
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

#ifndef _PROTOCOL_T1_H_
#define _PROTOCOL_T1_H_

#include <stdint.h>
#include "sc_errors.h"
#include "slot.h"

/** @defgroup PROTOCOL_T1 T=1 protocol internal API
 *
 * @ingroup SMARTCARD_DRIVER
 *
 * This file defines the T=1 protocol API.\n
 * This is not intended to be directly used by the user.
 *
 * @{
 */

 /** @typedef   T1ProtocolState_t
  *  @brief     This declares all the T1 protocol state machine states
  */
typedef enum {
    T1_SEND_BLOCK_STATE = 0,    /**< Default state to send the first block to the card */
    T1_RECEIVE_BLOCK_STATE,     /**< Once a block has been sent, we enter this state to receive the response block*/
    T1_IS_LAST_BLOCK_STATE,     /**< check if it was the last block exchange */
    T1_SEND_RBLOCK_STATE,       /**< Send a R-Block */
    T1_SEND_BLOCK_AGAIN_STATE,  /**< In case of error, this state sends the last block once again */
    T1_SEND_WTX_RSP_STATE,      /**< Send a Wait Time eXtension response (S-Block) to the card */
    T1_SEND_IFS_RSP_STATE,      /**< Send the IFS response (S-Block) to the card */
    T1_SEND_R_M1_STATE,         /**< Acknowledge chained block (initiated by the card) */
    T1_RESYNCH_STATE,           /**< Generate a resynchronization request */
    T1_SEND_ABORT_RSP_STATE,    /**< Answer to an ABORT request */
    T1_DEACTIVATION_STATE,      /**< Transfer is done, deactivate the card */
    T1_STATE_DONE,              /**< transmit done */
} T1ProtocolState_t;


/** @fn SendT1
 *  @brief  Send a T1 APDU and get the Icc response
 *  @param [in]  slotid          Slot Id to activate
 *  @param [in]  TxBuff          APDU to send buffer
 *  @param [in]  SendDataLen     Length of the APDU to transmit
 *  @param [out] RxBuff          Receive buffer (Must be at least 2 bytes in case 1)
 *  @param [out] ReceiveDataLen  pointer on the received length
 *  @param [in]  pfSendWTE       pointer on the WTX handler (if not in EMV mode)
 *
 *
 *  Send a T=1 command (in Tx buffer) and get the response (in Rx buffer)
 */
IccReturn_t SendT1(uint8_t slotid,
                   uint8_t *TxBuff, uint32_t SendDataLen,
                   uint8_t *RxBuff,  uint32_t *ReceiveDataLen,
                   void (*fSend_WTE) (void) );


/** @} */ /*defgroup*/
/** @} */ /*file*/
#endif /*_PROTOCOL_T1_H_*/
