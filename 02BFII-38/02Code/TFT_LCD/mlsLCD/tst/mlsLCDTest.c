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
 * @defgroup mlsLcd.c   mlsLcdFont.c
 * @brief mlsLcdFont.c
 *
 * Add more details about module
 * @{
 */

/**
 * @file    mlsLcdFont.c
 *
 * @date    Octo 16, 2017
 * @author  stylvn008
 */
/********** Include section ***************************************************/
#include "mlsLCDDriver.h"
/********** Local Constant and compile switch definition section **************/
#include <MAX325xx.h>
#include "mlsCalibri15pts.h"

/********** Local Type definition section *************************************/

/********** Local Macro definition section ************************************/

/********** Global variable definition section ********************************/
/********** Local (static) variable definition section ************************/

/********** Local (static) function declaration section ***********************/

/********** Local function definition section *********************************/

/********** Global function definition section ********************************/

mlsErrorCode_t mlsLCDTestMain()
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;

	mlsLCDDrawScreen(LCD_WHITE);
	mlsOsalDelayMs(1000);
	mlsLcd_LoadString(1, "Truong cute :D :D :D");
	mlsLcd_LoadString(2, "Truong cute :D :D :D");
	mlsLcd_DisplayString(1);
	mlsLcd_DisplayString(2);
	mlsLcd_DisplayString(4);
	mlsOsalDelayMs(5000);

	return errorCode;
}
