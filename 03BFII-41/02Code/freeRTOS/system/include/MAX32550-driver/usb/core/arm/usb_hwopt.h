/*
 * @file usb_hwopt.h MML USB_HWOPT Driver Header File
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

#ifndef _USBIO_HWOPT_H_
#define _USBIO_HWOPT_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup USB_HWOPT USB Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#include "MAX325xx.h"

/* There are no configuration options for this target */
typedef void usb_cfg_options_t;

/**
 * @brief Put the transceiver into a low power state.
 */
static inline void usb_sleep(void)
{
   USB->DEV_CN |= USB_DEV_CN_ULPM_Msk;
}

/**
 * @brief Power up the USB transceiver, must be called once the device wakes from sleep.
 */
static inline void usb_wakeup(void)
{
   USB->DEV_CN &= ~USB_DEV_CN_ULPM_Msk;
}

/**
 * @brief Send a remote wakeup signal to the host.
 */
static inline void usb_remote_wakeup(void)
{
   USB->DEV_CN |= USB_DEV_CN_SIGRWU_Msk;
}

/** @} *//* @defgroup USB_HWOPT */

#ifdef __cplusplus
}
#endif

#endif /* _USB_HWOPT_H_ */
