/*
 * @file mml_tmr.c MML TMR Driver C File
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

#ifdef _MML_TMR_H_

#include <errors.h>

#define	K_COBRA_RESET_WAIT_LOOP_MAX				100000

/** This structure is located in bss section, therefore it shall be zero-ized
 * at the very beginning of the application */
__attribute__((section(".bss")))          mml_tmr_context_t tmr_context;

#define M_MML_CHECK_PARAMS(__tmr_conf__)	( ( ( TMRn_CN_PRES_div4096 < ((mml_tmr_config_t)__tmr_conf__).clock ) ||\
						    ( TMRn_CN_TMODE_captureCompare < ((mml_tmr_config_t)__tmr_conf__).mode ) ||\
						    ( TMRn_CN_TPOL_activeLo < ((mml_tmr_config_t)__tmr_conf__).polarity ) ) ? COMMON_ERR_OUT_OF_RANGE : NO_ERROR )

static const unsigned char timer_irqs[] = { Timer0_IRQn, Timer1_IRQn, Timer2_IRQn, Timer3_IRQn, Timer4_IRQn, Timer5_IRQn  };

void mml_tmr_pre_init(void) {

	/* Attach vector */
	NVIC_SetVector(Timer0_IRQn, (uint32_t) TIMER0_IRQHandler);
	NVIC_SetVector(Timer1_IRQn, (uint32_t) TIMER1_IRQHandler);
	NVIC_SetVector(Timer2_IRQn, (uint32_t) TIMER2_IRQHandler);
	NVIC_SetVector(Timer3_IRQn, (uint32_t) TIMER3_IRQHandler);
	NVIC_SetVector(Timer4_IRQn, (uint32_t) TIMER4_IRQHandler);
	NVIC_SetVector(Timer5_IRQn, (uint32_t) TIMER5_IRQHandler);


	/* Set priority level for interrupt */
	NVIC_SetPriority(Timer0_IRQn, priority_table[Timer0_IRQn]);
	NVIC_SetPriority(Timer1_IRQn, priority_table[Timer1_IRQn]);
	NVIC_SetPriority(Timer2_IRQn, priority_table[Timer2_IRQn]);
	NVIC_SetPriority(Timer3_IRQn, priority_table[Timer3_IRQn]);
	NVIC_SetPriority(Timer4_IRQn, priority_table[Timer4_IRQn]);
	NVIC_SetPriority(Timer5_IRQn, priority_table[Timer5_IRQn]);
}

int mml_tmr_set_interrupt_priority(int vector, mml_intc_prio_t priority) {
	/* First check if vector is in correct range */
	if((vector > Timer5_IRQn) || (vector < Timer0_IRQn))
		return COMMON_ERR_INVAL;

	/* Check if priority is in correct range */
	if (priority > MML_INTC_PRIO_7)
		return COMMON_ERR_INVAL;

	/* Then update priority_table */
	priority_table[vector] = priority;
	/* then set interrupt priority */
	NVIC_SetPriority(vector, priority);

	return NO_ERROR;
}

int mml_tmr_get_free_timer(mml_tmr_id_t *id) {
	mml_tmr_id_t i = MML_TMR_DEV_MIN;

	if (!id) {
		return COMMON_ERR_NULL_PTR;
	}

	do {
		/* Check if timer is disabled and not initialized */
		if ((tmr_context.tmr[i].state == MML_TMR_STATE_NOT_INITIALIZED) && (tmr_context.tmr[i].status == MML_TMR_STATUS_DISABLED)) {
			*id = i;
			return NO_ERROR;
		}
		i++;
	} while (i <= MML_TMR_DEV_MAX);

	/* Here *id is MML_TMR_DEV_MAX + 1 */
	*id = i;

	return MML_TMR_ERR_NO_TIMER_AVAILABLE;
}

