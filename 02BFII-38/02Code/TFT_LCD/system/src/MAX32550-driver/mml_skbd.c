/*
 * @file mml_skbd.c MML SKBD Driver C File
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

#ifdef _MML_SKBD_H_

#include <errors.h>
#include <string.h>

/** Variables ******************************************************************/
/** Keypad context info */
__attribute__((section(".bss")))      mml_skbd_context_t mml_skbd_context;


void mml_keypad_pre_init(void) {

	/* Attach vector */
	NVIC_SetVector(Secure_Keypad_IRQn, (uint32_t)SKBD_IRQHandler);

	/* Set priority level for interrupt */
	NVIC_SetPriority(Secure_Keypad_IRQn, priority_table[Secure_Keypad_IRQn]);
}

int mml_keypad_set_interrupt_priority(mml_intc_prio_t priority) {
	/* Check if priority is in correct range */
	if (priority > MML_INTC_PRIO_7)
		return COMMON_ERR_INVAL;

	/* Then update priority_table */
	priority_table[Secure_Keypad_IRQn] = priority;
	/* then set interrupt priority */
	NVIC_SetPriority(Secure_Keypad_IRQn, priority);

	return NO_ERROR;
}

int mml_keypad_init(mml_skbd_config_t config) {
	int result = NO_ERROR;

	if (!mml_skbd_context.first_init) {
		unsigned int temp;
		/* Number of output pins */
		unsigned int outputs = 0;
		/* Check for the I/O pin overlaps */
		if ((config.outputs & config.inputs) || !config.inputs
				|| !config.outputs) {
			return MML_SKBD_ERR_INVALID_PIN_CONFIGURATION;
		}
		/* Memset like procedure */
		memset((unsigned char*) &mml_skbd_context, 0x00,
				sizeof(mml_skbd_context));
#ifdef _SKBD_RESET_AT_INIT_
		/* Reset the keypad controller */
		GCR->RSTR |= GCR_RSTR_KBD_Msk;
		/* Wait until keypad reset completes */
		while( GCR_RSTR_KBD_Msk & GCR->RSTR );
#endif /* _SKBD_RESET_AT_INIT_ */
		/* Enable the keypad clock i.e. just in case clock is disabled */
		GCR->PERCKCN &= ~(GCR_PERCKCN_KBDD_Msk);
		/* I/O pin direction selection for the keypad pins */
		/* Configure keypad output pins */
		SKBD->CR0 |= config.outputs;
		/* Configure keypad input pins */
		SKBD->CR0 &= ~(config.inputs);
		/* Configure the keypad input/output pins(GPIO's) to Primary alternate functionality KBD[7:0]*/
		int inputsGPIO0 = (config.inputs & 0x0FF);
		int outputsGPIO0 = (config.outputs & 0x0FF);
		/* Configure KBD[7:0] */
		GPIO0->EN &= ~(inputsGPIO0 | outputsGPIO0);
		GPIO0->EN1 &= ~(inputsGPIO0 | outputsGPIO0);
		GPIO0->PAD_CFG1 |= (inputsGPIO0 | outputsGPIO0);
		GPIO0->PAD_CFG2 &= ~(inputsGPIO0 | outputsGPIO0);
		/* Count the number of output keypad lines */
		temp = config.outputs;
		while (temp) {
			temp &= (temp - 1);
			outputs++;
		}
		/* Configure the keypad  */
		temp = ((( MML_SKBD_CR1_CLEAR_MASK_NOOFST & config.reg_erase)
				<< SKBD_CR1_CLEAR_Pos) | SKBD_CR1_AUTOEN_Msk);
		temp |= (config.debounce << SKBD_CR1_DBTM_Pos) & SKBD_CR1_DBTM_Msk;
		temp |= (outputs << SKBD_CR1_OUTNB_Pos) & SKBD_CR1_OUTNB_Msk;
		SKBD->CR1 |= temp;
		while (!( SKBD->SR & SKBD_SR_BUSY_Msk));
		/* Setup IRQ */
		if (config.irq_handler) {
			mml_skbd_context.irq_handler = config.irq_handler;
			NVIC_EnableIRQ(Secure_Keypad_IRQn);
		}
		/* To be done once only */
		mml_skbd_context.first_init = 1;
		mml_skbd_context.state = MML_SKBD_STATE_INITIALIZED;
	} else {
		result = MML_SKBD_ERR_ALREAD_INITIALIZED;
	}
	return result;
}

