/*
 * @file MAX325xx_uart.h
 *
 * MAX32550, MAX32552, MAX32555, MAX32560 & MAX32565 Smartcard UART driver
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

#ifndef _UART_MAX325xx_H_
#define _UART_MAX325xx_H_

#include <stdint.h>
#include "sc_errors.h"
#include "slot.h"
#include "sc_config.h"

#define SC_CLOCK_MAX            4800000
#define SC_CLOCK_MIN            2000000

/** @defgroup UART_MAX325xx MAX32590, MAX32550, MAX32552, MAX32555, MAX32560, MAX32565 Smartcard UART driver
 *
 * @ingroup ICC_ABSTRACTION
 *
 * This file defines UART access functions for direct calls from the abstraction layer.\n
 * This is not intended to be directly used by the user.
 *
 * @{
 */

#if defined(__MAX32552) || defined(__MAX32560) || defined(__MAX32565) || defined(__MAX32590)
# define MAX325xx_INTERFACE_NUMBER           2
#elif defined(__MAX32550) || defined(MAX32550_B1) || defined(__MAX32555)
# define MAX325xx_INTERFACE_NUMBER           1
#else
#error	Unkown chip. (you must define either __MAX32550, MAX32550_B1, __MAX32555, __MAX32552, __MAX32560 or __MAX32565)
#endif

/** @def MAX325xx_UART_INTERRUPT_MASK
 *  @brief Smartcard UART interrupt mask
 *
 *  this mask is used to disable all the UART interrupts except the AFE
 *  (PHY) interrupts.
 */
#define MAX325xx_UART_INTERRUPT_MASK      (0x1FF)

/** @fn                     UartInit
 *  @brief                  Initialize the UART driver
 *  @param [in] UartId_t     Interface number (cf #UartId_t)
 *  @param [in]  SlotCtx            Slot configuration context pointer (cf #SlotContext_t)
 *
 *  Initialize the UART driver.\n
 *
 *  @retval    ICC_OK                   if the driver has been registered
 *  @retval    ICC_ERR_BAD_PARAMETER    if the Interface id (uart id) is out of range
 *  @retval    ICC_ERR_NULL_PTR         if we did not get the virtual address
 *
 * @note On non-Linux plateforms, this must be called during the Stack initialization.
 * @note the GPIOs must be configured as Alternate function by the caller (OS-Dependant task)
 * @note the GCR.SCCK must be configured by the caller.
 */
IccReturn_t  UartInit(UartId_t id, SlotContext_t  *SlotCtx);

/** @fn                     UartSetClock
 *  @brief					Initialize the UART clock
 *  @param [in] UartId	    Interface number
 *  @param [in] clockfreq	Desired frequency
 *
 *  Initialize the UART clock.\n
 *
 *  @retval		ICC_OK                   if the driver has been registered
 *  @retval		ICC_ERR_BAD_SLOT		 if UartId parameter is out of range
 *  @retval		ICC_ERR_BAD_PARAMETER    if clockfreq parameter is out of range
 *  @retval		ICC_ERR_NULL_PTR         if we did not get the virtual address
 *  @retval		ICC_ERR_NOTPERMITTED	 if interface is already powered
 *
 * @note On non-Linux plateforms, this must be called during the Stack initialization.
 * @note the GPIOs must be configured as Alternate function by the caller (OS-Dependant task)
 * @note the GCR.SCCK must be configured by the caller.
 */
IccReturn_t UartSetClock(SlotContext_t  *SlotCtx, unsigned int clockfreq);

/** clear uart pending interrupts, this is used when no smartcard is inserted
 * but an uart interrupt fires (when removing the card during an active state)
 * @param [in]  SlotCtx            Slot configuration context pointer (cf #SlotContext_t)
 */
int32_t UartInterrupt_clear(SlotContext_t  *SlotCtx, SCISR_t Status);
/** @} */ /*defgroup*/
/** @} */ /*file*/


#endif /* _UART_MAX325xx_H_*/