int mml_tmr_init(mml_tmr_id_t id, mml_tmr_config_t *config) {
	int result = COMMON_ERR_UNKNOWN;
	unsigned int tmr_clk_id = 0;

	/* For the first time initialization */
	if (!tmr_context.first_init) {
		/* Initialize the Timers context information to zero's */
		tmr_context.tmr[MML_TMR_DEV0].reg_tmr = TMR0;
		tmr_context.tmr[MML_TMR_DEV1].reg_tmr = TMR1;
		tmr_context.tmr[MML_TMR_DEV2].reg_tmr = TMR2;
		tmr_context.tmr[MML_TMR_DEV3].reg_tmr = TMR3;
		tmr_context.tmr[MML_TMR_DEV4].reg_tmr = TMR4;
		tmr_context.tmr[MML_TMR_DEV5].reg_tmr = TMR5;
#ifdef _TIMER_RESET_AT_INIT_
		/* Reset all Timer's interface */
		GCR->RSTR |= MML_GCR_RSTR_TIMERS_MASK;
		/* Wait for TIMER reset to be done */
		while( MML_GCR_RSTR_TIMERS_MASK & GCR->RSTR );
		/* Stop clock of each TIMER. It'll be set independently */
		GCR->PERCKCN |= GCR_PERCKCN_T0D_Msk;
		GCR->PERCKCN |= GCR_PERCKCN_T1D_Msk;
		GCR->PERCKCN |= GCR_PERCKCN_T2D_Msk;
		GCR->PERCKCN |= GCR_PERCKCN_T3D_Msk;
		GCR->PERCKCN |= GCR_PERCKCN_T4D_Msk;
		GCR->PERCKCN |= GCR_PERCKCN_T5D_Msk;
#endif /* _TIMER_RESET_AT_INIT_ */
		tmr_context.first_init = 1;
	}
	/* Validate timer identifier */
	switch (id) {
	case MML_TMR_DEV0:
		tmr_clk_id = MML_PERCKCN_DEV_T0;
		break;
	case MML_TMR_DEV1:
		tmr_clk_id = MML_PERCKCN_DEV_T1;
		break;
	case MML_TMR_DEV2:
		tmr_clk_id = MML_PERCKCN_DEV_T2;
		break;
	case MML_TMR_DEV3:
		tmr_clk_id = MML_PERCKCN_DEV_T3;
		break;
	case MML_TMR_DEV4:
		tmr_clk_id = MML_PERCKCN_DEV_T4;
		break;
	case MML_TMR_DEV5:
		tmr_clk_id = MML_PERCKCN_DEV_T5;
		break;
	default:
		return COMMON_ERR_INVAL;
	}
	/* Check state */
	if (MML_TMR_STATE_INITIALIZED == tmr_context.tmr[id].state) {
		result = COMMON_ERR_BAD_STATE;
	} else if (!config) {
		result = COMMON_ERR_NULL_PTR;
	}
	/* Validate the configuration parameters */
	else if (NO_ERROR == M_MML_CHECK_PARAMS(*config)) {
		/* Enable the timer clock */
		/* Clear the bit position to enable clock to timer device */
		GCR->PERCKCN &= ~(1 << tmr_clk_id);
		/* Default value (Timer is disabled) */
		tmr_context.tmr[id].reg_tmr->CN = 0;
		tmr_context.tmr[id].reg_tmr->INT = TMRn_INT_IRQ_CLR_Msk;
		tmr_context.tmr[id].reg_tmr->CNT = config->count;
		tmr_context.tmr[id].reg_tmr->CMP = config->timeout;
		result = mml_tmr_mode_ctrl(id, *config);
		if (NO_ERROR == result) {
			tmr_context.tmr[id].state = MML_TMR_STATE_INITIALIZED;
			tmr_context.tmr[id].status = MML_TMR_STATUS_DISABLED;
			tmr_context.tmr[id].handler = config->handler;
			if (config->handler) {
				NVIC_EnableIRQ(timer_irqs[id]);
			}
		}
	}
	return result;
}

