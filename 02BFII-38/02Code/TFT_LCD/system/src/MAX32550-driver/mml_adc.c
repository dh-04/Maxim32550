/*
 * @file mml_adc.c MML ADC Driver C File
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

#ifdef _MML_ADC_H_
#ifndef _MML_GCR_H_
#error "*** GCR driver must be included ***"
#endif

#include <errors.h>

__attribute__((section(".bss"))) mml_adc_context_t mml_adc_context;


void mml_adc_pre_init(void) {
	/* Attach vector */
	NVIC_SetVector(ADC_IRQn, (uint32_t)ADC_IRQHandler);

	/* Set priority level for interrupt */
	NVIC_SetPriority(ADC_IRQn, priority_table[ADC_IRQn]);
}

int mml_adc_set_interrupt_priority(mml_intc_prio_t priority) {
	/* Check if priority is in correct range */
	if (priority > MML_INTC_PRIO_7)
		return COMMON_ERR_INVAL;

	/* Then update priority_table */
	priority_table[ADC_IRQn] = priority;
	/* then set interrupt priority */
	NVIC_SetPriority(ADC_IRQn, priority);

	return NO_ERROR;
}

int mml_adc_init(mml_adc_config_t config) {
	int result = COMMON_ERR_UNKNOWN;

	GCR->PERCKCN &= ~(GCR_PERCKCN_ADCD_Msk);
	// force to reset the value
	ADC->CN = 0;
	result = mml_adc_set_config(config);
	return result;
}

int mml_adc_set_config(mml_adc_config_t config) {
	int result = COMMON_ERR_UNKNOWN;

	if (config.alignment > ADC_CN_ADALGN_leftAlign) {
		return COMMON_ERR_INVAL;
	}

	/* Select data alignment */
	ADC->CN = (ADC->CN & ~(ADC_CN_ADALGN_Msk)) | ((config.alignment << ADC_CN_ADALGN_Pos) & ADC_CN_ADALGN_Msk);
	/* Select channel */
	result = mml_adc_select_input_channel(config.channel);
	if (result)
		return result;
	/* Enable internal temperature sensor if selected as channel */
	if (config.channel == ADC_CN_ADCH_tempSensor) {
		ADC->CN |= ADC_CN_TSEN_Msk;
	}
	/* Select scale  */
	result = mml_adc_select_scale_input(config.scale);
	if (result)
		return result;
	/* Select clock */
	result = mml_adc_set_adcclk(config.clock);
	if (result)
		return result;
	/* Select reference */
	result = mml_adc_select_reference(config.reference);

	return result;
}

int mml_adc_interrupt_config(mml_adc_interrupt_config_t int_config) {
	/* Check input parameters */
	if (int_config > MML_ADC_INTERRUPT_CONFIG_MAX) {
		return COMMON_ERR_INVAL;
	}
	/* Set config */
	switch (int_config) {
	case MML_ADC_INTERRUPT_CONFIG_ENABLE_ADDAIE:
		ADC->CN |= ADC_CN_ADDAIE_Msk;
		break;
	case MML_ADC_INTERRUPT_CONFIG_DISABLE_ADDAIE:
		ADC->CN &= ~( ADC_CN_ADDAIE_Msk);
		break;
	case MML_ADC_INTERRUPT_CONFIG_ENABLE_ADLTIE:
		ADC->CN |= ADC_CN_ADLTIE_Msk;
		break;
	case MML_ADC_INTERRUPT_CONFIG_DISABLE_ADLTIE:
		ADC->CN &= ~(ADC_CN_ADLTIE_Msk);
		break;
	case MML_ADC_INTERRUPT_CONFIG_ENABLE_ADHTIE:
		ADC->CN |= ADC_CN_ADHTIE_Msk;
		break;
	case MML_ADC_INTERRUPT_CONFIG_DISABLE_ADHTIE:
		ADC->CN &= ~(ADC_CN_ADHTIE_Msk);
		break;
	}

	if (ADC->CN & ( ADC_CN_ADDAIE_Msk | ADC_CN_ADLTIE_Msk | ADC_CN_ADHTIE_Msk) ) {
		if (mml_adc_context.interrupt_enable == 0) {
			NVIC_SetPriority(ADC_IRQn, priority_table[ADC_IRQn]);
			NVIC_EnableIRQ(ADC_IRQn);
			mml_adc_context.interrupt_enable = 1;
		}
	} else {
		if (mml_adc_context.interrupt_enable == 1) {
			NVIC_DisableIRQ(ADC_IRQn);
			mml_adc_context.interrupt_enable = 0;
		}
	}

	return NO_ERROR;
}

