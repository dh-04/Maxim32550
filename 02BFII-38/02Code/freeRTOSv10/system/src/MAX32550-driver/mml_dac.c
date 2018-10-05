/*
 * @file mml_dac.c MML DAC Driver C File
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

#ifdef _MML_DAC_H_

#include <errors.h>

__attribute__((section(".bss"))) mml_dac_context_t mml_dac_context;

void mml_dac_pre_init(void) {
	/* Attach vector */
	NVIC_SetVector(DAC_IRQn, (uint32_t)DAC_IRQHandler);

	/* Set priority level for interrupt */
	NVIC_SetPriority(DAC_IRQn, priority_table[DAC_IRQn]);
}

int mml_dac_set_interrupt_priority(mml_intc_prio_t priority) {
	/* Check if priority is in correct range */
	if (priority > MML_INTC_PRIO_7)
		return COMMON_ERR_INVAL;

	/* Then update priority_table */
	priority_table[DAC_IRQn] = priority;
	/* then set interrupt priority */
	NVIC_SetPriority(DAC_IRQn, priority);

	return NO_ERROR;
}

void mml_dac_init(void) {

	if (!mml_dac_context.first_init) {
		int 		 counter = 0;
		unsigned int freq_apb;
		int result;
		/* First you have to power-up the PLL1 and wait it's locked */
		mml_gcr_enable_pll1();
		/* Enable DAC clock */
		GCR->PERCKCN &= ~(GCR_PERCKCN_DACD_Msk);
		/* Power-up the DAC */
		mml_dac_power_up();

		/* Wait ~200us for the DAC to power-up*/
		result = mml_get_apb_frequency(&freq_apb);
		if(result) {
			freq_apb = 54 * 1000000; // default 54MHz
		}
		// calc counter
		counter  = 200 * (freq_apb / 1000000); // calc how many clock equal 1us
		counter /= 2; // estimate while is 2 clock
		// wait
		while (--counter)
			;

		/* init done */
		mml_dac_context.first_init = 1;
	}
}

void mml_dac_reset(void) {
	unsigned int loop = K_COBRA_RESET_WAIT_LOOP_MAX;
	unsigned int mask;

	mask = (1 << MML_RSTR_DEV_DAC);
	/* Launch all GPIOs reset */
	GCR->RSTR |= mask;
	/* Wait until it's done */
	while ((GCR->RSTR & mask) && loop--);
}

int mml_dac_config_f(mml_dac_config config) {
	int result = COMMON_ERR_UNKNOWN;

	if (!mml_dac_context.first_init) {
		return COMMON_ERR_NOT_INITIALIZED;
	}

	result = mml_dac_config_ctrl_register(config.dac_ctrl_config);
	if (result)
		return result;
	result = mml_dac_config_dac_rate(config.dac_rate_config);
	if (result)
		return result;
	return result;
}

int mml_dac_read_ctrl_register(mml_dac_flag_ctrl_register ctrl,
		unsigned int *flag) {

	if (ctrl > MML_DAC_FLAG_CTRL_REGISTER_MAX) {
		return COMMON_ERR_INVAL;
	} else 	if (!mml_dac_context.first_init) {
		return COMMON_ERR_NOT_INITIALIZED;
	} else if (!flag) {
		return COMMON_ERR_NULL_PTR;
	}

	switch (ctrl) {
	case MML_DAC_FLAG_CTRL_REGISTER_DACFAECNT:
		*flag = (DAC->CTRL & DAC_CTRL_DACFAECNT_Msk) >> DAC_CTRL_DACFAECNT_Pos;
		break;
	case MML_DAC_FLAG_CTRL_REGISTER_DACFAF:
		*flag = ((DAC->CTRL & DAC_CTRL_DACFAF_Msk) >> DAC_CTRL_DACFAF_Pos);
		break;
	case MML_DAC_FLAG_CTRL_REGISTER_DACFAE:
		*flag = ((DAC->CTRL & DAC_CTRL_DACFE_Msk) >> DAC_CTRL_DACFE_Pos);
		break;
	case MML_DAC_FLAG_CTRL_REGISTER_DACINTERPMODE:
		*flag = ((DAC->CTRL & DAC_CTRL_DACINTERPMODE_Msk)
				>> DAC_CTRL_DACINTERPMODE_Pos);
		break;
	case MML_DAC_FLAG_CTRL_REGISTER_DACFFCNT:
		*flag = ((DAC->CTRL & DAC_CTRL_DACFFCNT_Msk) >> DAC_CTRL_DACFFCNT_Pos);
		break;
	case MML_DAC_FLAG_CTRL_REGISTER_DACSTMODE:
		*flag =
				((DAC->CTRL & DAC_CTRL_DACSTMODE_Msk) >> DAC_CTRL_DACSTMODE_Pos);
		break;
	case MML_DAC_FLAG_CTRL_REGISTER_CPUDACST:
		*flag = ((DAC->CTRL & DAC_CTRL_CPUDACST_Msk) >> DAC_CTRL_CPUDACST_Pos);
		break;
	case MML_DAC_FLAG_CTRL_REGISTER_DACMODE:
		*flag = ((DAC->CTRL & DAC_CTRL_DACMODE_Msk) >> DAC_CTRL_DACMODE_Pos);
		break;
	case MML_DAC_FLAG_CTRL_REGISTER_DACPU:
		*flag = ((DAC->CTRL & DAC_CTRL_DACPU_Msk) >> DAC_CTRL_DACPU_Pos);
		break;
	case MML_DAC_FLAG_CTRL_REGISTER_RST:
		*flag = ((DAC->CTRL & DAC_CTRL_DACRST_Msk) >> DAC_CTRL_DACRST_Pos);
		break;
	}
	return NO_ERROR;
}

