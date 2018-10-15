/**
 * @addtogroup mlsLCD
 * @{
 */

/**
 * @file	mlsTP.c
 *
 * @date 	Jan 15, 2018
 * @author	huynh
 */

/********** Include section ***************************************************/
#include "../inc/mlsTP.h"
#include "../../nxp/NxpNfcRdLib/intfs/phbalReg.h"
#include "../../nxp/NxpNfcRdLib/types/ph_Status.h"
#include "../../nxp/NxpNfcRdLib/types/ph_Status.h"
#include "../../nxp/NxpNfcRdLib/types/ph_TypeDefs.h"
#include "../mlsOsal/inc/mlsOsal.h"
#include "../mlsDebug/inc/mlsDebug.h"

/********** Local Constant and compile switch definition section **************/

/********** Local Type definition section *************************************/

/********** Local Macro definition section ************************************/

/********** Local (static) variable definition section ************************/
static mlsTouchData_t *touchData = Null;
struct mlsTouchCoord_t touchCoord = {0};
//static UInt8 temp_data = 0;

/********** Global variable definition section ********************************/
extern mlsMutexHandle_t gMaximI2CMutex;

/********** Local (static) function declaration section ***********************/
static void CPT_IRQHandler();

/********** Local function definition section *********************************/
static void CPT_IRQHandler()
{
#if (TP_HW_FT6206 == 1)
	mlsTP_GetReg(FT6206_TOUCH1_XH, &touchCoord.x_HB, 1);
	mlsTP_GetReg(FT6206_TOUCH1_XL, &touchCoord.x_LB, 1);
	mlsTP_GetReg(FT6206_TOUCH1_YH, &touchCoord.y_HB, 1);
	mlsTP_GetReg(FT6206_TOUCH1_YL, &touchCoord.y_LB, 1);
	touchData->y1 = TFT_YRES - ((((UInt16)touchCoord.x_HB & FT6206_POS_MSB_MASK) << 8) | touchCoord.x_LB) * TFT_YRES/TP_YRES;
	touchData->x1 = ((((UInt16)touchCoord.y_HB & FT6206_POS_MSB_MASK) << 8) | touchCoord.y_LB) * TFT_XRES/TP_XRES;

#elif (TP_HW_ST1633i == 1)
	// for ST1633i 2.8" TP 320x240, origin top right
	stx_report_data_t data;
	uint8_t i;

	for(i=0; i<5; i++)
	{
		mlsTP_GetReg(TOUCH_XYCOORD_H, (uint8_t *)&data.xy_highbyte, 1);
		if(data.xy_highbyte.valid & 0x01)
		{
			touchData->evFlag = PRESS_DOWN;
			mlsTP_GetReg(TOUCH_XCOORD_L, &data.x_lowbyte, 1);
			mlsTP_GetReg(TOUCH_YCOORD_L, &data.y_lowbyte, 1);
			touchData->y1 = ((((uint16_t)data.xy_highbyte.x_h) << 8) | data.x_lowbyte) * TFT_XRES/TP_XRES;
			touchData->x1 = TFT_YRES - ((((uint16_t)data.xy_highbyte.y_h) << 8) | data.y_lowbyte) * TFT_YRES/TP_YRES;
		}
		else
		{
			touchData->evFlag = LIFT_UP;
			touchData->x1 = 0;
			touchData->y1 = 0;
		}
	}
#endif

	INT_GPIO_CLEAR(CPT_PORT, CPT_IRQ);
}

