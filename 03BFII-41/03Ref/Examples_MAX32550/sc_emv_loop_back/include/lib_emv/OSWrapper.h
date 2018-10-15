/*
 * @file OSWrapper.h
 *
 * Smartcard OS function wrapper
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

#ifndef _SC_OSWRAPPER_H_
#define _SC_OSWRAPPER_H_

#include <stdint.h>
#include "sc_errors.h"
#include "sc_config.h"
#include "slot.h"

#include MAX325XX_H_PATH  /*for the Smart_Card_i_IRQn */

/** @defgroup OSWRAPPER Smartcard OS function wrapper
 *
 * @ingroup ICC_ABSTRACTION
 *
 * This file defines OS dependant functions/calls as register, memory map....
 *
 * @{
 */

#if defined(__MAX32552) || defined(__MAX32560) || defined(__MAX32565)
/** @def        MML_INTNUM_SC0
 *  @brief      MAX325xx interrupt vector for UART 0
 */
#define MAX325xx_INTNUM_SC0     Smart_Card_0_IRQn

/** @def        MML_INTNUM_SC1
 *  @brief      MAX325xx interrupt vector for UART 1
 */
#define MAX325xx_INTNUM_SC1     Smart_Card_1_IRQn
#elif defined(__MAX32550) || defined(MAX32550_B1) || defined(__MAX32555)
/** @def        MML_INTNUM_SC0
 *  @brief      MAX325xx interrupt vector for UART
 */
#define MAX325xx_INTNUM_SC0     Smart_Card_IRQn
#endif


/** @def        MAX_INTERFACE_NUMBER
 *  @brief      Number of available UART interface(s)
 */
#if defined(__MAX32550) || defined(MAX32550_B1) || defined(__MAX32555)
# define MAX_INTERFACE_NUMBER    1
#elif defined(__MAX32590)
# error The EMV Stack v2.0 does not support COBRA version of MAX32590
#elif defined(__MAX32552) || defined(__MAX32560) || defined(__MAX32565)
# define MAX_INTERFACE_NUMBER	 2
#else
#error	Unkown chip. (you must define either __MAX32550, MAX32550_B1, __MAX32555, __MAX32552, __MAX32560 or __MAX32565)
#endif

/** @fn                     OSWrapperMap
 *  @brief                  Map a memory region and returns the virtual address
 *  @param [in] uintptr_t    Physical address of the UART
 *
 *  Map a memory region and returns the virtual address
 *
 *  @retval    NULL             if it failed
 *  @retval    uintptr_t type   the virtual address.
 */
uintptr_t OSWrapper_Map(uintptr_t physical_address);


/** @fn                          OSWrapper_WriteReg
 *  @brief                       Write a 32bit value in a register and
 *                               wait 2µs
 *  @param [in] virtual_address  Virtual address of the *REGISTER*
 *  @param [in] value            a 32bit value to write in the register
 *
 *  @note The input/output parameters are not checked against NULL pointers !
 */
void OSWrapper_WriteReg(uintptr_t virtual_address, volatile uint32_t value);

/** @fn                          OSWrapper_WriteReg
 *  @brief                       Write a 32bit value in a register
 *  @param [in] virtual_address  Virtual address of the *REGISTER*
 *  @param [in] value            a 32bit value to write in the register
 *
 *  @note The input/output parameters are not checked against NULL pointers !
 */
void OSWrapper_WriteReg_nodelay(uintptr_t virtual_address,
                                volatile uint32_t value);

/** @fn                          OSWrapper_ReadReg
 *  @brief                       Read a 32bit value from a register
 *  @param [in] virtual_address  Virtual address of the *REGISTER*
 *  @param [in] value            a pointer on a 32bit value
 *
 *  @note The input/output parameters are not checked against NULL pointers !
 */
void OSWrapper_ReadReg(uintptr_t virtual_address, volatile uint32_t *value);