int mml_keypad_enable_interrupt_events(unsigned int events) {
	int result = NO_ERROR;

	if (MML_SKBD_STATE_INITIALIZED != mml_skbd_context.state) {
		result = MML_SKBD_ERR_NOT_INITIALIZED;
	} else {
		events &= (MML_SKBD_EVENT_PUSH | MML_SKBD_EVENT_RELEASE | MML_SKBD_EVENT_OVERRUN);
		SKBD->IER |= events;
	}
	return result;
}

extern inline int mml_keypad_disable_interrupt_events(unsigned int events) {
	events &= (MML_SKBD_EVENT_PUSH | MML_SKBD_EVENT_RELEASE | MML_SKBD_EVENT_OVERRUN);
	SKBD->IER &= ~events;
	return NO_ERROR;
}

extern inline int mml_keypad_clear_interrupt_status(unsigned int status) {
	status &= (MML_SKBD_INTERRUPT_STATUS_PUSHIS | MML_SKBD_INTERRUPT_STATUS_RELEASEIS | MML_SKBD_INTERRUPT_STATUS_OVERIS);
	SKBD->ISR &= ~status;
	return NO_ERROR;
}

extern inline int mml_keypad_interrupt_status(unsigned int *status) {
	if (status == NULL) {
		return COMMON_ERR_NULL_PTR;
	}
	*status = SKBD->ISR;
	return NO_ERROR;
}

int mml_keypad_read_keys(mml_skbd_keys_t *keys) {
	volatile unsigned short *key;
	volatile unsigned int i = 0;
	volatile unsigned int temp;
	volatile unsigned int *key_reg;

	if (keys == NULL) {
		return COMMON_ERR_NULL_PTR;
	}
	
	key = (unsigned short*) &keys->key0;
	key_reg = (unsigned int*) &SKBD->K0R;
	for (i = 0; i < MML_SKBD_TOTAL_KEY_REGS; i++) {
		if (!( SKBD->CR1 & SKBD_CR1_CLEAR_Msk)
				&& ( SKBD->IER & SKBD_IER_PUSHIE_Msk)) {
			if (!(*key_reg & ( SKBD_KR_PUSH_Msk | SKBD_KR_READ_Msk))) {
				*key++ = ((*key_reg & SKBD_KR_IOIN_Msk) | ((*key_reg & SKBD_KR_IOOUT_Msk) >> 1) | MML_SKBD_KEY_PRESS_FLAG);
			}
		} else if (!( SKBD->CR1 & SKBD_CR1_CLEAR_Msk)
				&& ( SKBD->IER & SKBD_IER_RELEASEIE_Msk)) {
			temp = *key_reg;
			if ((temp & SKBD_KR_PUSH_Msk) && !(temp & SKBD_KR_READ_Msk)) {
				*key++ = ((*key_reg & SKBD_KR_IOIN_Msk) | ((*key_reg & SKBD_KR_IOOUT_Msk) >> 1) | MML_SKBD_KEY_PRESS_FLAG);
			}
		} else {
			temp = *key_reg;
			if (!(temp & SKBD_KR_READ_Msk)) {
				*key++ = ((temp & SKBD_KR_IOIN_Msk) | ((temp & SKBD_KR_IOOUT_Msk) >> 1) | MML_SKBD_KEY_PRESS_FLAG);
			}
		}
		key_reg++;
	}
	return NO_ERROR;
}

int mml_keypad_close(void) {
	/* Reset the keypad controller */
	GCR->RSTR |= GCR_RSTR_KBD_Msk;
	/* Wait until keypad reset completes */
	while ( GCR_RSTR_KBD_Msk & GCR->RSTR);
	NVIC_DisableIRQ(Secure_Keypad_IRQn);
	mml_skbd_context.state = MML_SKBD_STATE_CLOSED;
	mml_skbd_context.first_init = 0;
	return NO_ERROR;
}

void SKBD_IRQHandler(void) {
	if (mml_skbd_context.irq_handler) {
		mml_skbd_context.irq_handler();
	}
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(Secure_Keypad_IRQn);
}

const char * mml_skbd_get_version(void) {
	return MML_SKBD_VERSION_STRING;
}

#endif //_MML_SKBD_H_

/******************************************************************************/
/* EOF */
