/*
 * @file touch_screen.c
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

/** Global includes */
#include <MAX325xx.h>
#include <errors.h>
#include <stdio.h>
/** Other includes */
#include "mml_gpio.h"
#include "mml_i2c.h"
/** Local includes */
#include "touch_screen.h"
#include <string.h>

//#pragma GCC optimize ("O0")


static const unsigned char touch_base[] = { 0x00, 0x03, 0x09, 0x0F, 0x15, 0x1B };
static TS_Buttons_t ts_buttons[TS_MAX_BUTTONS];
static int ts_button_index = 0;
static int is_tp_pressed = 0;

/******************************************************************************/
static void test_IRQHandler(void *params)
{
	(void) params;

	is_tp_pressed = 1;
	ts_stop();
	NVIC_ClearPendingIRQ(GPIO1_IRQn);
}

static int read_reg(unsigned char addr, unsigned char * data)
{
  int err;
  unsigned int len = 1;
  unsigned int bus_status;
  int counter = 10000;

  // Write the register address to read
  if ((err = mml_i2c_write_start(MML_I2C_DEV0, TS_SLAVE_ADDR, &addr, &len)) != NO_ERROR) {
    return err;
  }

  // Read the register
  if ((err = mml_i2c_ioctl(MML_I2C_DEV0, MML_I2C_SET_READCOUNT, &len)) != NO_ERROR) {
    return err;
  }
  if ((err = mml_i2c_read_start(MML_I2C_DEV0, TS_SLAVE_ADDR)) != NO_ERROR) {
    return err;
  }

  // Wait for the read to complete, and get the data
  do {
    mml_i2c_bus_status(MML_I2C_DEV0, &bus_status);
    if (--counter == 0) {
    	break; // timeout
    }
  } while ( (bus_status & I2C_SR_RXEMPTY_Msk) || (bus_status & I2C_SR_BUSY_Msk) );
  if ((err = mml_i2c_read(MML_I2C_DEV0, data, &len)) != NO_ERROR) {
    return err;
  }
  if (len != 1) {
    return COMMON_ERR_UNKNOWN;
  }

  return NO_ERROR;
}

static int write_reg(unsigned char addr, unsigned char data)
{
  int err;
  unsigned char msg[2] = { addr, data };
  unsigned int len = 2;

  // Write the register address to read
  if ((err = mml_i2c_write_start(MML_I2C_DEV0, TS_SLAVE_ADDR, msg, &len)) != NO_ERROR) {
    return err;
  }

  return NO_ERROR;
}

static int read_touch(unsigned int index, unsigned char data[4])
{
  int err;
  unsigned int len;
  unsigned int bus_status;

  // Write the register address to read
  len = 1;
  if ((err = mml_i2c_write_start(MML_I2C_DEV0, TS_SLAVE_ADDR, &touch_base[index], &len)) != NO_ERROR) {
    return err;
  }

  // Read the registers
  len = 4;
  if ((err = mml_i2c_ioctl(MML_I2C_DEV0, MML_I2C_SET_READCOUNT, &len)) != NO_ERROR) {
    return err;
  }
  if ((err = mml_i2c_read_start(MML_I2C_DEV0, TS_SLAVE_ADDR)) != NO_ERROR) {
    return err;
  }

  // Wait for the read to complete, and get the data
  do {
    mml_i2c_bus_status(MML_I2C_DEV0, &bus_status);
  } while ( (bus_status & I2C_SR_RXEMPTY_Msk) || (bus_status & I2C_SR_BUSY_Msk) );
  if ((err = mml_i2c_read(MML_I2C_DEV0, data, &len)) != NO_ERROR) {
    return err;
  }
  if (len != 4) {
    return COMMON_ERR_UNKNOWN;
  }

  return NO_ERROR;
}

static int is_inBox(int x, int y, int x0, int y0, int x1, int y1)
{
	if ( (x >= x0) && (x <= x1) && (y >= y0) && (y <= y1) ) {
    	return 1;
    }
    return 0;
}

