/*
 * @file enumerate.h USB_ENUM Driver Header File
 * @brief USB Device Enumeration Routines
 *
 ******************************************************************************
 * Copyright (C) 2016-2018 Maxim Integrated Products, Inc., All rights Reserved.
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

#ifndef _ENUMERATE_H_
#define _ENUMERATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <usb_protocol.h>

/** @defgroup ENUM ENUM Driver
 *
 * @ingroup USB_HWOPT
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define ENUM_VERS_MAJOR	<VERSMAJ>
#define ENUM_VERS_MINOR	<VERSMIN>
#define ENUM_VERS_PATCH	<VERSPAT>
#define ENUM_VERSION_STRING	"v" xstr(ENUM_VERS_MAJOR) "." xstr(ENUM_VERS_MINOR) "." xstr(ENUM_VERS_PATCH)

/// User can register callbacks for various control endpoint requests
typedef enum {
  ENUM_CLASS_REQ,
  ENUM_VENDOR_REQ,
  ENUM_SETCONFIG,
  ENUM_SETINTERFACE,
  ENUM_GETINTERFACE,
  ENUM_SETFEATURE,
  ENUM_CLRFEATURE,
  ENUM_NUM_CALLBACKS
} enum_callback_t;

/// User also can register device, config, and string descriptors
typedef enum {
  ENUM_DESC_DEVICE = 0, /// index qualifier ignored
  ENUM_DESC_CONFIG = 1, /// index qualifier ignored
  ENUM_DESC_STRING = 2, /// index is used to futher qualify string descriptor
  ENUM_NUM_DESCRIPTORS
} enum_descriptor_t;

/**
 * Return the version of the USB ENUM driver.
 *
 */
const char * enum_get_version(void);

/**
 *  \brief    Initialize the enumeration module
 *  \details  Initialize the enumeration module
 *  \return   Zero (0) for success, non-zero for failure
 */
int enum_init(void);

/**
 *  \brief    Register a descriptor
 *  \details  Register a descriptor
 *  \param    type    type of descriptor being registered
 *  \param    desc    pointer to the descriptor
 *  \param    index   index of the string descriptor. ignored for other descriptor types
 *  \return   Zero (0) for success, non-zero for failure
 */
int enum_register_descriptor(enum_descriptor_t type, const uint8_t *desc, uint8_t index);

/**
 *  \brief    Register an enumeration event callback
 *  \details  Register an enumeration event callback
 *  \param    type    event upon which the callback will occur
 *  \param    func    function to be called
 *  \param    cbdata   data to be passed to the callback function
 *  \return   Zero (0) for success, non-zero for failure
 */
int enum_register_callback(enum_callback_t type, int (*func)(usb_setup_pkt *sup, void *cbdata), void *cbdata);

/**
 *  \brief    Register a handler for device class descriptors
 *  \details  Register a handler for devuce class descriptors. The handler is used to respond to device class
 *            get descriptor requests from the host. The handler function shall update desc and desclen with
 *            a pointer to the descriptor and its length.
 *  \param    func    function to be called
 *  \return   Zero (0) for success, non-zero for failure
 */
int enum_register_getdescriptor(void (*func)(usb_setup_pkt *sup, const uint8_t **desc, uint16_t *desclen));

/**
 *  \brief    Gets the current configuration value.
 *  \details  Gets the current configuration value.
 *  \return   The current configuration value.
 */
uint8_t enum_getconfig(void);

/**
 *  \brief    Clears the configuration value.
 *  \details  Clears the configuration value. This function should be called when a host disconnect is detected.
 */
void enum_clearconfig(void);

/** @} *//* @defgroup MML_USB_ENUM */

#ifdef __cplusplus
}

#endif

#endif //MML_USB_
