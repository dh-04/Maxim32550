/*
 * @file ProtocolIT0_private.h
 *
 * T=0 protocol private functions
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

#ifndef _PROTOCOL_T0_PRIV_H_
#define _PROTOCOL_T0_PRIV_H_
#include <stdint.h>
#include "sc_errors.h"
#include "ProtocolT0.h"
#include "ProtocolT1_BlockMgt.h" /* for the T1_MAX_BLOCK_SIZE define*/
#include "sc_types.h"

/** @defgroup PROTOCOL_T0_PRIVATE T=0 protocol internal functions
 *
 * @ingroup SMARTCARD_DRIVER
 *
 * This file defines the T=0 protocol private functions.\n
 * This is not intended to be directly used by the user.
 *
 * @{
 */


/** @def     MAXDATALEN
 *  @brief   APDU Response maximum length (256 bytes)
 *
 *  MAXDATALEN defines the maximum length of the Card Response.
 */
#define MAXDATALEN 256

/**
 * @func  T0Case1
 * @param [in]  SlotCtx       pointer on the #SlotContext_t
 * @param [in]  TxBuff        Buffer to send
 * @param [in]  ScratchPadBuff     temporary receive buffer
 * @param [in]  RxPtr          pointer on the current byte in the caller #RxBuff
 * @param [in]  RxLen          pointer on the caller Received length (#RxLen)
 * @param [out] pSW1           pointer on the 1st status byte
 * @param [out] pSW2           pointer on the 2nd status byte
 *
 * @return it returns ICC_OK on success
 * @return an #IccReturn_t return code on failure
 *
 * Process a Case 1  APDU exchange, refer to #Case_t
 */
IccReturn_t T0Case1(SlotContext_t *SlotCtx, uint8_t *TxBuff,
                    IccRequest_t  *RxRequest,
                    uint8_t *ScratchPadBuff, uint8_t **RxPtr, uint32_t *RxLen,
                    uint8_t *pSW1, uint8_t *pSW2);


/**
 * @func  T0Case2
 * @param [in]  SlotCtx       pointer on the #SlotContext_t
 * @param [in]  TxBuff        Buffer to send
 * @param [in]  LeLEN          Data length to send
 * @param [in]  ScratchPadBuff     temporary receive buffer
 * @param [in]  RxPtr          pointer on the current byte in the caller #RxBuff
 * @param [in]  RxLen          pointer on the caller Received length (#RxLen)
 * @param [out] pSW1           pointer on the 1st status byte
 * @param [out] pSW2           pointer on the 2nd status byte
 * @param [in] pfSendWTE       pointer on the WTX handler (if not in EMV mode)
 *
 * @return it returns ICC_OK on success
 * @return an #IccReturn_t return code on failure
 *
 * Process a Case 2  APDU exchange, refer to #Case_t
 * @note Extended APDU is not supported.
 */
IccReturn_t T0Case2 ( SlotContext_t *SlotCtx, uint8_t *TxBuff,
                      IccRequest_t  *RxRequest,
                       uint16_t LeLEN, uint8_t *ScratchPadBuff,
                       uint16_t MaxBuffSize, uint8_t **RxPtr, uint32_t *RxLen,
                       uint8_t *pSW1, uint8_t *pSW2,void (*pfSendWTE) (void) );

/**
 * @func  T0Case3
 * @param [in]  SlotCtx       pointer on the #SlotContext_t
 * @param [in]  TxBuff        Buffer to send
 * @param [in]  ScratchPadBuff     temporary receive buffer
 * @param [in]  RxPtr          pointer on the current byte in the caller #RxBuff
 * @param [in]  RxLen          pointer on the caller Received length (#RxLen)
 * @param [out] pSW1           pointer on the 1st status byte
 * @param [out] pSW2           pointer on the 2nd status byte
 * @param [in] pfSendWTE       pointer on the WTX handler (if not in EMV mode)
 *
 * @return it returns ICC_OK on success
 * @return an #IccReturn_t return code on failure
 *
 * Process a Case 3  APDU exchange, refer to #Case_t
 * @note this function is also used on the first part of a Case 4 exchange.
 * @note Extended APDU is not supported.
 */
IccReturn_t T0Case3 ( SlotContext_t *SlotCtx,uint8_t *TxBuff,
                      IccRequest_t  *RxRequest,
                      uint8_t *ScratchPadBuff, uint8_t **RxPtr, uint32_t *RxLen,
                      uint8_t *pSW1, uint8_t *pSW2,  void (*pfSendWTE) (void));

/**
 * @func  T0GetProcByte
 * @param [in]  SlotCtx        pointer on the #SlotContext_t
 * @param [in]  RxRequest       pointer on the Rx request structure (#IccRequest_t)
 *
 * @return it returns ICC_OK on success
 * @return an #IccReturn_t return code on failure
 *
 * Initialize the Smartcard driver to receive data and returns when at least
 * one byte (PCB) has been received.
 */
IccReturn_t T0GetProcByte(SlotContext_t  *SlotCtx,
                          IccRequest_t  *RxRequest);

/**
 * @func  T0GetMoreRxBytes
 * @param [in]  SlotCtx        pointer on the #SlotContext_t
 * @param [in]  FinalOffset
 * @param [in]  InitialOffset
 * @param [in]  SaveByte        if set, the received bytes will be stored in \n
 *                              the user Rx buffer (#RxPtr)
 * @param [out] ScratchPadBuff      Temporary receive buffer
 * @param [out]  RxPtr          pointer on the current byte in the caller #RxBuff
 * @param [out]  RxLen          pointer on the caller Received length
 *
 * @return it returns ICC_OK on success
 * @return an #IccReturn_t return code on failure
 *
 * This functions gets (FinalOffset-InitialOffset) bytes from the card. \n
 * if #SaveByte is set, the received bytes are stored starting at the #RxPtr address.\n
 * if #SaveByte is set, RxLen is updated with the number of bytes read.
 *
 */
IccReturn_t T0GetMoreRxBytes (SlotContext_t *SlotCtx,
                              uint16_t FinalOffset, uint16_t InitialOffset,
                              boolean_t SaveByte, uint8_t *ScratchPadBuff,
                              uint8_t **RxPtr, uint32_t *RxLen);

/**
 * @func  SendT0
 * @param [in]  TxBuff          APDU to send buffer
 * @param [in]  TxLen           Length of the APDU to transmit
 * @param [out] LcLen           computed Lc bytes number
 * @param [out] LeLen           computed Le bytes number
 *
 * @return the APDU Case_[1-4] or CASE_UNKNOWN if not recognized
 *
 * This functions returns the APDU Case. \n
 * @note Extended APDU is not supported.
 */
Case_t T0DetectCmdCase(uint8_t *TxBuff, uint16_t TxLen,
                       uint16_t *LcLen, uint16_t *LeLen);

/**
 * @func  T0TransmitHeader
 * @param [in]  SlotCtx        pointer on the #SlotContext_t
 * @param [in]  TxBuff          APDU to send
 * @param [in]  len             length of the data to transmit/receive
 *
 * @return it returns ICC_OK on success
 * @return an #IccReturn_t return code on failure
 *
 * This functions generates uses TxBuff to generate a command header and
 * set the Lc value to #len
 */
IccReturn_t T0TransmitHeader(SlotContext_t *SlotCtx, uint8_t *TxBuff, uint8_t len);


/** @} */ /*@defgroup PROTOCOL_T0_PRIV    */
/** @} */ /*@file    protocolt0_private.h    */
#endif /* _PROTOCOL_T0_PRIV_H_ */
