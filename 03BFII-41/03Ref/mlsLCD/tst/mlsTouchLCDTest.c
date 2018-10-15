/*******************************************************************************
 *  (C) Copyright 2009 Molisys Solutions Co., Ltd. , All rights reserved       *
 *                                                                             *
 *  This source code and any compilation or derivative thereof is the sole     *
 *  property of Molisys Solutions Co., Ltd. and is provided pursuant to a      *
 *  Software License Agreement.  This code is the proprietary information      *
 *  of Molisys Solutions Co., Ltd and is confidential in nature.  Its use and  *
 *  dissemination by any party other than Molisys Solutions Co., Ltd is        *
 *  strictly limited by the confidential information provisions of the         *
 *  Agreement referenced above.                                                *
 ******************************************************************************/

/**
 * @defgroup mlsLcd.c   mlsTouchLCDTest.c
 * @brief mlsTouchLCDTest.c
 *
 * Add more details about module
 * @{
 */

/**
 * @file    mlsTouchLCDTest.c
 *
 * @date    Jan 16, 2018
 * @author  huynh
 */
/********** Include section ***************************************************/
#include "../mlsLCD/inc/mlsLCDDriver.h"
#include "../mlsLCD/inc/mlsTP.h"

/********** Local Constant and compile switch definition section **************/

/********** Local Type definition section *************************************/

/********** Local Macro definition section ************************************/

/********** Global variable definition section ********************************/
mlsTouchData_t gTouchData;

/********** Local (static) variable definition section ************************/

/********** Local (static) function declaration section ***********************/
extern mlsErrorCode_t mlsLCDDrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
extern mlsErrorCode_t mlsLCDDrawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);

/********** Local function definition section *********************************/

/********** Global function definition section ********************************/

mlsErrorCode_t mlsTouchLCDTestMain()
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;
#if (TP_HW_ST1633i == 1)
	uint8_t value;
	char buff[20] = {0};
#endif
	mlsLCDDrawScreen(LCD_BLACK);
	mlsLCDDrawScreen(LCD_BLUE);
	mlsLCDDrawScreen(LCD_GREEN);
	mlsLCDDrawScreen(LCD_WHITE);
	mlsLcd_LoadString(LINE1,"STYL SOLUTIONS");

	//Initialize Touch module
	errorCode = mlsTP_init((Void *)&gTouchData);
	if (errorCode != MLS_SUCCESS)
	{
		return errorCode;
	}

#if (TP_HW_ST1633i == 1)
	//Get firmware version
	mlsTP_GetReg(TOUCH_FW_VERSION, &value, 1);
	lite_sprintf(&buff[0], "Touch FW version: %d",value);
	mlsLcd_LoadString(LINE2, buff);

	value = 0x03;
	//Set 3s timeout to Idle mode
	mlsTP_SetReg(TOUCH_TIMEOUT, value);
	//Get timeout value
	mlsTP_GetReg(TOUCH_TIMEOUT, &value, 1);
	lite_sprintf(&buff[0], "Time to Idle: %ds",value);
	mlsLcd_LoadString(LINE3, buff);
	mlsLcd_DisplayString(LINE1 | LINE2 | LINE3 | LINE4);
#endif

	//Clear screen button
	mlsLCDDrawFilledRectangle(300,0,320,20,LCD_RED);

	while(1)
	{
		if ((gTouchData.x1 > 300) && (gTouchData.y1 < 20))
		{
			mlsLCDDrawScreen(LCD_WHITE);
			mlsLCDDrawFilledRectangle(300,0,320,20,LCD_RED);
		}
		mlsLCDDrawCircle(gTouchData.x1, gTouchData.y1, 2, LCD_BLUE);
		mlsOsalDelayMs(10);
	}

	return errorCode;
}


