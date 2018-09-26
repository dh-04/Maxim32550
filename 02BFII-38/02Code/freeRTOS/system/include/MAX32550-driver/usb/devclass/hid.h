/*
 * @file hid.h HID Driver Header File
 * @brief Human Interface Device Class.
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

#ifndef _HID_H_
#define _HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup HID HID Driver
 *
 * @ingroup USB_HWOPT
 *
 * @{
 */

#define HID_MAX_PACKET 	  64

/// USB HID class requests
#define HID_GET_REPORT    0x01
#define HID_GET_IDLE      0x02
#define HID_GET_PROTOCOL  0x03
#define HID_SET_REPORT    0x09
#define HID_SET_IDLE      0x0a
#define HID_SET_PROTOCOL  0x0b

/// Class-specific descriptor types for GET_DESCRIPTOR
#define DESC_HID          0x21
#define DESC_REPORT       0x22

/// HID Descriptor
#if defined(__GNUC__)
typedef struct __attribute__((packed)) {
#else
typedef __packed struct {
#endif
  uint8_t  bFunctionalLength;
  uint8_t  bDescriptorType;
  uint16_t bcdHID;
  uint8_t  bCountryCode;
  uint8_t  bNumDescriptors;
  uint8_t  bHIDDescriptorType;
  uint16_t wDescriptorLength;
} hid_descriptor_t;

/** @} *//* @defgroup HID */

#ifdef __cplusplus
}
#endif

#endif /* _HID_H_ */
