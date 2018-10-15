/*
 * @file ProtocolIT1_BlockMgt.h
 *
 * T=1 protocol Block Management header
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

#ifndef _T1_BLOCK_MGT_H_
#define _T1_BLOCK_MGT_H_

#include <stdint.h>

/** @defgroup PROTOCOL_T1_BLOCKMGT T=1 protocol Block management
 *  @ingroup  PROTOCOL_T1
 *
 * This file defines the T=1 protocol block management functions and
 * Macros.
 *
 * @{
 */


/** @enum  T1BlockOffsets
 *  @brief T1 Block byte offsets
 *
 * This enum declares the NAD, PCB, LEN, PAYLOAD offsets in the T=1 block
 */
enum T1BlockOffsets {
    NAD_OFFSET = 0,     /**< NAD is the first block byte */
    PCB_OFFSET,         /**< PCB is the second block byte */
    LEN_OFFSET,         /**< LEN is the third block byte */
    PAYLOAD_OFFSET,     /**< Payload start at the fourth block byte */
};

#define PCB_MBIT            0x20    /**< M bit: More data to come bit (chaining) */
#define PCB_I_NBIT          0x40    /**< N bit: IBlock Sequence toggle bit */
#define PCB_R_NBIT          0x10    /**< N bit: RBlock Sequence toggle bit */

#define T1_MAX_BLOCK_SIZE   259     /**< 254 payload bytes + NAD + PCB + LEN + CRC16*/

#define BLOCK_TYPE_MASK     0xC0    /**< Block type PCB mask value */
#define BLOCK_ERROR_MASK    0x03    /**< Error flags are the 2 last bits */
#define IBLOCK_TYPE         0x00    /**< I-Block type PCB value */
#define RBLOCK_TYPE         0x80    /**< R-Block type PCB value */
#define SBLOCK_TYPE         0xC0    /**< S-Block type PCB value */


/** @enum  SBlockTypes
 * @brief  Types for S-block
 */
enum SBlockTypes {
    S_RESYNCH_REQ       = 0xC0, /**< Resynchronisation request */
    S_RESYNCH_RSP       = 0xE0, /**< Resynchronisation response */
    S_IFS_REQ           = 0xC1, /**< IFS request */
    S_IFS_RSP           = 0xE1, /**< Resynchronisation response */
    S_ABORT_REQ         = 0xC2, /**< ABORT request */
    S_ABORT_RSP         = 0xE2, /**< ABORT response */
    S_WTX_REQ           = 0xC3, /**< Wait time extension request */
    S_WTX_RSP           = 0xE3, /**< Wait time extension response */
    S_ERROR_VPP         = 0xE4, /**< VPP Error (not managed) */
};




/******************************************************************************
 ******************************************************************************
 *
 * T=1 Block Management API
 *
 */


/** @fn                     CheckMBit
 *  @brief                  return bTRUE if the PCB indicates More data to come
 *  @param [in] PCBtoCheck   Block PCB value to check

 *  @return                 returns a #boolean_t value
 *  @retval     bTRUE        if the M bit is set in the PCB
 *  @retval     bFALSE       if the M bit is not set
 *
 */
boolean_t CheckMBit(uint8_t PCBtoCheck);


/** @fn PCBValueIBlock
 *  @brief Set the PCB byte value for an I-Block according to the N, M parameters
 *  @param [in] N    Sequence toggle bit
 *  @param [in] M    value of the block to follow bit (chaining)
 *  @return         returns PCB value for the I block type specified
 */
uint8_t PCBValueIBlock(boolean_t N,boolean_t M);


/** @fn PCBValueRBlock
 *  @brief          Set the PCB byte value for an R-Block
 *  @param [in] N    Sequence toggle bit
 *  @return         returns PCB value for the R block type
 */
uint8_t PCBValueRBlock (boolean_t N);

/** @fn             SendIBlock
 *  @brief          Send an I-Block to the card
 *  @param [in] SlotCtx   pointer on the Slot configuration (see #SlotContext_t)
 *  @param [in] N          Sequence toggle bit
 *  @param [in] M          value of the block to follow bit
 *  @param [in] TxPtr      Pointer on the TxBuffer pointer
 *  @param [in] bDataSize  Length of the data to send
 *  @return               returns a #IccReturn_t error code
 *  @retval     ICC_OK    when everything went well
 */
IccReturn_t SendIBlock(SlotContext_t  *SlotCtx, boolean_t N, boolean_t M,
                       uint8_t **TxPtr, uint8_t DataSize);

/** @fn                   SendRBlock
 *  @brief                Send a R-Block to the card
 *  @param [in] SlotCtx   pointer on the Slot configuration (see #SlotContext_t)
 *  @param [in] N          Sequence toggle bit
 *  @param [in] bError     Error identifier (0 => ok, 1 => EDC error, 2 => other error)
 *  @return               returns a #IccReturn_t error code
 *  @retval     ICC_OK    when everything went well
 */
IccReturn_t SendRBlock(SlotContext_t  *SlotCtx, boolean_t N, uint8_t bError);

/** @fn                   SendSBlock
 *  @brief                Send a S-Block to the card
 *  @param [in] SlotCtx   pointer on the Slot configuration (see #SlotContext_t)
 *  @param [in] BlockType  type of S-Block (ABORT, IFS, RESYNCH, WTX)
 *  @param [in] INFvalue   value for the only one byte INF field if IFS or WTX
 *  @return               returns a #IccReturn_t error code
 *  @retval     ICC_OK    when everything went well
 */
IccReturn_t SendSBlock(SlotContext_t  *SlotCtx, uint8_t BlockType,uint8_t INFvalue);

/** @fn                     ReceiveBlock
 *  @brief                  Receives a block from the smartcard
 *  @param [in] SlotCtx     pointer on the Slot configuration (see #SlotContext_t)
 *  @param [in] ReceivedPCB  PCB of the received block
 *  @param [in] RxPtr        pointer on the RxBuffer pointer
 *  @param [in] ReceivedINF  pointer on the received length
 *  @return                 returns a #IccReturn_t error code
 *  @retval     ICC_OK      when everything went well
 *
 *  Receives a block from the smartcard and gives its PCB and its LEN
 */
IccReturn_t ReceiveBlock (SlotContext_t  *SlotCtx, uint8_t *ReceivedPCB,
                           uint8_t **RxPtr, uint8_t *ReceivedINF);

/** @] */ /* defgroup */
/** @} */ /*file*/
#endif /* _T1_BLOCK_MGT_H_ */