/******************************************************************************/
int ts_init(void)
{
  int err;
  unsigned char temp;
  mml_i2c_config_t i2c_config;
  mml_gpio_config_t gpio_config;

  // Initialize I2C
  i2c_config.baudrate = 400000;
  i2c_config.rxfifo_thr = 1;
  i2c_config.flags = MML_I2C_AUTOSTART_ENABLE;
  //i2c_config.irq_handler = (irq_handler_t)I2C_IRQHandler; // just not to provide null pointer
  i2c_config.irq_handler = NULL;
  if ((err = mml_i2c_init(MML_I2C_DEV0, &i2c_config)) != NO_ERROR) {
    return err;
  }

  // Configure touch screen "interrupt" input
  gpio_config.gpio_direction = MML_GPIO_DIR_IN;
  gpio_config.gpio_function = MML_GPIO_NORMAL_FUNCTION;
  gpio_config.gpio_pad_config = MML_GPIO_PAD_PULLUP;
  gpio_config.gpio_intr_mode 	= 0;
  gpio_config.gpio_intr_polarity = 0;
  err = mml_gpio_init(TS_INT_GPIO_DEV, TS_INT_GPIO_PIN, 1, gpio_config);
  if (err != NO_ERROR) {
    return err;
  }

  // Configure wake output
  gpio_config.gpio_direction = MML_GPIO_DIR_OUT;
  gpio_config.gpio_function = MML_GPIO_NORMAL_FUNCTION;
  gpio_config.gpio_pad_config = MML_GPIO_PAD_NORMAL;
  gpio_config.gpio_intr_mode 	= 0;
  gpio_config.gpio_intr_polarity = 0;
  err = mml_gpio_init(TS_WAKE_GPIO_DEV, TS_WAKE_GPIO_PIN, 1, gpio_config);
  if (err != NO_ERROR) {
    return err;
  }
  mml_gpio_write_bit_pattern(TS_WAKE_GPIO_DEV, TS_WAKE_GPIO_PIN, 1, 1);

  // Check for error
  if ((err = read_reg(REG_ERR, &temp)) != NO_ERROR) {
    return err;
  }

  if (temp != 0x00) {
    return COMMON_ERR_UNKNOWN;
  }

  // Enable touch screen "interrupt"
  if ((err = write_reg(REG_MODE, 1)) != NO_ERROR) {
    return err;
  }

  ts_button_index = 0;

  return NO_ERROR;
}

void ts_start(void)
{
	mml_gpio_attach_handler(TS_INT_GPIO_DEV, TS_INT_GPIO_PIN, &test_IRQHandler, 0);
    NVIC_SetPriority(GPIO1_IRQn, MML_INTC_PRIO_5);
    NVIC_EnableIRQ(GPIO1_IRQn);

    mml_gpio_enable_interrupt(TS_INT_GPIO_DEV, TS_INT_GPIO_PIN, 1);
    // Pass into interrup mode
    write_reg(REG_MODE, 1);
}

void ts_stop(void)
{
    mml_gpio_disable_interrupt(TS_INT_GPIO_DEV, TS_INT_GPIO_PIN, 1);
    // Pass into polling mode
    write_reg(REG_MODE, 0);

}

int ts_set_mode(ts_mode_e mode)
{
  unsigned int pinstate;
  int err;

  // Check for hibernation
  if ((err = mml_gpio_read_bit_pattern(TS_WAKE_GPIO_DEV, TS_WAKE_GPIO_PIN, 1, &pinstate)) != NO_ERROR) {
    return err;
  }

  switch (mode) {
    case TS_MODE_ACTIVE:
      if (!pinstate) {
        mml_gpio_write_bit_pattern(TS_WAKE_GPIO_DEV, TS_WAKE_GPIO_PIN, 1, 1);
      }
      if ((err = write_reg(REG_PMODE, PMODE_ACTIVE)) != NO_ERROR) {
        return err;
      }
      break;
    case TS_MODE_MONITOR:
      if (!pinstate) {
        mml_gpio_write_bit_pattern(TS_WAKE_GPIO_DEV, TS_WAKE_GPIO_PIN, 1, 1);
      }
      if ((err = write_reg(REG_PMODE, PMODE_MONITOR)) != NO_ERROR) {
        return err;
      }
      break;
    case TS_MODE_HIBERNATE:
      mml_gpio_write_bit_pattern(TS_WAKE_GPIO_DEV, TS_WAKE_GPIO_PIN, 1, 0);
      break;
    default:
      return COMMON_ERR_INVAL;
  }

  return NO_ERROR;
}

