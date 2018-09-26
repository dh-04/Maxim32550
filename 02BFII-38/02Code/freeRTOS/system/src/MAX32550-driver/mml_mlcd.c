/*
 * @file mml_mlcd.c MML MLCD Driver C File
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

#include <MAX325xx.h>

#ifdef _MML_MLCD_H_

#include <errors.h>



#define CLR_BIT(reg, offset)	reg &= ~(1 << (offset))
#define SET_BIT(reg, offset)	reg |=  (1 << (offset))

int mml_mlcd_config(mml_lcd_config config) {

	/* Configure GPIO */
	mml_gpio_config_t config_gpio;
	config_gpio.gpio_function = MML_GPIO_SECONDARY_ALT_FUNCTION;
	config_gpio.gpio_pad_config = MML_GPIO_PAD_NORMAL;
	/* Initialization for MLCD connect to GPIO PORT 1 pin 17 to 27 */
	mml_gpio_init(MML_GPIO_DEV1, 17, 11, config_gpio);
	/* Configure control register */
	mml_mlcd_cn_config_f(config.cn_config);
	/* Configure enable period register */
	mml_mlcd_config_epr(config.epr);

	return NO_ERROR;
}

int mml_mlcd_cn_config_f(mml_lcd_cn_config config) {
	int result = COMMON_ERR_UNKNOWN;
	/* Enable data transmit interrupt */
	/* Clear interrupt flag in status register before enabling */
	CLR_BIT(MLCD->SR, MLCD_SR_TXIS_Pos);
	if (config.txie) {
		SET_BIT(MLCD->CN, MLCD_CN_TXIE_Pos);
	} else {
		CLR_BIT(MLCD->CN, MLCD_CN_TXIE_Pos);
	}

	/* Enable data receive interrupt */
	/* Clear interrupt flag in status register before enabling */
	CLR_BIT(MLCD->SR, MLCD_SR_RXIS_Pos);
	if (config.rxie) {
		SET_BIT(MLCD->CN, MLCD_CN_RXIE_Pos);
	} else {
		CLR_BIT(MLCD->CN, MLCD_CN_RXIE_Pos);
	}

	/* Enable automatic mode or manual 0 Manual and 1 automatic*/
	if (config.ae) {
		SET_BIT(MLCD->CN, MLCD_CN_AE_Pos);
	} else {
		CLR_BIT(MLCD->CN, MLCD_CN_AE_Pos);
	}

	/* Bus width select */
	/* Check automatic mode enabled */
	if ((MLCD->CN & MLCD_CN_AE_Msk)) {
		if (config.nibble) {
			SET_BIT(MLCD->CN, MLCD_CN_NIBBLE_Pos);
		} else {
			CLR_BIT(MLCD->CN, MLCD_CN_NIBBLE_Pos);
		}
	}
	/* Manual mode, nibble must be 0 */
	else {
		if (config.nibble == 0) {
			CLR_BIT(MLCD->CN, MLCD_CN_NIBBLE_Pos);
		} else {
			result = COMMON_ERR_INVAL;
		}
	}
	/* Set the logic level of the LCD_RS pin */
	mml_mlcd_cn_set_rs(config.rs);
	/* Set the logic level of the LCD_RW pin */
	mml_mlcd_cn_set_rw(config.rw);
	/* Check automatic mode enabled */
	if ((MLCD->CN & MLCD_CN_AE_Msk)) {
		/* Set polarity of the LCD_EN pin */
		mml_mlcd_cn_set_enb(config.enb);
	}
	result = NO_ERROR;
	return result;
}

int mml_mlcd_read_cn(unsigned int *status) {

	if (!status) {
		return COMMON_ERR_NULL_PTR;
	}

	*status = MLCD->CN;
	return NO_ERROR;
}

void mml_mlcd_config_epr(unsigned char value) {
	MLCD->EPR = (value & MLCD_EPR_EP_Msk);
}

unsigned int mml_mlcd_read_epr(void) {
	return MLCD->EPR & MLCD_EPR_EP_Msk;
}

int mml_mlcd_sr_clear_flag(mml_mlcd_sr_register sr_clear) {

	switch (sr_clear) {
	case MML_MLCD_SR_REGISTER_TXIS:
		CLR_BIT(MLCD->SR, MLCD_SR_TXIS_Pos);
		break;
	case MML_MLCD_SR_REGISTER_RXIS:
		CLR_BIT(MLCD->SR, MLCD_SR_RXIS_Pos);
		break;
	default:
		return COMMON_ERR_INVAL;
	}
	return NO_ERROR;
}

unsigned int mml_mlcd_read_sr(void) {

	return MLCD->SR;
}

void mml_mlcd_write_data_automatic(unsigned char data) {

	SET_BIT(MLCD->CN, MLCD_CN_RS_Pos);
	CLR_BIT(MLCD->CN, MLCD_CN_RW_Pos);
	MLCD->DATA = (data & MLCD_DATA_DATA_Msk);
}

void mml_mlcd_write_cmd_automatic(unsigned char cmd) {

	CLR_BIT(MLCD->CN, MLCD_CN_RS_Pos);
	CLR_BIT(MLCD->CN, MLCD_CN_RW_Pos);
	MLCD->DATA = (cmd & MLCD_DATA_DATA_Msk);
}

unsigned char mml_mlcd_get_data_automatic(void) {

	SET_BIT(MLCD->CN, MLCD_CN_READ_Pos);
	while((MLCD->SR & MLCD_SR_RXIS_Msk) == 0);
	MLCD->SR &= ~MLCD_SR_RXIS_Msk;

	return  MLCD->DATA;
}

void mml_mlcd_cn_set_rs(uint32_t value) {

	if (value) {
		SET_BIT(MLCD->CN, MLCD_CN_RS_Pos);
	} else {
		CLR_BIT(MLCD->CN, MLCD_CN_RS_Pos);
	}
}

void mml_mlcd_cn_set_rw(uint32_t value) {

	if (value) {
		SET_BIT(MLCD->CN, MLCD_CN_RW_Pos);
	} else {
		CLR_BIT(MLCD->CN, MLCD_CN_RW_Pos);
	}
}

void mml_mlcd_cn_set_enb(uint32_t value) {

	if (value) {
		SET_BIT(MLCD->CN, MLCD_CN_ENB_Pos);
	} else {
		CLR_BIT(MLCD->CN, MLCD_CN_ENB_Pos);
	}
}

void mml_mlcd_write_data_manual(uint32_t data) {
	MLCD->DATA = (data & MLCD_DATA_DATA_Msk);
}

unsigned char mml_mlcd_get_data_manual(void) {
	return MLCD->DATA;
}

const char * mml_mlcd_get_version(void) {
	return MML_MLCD_VERSION_STRING;
}

#endif //_MML_MLCD_H_

/******************************************************************************/
/* EOF */

