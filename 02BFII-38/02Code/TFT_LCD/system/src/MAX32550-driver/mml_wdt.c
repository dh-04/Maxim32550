/*
 * @file mml_wdt.c MML WDT Driver C File
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

#ifdef _MML_WDT_H_

#include <errors.h>



void mml_wdt_pre_init(void) {

	/* Attach vector */
	NVIC_SetVector(Watchdog_Timer_IRQn, (uint32_t) WDG_IRQHandler);

	/* Set priority level for interrupt */
	NVIC_SetPriority(Watchdog_Timer_IRQn, priority_table[Watchdog_Timer_IRQn]);
}

int mml_wdt_set_interrupt_priority(mml_intc_prio_t priority) {
	/* Check if priority is in correct range */
	if (priority > MML_INTC_PRIO_7)
		return COMMON_ERR_INVAL;

	/* Then update priority_table */
	priority_table[Watchdog_Timer_IRQn] = priority;
	/* then set interrupt priority */
	NVIC_SetPriority(Watchdog_Timer_IRQn, priority);

	return NO_ERROR;
}

int mml_wdt_set_configuration(mml_wdt_config config) {
	int result = COMMON_ERR_UNKNOWN;

	/* Reset control register before configuration */
	WDT->CTRL &= ~RESET_CONFIG_Msk;

	/* reset counter */
	mml_wdt_counter_reset();
	
	/* Set reset period */
	result = mml_wdt_set_rst_period(config.rst_period);
	if (result)
		return result;

	/* Set interrupt period */
	/* Check if reset have to be enabled */
	if(config.rst_ien) {
		/* Check if int_period is inferior at rst_period */
		if ((unsigned int) (config.int_period) < (unsigned int) (config.rst_period)){
			/* The time to reached interrupt is superior at time to reset Watchdog counter */
			return COMMON_ERR_INVAL;
		}
		result = mml_wdt_set_int_period(config.int_period);
		if (result)
			return result;
	} else {
		result = mml_wdt_set_int_period(config.int_period);
		if (result)
			return result;
	}

	/* Reset Settings */
	if (config.rst_ien) {
		mml_wdt_enable_reset();
	} else {
		mml_wdt_disable_reset();
	}

	/* Interrupt Settings */
	if (config.int_en) {
		mml_wdt_enable_int();
	} else {
		mml_wdt_disable_int();
	}

	return result;
}

void mml_wdt_enable(void) {
	mml_wdt_counter_reset();
	WDT->CTRL |= WDT_CTRL_WDT_EN_Msk;
}

void mml_wdt_disable(void) {
	WDT->CTRL &= ~(WDT_CTRL_WDT_EN_Msk);
}

int mml_wdt_set_rst_period(WDT_CTRL_RST_PERIOD_Type rst_period) {
	/* Check input parameter */
	if (rst_period > WDT_CTRL_RST_PERIOD_wdt2pow16) {
		return COMMON_ERR_INVAL;
	}
	WDT->CTRL |= (rst_period << WDT_CTRL_RST_PERIOD_Pos) & WDT_CTRL_RST_PERIOD_Msk;
	return NO_ERROR;
}

int mml_wdt_set_int_period(WDT_CTRL_INT_PERIOD_Type int_period) {
	/* Check input parameter */
	if (int_period > WDT_CTRL_INT_PERIOD_wdt2pow16) {
		return COMMON_ERR_INVAL;
	}
	WDT->CTRL |= ((int_period << WDT_CTRL_INT_PERIOD_Pos) & WDT_CTRL_INT_PERIOD_Msk);
	return NO_ERROR;
}

void mml_wdt_enable_int(void) {
	WDT->CTRL |= WDT_CTRL_INT_EN_Msk;
	NVIC_EnableIRQ(Watchdog_Timer_IRQn);
}

void mml_wdt_disable_int(void) {
	WDT->CTRL &= ~( WDT_CTRL_INT_EN_Msk);
	NVIC_DisableIRQ(Watchdog_Timer_IRQn);
}

void mml_wdt_enable_reset(void) {
	WDT->CTRL |= WDT_CTRL_RST_EN_Msk;
}

void mml_wdt_disable_reset(void) {
	WDT->CTRL &= ~(WDT_CTRL_RST_EN_Msk);
}

int mml_wdt_read_flag(mml_wdt_interupt_flag_t flag, uint32_t *status) {

	if (!status) {
		return COMMON_ERR_NULL_PTR;
	}

	switch (flag) {
	case MML_WDT_INTERRUPT_FLAG_INT_FLAG:
		*status = (WDT->CTRL & WDT_CTRL_INT_FLAG_Msk);
		break;
	case MML_WDT_INTERRUPT_FLAG_RST_FLAG:
		*status = (WDT->CTRL & WDT_CTRL_RST_FLAG_Msk);
		break;
	default:
		return COMMON_ERR_INVAL;
	}

	return NO_ERROR;
}

int mml_wdt_clear_flag(mml_wdt_interupt_flag_t flag) {
	switch (flag) {
	case MML_WDT_INTERRUPT_FLAG_INT_FLAG:
		WDT->CTRL &= ~(WDT_CTRL_INT_FLAG_Msk);
		break;
	case MML_WDT_INTERRUPT_FLAG_RST_FLAG:
		WDT->CTRL &= ~(WDT_CTRL_RST_FLAG_Msk);
		break;
	default:
		return COMMON_ERR_INVAL;
	}
	return NO_ERROR;
}

void mml_wdt_counter_reset(void) {
	WDT->RST = MML_WDT_RST_TRST_1ST_WORD;
	WDT->RST = MML_WDT_RST_TRST_2ND_WORD;
}

const char * mml_wdt_get_version(void) {
	return MML_WDT_VERSION_STRING;
}

#endif //_MML_WDT_H_

/******************************************************************************/
/* EOF */