int mml_dac_config_ctrl_register(mml_dac_ctrl_config config) {
	if ((config.interpolation_mode > DAC_CTRL_DACINTERPMODE_interp1to8)
			| (config.dacmode > DAC_CTRL_DACMODE_toModeChange)) {
		return COMMON_ERR_INVAL;
	} else 	if (!mml_dac_context.first_init) {
		return COMMON_ERR_NOT_INITIALIZED;
	}

	DAC->CTRL = (DAC->CTRL & ~DAC_CTRL_DACFAECNT_Msk) |
				((config.dacfaecnt << DAC_CTRL_DACFAECNT_Pos) & DAC_CTRL_DACFAECNT_Msk);
	DAC->CTRL = (DAC->CTRL & ~DAC_CTRL_DACINTERPMODE_Msk) |
				((config.interpolation_mode << DAC_CTRL_DACINTERPMODE_Pos) & DAC_CTRL_DACINTERPMODE_Msk);
	DAC->CTRL = (DAC->CTRL & ~DAC_CTRL_DACFFCNT_Msk) |
				((config.dacffcnt << DAC_CTRL_DACFFCNT_Pos) & DAC_CTRL_DACFFCNT_Msk);
	DAC->CTRL = (DAC->CTRL & ~DAC_CTRL_DACSTMODE_Msk) |
				((config.dacstmode << DAC_CTRL_DACSTMODE_Pos) & DAC_CTRL_DACSTMODE_Msk);
	DAC->CTRL = (DAC->CTRL & ~DAC_CTRL_DACMODE_Msk) |
				((config.dacmode << DAC_CTRL_DACMODE_Pos) & DAC_CTRL_DACMODE_Msk);
	DAC->CTRL = (DAC->CTRL & ~DAC_CTRL_DACRST_Msk) |
				((config.dacrst << DAC_CTRL_DACRST_Pos) & DAC_CTRL_DACRST_Msk);

	return NO_ERROR;
}

void mml_dac_power_up(void) {
	DAC->CTRL |= DAC_CTRL_DACPU_Msk;
}

int mml_dac_read_dac_rate_register(mml_dac_flag_rate_register dac_rate,
		unsigned int *flag) {
	if (dac_rate > MML_DAC_FLAG_RATE_REGISTER_MAX) {
		return COMMON_ERR_INVAL;
	} else if (!flag) {
		return COMMON_ERR_NULL_PTR;
	}

	switch (dac_rate) {
	case MML_DAC_FLAG_RATE_REGISTER_DACRATECNT:
		*flag = ((DAC->RATE & DAC_RATE_DACRATECNT_Msk)
				>> DAC_RATE_DACRATECNT_Pos);
		break;
	case MML_DAC_FLAG_RATE_REGISTER_DACSAMPLECNT:
		*flag = ((DAC->RATE & DAC_RATE_DACSAMPLECNT_Msk)
				>> DAC_RATE_DACSAMPLECNT_Pos);
		break;
	}
	return NO_ERROR;
}

int mml_dac_config_dac_rate(mml_dac_rate_config config) {
	/* Check if dacratecnt is inferior at 0x46 (minimum value to have a sampling time superior at 3us */
	if (config.dacratecnt < 0x46) {
		return COMMON_ERR_INVAL;
	} else if (!mml_dac_context.first_init) {
		return COMMON_ERR_NOT_INITIALIZED;
	}

	DAC->RATE = ((config.dacratecnt << DAC_RATE_DACRATECNT_Pos)
			& DAC_RATE_DACRATECNT_Msk);
	DAC->RATE |= ((config.dacsamplecnt << DAC_RATE_DACSAMPLECNT_Pos)
			& DAC_RATE_DACSAMPLECNT_Msk);
	return NO_ERROR;
}