/********** Global function definition section ********************************/
mlsErrorCode_t mlsTP_init(Void *param)
{
	mlsErrorCode_t errCode = MLS_SUCCESS;

	touchData = (mlsTouchData_t *)param;

	mml_gpio_config_t gpio_config_out;
	/** Fill parameters */
	gpio_config_out.gpio_direction 		= MML_GPIO_DIR_OUT;
	gpio_config_out.gpio_function 		= MML_GPIO_NORMAL_FUNCTION;
	gpio_config_out.gpio_intr_mode 		= 0;
	gpio_config_out.gpio_intr_polarity 	= 0;
	gpio_config_out.gpio_pad_config 	= MML_GPIO_PAD_NORMAL;
	/** Initialize */
	errCode = mml_gpio_init(CPT_PORT, CPT_RESET, 1, gpio_config_out);
	if (errCode != MLS_SUCCESS)
	{
		return errCode;
	}

	mml_gpio_config_t gpio_config_in;
	/** Fill parameters */
	gpio_config_in.gpio_direction 		= MML_GPIO_DIR_IN;
	gpio_config_in.gpio_function 		= MML_GPIO_NORMAL_FUNCTION;
	gpio_config_in.gpio_intr_mode 		= MML_GPIO_INT_MODE_EDGE_TRIGGERED;
	gpio_config_in.gpio_intr_polarity 	= MML_GPIO_INT_POL_RAISING;
	gpio_config_in.gpio_pad_config 		= MML_GPIO_PAD_NORMAL;
	/** Initialize */
	errCode = mml_gpio_init(CPT_PORT, CPT_IRQ, 1, gpio_config_in);
	if (errCode != MLS_SUCCESS)
	{
		return errCode;
	}
	/** IRQ Handler */
	errCode = mml_intc_setup_irq(MML_INTNUM_GPIO0, MML_INTC_PRIO_5, CPT_IRQHandler);
	if (errCode != MLS_SUCCESS)
	{
		return errCode;
	}
	mml_gpio_enable_interrupt(CPT_PORT, CPT_IRQ, 1);

	mlsTP_Reset();


	return errCode;
}

mlsErrorCode_t mlsTP_Reset(void)
{
	mml_gpio_write_bit_pattern(CPT_PORT, CPT_RESET, 1, 1);
	mlsOsalDelayMs(10);
	mml_gpio_write_bit_pattern(CPT_PORT, CPT_RESET, 1, 0);
	mlsOsalDelayMs(10);
	mml_gpio_write_bit_pattern(CPT_PORT, CPT_RESET, 1, 1);
	mlsOsalDelayMs(10);

	return MLS_SUCCESS;
}

mlsErrorCode_t mlsTP_SetReg(uint8_t reg, uint8_t value)
{
	mlsOsalMutexLock(&gMaximI2CMutex, MLS_OSAL_MAX_DELAY);

	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = value;
	unsigned int len = 2;

#if (TP_HW_FT6206 == 1)
	mml_i2c_write_start(SLAVE_ADDRESS, buf, &len);
#elif (TP_HW_ST1633i == 1)
	mml_i2c_write_start(SLAVE_ADDRESS << 1, buf, &len);
#endif

	mml_i2c_stop();

	mlsOsalMutexUnlock(&gMaximI2CMutex);

	return MLS_SUCCESS;
}

mlsErrorCode_t mlsTP_GetReg(uint8_t reg, uint8_t *result, unsigned int length)
{
	mlsOsalMutexLock(&gMaximI2CMutex, MLS_OSAL_MAX_DELAY);
	mlsErrorCode_t errorCode = MLS_SUCCESS;
	uint8_t buf[1];
	buf[0] = reg;

#if (TP_HW_FT6206 == 1)
	mml_i2c_write_start(SLAVE_ADDRESS, buf, &length);
    mml_i2c_stop();
	mml_i2c_read_start(SLAVE_ADDRESS);
#elif (TP_HW_ST1633i == 1)
	mml_i2c_write_start(SLAVE_ADDRESS << 1, buf, &length);
    mml_i2c_stop();
	mml_i2c_read_start(SLAVE_ADDRESS << 1);
#endif

	mml_i2c_read(result, &length);

	if (length == 0)
	{
		errorCode = MLS_ERROR;
	}

    mml_i2c_stop();

    mlsOsalMutexUnlock(&gMaximI2CMutex);

	return errorCode;
}

/**@}*/