int mml_tmr_close(mml_tmr_id_t id) {
	/* Validate the timer id */
	if (MML_TMR_DEV_COUNT <= id) {
		return COMMON_ERR_INVAL;
	} else if (MML_TMR_STATE_INITIALIZED != tmr_context.tmr[id].state) {
		return COMMON_ERR_BAD_STATE;
	} else if (MML_TMR_STATUS_ENABLED == tmr_context.tmr[id].status) {
		return MML_TMR_ERR_NOT_ALLOWED;
	} else {
		unsigned int tmp;
		/* Disable the timer clock */
		tmp = GCR->PERCKCN;
		/* Set the bit position */
		tmp |= ((0x1 << id) << MML_TMR_ACTIVE_OFST);
		GCR->PERCKCN = tmp;
		/* Timer state is not initialized */
		tmr_context.tmr[id].state = MML_TMR_STATE_NOT_INITIALIZED;
		/* Timer has already been disabled */
		return NO_ERROR;
	}
}

int mml_tmr_reset_interface(void) {
	unsigned int loop = K_COBRA_RESET_WAIT_LOOP_MAX;

	/* Launch all Timers reset */
	GCR->RSTR |= MML_GCR_RSTR_TIMERS_MASK;
	/* Wait until it's done */
	while ((GCR->RSTR & (MML_GCR_RSTR_TIMERS_MASK)) && loop--);
	if (loop == 0)
		return COMMON_ERR_IN_PROGRESS;
	return NO_ERROR;
}

int mml_tmr_config(mml_tmr_id_t id, mml_tmr_config_t *config) {
	int result = COMMON_ERR_UNKNOWN;

	if (id >= MML_TMR_DEV_COUNT) {
		return COMMON_ERR_INVAL;
	}

	/* Check state */
	if (MML_TMR_STATE_INITIALIZED != tmr_context.tmr[id].state) {
		result = MML_TMR_ERR_NOT_INITIALIZED;
	} else if (MML_TMR_STATUS_ENABLED == tmr_context.tmr[id].status) {
		result = MML_TMR_ERR_NOT_ALLOWED;
	} else if (!config) {
		result = COMMON_ERR_NULL_PTR;
	}
	/* Validate the configuration parameters */
	else if (NO_ERROR == M_MML_CHECK_PARAMS(*config)) {
		result = mml_tmr_mode_ctrl(id, *config);
		if (NO_ERROR == result) {
			if (config->handler) {
				mml_tmr_interrupt_clear(id);
				/* Setup IRQ */
				tmr_context.tmr[id].handler = config->handler;
				NVIC_EnableIRQ(timer_irqs[id]);
			}
		}
	}
	return result;
}

int mml_tmr_enable(mml_tmr_id_t id) {

	/* Validate the timer id */
	if (MML_TMR_DEV_COUNT <= id) {
		return COMMON_ERR_INVAL;
	} else if (MML_TMR_STATE_INITIALIZED != tmr_context.tmr[id].state) {
		return MML_TMR_ERR_NOT_INITIALIZED;
	} else if (MML_TMR_STATUS_ENABLED == tmr_context.tmr[id].status) {
		return MML_TMR_ERR_NOT_ALLOWED;
	}

	/* Then enable it */
	tmr_context.tmr[id].reg_tmr->CN |= TMRn_CN_TEN_Msk;
	tmr_context.tmr[id].status = MML_TMR_STATUS_ENABLED;

	return NO_ERROR;
}

int mml_tmr_disable(mml_tmr_id_t id) {

	/* Validate the timer id */
	if (MML_TMR_DEV_COUNT <= id) {
		return COMMON_ERR_INVAL;
	} else if (MML_TMR_STATE_INITIALIZED != tmr_context.tmr[id].state) {
		return MML_TMR_ERR_NOT_INITIALIZED;
	}

	tmr_context.tmr[id].reg_tmr->CN &= ~TMRn_CN_TEN_Msk;
	tmr_context.tmr[id].status = MML_TMR_STATUS_DISABLED;

	return NO_ERROR;
}

