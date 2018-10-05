/*
 * @file mml_gpio.c MML GPIO Driver C File
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

#ifdef _MML_GPIO_H_

#include <errors.h>

/** GPIO device ID context information */
__attribute__((section(".bss")))                                         mml_gpio_context_t mml_gpio_context;

void mml_gpio_pre_init(void) {

	/* Attach vector */
	NVIC_SetVector(GPIO0_IRQn, (int)GPIO0_IRQHandler);
	NVIC_SetVector(GPIO1_IRQn, (int)GPIO1_IRQHandler);
	NVIC_SetVector(GPIO2_IRQn, (int)GPIO2_IRQHandler);

	/* Set priority level for interrupt */
	NVIC_SetPriority(GPIO0_IRQn, priority_table[GPIO0_IRQn]);
	NVIC_SetPriority(GPIO1_IRQn, priority_table[GPIO1_IRQn]);
	NVIC_SetPriority(GPIO2_IRQn, priority_table[GPIO2_IRQn]);
}

int mml_gpio_set_interrupt_priority(int vector, mml_intc_prio_t priority) {
	/* First check if vector is in correct range */
	if((vector > GPIO2_IRQn) || (vector < GPIO0_IRQn))
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

int mml_gpio_init(mml_gpio_id_t dev_id, int offset, int bits_count,
		mml_gpio_config_t config) {
	int result = COMMON_ERR_UNKNOWN;

	/* First initialization  */
	if (!mml_gpio_context.first_init) {
		mml_gpio_context.port[MML_GPIO_DEV0].reg = GPIO0;
		mml_gpio_context.port[MML_GPIO_DEV1].reg = GPIO1;
		mml_gpio_context.port[MML_GPIO_DEV2].reg = GPIO2;
		mml_gpio_context.port[MML_GPIO_DEV0].vector = GPIO0_IRQn;
		mml_gpio_context.port[MML_GPIO_DEV1].vector = GPIO1_IRQn;
		mml_gpio_context.port[MML_GPIO_DEV2].vector = GPIO2_IRQn;
#ifdef _GPIO_RESET_AT_INIT_
		/* Reset all GPIOs' interface */
		GCR->RSTR |= (GCR_RSTR_GPIO0_Msk | GCR_RSTR_GPIO1_Msk | GCR_RSTR_GPIO2_Msk);
		/* Wait for GPIOs reset to be done */
		while((GCR_RSTR_GPIO0_Msk | GCR_RSTR_GPIO1_Msk | GCR_RSTR_GPIO2_Msk) & GCR->RSTR );
#endif /* _GPIO_RESET_AT_INIT_ */
		/* To be done once only */
		mml_gpio_context.first_init = 1;
	}
	switch (dev_id) {
	case MML_GPIO_DEV0:
		/* Enable GPIO0 clock */
		GCR->PERCKCN &= ~(GCR_PERCKCN_GPIO0D_Msk);
		break;
	case MML_GPIO_DEV1:
		/* Enable GPIO1 clock */
		GCR->PERCKCN &= ~(GCR_PERCKCN_GPIO1D_Msk);
		break;
	case MML_GPIO_DEV2:
		/* Enable GPIO2 clock */
		GCR->PERCKCN &= ~(GCR_PERCKCN_GPIO2D_Msk);
		break;
	default:
		return COMMON_ERR_INVAL;
	}
	/* Check input parameters */
	result = mml_gpio_check(dev_id, offset, bits_count);
	if (result)
		return result;

	result = mml_gpio_set_config(dev_id, offset, bits_count, config);
	if (NO_ERROR == result) {
		mml_gpio_context.port[dev_id].state = MML_GPIO_STATE_INITIALIZED;
	}
	return result;
}

int mml_gpio_reset_interface(void) {
	unsigned int loop = K_COBRA_RESET_WAIT_LOOP_MAX;

	/* Launch all GPIOs reset */
	GCR->RSTR |= (GCR_RSTR_GPIO0_Msk | GCR_RSTR_GPIO1_Msk | GCR_RSTR_GPIO2_Msk);
	/* Wait until it's done */
	while ((GCR->RSTR
			& (GCR_RSTR_GPIO0_Msk | GCR_RSTR_GPIO1_Msk | GCR_RSTR_GPIO2_Msk))
			&& loop--);
	if (loop == 0)
		return COMMON_ERR_IN_PROGRESS;

	return NO_ERROR;
}

int mml_gpio_close(mml_gpio_id_t dev_id) {
	int result = COMMON_ERR_UNKNOWN;

#ifdef _INPUT_STRICT_CHECK_
	/* Validate the input parameter dev_id */
	if ( MML_GPIO_DEV_COUNT <= dev_id ) {
		return MML_GPIO_ERR_INVALID_DEVICE;
	}
#endif /* _INPUT_STRICT_CHECK_ */
	/* Clear OUT, IRQ and WAKE_EN */
	mml_gpio_clear_out_int_wake(dev_id, 0, MML_GPIO_BIT_RANGE_NB);
	/* Disable GPIO's output */
	result = mml_gpio_disable_output(dev_id, 0, MML_GPIO_BIT_RANGE_NB);
	if (NO_ERROR == result) {
		/* RESET not supported by MQ55 currently */
		mml_gpio_context.port[dev_id].state = MML_GPIO_STATE_NOT_INITIALIZED;
	}
	return result;
}

int mml_gpio_set_config(mml_gpio_id_t dev_id, int offset, int bits_count,
		mml_gpio_config_t config) {
	int result = COMMON_ERR_UNKNOWN;

	/* Check input parameters */
	result = mml_gpio_check(dev_id, offset, bits_count);
	if (result) {
		return result;
	}
	/* Clear OUT, IRQ and WAKE_EN */
	mml_gpio_clear_out_int_wake(dev_id, offset, bits_count);

	/* Configure GPIO function for range of pins */
	result = mml_gpio_configure_function(dev_id, config.gpio_function, offset,
			bits_count);
	if ((NO_ERROR == result) && (MML_GPIO_NORMAL_FUNCTION == config.gpio_function)) {
		/* Configure the PAD */
		result = mml_gpio_pad_configure(dev_id, config.gpio_pad_config, offset, bits_count);
		if (NO_ERROR != result) {
			return result;
		}

		/* Check GPIO's direction */
		if (MML_GPIO_DIR_OUT == config.gpio_direction) {
			/* Enable GPIO's output */
			result = mml_gpio_enable_output(dev_id, offset, bits_count);
		} else {
			/* Enable GPIO's input */
			result = mml_gpio_disable_output(dev_id, offset, bits_count);
			if (result) {
				return result;
			}

			/* (config.gpio_intr_mode == 0) && (config.gpio_intr_polarity == 0) 
			 * means no interrupt
			 */
			if (config.gpio_intr_mode + config.gpio_intr_polarity) {
				/* Configure GPIO Interrupt */
				result = mml_gpio_configure_interrupt(dev_id, offset, bits_count,
						config.gpio_intr_mode, config.gpio_intr_polarity);
			}
		}
	}

	return result;
}

int mml_gpio_pad_configure(mml_gpio_id_t dev_id, mml_gpio_pad_config_t config,
		int offset, int bits_count) {
	int result = COMMON_ERR_UNKNOWN;

	result = mml_gpio_check(dev_id, offset, bits_count);
	if (NO_ERROR == result) {
		unsigned int val1;
		unsigned int val2;
		/* Create Mask */
		unsigned int mask = ~(0xFFFFFFFF << bits_count);
		/* Read the current values */
		val1 = mml_gpio_context.port[dev_id].reg->PAD_CFG1;
		val2 = mml_gpio_context.port[dev_id].reg->PAD_CFG2;
		switch (config) {
		case MML_GPIO_PAD_NORMAL:
			/* Clear the corresponding n-bits from position offset */
			val1 &= ~(mask << offset);
			val2 &= ~(mask << offset);
			break;
		case MML_GPIO_PAD_PULLUP:
			/* Set the corresponding n-bits from position offset */
			val1 |= (mask << offset);
			/* Clear the corresponding n-bits from position offset */
			val2 &= ~(mask << offset);
			break;
		case MML_GPIO_PAD_PULLDOWN:
			/* Clear the corresponding n-bits from position offset */
			val1 &= ~(mask << offset);
			/* Set the corresponding n-bits from position offset */
			val2 |= (mask << offset);
			break;
		case MML_GPIO_PAD_WEAK_LATCH:
			/* Set the corresponding n-bits from position offset */
			val1 |= (mask << offset);
			val2 |= (mask << offset);
			break;
		default:
			return MML_GPIO_ERR_OUT_OF_RANGE;
		}
		mml_gpio_context.port[dev_id].reg->PAD_CFG1 = val1;
		mml_gpio_context.port[dev_id].reg->PAD_CFG2 = val2;
		result = NO_ERROR;
	}
	return result;
}

int mml_gpio_configure_function(mml_gpio_id_t dev_id,
		mml_gpio_function_t function, int offset, int bits_count) {
	int result = COMMON_ERR_UNKNOWN;

	result = mml_gpio_check(dev_id, offset, bits_count);
	if (NO_ERROR == result) {
		unsigned int en_val;
		unsigned int en1_val;
		/* Create Mask */
		unsigned int mask = ~(0xFFFFFFFF << bits_count);

		/* Assign the GPIO device base address */
		/* Read the current values */
		en_val = mml_gpio_context.port[dev_id].reg->EN;
		en1_val = mml_gpio_context.port[dev_id].reg->EN1;
		/* Create the register values */
		switch (function) {
		case MML_GPIO_NORMAL_FUNCTION:
			/* Set the corresponding n-bits from position offset */
			en_val |= (mask << offset);
			/* Clear the corresponding n-bits from position offset */
			en1_val &= ~(mask << offset);
			break;
		case MML_GPIO_SECONDARY_ALT_FUNCTION:
			/* Clear the corresponding n-bits from position offset */
			en_val &= ~(mask << offset);
			en1_val &= ~(mask << offset);
			break;
		case MML_GPIO_TERTIARY_ALT_FUNCTION:
			/* Clear the corresponding n-bits from position offset */
			en_val &= ~(mask << offset);
			/* Set the corresponding n-bits from position offset */
			en1_val |= (mask << offset);
			break;
		default:
			return MML_GPIO_ERR_OUT_OF_RANGE;
		}
		mml_gpio_context.port[dev_id].reg->EN = en_val;
		mml_gpio_context.port[dev_id].reg->EN1 = en1_val;
		result = NO_ERROR;
	}
	return result;
}

int mml_gpio_enable_output(mml_gpio_id_t dev_id, int offset, int bits_count) {
	int result = COMMON_ERR_UNKNOWN;

	result = mml_gpio_check(dev_id, offset, bits_count);
	if (NO_ERROR == result) {
		unsigned int val;
		/* Create bits_count Mask */
		unsigned int mask = ~(0xFFFFFFFF << bits_count);
		val = mml_gpio_context.port[dev_id].reg->OUT_EN;
		/* set n-bits from offset position */
		val |= (mask << offset);
		/* Configure the GPIO pins in output mode */
		mml_gpio_context.port[dev_id].reg->OUT_EN = val;
	}
	return result;
}

int mml_gpio_disable_output(mml_gpio_id_t dev_id, int offset, int bits_count) {
	int result = COMMON_ERR_UNKNOWN;

	result = mml_gpio_check(dev_id, offset, bits_count);
	if (NO_ERROR == result) {
		/* Create bits_count Mask */
		unsigned int mask = ~(0xFFFFFFFF << bits_count);
		unsigned int val;
		/* Assign the GPIO base address */
		val = mml_gpio_context.port[dev_id].reg->OUT_EN;
		/* Clear n-bits from offset position */
		val &= ~(mask << offset);
		/* Disable the GPIO output mode */
		mml_gpio_context.port[dev_id].reg->OUT_EN = val;
	}
	return result;
}

int mml_gpio_enable_interrupt(mml_gpio_id_t dev_id, int offset, int bits_count) {
	int result = COMMON_ERR_UNKNOWN;

	result = mml_gpio_check(dev_id, offset, bits_count);
	if (NO_ERROR == result) {

#ifndef COBRA_COMPATIBILITY
		if(mml_gpio_context.port[dev_id].reg->INT_EN == 0) {
			NVIC_EnableIRQ(mml_gpio_context.port[dev_id].vector);
		}
#endif //COBRA_COMPATIBILITY

		/* Create bits_count Mask */
		unsigned int mask = ~(0xFFFFFFFF << bits_count);
		/* Enable the GPIO pins interrupt */
		mml_gpio_context.port[dev_id].reg->INT_EN_SET = (mask << offset);
	}
	return result;
}

int mml_gpio_disable_interrupt(mml_gpio_id_t dev_id, int offset, int bits_count) {
	int result = COMMON_ERR_UNKNOWN;

	result = mml_gpio_check(dev_id, offset, bits_count);
	if (NO_ERROR == result) {
		/* Create bits_count Mask */
		unsigned int mask = ~(0xFFFFFFFF << bits_count);
		/* Disable the GPIO pins interrupt */
		mml_gpio_context.port[dev_id].reg->INT_EN_CLR = (mask << offset);

#ifndef COBRA_COMPATIBILITY
		if(mml_gpio_context.port[dev_id].reg->INT_EN == 0) {
			NVIC_DisableIRQ(mml_gpio_context.port[dev_id].vector);
		}
#endif //COBRA_COMPATIBILITY

	}
	return result;
}

int mml_gpio_clear_interrupt(mml_gpio_id_t dev_id, int offset, int bits_count) {
	int result = COMMON_ERR_UNKNOWN;

	result = mml_gpio_check(dev_id, offset, bits_count);
	if (NO_ERROR == result) {
		/* Create bits_count Mask */
		unsigned int mask = ~(0xFFFFFFFF << bits_count);
		mml_gpio_context.port[dev_id].reg->INT_CLR = (mask << offset);
	}
	return result;
}

int mml_gpio_get_interrupt_status(mml_gpio_id_t dev_id, unsigned int *status) {

	/* Check input parameters */
	if (MML_GPIO_DEV_COUNT <= dev_id) {
		return MML_GPIO_ERR_INVALID_DEVICE;
	}
	if (status == 0) {
		return COMMON_ERR_INVAL;
	}

	/* Get interrupt status */
	*status = mml_gpio_context.port[dev_id].reg->INT_STAT;

	return NO_ERROR;
}

int mml_gpio_config_open_drain(mml_gpio_id_t dev_id, const unsigned int offset,
		unsigned int bits_count, mml_gpio_open_drain_t config) {
	int result = COMMON_ERR_UNKNOWN;

	/* Check input parameters */
	if ((MML_GPIO_DEV0 != dev_id) && (MML_GPIO_OPEN_DRAIN_COUNT > config)) {
		result = MML_GPIO_ERR_OUT_OF_RANGE;
	}
	/* Check the offset is valid */
	else if ( MML_GPIO_BIT_RANGE_MAX < offset) {
		result = MML_GPIO_ERR_OUT_OF_RANGE;
	}
	/* Check the bits count is valid */
	else if (bits_count > ( MML_GPIO_BIT_RANGE_NB - offset)) {
		result = MML_GPIO_ERR_OUT_OF_RANGE;
	} else {
		result = NO_ERROR;
	}
	return result;
}

int mml_gpio_write_dataout(mml_gpio_id_t dev_id, unsigned int data) {
#ifdef _INPUT_STRICT_CHECK_
	/* Validate the input parameter dev_id */
	if ( MML_GPIO_DEV_COUNT <= dev_id ) {
		return MML_GPIO_ERR_INVALID_DEVICE;
	}
#endif /* _INPUT_STRICT_CHECK_ */
	mml_gpio_context.port[dev_id].reg->OUT = data;
	return NO_ERROR;
}

int mml_gpio_read_dataout(mml_gpio_id_t dev_id, unsigned int *data) {
#ifdef _INPUT_STRICT_CHECK_
	/* Validate the input parameter dev_id */
	if ( MML_GPIO_DEV_COUNT <= dev_id ) {
		return MML_GPIO_ERR_INVALID_DEVICE;
	}
#endif /* _INPUT_STRICT_CHECK_ */
	/* Check the data pointer is NULL */
	if (!data) {
		return COMMON_ERR_NULL_PTR;
	}

	*data = mml_gpio_context.port[dev_id].reg->OUT;
	return NO_ERROR;
}

int mml_gpio_read_datain(mml_gpio_id_t dev_id, unsigned int *data) {
#ifdef _INPUT_STRICT_CHECK_
	/* Validate the input parameter dev_id */
	if ( MML_GPIO_DEV_COUNT <= dev_id ) {
		return MML_GPIO_ERR_INVALID_DEVICE;
	}
#endif /* _INPUT_STRICT_CHECK_ */
	/* Check the data pointer is NULL */
	if (!data) {
		return COMMON_ERR_NULL_PTR;
	}

	/* Read DATA_IN register */
	*data = mml_gpio_context.port[dev_id].reg->IN;
	return NO_ERROR;
}

int mml_gpio_write_buffer(mml_gpio_id_t dev_id, unsigned int *buffer,
		int length) {
	int result = NO_ERROR;

#ifdef _INPUT_STRICT_CHECK_
	/* Validate the input parameter dev_id */
	if ( MML_GPIO_DEV_COUNT <= dev_id ) {
		return MML_GPIO_ERR_INVALID_DEVICE;
	}
#endif /* _INPUT_STRICT_CHECK_ */
	/* Check the buffer pointer is NULL */
	if (!buffer) {
		result = COMMON_ERR_NULL_PTR;
	} else {
		register unsigned int i;

		/* Write the data words in loop */
		for (i = 0; i < (unsigned int) length; i++) {
			mml_gpio_write_dataout(dev_id, buffer[i]);
		}
	}
	return result;
}

int mml_gpio_read_buffer(mml_gpio_id_t dev_id, unsigned int *buffer, int length) {
	int result = NO_ERROR;

#ifdef _INPUT_STRICT_CHECK_
	/* Validate the input parameter dev_id */
	if ( MML_GPIO_DEV_COUNT <= dev_id ) {
		return MML_GPIO_ERR_INVALID_DEVICE;
	}
#endif /* _INPUT_STRICT_CHECK_ */
	/* Check the buffer pointer is NULL */
	if (!buffer) {
		result = COMMON_ERR_NULL_PTR;
	} else {
		unsigned int i;

		/*  Read the data words in loop */
		for (i = 0; i < (unsigned int) length; i++) {
			mml_gpio_read_datain(dev_id, buffer + i);
		}
	}
	return result;
}

int mml_gpio_write_bit_pattern(mml_gpio_id_t dev_id, int offset, int bits_count,
		unsigned int data) {
	int result = COMMON_ERR_UNKNOWN;

	result = mml_gpio_check(dev_id, offset, bits_count);
	if (NO_ERROR == result) {
		/* Create bits_count Mask */
		unsigned int mask = ~(0xFFFFFFFF << bits_count);
		unsigned int temp;

		temp = mml_gpio_context.port[dev_id].reg->OUT;
		temp &= ~(mask << offset);
		data &= mask;
		temp |= (data << offset);
		mml_gpio_context.port[dev_id].reg->OUT = temp;
	}
	return result;
}

int mml_gpio_read_bit_pattern(mml_gpio_id_t dev_id, int offset, int bits_count,
		unsigned int *data) {
	int result = COMMON_ERR_UNKNOWN;

	if (!data) {
		return COMMON_ERR_NULL_PTR;
	}

	result = mml_gpio_check(dev_id, offset, bits_count);
	if (NO_ERROR == result) {
		/* Create bits_count Mask */
		unsigned int mask = ~(0xFFFFFFFF << bits_count);

		*data = mml_gpio_context.port[dev_id].reg->IN;
		*data >>= offset;
		*data &= mask;
	}
	return result;
}

int mml_gpio_pin_output(mml_gpio_id_t dev_id, const unsigned int offset,
		mml_gpio_pin_data_t output) {
	int result = NO_ERROR;

#ifdef _INPUT_STRICT_CHECK_
	/* Check input parameters */
	if ( MML_GPIO_DEV_COUNT <= dev_id ) {
		return MML_GPIO_ERR_INVALID_DEVICE;
	}
#endif /* _INPUT_STRICT_CHECK_ */
	/* Check the offset */
	if ( MML_GPIO_BIT_RANGE_MAX < offset) {
		result = MML_GPIO_ERR_OUT_OF_RANGE;
	} else {
		unsigned int temp = 0;

#ifdef _GPIO_DIRECT_OUTPUT_
		temp = mml_gpio_context.port[dev_id].reg->OUT;
#endif /* _GPIO_DIRECT_OUTPUT_ */

		/* Choose according to 'ouput' */
		switch (output) {
		case MML_GPIO_OUT_LOGIC_ZERO:
#ifdef _GPIO_DIRECT_OUTPUT_
			temp &= ~(1 << offset);
			mml_gpio_context.port[dev_id].reg->OUT = temp;
#else
			temp = mml_gpio_context.port[dev_id].reg->OUT_CLR;
			temp |= (1 << offset);
			mml_gpio_context.port[dev_id].reg->OUT_CLR = temp;
#endif /* _GPIO_DIRECT_OUTPUT_ */
			break;
		case MML_GPIO_OUT_LOGIC_ONE:
#ifdef _GPIO_DIRECT_OUTPUT_
			temp |= ( 1 << offset );
			mml_gpio_context.port[dev_id].reg->OUT = temp;
#else
			temp = mml_gpio_context.port[dev_id].reg->OUT_SET;
			temp |= (1 << offset);
			mml_gpio_context.port[dev_id].reg->OUT_SET = temp;
#endif /* _GPIO_DIRECT_OUTPUT_ */
			break;
		default:
			result = MML_GPIO_ERR_OUT_OF_RANGE;
			break;
		}
	}
	return result;
}

int mml_gpio_pin_input(mml_gpio_id_t dev_id, const unsigned int offset,
		mml_gpio_pin_data_t *input) {
	int result = NO_ERROR;

#ifdef _INPUT_STRICT_CHECK_
	/* Check input parameters */
	if ( MML_GPIO_DEV_COUNT <= dev_id ) {
		return MML_GPIO_ERR_INVALID_DEVICE;
	} else if (!input) {
		return COMMON_ERR_NULL_PTR;
	}
#endif /* _INPUT_STRICT_CHECK_ */
	/* Check the offset */
	if ( MML_GPIO_BIT_RANGE_MAX < offset) {
		result = MML_GPIO_ERR_OUT_OF_RANGE;
	} else {
		if (mml_gpio_context.port[dev_id].reg->IN & (1 << offset)) {
			*input = MML_GPIO_OUT_LOGIC_ONE;
		} else {
			*input = MML_GPIO_OUT_LOGIC_ZERO;
		}
	}
	return result;
}

int mml_gpio_check(mml_gpio_id_t dev_id, int offset, int bits_count) {

	/* Check input parameters */
	if (MML_GPIO_DEV_COUNT <= dev_id) {
		return MML_GPIO_ERR_INVALID_DEVICE;
	}
	/* Check the offset */
	else if ( MML_GPIO_BIT_RANGE_MAX < (unsigned int)offset) {
		return MML_GPIO_ERR_OUT_OF_RANGE;
	}
	/* Check the bits count is valid */
	else if (( MML_GPIO_BIT_RANGE_MIN > bits_count)
			|| (bits_count > ( MML_GPIO_BIT_RANGE_NB - offset))) {
		return MML_GPIO_ERR_OUT_OF_RANGE;
	} else {
		return NO_ERROR;
	}
}

void mml_gpio_clear_out_int_wake(mml_gpio_id_t dev_id, int offset,
		int bits_count) {
	unsigned int temp;
	unsigned int mask = ~(0xFFFFFFFF << bits_count);

#ifdef _INPUT_STRICT_CHECK_
	/* Validate the input parameter dev_id */
	if ( MML_GPIO_DEV_COUNT <= dev_id ) {
		return;
	}
#endif /* _INPUT_STRICT_CHECK_ */

	/* Clear GPIO output pins to driver logic zero */
	temp = mml_gpio_context.port[dev_id].reg->OUT;
	/* Clear the corresponding n-bits from position offset */
	temp &= ~(mask << offset);
	mml_gpio_context.port[dev_id].reg->OUT = temp;
	/* Disable corresponding interrupts */
	temp = mml_gpio_context.port[dev_id].reg->INT_EN;
	/* Clear the corresponding n-bits from position offset */
	temp &= ~(mask << offset);
	mml_gpio_context.port[dev_id].reg->INT_EN = temp;
	/* Disable PMU wake for GPIO pins */
	temp = mml_gpio_context.port[dev_id].reg->WAKE_EN;
	/* Clear the corresponding n-bits from position offset */
	temp &= ~(mask << offset);
	mml_gpio_context.port[dev_id].reg->WAKE_EN = temp;
}

void mml_gpio_flip(mml_gpio_id_t dev_id, unsigned int value) {
	register unsigned int old;

#ifdef _INPUT_STRICT_CHECK_
	/* Validate the input parameter dev_id */
	if ( MML_GPIO_DEV_COUNT <= dev_id ) {
		return;
	}
#endif /* _INPUT_STRICT_CHECK_ */

	old = mml_gpio_context.port[dev_id].reg->OUT;
	mml_gpio_context.port[dev_id].reg->OUT = (old & ~value) | value;
	mml_gpio_context.port[dev_id].reg->OUT = (old & ~value);
}

int mml_gpio_configure_interrupt(mml_gpio_id_t dev_id, int offset,
		int bits_count, mml_gpio_intr_mode_t int_mode,
		mml_gpio_intr_polarity_t polarity) {
	unsigned int val_int_mode;
	unsigned int val_polarity;
	unsigned int mask = ~(0xFFFFFFFF << bits_count);
	int result = COMMON_ERR_UNKNOWN;

	/* Check input parameters */
	result = mml_gpio_check(dev_id, offset, bits_count);
	if (result)
		return result;
	
	if (polarity == MML_GPIO_INT_POL_DUAL_EDGE) {
		mml_gpio_context.port[dev_id].reg->INT_MOD |= (mask << offset);
		mml_gpio_context.port[dev_id].reg->INT_DUAL_EDGE |= (mask << offset);
	} else {
		val_int_mode = mml_gpio_context.port[dev_id].reg->INT_MOD;
		val_polarity = mml_gpio_context.port[dev_id].reg->INT_POL;

		/* Configure Interrupt mode & polarity */
		if (int_mode == MML_GPIO_INT_MODE_LEVEL_TRIGGERED) {
			/* Set the corresponding n-bits from position offset */
			val_int_mode &= ~(mask << offset);
			/* Set Polarity */
			if (polarity == MML_GPIO_INT_POL_LOW) {
				val_polarity &= ~(mask << offset);
			} else if (polarity == MML_GPIO_INT_POL_HIGH) {
				val_polarity |= (mask << offset);
			} else {
				return MML_GPIO_ERR_INVALID;
			}
		} else if (int_mode == MML_GPIO_INT_MODE_EDGE_TRIGGERED) {
			val_int_mode |= (mask << offset);
			/* Set Polarity */
			if (polarity == MML_GPIO_INT_POL_FALLING) {
				val_polarity &= ~(mask << offset);
			} else if (polarity == MML_GPIO_INT_POL_RAISING) {
				val_polarity |= (mask << offset);
			} else {
				return MML_GPIO_ERR_INVALID;
			}
		} else {
			return MML_GPIO_ERR_INVALID;
		}

		mml_gpio_context.port[dev_id].reg->INT_MOD = val_int_mode;
		mml_gpio_context.port[dev_id].reg->INT_POL = val_polarity;
	}

	return result;
}

const char * mml_gpio_get_version(void) {
	return MML_GPIO_VERSION_STRING;
}

#ifndef COBRA_COMPATIBILITY
int mml_gpio_attach_handler(mml_gpio_id_t id, unsigned int pin, mml_gpio_handler_t handler, void *param) {

	/* Validate the timer id */
	if (MML_GPIO_DEV_COUNT <= id)
		return MML_GPIO_ERR_INVALID_DEVICE;
	/* Check state */
	if (MML_GPIO_STATE_INITIALIZED != mml_gpio_context.port[id].state) {
		return MML_GPIO_ERR_NOT_INITIALIZED;
	}
	if (!handler) {
		return COMMON_ERR_NULL_PTR;
	}

	/* Check if pin is configured for GPIO */
	if (!(mml_gpio_context.port[id].reg->EN & (1<<pin)))
		return MML_GPIO_ERR_BAD_CONFIG;

	/* Check if GPIO is configured for input */
	if (mml_gpio_context.port[id].reg->OUT_EN & (1<<pin))
		return MML_GPIO_ERR_BAD_CONFIG;

	mml_gpio_context.port[id].tab_handler[pin] = handler;
	mml_gpio_context.port[id].tab_param[pin] = param;
	return NO_ERROR;
}

int mml_gpio_deattach_handler(mml_gpio_id_t id, unsigned int pin) {
	/* Validate the timer id */
	if (MML_GPIO_DEV_COUNT <= id)
		return MML_GPIO_ERR_INVALID_DEVICE;
	/* Check state */
	if (MML_GPIO_STATE_INITIALIZED != mml_gpio_context.port[id].state) {
		return MML_GPIO_ERR_NOT_INITIALIZED;
	} else {
		mml_gpio_context.port[id].tab_handler[pin] = 0;
		return NO_ERROR;
	}
}

static void GPIOn_IRQHandler(mml_gpio_id_t dev) {
	unsigned int gpio_int_stat, gpio_int_stat_update;

	// Memorised GPIO0_INT_STAT register
	gpio_int_stat = mml_gpio_context.port[dev].reg->INT_STAT;
	// Update variable used to treat interrupt correctly
	gpio_int_stat_update = gpio_int_stat;

	for (int k = 0; (k <= MML_GPIO_BIT_RANGE_MAX) && (gpio_int_stat_update) ; k++) {
		//Check which pins has an interrupt pending
		if((gpio_int_stat_update & (1<<k))) {
			//Check if we have attached an handler for this pin
			if (mml_gpio_context.port[dev].tab_handler[k]) {
				mml_gpio_context.port[dev].tab_handler[k](mml_gpio_context.port[dev].tab_param[k]);
			}
			// Clear treated interrupt
			gpio_int_stat_update &= ~(1<<k);
		}
	}

	//Now clear GPIO0_INT_STAT
	mml_gpio_context.port[dev].reg->INT_CLR = gpio_int_stat;
}

void GPIO0_IRQHandler(void) {
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(GPIO0_IRQn);
	/* Find an treat GPIO root cause */
	GPIOn_IRQHandler(MML_GPIO_DEV0);
}

void GPIO1_IRQHandler(void) {
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(GPIO1_IRQn);
	/* Find an treat GPIO root cause */
	GPIOn_IRQHandler(MML_GPIO_DEV1);
}

void GPIO2_IRQHandler(void) {
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(GPIO2_IRQn);
	/* Find an treat GPIO root cause */
	GPIOn_IRQHandler(MML_GPIO_DEV2);
}
#endif //COBRA_COMPATIBILITY

#endif //_MML_GPIO_H_

/*****************************************************************************/
/* EOF */