/** @fn                          OSWrapper_SystemIRQEnable
 *  @brief                       register the IRQ handler
 *  @param [in] UartId           Interface number
 *
 *  @return     it returns an integer  (32bit signed)
 *  @retval     ICC_ERR_BAD_PARAMETER    Wrong interface number (uartid)
 *  @retval     ICC_ERR_UNKOWN    		if error
 *  @retval     NO_ERROR                register succeed
 */
IccReturn_t OSWrapper_SystemIRQEnable(uint8_t UartId);

/** @fn                          OSWrapper_SystemIRQDisable
 *  @brief                       register the IRQ handler
 *  @param [in] UartId           Interface number
 *
 *  @return     it returns an integer  (32bit signed)
 *  @retval     ICC_ERR_BAD_PARAMETER    Wrong interface number (uartid)
 *  @retval     ICC_ERR_UNKOWN    		if error
 *  @retval     NO_ERROR                register succeed
 */
IccReturn_t OSWrapper_SystemIRQDisable(uint8_t UartId);

/** @fn                          OSWrapper_RegisterIRQ
 *  @brief                       register the IRQ handler
 *  @param [in] UartId           Interface number
 *  @param [in] handler          pointer on the interrupt handler
 *  @param [in] SlotCtx          Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in] isPHYIRQ         Set to TRUE if the IRQ is related to the PHY
 *
 *  @return     it returns an integer  (32bit signed)
 *  @retval     OS_ERR_BAD_INTERFACE    Wrong interface number (uartid)
 *  @retval     COMMON_ERR_INVAL        if an internal error occured (wrong Vector number)
 *  @retval     NO_ERROR                register succeed
 */
IccReturn_t OSWrapper_RegisterIRQ(uint8_t UartId,
							  void (*handler)(SlotContext_t  *SlotCtx),
                              SlotContext_t  *SlotCtx, boolean_t isPHYIRQ);

/** @fn                          OSWrapper_UnregisterIRQ
 *  @brief                       unregister the IRQ handler
 *  @param [in] UartId           Interface number
 *  @param [in] SlotCtx          Slot configuration context pointer (cf #SlotContext_t)
 *  @param [in] isPHYIRQ         Set to TRUE if the IRQ is related to the PHY
 *
 *  @return     it returns an integer  (32bit signed)
 *  @retval     OS_ERR_BAD_INTERFACE    Wrong interface number (uartid)
 *  @retval     OS_ERR_BAD_SLOT         the Slotconf does not match with the current handler parameter.
 *  @retval     OS_OK                   Unregister succeed
 *
 */
IccReturn_t OSWrapper_UnregisterIRQ(uint8_t UartId,
                                SlotContext_t  *SlotCtx, boolean_t isPHYIRQ);

/** @fn                          OSWrapper_Interrupt_disable
 *  @brief                       disable all the interrupt sources
 */
void OSWrapper_Interrupt_disable( void );


/** @fn                          OSWrapper_Interrupt_enable
 *  @brief                       enable all the interrupt sources
 */
void OSWrapper_Interrupt_enable( void );

/** @fn                          OSWrapper_getUARTbyId
 *  @brief                       return an UART supported operations structure (cf #UartOps_t)
 *  @param [in] UartId           Interface number
 *
 *  @return   a pointer on the #UartData_t for the corresponding UartId
 *  @reval    #NULL     if the UartId is unknown
 */
UartData_t *OSWrapper_getUARTbyId(UartId_t uartid);


/** @fn                          OSWrapper_getSlotbyId
 *  @brief                       return a Slot supported operations structure (cf #SlotOps_t)
 *  @param [in] slotid           Slot Number
 *
 *  @return   a pointer on the #SlotData_t for the corresponding slotid
 *  @reval    #NULL     if the slotid is unknown
 */
SlotData_t *OSWrapper_getSlotbyId(uint8_t slotid);


