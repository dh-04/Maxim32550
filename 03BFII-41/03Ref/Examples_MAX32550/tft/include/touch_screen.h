/*
 * touch_screen.h -- Driver for FocalTech FT5x06 touch screen controller.
 *
 * ----------------------------------------------------------------------------
 * Copyright (c) 2018, Maxim Integrated Products
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MAXIM INTEGRATED PRODUCTS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL MAXIM INTEGRATED PRODUCTS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
* \file
* \brief    Touch Screen Driver
* \details  Driver for FocalTech FT5x06 touch screen controller.
* \details  This file defines the driver API including data types and function
*           prototypes.
*/

#ifndef _TOUCH_SCREEN_H_
#define _TOUCH_SCREEN_H_

#include <stdint.h>

#define TS_SLAVE_ADDR   (0x70)

#define REG_DEV_MODE    (0x00)
#define REG_GESTURE_ID  (0x01)
#define REG_TOUCH_PTS   (0x02)

#define REG_TOUCH_BASE  (0x03)
#define XH_OFST         (0x0)
#define XL_OFST         (0x1)
#define YH_OFST         (0x2)
#define YL_OFST         (0x3)

#define REG_MODE        (0xA4)
#define REG_PMODE       (0xA5)
#define REG_FIRMID      (0xA6)
#define REG_ERR         (0xA9)

#define PMODE_ACTIVE    (0x00)
#define PMODE_MONITOR   (0x01)
#define PMODE_HIBERNATE (0x03)

/***** Definitions *****/
/// Max number of points reported by the touch screen
#define TS_NUM_POINTS   5

/// INT pin's GPIO device
#define TS_INT_GPIO_DEV    MML_GPIO_DEV1
/// INT pin's GPIO pin
#define TS_INT_GPIO_PIN    (30)

/// WAKE pin's GPIO device
#define TS_WAKE_GPIO_DEV   MML_GPIO_DEV1
/// WAKE pin's GPIO pin
#define TS_WAKE_GPIO_PIN   (31)

#define TS_MAX_BUTTONS  16

typedef struct _TS_Buttons_t {
	int x0;
	int y0;
	int x1;
	int y1;
	int key_code;
} TS_Buttons_t;

/// Touch screen modes. See datasheet for details.
typedef enum {
  TS_MODE_INVALID = 0,
  TS_MODE_ACTIVE,
  TS_MODE_MONITOR,
  TS_MODE_HIBERNATE,
} ts_mode_e;

/// Touch event types
typedef enum {
  TS_PRESS    = 0,
  TS_RELEASE  = 1,
  TS_CONTACT  = 2
} ts_event_e;

/// Gestures
typedef enum {
  TS_NO_GESTURE = 0x00,
  TS_ZOOM_IN    = 0x48,
  TS_ZOOM_OUT   = 0x49
} ts_gesture_e;

/// Event information
typedef struct {
  ts_event_e event;
  uint16_t x;
  uint16_t y;
} ts_touch_t;

/// Information from all touch points
typedef struct {
  ts_touch_t points[TS_NUM_POINTS];
  unsigned int num_points;
  ts_gesture_e gesture;
} ts_touches_t;

/***** Function Prototypes *****/

/**
*   \brief    Initialize the touch screen controller.
*   \details  Initializes the touch screen controller including the I2C
*             communication interface and GPIO control pins.
*   \returns  #NO_ERROR if successful
*/
int ts_init(void);

/**
*   \brief    Sets the touch screen controller operation mode.
*   \details  See datasheet for operating mode details.
*   \param    mode    desired operation mode
*   \returns  #NO_ERROR if successful
*/
int ts_set_mode(ts_mode_e mode);

/**
*   \brief    Retrieves touch screen events from the controller.
*   \param    mode    desired operation mode
*   \returns  #NO_ERROR if successful
*/
int ts_get_touches(ts_touches_t * touches);

void ts_start(void);
void ts_stop(void);

int ts_get_key(void);
int ts_register_button(int x0, int y0, int x1, int y1, int on_press_expected_code);
int ts_clr_buttons(void);

#endif /* _TOUCH_SCREEN_H_ */