int mml_tmr_read(mml_tmr_id_t id, unsigned int *time) {

	/* Validate the timer id */
	if (MML_TMR_DEV_COUNT <= id) {
		return COMMON_ERR_INVAL;
	} else if (MML_TMR_STATE_NOT_INITIALIZED == tmr_context.tmr[id].state) {
		return MML_TMR_ERR_NOT_INITIALIZED;
	} else if (!time) {
		return COMMON_ERR_NULL_PTR;
	}

	*time = tmr_context.tmr[id].reg_tmr->CNT;

	return NO_ERROR;
}

int mml_tmr_ioctl(mml_tmr_id_t id, mml_tmr_ioctl_t cmd, void *data) {
	int result = COMMON_ERR_UNKNOWN;

	/* Validate the timer id */
	if (MML_TMR_DEV_COUNT <= id) {
		result = COMMON_ERR_INVAL;
	} else if (MML_TMR_STATE_INITIALIZED != tmr_context.tmr[id].state) {
		result = MML_TMR_ERR_NOT_INITIALIZED;
	} else if (MML_TMR_STATUS_ENABLED == tmr_context.tmr[id].status) {
		result = MML_TMR_ERR_NOT_ALLOWED;
	} else if (!data) {
		result = COMMON_ERR_NULL_PTR;
	} else {
		unsigned int temp;
		unsigned int tmp = 0;
		mml_tmr_config_t *config;

		switch (cmd) {
		case MML_TMR_CMD_SET_PRESCALE:
			temp = *(TMRn_CN_PRES_Type*) data;
			if (TMRn_CN_PRES_div4096 < temp) {
				result = COMMON_ERR_OUT_OF_RANGE;
			} else {
				/* Retrieve "old" control register value */
				tmp = tmr_context.tmr[id].reg_tmr->CN;
				/* Remove presacler */
				tmp &= ~(TMRn_CN_PRES_Msk | TMRn_CN_PRES3_Msk);
				/* Set new value */
				/* TMR0CTRL[5:3] = PRES[2:0] */
				tmp |= ((temp & 0x07) << TMRn_CN_PRES_Pos);
				/* TMR0CTRL[8] = PRES[3] */
				tmp |= (((temp & 0x08)>>3) << TMRn_CN_PRES3_Pos);
				tmr_context.tmr[id].reg_tmr->CN = tmp;
				result = NO_ERROR;
			}
			break;
		case MML_TMR_CMD_SET_MODE:
			config = (mml_tmr_config_t*) data;
			if (TMRn_CN_TMODE_captureCompare < config->mode) {
				result = COMMON_ERR_OUT_OF_RANGE;
			} else if ((TMRn_CN_TMODE_pwm == config->mode)
					&& (tmr_context.tmr[id].reg_tmr->CMP < config->pwm_value)) {
				result = COMMON_ERR_OUT_OF_RANGE;
			} else {
				if (TMRn_CN_TMODE_pwm == config->mode) {
					tmr_context.tmr[id].reg_tmr->PWM = config->pwm_value;
				}
				/* Retrieve "old" control register value */
				tmp = tmr_context.tmr[id].reg_tmr->CN;
				/* Remove mode */
				tmp &= ~TMRn_CN_TMODE_Msk;
				/* Set new value */
				/* TMR0CTRL[2:0] = TMODE[2:0] */
				tmp |= config->mode;
				tmr_context.tmr[id].reg_tmr->CN = tmp;
				result = NO_ERROR;
			}
			break;
		case MML_TMR_CMD_SET_IO_POLARITY:
			temp = *(TMRn_CN_TPOL_Type*) data;
			if (TMRn_CN_TPOL_activeLo < temp) {
				result = COMMON_ERR_OUT_OF_RANGE;
			} else {
				/* Retrieve "old" control register value */
				tmp = tmr_context.tmr[id].reg_tmr->CN;
				/* Remove polarity */
				tmp &= ~TMRn_CN_TPOL_Msk;
				/* Set new value */
				tmp |= (temp << TMRn_CN_TPOL_Pos);
				tmr_context.tmr[id].reg_tmr->CN = tmp;
				result = NO_ERROR;
			}
			break;
		default:
			result = COMMON_ERR_NO_MATCH;
			break;
		}
	}
	return result;
}