/** @fn                          OSWrapper_getSlotCtxbyId
 *  @brief                       return a Slot context structure (cf #SlotContext_t)
 *  @param [in] slotid           Slot Number
 *
 *  @return   a pointer on the #SlotContext_t for the corresponding slotid
 *  @reval    #NULL     if the slotid is unknown
 */
SlotContext_t  *OSWrapper_getSlotCtxbyId(uint8_t SlotId);





/** @fn                          OSWrapper_RegisterUART
 *  @brief                       register an UART supported operations pointer (cf #UartOps_t)
 *  @param [in] UartId           Interface number
 *  @param [in] ops              pointer on the operations to register
 *  @param [in] UartPrivateData  interface private (internal) data pointer
 *
 *  @return   an #IccReturn_t  error code
 *  @reval    ICC_OK            on success
 */
IccReturn_t OSWrapper_RegisterUART(UartId_t uartid, UartOps_t *ops,
                                   void *UartPrivateData);

/** @fn							OSWrapper_UnRegisterUART
 *  @brief						Unregister an UART supported operations pointer (cf #UartOps_t)
 *  @param [in] UartId			Interface number
 *
 *  @return   an #IccReturn_t	error code
 *  @reval    ICC_OK            on success
 */
IccReturn_t OSWrapper_UnRegisterUART(UartId_t uartid);

/** @fn                          OSWrapper_RegisterSlot
 *  @brief                       register an AFE supported operations pointer (cf #SlotOps_t)
 *  @param [in] slotid           Slot Number
 *  @param [in] ops              pointer on the operations to register
 *  @param [in] SlotPrivateData  Slot private (internal) data pointer
 *
 *  @return   an #IccReturn_t  error code
 *  @reval    ICC_OK            on success */
IccReturn_t OSWrapper_RegisterSlot(uint8_t slotid, SlotOps_t *ops,
                                    void *SlotPrivateData);

/** @fn                          OSWrapper_UnRegisterSlot
 *  @brief                       UnRegister an AFE supported operations pointer (cf #SlotOps_t)
 *  @param [in] slotid            Slot Number
 *
 *  @return   an #IccReturn_t  error code
 *  @reval    ICC_OK            on success
 */
IccReturn_t OSWrapper_UnRegisterSlot(uint8_t slotid);

/** @fn                          OSWrapper_memdestroy
 *  @brief                       destroy a buffer content
 *  @param [in] Buffer           pointer on the buffer to erase
 *  @param [in] length           number of *bytes* to erase
 *
 */
void OSWrapper_memdestroy(uint8_t *Buffer, int32_t length);

#if defined(__MAX32550) || defined(MAX32550_B1) || defined(__MAX32555)
/** @fn                          OSWrapper_resetSC
 *  @brief                       Reset the SC block
 */
void OSWrapper_resetSC(void);
#elif defined(__MAX32552) || defined(__MAX32560) || defined(__MAX32565)
/** @fn                          OSWrapper_resetSC0
 *  @brief                       Reset the SC0 block
 */
void OSWrapper_resetSC0(void);

/** @fn                          OSWrapper_resetSC1
 *  @brief                       Reset the SC1 block
 */
void OSWrapper_resetSC1(void);
#endif

/** @fn                          OSWrapper_EnterCritical
 *  @brief                       Disable the scheduler and the interrupt sources
 */
void OSWrapper_EnterCritical( void );


/** @fn                          OSWrapper_ExitCritical
 *  @brief                       Enable the scheduler and the interrupt sources back.
 */
void OSWrapper_ExitCritical( void );

/** @fn                     udelay
 *  @brief                  Basic wait function
 *
 *  @param [in]  us         Delay to wait in microseconds
 */
void __attribute__ ((optimize("O0"))) udelay(int us);

/** @} */ /*defgroup*/
/** @} */ /*file*/


#endif /* _SC_OSWRAPPER_H_ */
