/*
 * @file smartcard_api.h
 *
 * Smartcard API for users
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


#ifndef _SMARTCARD_API_H_
#define _SMARTCARD_API_H_

#include <stdint.h>
#include "sc_errors.h"
#include "sc_states.h"
#include "sc_config.h"

/** @defgroup SMARTCARD_DRIVER Smartcard and Analog Front End Driver for MAX325xx
 *  @ingroup EX_SC_EMV_LOOP_BACK
 *
 * @{
 */

/** @defgroup SMARTCARD_DRIVER_API Smartcard and Analog Front End Driver Application interface
 *
 * This file defines all the functions which are user accessible.
 *
 * @{
 */

/** @typedef        ioctl_t IO control values
 */
typedef enum {
    IOCTL_RESET = 0,            /**< Card Reset request (Card reset when *pparam is not 0) 													*/
    IOCTL_POWER_CARD,           /**< Card Power Up/Down (Card powered when *pparam is not 0)												*/
    IOCTL_SET_VOLTAGE,          /**< Set Card Voltage pparam is a pointer on a #IccVoltage_t value											*/
    IOCTL_SET_INITPARAMS,       /**< Set Init params (used during the card activation), pparams points on a #ActivationParams_t struct		*/
    IOCTL_SET_EMVMODE,          /**< Set the Stack Mode (EMV if *pparam is not 0)															*/
    IOCTL_SET_FIDI,             /**< Change (force) the FIDI (TA1) value *USE WITH CAUTION*													*/
    IOCTL_SET_GUARD_TIME,       /**< Change (force) the GT value																			*/
    IOCTL_SET_CWT,              /**< Change (force) the CWT value (must be done after the card activation and the PPS to have an effect)	*/
    IOCTL_SET_BWT,              /**< Change (force) the BWT value (must be done after the card activation and the PPS to have an effect)	*/
    IOCTL_SET_WTE_HANDLER,      /**< set the user Wait time extension handler (called only if not in EMV mode)								*/
    IOCTL_SET_PRESENCE_IRQ,     /**< Register User handler for the card status changes														*/
    IOCTL_REMOVE_PRESENCE_IRQ,  /**< Unregister User handler for the card status changes													*/
    IOCTL_GET_FIDI,             /**< Returns the actual FiDi value in *pparam																*/
    IOCTL_GET_GUARD_TIME,       /**< Returns the actual GT value in *pparam																	*/
    IOCTL_GET_CWT,              /**< Returns the actual CWT value in *pparam																*/
    IOCTL_GET_WT,               /**< Returns the actual BWT/WWT value in *pparam															*/
    IOCTL_GET_CARD_STATE,       /**< Returns the card state in *pparam, cf #IccReturn_t														*/
    IOCTL_GET_CARD_CLASS,       /**< Returns the card class in *pparam, cf #IccReturn_t														*/
    IOCTL_SET_CLOCK_FREQ,       /**< Change the smartcard clock frequency																	*/
	IOCTL_ENABLE_AFE_INTERRUPT,	/**< Enable AFE interrupt.																					*/
    IOCTL_CARD_RESET,           /**< Do a card reset (internal use)																			*/
} sc_ioctl_t;


#define xstr(s) str(s)
#define str(s) #s
#define SCAPI_VERS_MAJOR	2
#define SCAPI_VERS_MINOR	3
#define SCAPI_VERS_PATCH	2
#define SCAPI_VERSION_STRING	xstr(CHIP)"_v" xstr(SCAPI_VERS_MAJOR) "." xstr(SCAPI_VERS_MINOR) "." xstr(SCAPI_VERS_PATCH)

/**
 * Return the version of the Smart card API.
 * 
 */
const char * SCAPI_get_version(void);

/** @fn         SCAPI_open
 *  @brief      Open a Slot
 *
 *  @param [in] UartId		Uart id, cf #UartId_t
 *  @param [in] SlotId      slot number, cf #MAX325xxSlots_t
 *
 *  @return     returns an #IccReturn_t error code
 *
 */
IccReturn_t SCAPI_open(UartId_t UartId, uint8_t SlotId);

/** @fn         SCAPI_close
 *  @brief      close a Slot
 *
 *  @param [in]  SlotId      Slot (AFE) Id.
 *
 *  @return     returns an #IccReturn_t error code
 *  @note       This function is available only for COBRA targets
 *
 */
IccReturn_t SCAPI_close(uint8_t SlotId);

/** @fn         SCAPI_write
 *  @brief      send an APDU to the card
 *
 *  @param [in]  SlotId      Slot (AFE) Id.
 *  @param [in]  pBuffer     pointer on the command buffer (APDU to send)
 *  @param [in]  length      length of the APDU to send (in bytes)
 *
 *  @return     returns an #IccReturn_t error code
 *  @note       This function is available only for COBRA targets
 *
 */
IccReturn_t SCAPI_write(uint8_t SlotId, uint8_t *pBuffer, uint16_t length);

/** @fn         SCAPI_read
 *  @brief      read the response from the card
 *
 *  @param [in]  SlotId      Slot (AFE) Id.
 *  @param [out] pBuffer     pointer on the answer buffer
 *  @param [in]  plength      pointer on the length to read
 *  @param [out] plength     pointer on the read length
 *  @return     returns an #IccReturn_t error code
 *
 */
IccReturn_t SCAPI_read(uint8_t SlotId, uint8_t *pBuffer, uint32_t *plength);

/** @fn         SCAPI_ioctl
 *  @brief      Set driver parameters and get driver state
 *
 *  @param [in]  SlotId      Slot (AFE) Id.
 *  @param [in]  control     #sc_ioctl_t request
 *  @param [in]  pparam      pointer on the parameter to set/get
 *
 *  @return     returns an #IccReturn_t error code
 *  @note       This function is available only for COBRA targets
 *
 */
IccReturn_t SCAPI_ioctl(uint8_t SlotId, sc_ioctl_t control, void *pparam);

/** @} */ /*defgroup */
/** @} */ /*defgroup */
/** @} */ /*file*/
#endif /*_SMARTCARD_API_H_*/
