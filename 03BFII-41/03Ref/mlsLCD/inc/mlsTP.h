/**
 * @defgroup mlsLCD
 * @brief
 *
 * Touch Panel I2C library
 * @{
 */

/**
 * @file	mlsTP.h
 *
 * @date 	Jan 15, 2018
 * @author	huynh
 */

#ifndef MLSTP_H_
#define MLSTP_H_

/********** Include section ***************************************************/
#include "mlsInclude.h"
#include <mml_i2c.h>
#include <mml_gpio.h>
#include "../mlsOsal/inc/mlsOsal.h"

/********** Constant  and compile switch definition section *******************/
#define TP_HW_FT6206			1
#define TP_HW_ST1633i			0

#if (				\
TP_HW_FT6206+ 	\
TP_HW_ST1633i 	\
!= 1)
#error "Only support one item at same time!"
#endif

#define I2C_BAUDRATE		100000
#define CPT_PORT			MML_GPIO_DEV0
#define CPT_RESET			10
#define CPT_IRQ				11
#define CPT_SDA_PIN			23
#define CPT_SCL_PIN			24

#define TP_XRES				480
#define TP_YRES				320
#define TFT_XRES			320
#define TFT_YRES			240

#if (TP_HW_FT6206 == 1)

#define SLAVE_ADDRESS   	0x70
#define FT6206_DEV_MODE		0x00
#define FT6206_GEST_ID		0x01
#define FT6206_TD_STAT		0x02

#define FT6206_TOUCH1_XH	0x03
#define FT6206_TOUCH1_XL	0x04
#define FT6206_TOUCH1_YH	0x05
#define FT6206_TOUCH1_YL	0x06
#define FT6206_TOUCH1_Z		0x07
#define FT6206_TOUCH1_AREA	0x08

#define FT6206_TOUCH2_XH	0x09
#define FT6206_TOUCH2_XL	0x0A
#define FT6206_TOUCH2_YH	0x0B
#define FT6206_TOUCH2_YL	0x0C
#define FT6206_TOUCH2_Z		0x0D
#define FT6206_TOUCH2_AREA	0x0E

#define FT6206_EV_FLAG_MASK	0xC0
#define FT6206_POS_MSB_MASK	0x0F
#define FT6206_TOUCH_ID_MSK	0xF0
#define FT6206_AREA_MASK	0xF0

#define FT6206_PANEL_ID		0xA8

#elif (TP_HW_ST1633i == 1)

#define SLAVE_ADDRESS   	0x55
#define	TOUCH_FW_VERSION	0x00
#define	TOUCH_STATUS		0x01
#define	TOUCH_DEV_CTL		0x02
#define	TOUCH_TIMEOUT		0x03
#define	TOUCH_XYRES_HIGH	0x04
#define	TOUCH_XRES_LOW		0x05
#define	TOUCH_YRES_LOW		0x06
#define	TOUCH_SENS_CNT_H	0x07
#define	TOUCH_SENS_CNT_L	0x08
#define	TOUCH_KEYS			0x11
#define	TOUCH_XYCOORD_H		0x12
#define	TOUCH_XCOORD_L		0x13
#define	TOUCH_YCOORD_L		0x14
#define	TOUCH_MAX_CONTACT	0x3F
#define	TOUCH_SMART_WAKE_UP	0xF0
#define	TOUCH_PAGE			0xFF

#endif

/********** Type definition section *******************************************/
typedef enum
{
	PRESS_DOWN = 0b00,
	LIFT_UP = 0b01,
	CONTACT = 0b10,
	NO_EVENT = 0b11,
}mlsTouchEvFlag_t;

typedef struct mlsTouchCoord_t
{
	UInt8 x_HB;
	UInt8 x_LB;
	UInt8 y_HB;
	UInt8 y_LB;
}mlsTouchCoord_t;

typedef struct mlsTouchData_t
{
	UInt16 x1, y1;
	UInt8 z1, area1;
	UInt16 x2, y2;
	UInt8 z2, area2;
	UInt8 gestID;
	UInt8 numOfPoints;
	mlsTouchEvFlag_t evFlag;
	UInt8 devMode;
}mlsTouchData_t;

typedef struct {
	uint8_t y_h: 3,
	reserved: 1,
	x_h: 3,
	valid: 1;
} xy_highbyte_t;

typedef struct {
	xy_highbyte_t xy_highbyte;
	uint8_t x_lowbyte;
	uint8_t y_lowbyte;
} stx_report_data_t;

/********** Macro definition section*******************************************/

/********** Variable definition section ***************************************/

/********** Function declaration section **************************************/
/**
 * @fn      mlsTP_init
 * @brief   Initialize touch I2C module
 * @param   param is pointer to touch data output
 * @Return  error code
 */
mlsErrorCode_t mlsTP_init(Void *param);
mlsErrorCode_t mlsTP_Reset(void);
mlsErrorCode_t mlsTouchLCDTestMain();
mlsErrorCode_t mlsTP_SetReg(uint8_t reg, uint8_t value);
mlsErrorCode_t mlsTP_GetReg(uint8_t reg, uint8_t *result, unsigned int length);

#endif /* MLSTP_H_ */