int mml_dac_read_int_register(mml_dac_int_register int_register,
		unsigned int *flag) {
	if (int_register > MML_DAC_INT_REGISTER_MAX) {
		return COMMON_ERR_INVAL;
	} else if (!mml_dac_context.first_init) {
		return COMMON_ERR_NOT_INITIALIZED;
	} else if (!flag) {
		return COMMON_ERR_NULL_PTR;
	}

	switch (int_register) {
	case MML_DAC_INT_REGISTER_DACDONEISR:
		*flag = ((DAC->INT & DAC_INT_DACDONEISR_Msk) >> DAC_INT_DACDONEISR_Pos);
		break;
	case MML_DAC_INT_REGISTER_DACFUFISR:
		*flag = ((DAC->INT & DAC_INT_DACFUFISR_Msk) >> DAC_INT_DACFUFISR_Pos);
		break;
	case MML_DAC_INT_REGISTER_DACFAEISR:
		*flag = ((DAC->INT & DAC_INT_DACFAEISR_Msk) >> DAC_INT_DACFAEISR_Pos);
		break;
	case MML_DAC_INT_REGISTER_FUFUST:
		*flag = ((DAC->INT & DAC_INT_DACFUFST_Msk) >> DAC_INT_DACFUFST_Pos);
		break;
	case MML_DAC_INT_REGISTER_DACDONEIE:
		*flag = ((DAC->INT & DAC_INT_DACDONEIE_Msk) >> DAC_INT_DACDONEIE_Pos);
		break;
	case MML_DAC_INT_REGISTER_DACFUFIE:
		*flag = ((DAC->INT & DAC_INT_DACFUFIE_Msk) >> DAC_INT_DACFUFIE_Pos);
		break;
	case MML_DAC_INT_REGISTER_DACFAEIE:
		*flag = ((DAC->INT & DAC_INT_DACFAEIE_Msk) >> DAC_INT_DACFAEIE_Pos);
		break;
	}
	return NO_ERROR;
}

void mml_dac_set_dacfifo(unsigned char value) {
	DAC_FIFO->REG = value & DAC_FIFO_REG_DATA_Msk;
}

int mml_dac_interupt_enable(mml_dac_interupt_t interrupt) {

	if (!mml_dac_context.first_init) {
		return COMMON_ERR_NOT_INITIALIZED;
	}

	switch(interrupt) {
	case MML_DAC_INTERRUPT_DONE:
		DAC->INT |= DAC_INT_DACDONEIE_Msk;
		break;
	case MML_DAC_INTERRUPT_FUFI:
		DAC->INT |= DAC_INT_DACFUFIE_Msk;
		break;
	case MML_DAC_INTERRUPT_FAE:
		DAC->INT |= DAC_INT_DACFAEIE_Msk;
		break;
	default:
		return COMMON_ERR_INVAL;
	}

	if (mml_dac_context.interrupt_enable == 0) {
		NVIC_SetPriority(DAC_IRQn, priority_table[DAC_IRQn]);
		NVIC_EnableIRQ(DAC_IRQn);
		mml_dac_context.interrupt_enable = 1;
	}

	return NO_ERROR;
}

int mml_dac_interupt_disable(mml_dac_interupt_t interrupt) {
	switch (interrupt) {
	case MML_DAC_INTERRUPT_DONE:
		DAC->INT &= ~(DAC_INT_DACDONEIE_Msk);
		break;
	case MML_DAC_INTERRUPT_FUFI:
		DAC->INT &= ~(DAC_INT_DACFUFIE_Msk);
		break;
	case MML_DAC_INTERRUPT_FAE:
		DAC->INT &= ~(DAC_INT_DACFAEIE_Msk);
		break;
	default:
		return COMMON_ERR_INVAL;
	}
	if (!(DAC->INT & (DAC_INT_DACDONEIE_Msk | DAC_INT_DACFUFIE_Msk | DAC_INT_DACFAEIE_Msk))) {
			NVIC_DisableIRQ(DAC_IRQn);
			mml_dac_context.interrupt_enable = 0;
		}

	return NO_ERROR;
}

int mml_dac_read_interrupt(mml_dac_interupt_t interrupt) {
	switch(interrupt) {
	case MML_DAC_INTERRUPT_DONE:
		DAC->INT |= DAC_INT_DACDONEISR_Msk;
		break;
	case MML_DAC_INTERRUPT_FUFI:
		DAC->INT |= DAC_INT_DACFUFISR_Msk;
		break;
	case MML_DAC_INTERRUPT_FAE:
		DAC->INT |= DAC_INT_DACFAEISR_Msk;
		break;
	default:
		return COMMON_ERR_INVAL;
	}
	return NO_ERROR;
}

void mml_dac_deinit(void) {

	if (mml_dac_context.first_init) {
		/* Disable DAC power */
		DAC->CTRL &= ~DAC_CTRL_DACPU_Msk;
		/* Disable DAC clock */
		GCR->PERCKCN |= GCR_PERCKCN_DACD_Msk;
		/* Disable irq */
		NVIC_DisableIRQ(DAC_IRQn);
		mml_dac_context.interrupt_enable = 0;

		/* deinit done */
		mml_dac_context.first_init = 0;
	}
}

const char * mml_dac_get_version(void) {
	return MML_DAC_VERSION_STRING;
}

#endif //_MML_DAC_H_

/******************************************************************************/
/* EOF */