int mml_tmr_callback_handler_register(mml_tmr_id_t id,
		mml_tmr_handler_t handler) {
	/* Validate the timer id */
	if (MML_TMR_DEV_COUNT <= id) {
		return COMMON_ERR_INVAL;
	} else if (MML_TMR_STATE_INITIALIZED != tmr_context.tmr[id].state) {
		return MML_TMR_ERR_NOT_INITIALIZED;
	} else if (MML_TMR_STATUS_ENABLED == tmr_context.tmr[id].status) {
		return MML_TMR_ERR_NOT_ALLOWED;
	} else if (!handler) {
		return COMMON_ERR_NULL_PTR;
	}

	tmr_context.tmr[id].handler = handler;

	return NO_ERROR;
}

int mml_tmr_callback_handler_unregister(mml_tmr_id_t id) {
	/* Validate the timer id */
	if (MML_TMR_DEV_COUNT <= id) {
		return COMMON_ERR_INVAL;
	} else if (MML_TMR_STATE_INITIALIZED != tmr_context.tmr[id].state) {
		return MML_TMR_ERR_NOT_INITIALIZED;
	} else if (MML_TMR_STATUS_ENABLED == tmr_context.tmr[id].status) {
		return MML_TMR_ERR_NOT_ALLOWED;
	}

	tmr_context.tmr[id].handler = 0;
	return NO_ERROR;
}

int mml_tmr_interrupt_enable(mml_tmr_id_t id) {

	if (id >= MML_TMR_DEV_COUNT) {
		return MML_TMR_ERR_IRQ_SET;
	}

	/* Enable timer's irq */
	NVIC_EnableIRQ(timer_irqs[id]);
	return NO_ERROR;
}

int mml_tmr_interrupt_disable(mml_tmr_id_t id) {

	if (id >= MML_TMR_DEV_COUNT) {
		return MML_TMR_ERR_IRQ_SET;
	}

	/* Disable timer's irq */
	NVIC_DisableIRQ(timer_irqs[id]);
	return NO_ERROR;
}

extern inline void mml_tmr_interrupt_clear(mml_tmr_id_t id) {

	if (id >= MML_TMR_DEV_COUNT) {
		return;
	}

	/* Clear the source level interrupt */
	tmr_context.tmr[id].reg_tmr->INT = TMRn_INT_IRQ_CLR_Msk;
	NVIC_ClearPendingIRQ(timer_irqs[id]);
}

int mml_tmr_set_timeout_us(mml_tmr_id_t id, unsigned int timeout) {
	unsigned int	prescaler = 0;
	unsigned int	tmp		  = 0;
	unsigned int	result	  = COMMON_ERR_UNKNOWN;
	unsigned int	freq_apb;
	unsigned int	div_ratio = TMRn_CN_PRES_div1;
	unsigned int	max_timeout;

	if (id >= MML_TMR_DEV_COUNT) {
		return COMMON_ERR_INVAL;
	}

	/* First, check that timer is NOT enable */
	/* Retrieve old value ... */
	tmp = tmr_context.tmr[id].reg_tmr->CN;
	if (tmp & TMRn_CN_TEN_Msk) {
		return MML_TMR_ERR_ENABLE;
	}

	/* Get apb frequency */
	result = mml_get_apb_frequency(&freq_apb);
	if(result)
		return result;

	max_timeout   = (unsigned int)(-1)/freq_apb;
	prescaler     = (GCR->CLKCN & GCR_CLKCN_PSC_Msk) >> GCR_CLKCN_PSC_Pos;
	max_timeout <<= prescaler;

	while(div_ratio <= (TMRn_CN_PRES_div4096)) {
		/* Check if we can set timeout with this timer prescaler value */
		if (timeout > (unsigned long long)max_timeout * 1000000) {
			div_ratio++;
			max_timeout <<= 1; //mul 2
		} else {
			timeout = timeout/(1 << div_ratio);
			timeout = (unsigned int)((timeout * (unsigned long long)freq_apb) /1000000);
			tmr_context.tmr[id].reg_tmr->CMP = timeout;

			/* TMR0CTRL[5:3] = PRES[2:0] */
			tmp = ((div_ratio & 0x07) << TMRn_CN_PRES_Pos);
			/* TMR0CTRL[8] = PRES[3] */
			tmp |= (((div_ratio & 0x08)>>3) << 8 );
			tmr_context.tmr[id].reg_tmr->CN &= ~TMRn_CN_PRES_Msk;
			tmr_context.tmr[id].reg_tmr->CN &= ~TMRn_CN_PRES3_Msk;
			tmr_context.tmr[id].reg_tmr->CN |= tmp;
			return NO_ERROR;
		}
	}
	return COMMON_ERR_INVAL;
}

