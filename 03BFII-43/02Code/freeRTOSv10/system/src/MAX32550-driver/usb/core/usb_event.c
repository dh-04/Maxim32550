/*
 * @file usb_event.c USB EVENT Driver C File
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

#include <string.h>
#include <usb_event.h>
#include "mml_usb.h"

/* File Scope Data */
static maxusb_usbio_events_t events;
static usb_event_callback_t callback[MAXUSB_NUM_EVENTS];

int usb_event_enable(maxusb_event_t event, int (*func)(maxusb_event_t, void *), void *cbdata)
{
  if ( (event >= MAXUSB_NUM_EVENTS) || (func == NULL) ) {
    return -1;
  }

  callback[event].func = func;
  callback[event].cbdata = cbdata;

  return mml_usb_irq_enable(event);
}

int usb_event_disable(maxusb_event_t event)
{
  int result = -1;

  if (event >= MAXUSB_NUM_EVENTS) {
    return -1;
  }

  result = mml_usb_irq_disable(event);

  callback[event].func = NULL;
  callback[event].cbdata = NULL;

  return result;
}

int usb_event_clear(maxusb_event_t event)
{
  if (event >= MAXUSB_NUM_EVENTS) {
    return -1;
  }

  return mml_usb_irq_clear(event);
}

void usb_event_handler(void)
{
  mml_usb_irq_handler(&events);

  if (events.novbus && callback[MAXUSB_EVENT_NOVBUS].func) {
    callback[MAXUSB_EVENT_NOVBUS].func(MAXUSB_EVENT_NOVBUS, callback[MAXUSB_EVENT_NOVBUS].cbdata);
  }

  if (events.vbus && callback[MAXUSB_EVENT_VBUS].func) {
    callback[MAXUSB_EVENT_VBUS].func(MAXUSB_EVENT_VBUS, callback[MAXUSB_EVENT_VBUS].cbdata);
  }

  if (events.brst && callback[MAXUSB_EVENT_BRST].func) {
    callback[MAXUSB_EVENT_BRST].func(MAXUSB_EVENT_BRST, callback[MAXUSB_EVENT_BRST].cbdata);
  }

  if (events.brstdn && callback[MAXUSB_EVENT_BRSTDN].func) {
    callback[MAXUSB_EVENT_BRSTDN].func(MAXUSB_EVENT_BRSTDN, callback[MAXUSB_EVENT_BRSTDN].cbdata);
  }

  if (events.dpact && callback[MAXUSB_EVENT_DPACT].func) {
    callback[MAXUSB_EVENT_DPACT].func(MAXUSB_EVENT_DPACT, callback[MAXUSB_EVENT_DPACT].cbdata);
  }

  if (events.rwudn && callback[MAXUSB_EVENT_RWUDN].func) {
    callback[MAXUSB_EVENT_RWUDN].func(MAXUSB_EVENT_RWUDN, callback[MAXUSB_EVENT_RWUDN].cbdata);
  }

  if (events.bact && callback[MAXUSB_EVENT_BACT].func) {
    callback[MAXUSB_EVENT_BACT].func(MAXUSB_EVENT_BACT, callback[MAXUSB_EVENT_BACT].cbdata);
  }

  if (events.susp && callback[MAXUSB_EVENT_SUSP].func) {
    callback[MAXUSB_EVENT_SUSP].func(MAXUSB_EVENT_SUSP, callback[MAXUSB_EVENT_SUSP].cbdata);
  }

  if (events.sudav && callback[MAXUSB_EVENT_SUDAV].func) {
    callback[MAXUSB_EVENT_SUDAV].func(MAXUSB_EVENT_SUDAV, callback[MAXUSB_EVENT_SUDAV].cbdata);
  }
}
