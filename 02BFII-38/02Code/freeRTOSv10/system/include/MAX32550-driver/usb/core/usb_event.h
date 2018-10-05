/*
 * @file usb_event.h USB_EVENT Driver Header File
 * @brief Defines the API used for USB event handling
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

/*
 * Low-layer API calls
 *
 * These do not change, and provide the basis by which usb.c acceses the 
 *  hardware. All usbio.c drivers will provide these calls, or return an
 *  error if the function is not supported.
 * 
 */

#ifndef _USB_EVENT_H_
#define _USB_EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup USB_EVENT USB EVENT Driver
 *
 * @ingroup USB_HWOPT
 *
 * @{
 */

#include "mml_usb.h"

/***** Definitions *****/
typedef struct {
  int (*func)(maxusb_event_t, void *);
  void *cbdata;
} usb_event_callback_t;

/** 
 * @brief Register a callback for and enable the specified event
 * @param event   event number
 * @param func    function to be called
 * @param cbdata  parameter to call callback function with
 * @return This function returns zero (0) for success, non-zero for failure
 */
int usb_event_enable(maxusb_event_t event, int (*callback)(maxusb_event_t, void *), void *cbdata);

/** 
 * @brief Enable the specified event
 * @param event   event number
 * @return This function returns zero (0) for success, non-zero for failure
 */
int usb_event_disable(maxusb_event_t event);

/** 
 * @brief Clear the specified event
 * @param event   event number
 * @return This function returns zero (0) for success, non-zero for failure
 */
int usb_event_clear(maxusb_event_t event);

/**
 * @brief Processes USB events
 * This function should be called from the USB interrupt vector or periodically
 * from the application.
 */
void usb_event_handler(void);

/** @} *//* @defgroup USB_EVENT */

#ifdef __cplusplus
}
#endif

#endif /* _USB_EVENT_H_ */
