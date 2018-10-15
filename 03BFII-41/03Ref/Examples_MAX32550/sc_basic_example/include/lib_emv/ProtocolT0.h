/*
 * @file ProtocolIT0.h
 *
 * Send/Receive APDU using T=0 protocol
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

#ifndef _PROTOCOL_T0_H_
#define _PROTOCOL_T0_H_
#include <stdint.h>
#include "sc_types.h"

/** @defgroup PROTOCOL_T0 T=0 protocol internal API
 *
 * @ingroup SMARTCARD_DRIVER
 *
 * This file defines the T=0 protocol structures and functions.\n
 * This is not intended to be directly used by the user.
 *
 * T=0 APDU formatting:
 * @htmlonly
<table border="3">
    <tr> <!-- Array Title -->
        <th colspan="3">Command APDU</th>
    </tr>
    <tr> <!-- Array 2nd line (column titles) -->
        <th>Field name</th>
        <th>Length (bytes)</th>
        <th>Description</th>
    </tr>
    <tr><!--3rd CLA byte description -->
        <td>CLA</td>
        <td>1</td>
        <td>Instruction class - indicates the type of command</td>
    </tr>
    <tr>
        <td>INS</td>
        <td>1</td>
        <td>Instruction code - indicates the specific command</td>
    </tr>
        <tr>
        <td>P1-P2</td>
        <td>2</td>
        <td>Instruction parameters for the command</td>
    </tr>
    <tr>
        <td>L<sub>c</sub></td>
        <td>Absent or 1 </td>
        <td>Encodes the number (N<sub>c</sub>) of bytes of command data to follow<br />
            (3 bytes for Extended APDU, not supported)</td>
    </tr>
    <tr>
        <td>Command data</td>
        <td>N<sub>c</sub></td>
        <td>N<sub>c</sub> bytes of data</td>
    </tr>
    <tr>
        <td>L<sub>e</sub></td>
        <td>Absent or 1</td>
        <td>Encodes the maximum number (N<sub>e</sub>) of response bytes expected<br />
            (3 bytes for Extended APDU, not supported)</td>
    </tr>
    <tr>
        <th colspan="3">Response APDU</th>
    </tr>
    <tr>
        <td>Response data</td>
        <td>N<sub>r</sub> (at most N<sub>e</sub>)</td>
        <td>Response data</td>
    </tr>
    <tr>
        <td>SW1-SW2<br />
        (Command status)</td>
        <td>2</td>
        <td>Command processing status</td>
    </tr>
</table>
 * @endhtmlonly
 * @{
 */



/**
 * @fn          SendT0
 * @brief       Send an APDU to the card and get the response
 * @param [in]  slotid          Slot Id to activate
 * @param [in]  TxBuff          APDU to send buffer
 * @param [in]  TxLen           Length of the APDU to transmit
 * @param [out] RxBuff          Receive buffer (Must be at least 2 bytes in case 1)
 * @param [out] RxLen           pointer on the received length
 * @param [in]  pfSendWTE       pointer on the WTX handler (if not in EMV mode)
 *
 * @return it returns ICC_OK on success or an #IccReturn_t return ocode
 */
IccReturn_t SendT0(uint8_t slotid, uint8_t *TxBuff, uint32_t TxLen,
                   uint8_t *RxBuff, uint32_t *RxLen,
                   void (*pfSendWTE)(void));
#define T0_MAX_APDU_SIZE		258 /*256 + status */

/** @} */ /*@defgroup PROTOCOL_T0    */
/** @} */ /*@file    protocolt0.h    */
#endif /* _PROTOCOL_T0_H_*/