int mml_adc_clear_interrupt(mml_adc_clear_interrupt_t flag) {
	if (flag > MML_ADC_CLEAR_INTERRUPT_ADHTIF) {
		return COMMON_ERR_INVAL;
	}
	switch (flag) {
	case MML_ADC_CLEAR_INTERRUPT_ADDAI:
		ADC->ST &= ~(ADC_ST_ADDAI_Msk);
		break;
	case MML_ADC_CLEAR_INTERRUPT_ADOVF:
		ADC->ST &= ~(ADC_ST_ADOVF_Msk);
		break;
	case MML_ADC_CLEAR_INTERRUPT_ADLTIF:
		ADC->ST &= ~(ADC_ST_ADLTIF_Msk);
		break;
	case MML_ADC_CLEAR_INTERRUPT_ADHTIF:
		ADC->ST &= ~(ADC_ST_ADHTIF_Msk);
		break;
	}

	return NO_ERROR;
}

void mml_adc_reset(void) {
	unsigned int loop = K_COBRA_RESET_WAIT_LOOP_MAX;
	unsigned int mask;

	mask = (1 << MML_RSTR_DEV_ADC);
	/* Launch all GPIOs reset */
	GCR->RSTR |= mask;
	/* Wait until it's done */
	while ((GCR->RSTR & mask) && loop--);
}

int mml_adc_select_input_channel(ADC_CN_ADCH_Type channel) {
	if ((channel == 2) | (channel > ADC_CN_ADCH_tempSensor)) {
		return COMMON_ERR_INVAL;
	} else {
		ADC->CN = (ADC->CN & ~(ADC_CN_ADCH_Msk)) | ((channel << ADC_CN_ADCH_Pos) & ADC_CN_ADCH_Msk);
		return NO_ERROR;
	}
}

int mml_adc_select_scale_input(ADC_CN_ADSCL_Type scale_input) {
	if (scale_input > ADC_CN_ADSCL_enable) {
		return COMMON_ERR_INVAL;
	}
	ADC->CN = (ADC->CN & ~(ADC_CN_ADSCL_Msk)) | ((scale_input << ADC_CN_ADSCL_Pos) & ADC_CN_ADSCL_Msk);
	return NO_ERROR;
}

int mml_adc_select_reference(ADC_CN_ADREF_Type reference) {
	if (reference > ADC_CN_ADREF_internalRef) {
		return COMMON_ERR_INVAL;
	}
	ADC->CN =  (ADC->CN & ~(ADC_CN_ADREF_Msk)) | ((reference << ADC_CN_ADREF_Pos) & ADC_CN_ADREF_Msk);
	return NO_ERROR;
}

int mml_adc_set_adcclk(ADC_CN_ADCCLK_Type divider) {
	if (divider > ADC_CN_ADCCLK_div128) {
		return COMMON_ERR_INVAL;
	}
	ADC->CN =  (ADC->CN & ~(ADC_CN_ADCCLK_Msk)) | ((divider << ADC_CN_ADCCLK_Pos) & ADC_CN_ADCCLK_Msk);
	return NO_ERROR;
}

int mml_adc_start_conversion(void) {
	if ((ADC->CN & ADC_CN_ADEN_Msk) == 0) {
		return MML_ADC_ERRORS_NOT_ENABLED;
	} else {
		ADC->ST |= ADC_ST_ADCONV_Msk;
		return NO_ERROR;
	}
}

int mml_adc_stop_conversion(void) {
	if ((ADC->CN & ADC_CN_ADEN_Msk) == 0) {
		return MML_ADC_ERRORS_NOT_ENABLED;
	} else {
		ADC->ST &= ~(ADC_ST_ADCONV_Msk);
		return NO_ERROR;
	}
}

int mml_adc_read_status(unsigned int * status) {
	if (!status) {
		return COMMON_ERR_NULL_PTR;
	}
	*status = ADC->ST;
	return NO_ERROR;
}

int mml_adc_get_data(unsigned int *status) {
	if (!(ADC->ST & ADC_ST_ADDAI_Msk)) {
		return MML_ADC_ERRORS_CONVERSION_NOT_AVAILABLE;
	} else 	if (!status) {
		return COMMON_ERR_NULL_PTR;
	} else {
		*status = ADC->DATA;
		return NO_ERROR;
	}
}