int mml_tmr_mode_ctrl(mml_tmr_id_t id, mml_tmr_config_t config) {
	unsigned int tmp;

	/* First, check that timer is NOT enable */
	/* Retrieve old value ... */
	tmp = tmr_context.tmr[id].reg_tmr->CN;
	if (tmp & TMRn_CN_TEN_Msk) {
		return MML_TMR_ERR_ENABLE;
	}
	/*  Check timer mode */
	else if ((TMRn_CN_TMODE_pwm == config.mode)
			&& (config.timeout < config.pwm_value)) {
		return COMMON_ERR_NO_MATCH;
	} else {
		tmr_context.tmr[id].reg_tmr->CMP = config.timeout;
		tmr_context.tmr[id].reg_tmr->CNT = config.count;
		if (TMRn_CN_TMODE_pwm == config.mode) {
			tmr_context.tmr[id].reg_tmr->PWM = config.pwm_value;
		}
		/* ... to remove mode ... */
		tmp &= ~TMRn_CN_TMODE_Msk;
		/* ... polarity */
		tmp &= ~TMRn_CN_TPOL_Msk;
		/* ... and prescaler */
		tmp &= ~(TMRn_CN_PRES_Msk | TMRn_CN_PRES3_Msk);
		/* Now, set new value */
		/* TMR0CTRL[2:0] = TMODE[2:0] */
		tmp |= config.mode;
		/* TMR0CTRL[5:3] = PRES[2:0] */
		tmp |= ((config.clock & 0x07) << TMRn_CN_PRES_Pos);
		/* TMR0CTRL[8] = PRES[3] */
		tmp |= (((config.clock & 0x08)>>3) << 8);
		tmp |= (config.polarity << TMRn_CN_TPOL_Pos);
		tmr_context.tmr[id].reg_tmr->CN = tmp;
		return NO_ERROR;
	}
}

void mml_tmr_irq_handler(mml_tmr_id_t id) {

	if (id >= MML_TMR_DEV_COUNT) {
		return;
	}

	if (tmr_context.tmr[id].handler) {
		tmr_context.tmr[id].handler();
	}
}

void TIMER0_IRQHandler(void) {
	mml_tmr_irq_handler(MML_TMR_DEV0);
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(Timer0_IRQn);
}

void TIMER1_IRQHandler(void) {
	mml_tmr_irq_handler(MML_TMR_DEV1);
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(Timer1_IRQn);
}

void TIMER2_IRQHandler(void) {
	mml_tmr_irq_handler(MML_TMR_DEV2);
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(Timer2_IRQn);
}

void TIMER3_IRQHandler(void) {
	mml_tmr_irq_handler(MML_TMR_DEV3);
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(Timer3_IRQn);
}

void TIMER4_IRQHandler(void) {
	mml_tmr_irq_handler(MML_TMR_DEV4);
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(Timer4_IRQn);
}

void TIMER5_IRQHandler(void) {
	mml_tmr_irq_handler(MML_TMR_DEV5);
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(Timer5_IRQn);
}

const char * mml_tmr_get_version(void) {
	return MML_TMR_VERSION_STRING;
}

#endif /* _MML_TMR_H_ */

/* EOF */