int ts_get_touches(ts_touches_t * touches)
{
  int err;
//  unsigned int pinstate;
  unsigned char pts, temp, data[4];
  int pt;

  if (touches == NULL) {
    return COMMON_ERR_NULL_PTR;
  }

  // Clear touch data to 0
  memset(touches, 0, sizeof(ts_touches_t));


  // Check for error
  if ((err = read_reg(REG_ERR, &temp)) != NO_ERROR) {
    return err;
  }
  if (temp != 0x00) {
    return COMMON_ERR_UNKNOWN;
  }

  // Get number of points
  if ((err = read_reg(REG_TOUCH_PTS, &pts)) != NO_ERROR) {
    return err;
  }
  if (pts > 5) {
    return COMMON_ERR_INVAL;
  }
  touches->num_points = pts;

  // Get gesture
  if ((err = read_reg(REG_GESTURE_ID, &temp)) != NO_ERROR) {
    return err;
  }
  touches->gesture = (ts_gesture_e)temp;

  // Get all of the touches reported
  for (pt = 1; pt <= pts; pt++) {
    // Get touch information
    if ((err = read_touch(pt, data)) != NO_ERROR) {
      return err;
    }

    touches->points[pt-1].x = (((unsigned int)data[0] & 0xF) << 8) + data[1];
    touches->points[pt-1].y = (((unsigned int)data[2] & 0xF) << 8) + data[3];
    touches->points[pt-1].event = (ts_event_e)(data[0] >> 6);
  }

  return NO_ERROR;
}

int ts_register_button(int x0, int y0, int x1, int y1, int on_press_expected_code)
{

	if (ts_button_index >= TS_MAX_BUTTONS) {
		return -1;
	}

	ts_buttons[ts_button_index].x0 = x0;
	ts_buttons[ts_button_index].y0 = y0;
	ts_buttons[ts_button_index].x1 = x1;
	ts_buttons[ts_button_index].y1 = y1;
	ts_buttons[ts_button_index].key_code = on_press_expected_code;
	++ts_button_index;

    return 0;
}

int ts_clr_buttons(void)
{
	ts_button_index = 0;
    return 0;
}

int ts_get_key(void)
{
    ts_touches_t touches;
    unsigned int pt, nb_touches;
    int key = 0;
    int i;


    if (is_tp_pressed == 0) {
    	return 0;
    }
    is_tp_pressed = 0;

	// Check for touch events
	if (ts_get_touches(&touches) == NO_ERROR) {
		// I Take in count the first PRS event
		nb_touches = touches.num_points;

		if (nb_touches !=  0) {
			for (pt = 0; pt < touches.num_points; pt++) {
				if ( (touches.points[pt].event == TS_PRESS) || (touches.points[pt].event == TS_CONTACT) ) {
					printf("touches.num_points: %d  x:%d - y:%d\n", touches.num_points, touches.points[pt].x, touches.points[pt].y);
					for (i = 0; i < ts_button_index; i++) {
						if ( is_inBox(touches.points[pt].x, touches.points[pt].y, ts_buttons[i].x0, ts_buttons[i].y0, ts_buttons[i].x1, ts_buttons[i].y1) ) {
							key = ts_buttons[i].key_code;
							break;
						}
					}
					// Pass into interrupt mode
					write_reg(REG_MODE, 1);

					if (key > 0) {
						break;
					}
				}
			}

			// probably need to wait release :)
			do {
				ts_get_touches(&touches);
				printf("touches.num_points: %d - %d \n", touches.num_points, nb_touches);
			} while (nb_touches == touches.num_points);
		}
	}

	ts_start();

	return key;
}