int mml_adc_set_low_threshold(int value, ADC_CN_ADCH_Type channel) {
	if ((channel > ADC_CN_ADCH_tempSensor) | (channel == 2)) {
		return COMMON_ERR_INVAL;
	}
	if (channel == ADC_CN_ADCH_an0) {
		ADC->LT0 = (ADC->LT0 & ~(ADC_LT_ADLT_Msk)) | ((value << ADC_LT_ADLT_Pos) & ADC_LT_ADLT_Msk);
	} else if (channel == ADC_CN_ADCH_an1) {
		ADC->LT1 = (ADC->LT1 & ~(ADC_LT_ADLT_Msk)) | ((value << ADC_LT_ADLT_Pos) & ADC_LT_ADLT_Msk);
	} else if (channel == ADC_CN_ADCH_tempSensor) {
		ADC->LT2 = (ADC->LT2 & ~(ADC_LT_ADLT_Msk)) | ((value << ADC_LT_ADLT_Pos) & ADC_LT_ADLT_Msk);
	}
	return NO_ERROR;
}

int mml_adc_get_low_threshold(ADC_CN_ADCH_Type channel, unsigned int *adlt) {
	if ((channel > ADC_CN_ADCH_tempSensor) | (channel == 2)) {
		return COMMON_ERR_INVAL;
	} else if (!adlt) {
		return COMMON_ERR_NULL_PTR;
	}

	if (channel == ADC_CN_ADCH_an0) {
		*adlt = ((ADC->LT0 & ADC_LT_ADLT_Msk) >> ADC_LT_ADLT_Pos);
	} else if (channel == ADC_CN_ADCH_an1) {
		*adlt = ((ADC->LT1 & ADC_LT_ADLT_Msk) >> ADC_LT_ADLT_Pos);
	} else if (channel == ADC_CN_ADCH_tempSensor) {
		*adlt = ((ADC->LT2 & ADC_LT_ADLT_Msk) >> ADC_LT_ADLT_Pos);
	}
	return NO_ERROR;
}

int mml_adc_set_high_threshold(int value, ADC_CN_ADCH_Type channel) {
	if ((channel > ADC_CN_ADCH_tempSensor) | (channel == 2)) {
		return COMMON_ERR_INVAL;
	}
	if (channel == ADC_CN_ADCH_an0) {
		ADC->HT0 = (ADC->HT0 & ~(ADC_HT_ADHT_Msk)) | ((value << ADC_HT_ADHT_Pos) & ADC_HT_ADHT_Msk);
	} else if (channel == ADC_CN_ADCH_an1) {
		ADC->HT1 = (ADC->HT1 & ~(ADC_HT_ADHT_Msk)) | ((value << ADC_HT_ADHT_Pos) & ADC_HT_ADHT_Msk);
	} else if (channel == ADC_CN_ADCH_tempSensor) {
		ADC->HT2 = (ADC->HT2 & ~(ADC_HT_ADHT_Msk)) | ((value << ADC_HT_ADHT_Pos) & ADC_HT_ADHT_Msk);
	}
	return NO_ERROR;
}

int mml_adc_get_high_threshold(ADC_CN_ADCH_Type channel, unsigned int *adht) {
	if ((channel > ADC_CN_ADCH_tempSensor) | (channel == 2)) {
		return COMMON_ERR_INVAL;
	} else if (!adht) {
		return COMMON_ERR_NULL_PTR;
	}
	if (channel == ADC_CN_ADCH_an0) {
		*adht = ((ADC->HT0 & ADC_HT_ADHT_Msk) >> ADC_HT_ADHT_Pos);
	} else if (channel == ADC_CN_ADCH_an1) {
		*adht = ((ADC->HT1 & ADC_HT_ADHT_Msk) >> ADC_HT_ADHT_Pos);
	} else if (channel == ADC_CN_ADCH_tempSensor) {
		*adht = ((ADC->HT2 & ADC_HT_ADHT_Msk) >> ADC_HT_ADHT_Pos);
	}
	return NO_ERROR;
}

int mml_adc_operation_config(ADC_CN_ADEN_Type config) {
	if (config > ADC_CN_ADEN_enable) {
		return COMMON_ERR_INVAL;
	}
	switch (config) {
	case ADC_CN_ADEN_enable:
		ADC->CN |= ADC_CN_ADEN_Msk;
		break;
	case ADC_CN_ADEN_disable:
		ADC->CN &= ~(ADC_CN_ADEN_Msk);
		break;
	}
	return NO_ERROR;
}

const char * mml_adc_get_version(void) {
	return MML_ADC_VERSION_STRING;
}

#endif //_MML_ADC_H_

/******************************************************************************/
/* EOF */

