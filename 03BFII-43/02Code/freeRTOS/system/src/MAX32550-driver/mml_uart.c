/*
 * @file mml_uart.c MML UART Driver C File
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

#ifdef _MML_UART_H_

#include <errors.h>

#define	K_MML_UART_MAX_SYST_FREQ			217000000

/** Should be in .bss section */
__attribute__((section(".bss")))                                                mml_uart_context_t mml_uart_context;

void mml_uart_pre_init(void) {

	/* Attach vector */
	NVIC_SetVector(UART0_IRQn, (uint32_t) UART0_IRQHandler);
	NVIC_SetVector(UART1_IRQn, (uint32_t) UART1_IRQHandler);

	/* Set priority level for interrupt */
	NVIC_SetPriority(UART0_IRQn, priority_table[UART0_IRQn]);
	NVIC_SetPriority(UART1_IRQn, priority_table[UART1_IRQn]);
}

int mml_uart_set_interrupt_priority(int vector, mml_intc_prio_t priority) {
	/* First check if vector is in correct range */
	if((vector > UART1_IRQn) || (vector < UART0_IRQn))
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

int mml_uart_init(mml_uart_id_t port, mml_uart_config_t config) {
	int result = COMMON_ERR_UNKNOWN;
	unsigned int clock1;
	unsigned int clock2;
	mml_gpio_id_t gpio_dev;
	unsigned int gpio_offset;
	mml_gpio_config_t gpio_conf;

	/* First initialization */
	if (!mml_uart_context.first_init) {
		/* Assign interface base address */
		mml_uart_context.port[MML_UART_DEV0].reg = UART0;
		mml_uart_context.port[MML_UART_DEV1].reg = UART1;

#ifdef _UARTn_RESET_AT_INIT_
		/* Reset all UART's interface */
		GCR->RSTR |= (GCR_RSTR_UART0_Msk | GCR_RSTR_UART1_Msk);
		/* Wait for UARTs reset to be done */
		while( (GCR_RSTR_UART0_Msk | GCR_RSTR_UART1_Msk) & GCR->RSTR );
		/* Stop clock of each UART. It'll be set independently */
		GCR->PERCKCN |= ( 1 << MML_PERCKCN_DEV_UART0 );
		GCR->PERCKCN |= ( 1 << MML_PERCKCN_DEV_UART1 );
#endif /* _UARTn_RESET_AT_INIT_ */

		/* To be done once only */
		mml_uart_context.first_init = 1;
	}
	/* Validate the input parameter port */
	switch (port) {
	case MML_UART_DEV0:
		/* UART base assignment */
		mml_uart_context.port[port].reg = UART0;
		/* GPIO base assignment */
		gpio_dev = MML_GPIO_DEV0;
		/* Enable the alternate function for UART0 GPIO0[8:11] */
		gpio_offset = MML_UART0_GPIO_CFG_OFFSET;
		/* Enable the GPIO0 clock */
		clock2 = MML_PERCKCN_DEV_GPIO0;
		/* Enable the UART0 clock */
		clock1 = MML_PERCKCN_DEV_UART0;
		mml_uart_context.port[port].irq = UART0_IRQn;
		break;
	case MML_UART_DEV1:
		/* UART base assignment */
		mml_uart_context.port[port].reg = UART1;
		/* GPIO base assignment */
		gpio_dev = MML_GPIO_DEV0;
		/* Enable the alternate function for UART1 GPIO0[12:15] */
		gpio_offset = MML_UART1_GPIO_CFG_OFFSET;
		/* Enable the GPIO0 clock */
		clock2 = MML_PERCKCN_DEV_GPIO0;
		/* Enable the UART1 clock */
		clock1 = MML_PERCKCN_DEV_UART1;
		mml_uart_context.port[port].irq = UART1_IRQn;
		break;
	default:
		result = COMMON_ERR_OUT_OF_RANGE;
		goto mml_uart_init_out;
	}
	/* Enable chosen UART clock */
	GCR->PERCKCN &= ~(1 << clock1);
	/* Clock up GPIOx */
	GCR->PERCKCN &= ~(1 << clock2);
	/* Activate UART functionality in GPIOx */
	gpio_conf.gpio_direction = MML_GPIO_DIR_OUT;
	gpio_conf.gpio_function = MML_GPIO_SECONDARY_ALT_FUNCTION;
	gpio_conf.gpio_intr_mode = MML_GPIO_INT_MODE_EDGE_TRIGGERED;
	gpio_conf.gpio_intr_polarity = MML_GPIO_INT_POL_RAISING;
	gpio_conf.gpio_pad_config = MML_GPIO_PAD_NORMAL;
	result = mml_gpio_init(gpio_dev, gpio_offset, 4, gpio_conf);
	if (result) {
		goto mml_uart_init_out;
	}
	/* Disable all the UART port interrupts */
	UART0->INT_EN = 0;
	UART0->INT_STAT = 0;
	UART1->INT_EN = 0;
	UART1->INT_STAT = 0;
	/* Configure the UART communication parameters */
	result = mml_uart_set_config(port, &config);
	/* Flush Tx & Rx FIFOs */
	mml_uart_flush_raw(port, (UARTn_CTRL_TXFLUSH_Msk | UARTn_CTRL_RXFLUSH_Msk));
	mml_uart_init_out: return result;
}

int mml_uart_reset_interface(void) {
	unsigned int loop = K_COBRA_RESET_WAIT_LOOP_MAX;

	/* Launch all UARTs reset */
	GCR->RSTR |= (GCR_RSTR_UART0_Msk | GCR_RSTR_UART1_Msk);
	while ((GCR->RSTR & (GCR_RSTR_UART0_Msk | GCR_RSTR_UART1_Msk)) && loop--);
	if (loop == 0)
		return COMMON_ERR_IN_PROGRESS;
	return NO_ERROR;
}

int mml_uart_set_config(mml_uart_id_t port, mml_uart_config_t *config) {
	int result = NO_ERROR;
	register unsigned int temp = 0;
	volatile float 			div = 0.0;
	volatile unsigned int 	idiv;
	volatile unsigned int	ddiv;

	/* Validate the input parameter port */
	if (MML_UART_DEV_COUNT <= port) {
		return COMMON_ERR_OUT_OF_RANGE;
	}
	if (!config) {
		return COMMON_ERR_NULL_PTR;
	}
	/* Check UART port is initialized */
	if (!mml_uart_context.first_init) {
		return COMMON_ERR_BAD_STATE;
	}
	/* Check the configuration parameters */
	if (UARTn_CTRL_SIZE_bits8 < config->data_bits) {
		return COMMON_ERR_INVAL;
	}
	if (MML_UART_PARITY_NONE < config->parity) {
		return COMMON_ERR_INVAL;
	}
	/* If no parity is specified then ignore the parameter parity_mode */
	if ((MML_UART_PARITY_NONE != config->parity)
			&& (MML_UART_PARITY_MODE_ZEROS < config->parity_mode)) {
		return COMMON_ERR_INVAL;
	}
	if (UARTn_CTRL_STOP_stop15 < config->stop_bits) {
		return COMMON_ERR_INVAL;
	}
	if (UARTn_CTRL_RTSCTSF_enable < config->flwctrl) {
		return COMMON_ERR_INVAL;
	}
	if ((UARTn_CTRL_RTSCTSF_disable == config->flwctrl)
			&& (UARTn_PIN_RTS_hi < config->rts_ctl)) {
		return COMMON_ERR_INVAL;
	}
	/* Check the baud rate range */
	if (( MML_UART_BAUD_RATE_MIN > config->baudrate)
			|| ( MML_UART_BAUD_RATE_MAX < config->baudrate)) {
		return COMMON_ERR_INVAL;
	}
	/* Check if the UART is busy (not receiving nor transmitting data) */
	while ( (mml_uart_context.port[port].reg->STAT
			& (UARTn_STAT_TXBUSY_Msk | UARTn_STAT_RXBUSY_Msk)) );
	/* Formula for calculating the baud rate
	 * DIV = (Nominal Frequency) /(128 * Fdesired)
	 * IDIV = (integer) DIV
	 * DDIV = (DIV - IDIV) * 128
	 */
	/* Compute the integer part(IDIV) of DIV */
	div  = (K_MML_UART_MAX_SYST_FREQ / (MML_UART_BRR_DIV_FACTOR * (float)config->baudrate));
	idiv = (unsigned int)div;
	/* Compute the Decimal part(DDIV) of DIV */
	mml_uart_context.port[port].reg->BAUD0 = UARTn_BAUD0_IDIV_Msk & idiv;
	/* Compute the Decimal part(DDIV) of DIV */
	ddiv = (unsigned int)((div - (float)idiv) * MML_UART_BRR_DIV_FACTOR);
	mml_uart_context.port[port].reg->BAUD1 = UARTn_BAUD1_DDIV_Msk & ddiv;
	/* Set the communication control parameters */
	/* Set data bit transfer size */
	temp = mml_uart_context.port[port].reg->CTRL;
	temp &= ~UARTn_CTRL_SIZE_Msk;
	temp |= (config->data_bits << UARTn_CTRL_SIZE_Pos);
	/* Set UART Parity */
	if (MML_UART_PARITY_NONE == config->parity) {
		/* Disable the parity */
		temp &= ~UARTn_CTRL_PAREN_Msk;
	} else {
		/* Set the parity */
		temp |= UARTn_CTRL_PAREN_Msk;
		temp &= ~UARTn_CTRL_PAREO_Msk;
		temp |= (config->parity << UARTn_CTRL_PAREO_Pos);
		/* Set the parity mode based on 0's or 1's */
		if (MML_UART_PARITY_NONE != config->parity) {
			temp &= ~UARTn_CTRL_PARMD_Msk;
			temp |= (config->parity_mode << UARTn_CTRL_PARMD_Pos);
		}
	}
	/* Set stop bits */
	temp &= ~UARTn_CTRL_STOP_Msk;
	temp |= (config->stop_bits << UARTn_CTRL_STOP_Pos);
	/* Set hardware flow control */
	temp &= ~UARTn_CTRL_RTSCTSF_Msk;
	temp |= (config->flwctrl << UARTn_CTRL_RTSCTSF_Pos);
	/* Set character threshold to 1 */
	temp &= ~UARTn_CTRL_RXTHD_Msk;
	temp |= (0x1 << UARTn_CTRL_RXTHD_Pos);
	/* Finally write the configurations in the UART control register */
	mml_uart_context.port[port].reg->CTRL = temp;
	/* UART RTS IO level [Pin register] */
	if (UARTn_CTRL_RTSCTSF_disable != config->flwctrl) {
		mml_uart_context.port[port].reg->PIN &= ~UARTn_PIN_RTS_Msk;
		mml_uart_context.port[port].reg->PIN |= (config->rts_ctl << UARTn_PIN_RTS_Pos);
	}
	/* Check if handler is not null */
	if (config->handler) {
		/* Enable the interrupt */
		mml_uart_context.port[port].handler = config->handler;
		NVIC_EnableIRQ(mml_uart_context.port[port].irq);
	}
	return result;
}

int mml_uart_read_char(mml_uart_id_t port, unsigned char *data) {
	register unsigned int tmp;

	/* Validate the input parameter port */
	if (MML_UART_DEV_COUNT <= port) {
		return COMMON_ERR_OUT_OF_RANGE;
	} else if (!data) {
		return COMMON_ERR_NULL_PTR;
	}
	/* Check UART port is initialized */
	else if (!mml_uart_context.port[port].reg) {
		return COMMON_ERR_NOT_INITIALIZED;
	} else {
		/* Check if the RX FIFO is empty */
		if (!(mml_uart_context.port[port].reg->STAT & UARTn_STAT_RXEMPTY_Msk)) {
			/* Retrieve character */
			tmp = mml_uart_context.port[port].reg->DATA;
			/* Check if the parity error occurred during the byte reception */
			if (!(tmp & UARTn_DATA_PARITY_Msk)) {
				/* No parity error receive the data */
				*data = (unsigned char) tmp;
				return NO_ERROR;
			} else {
				/* Parity error */
				return COMMON_ERR_NO_MATCH;
			}
		} else
			return COMMON_ERR_RUNNING;
	}
}

int mml_uart_write_char(mml_uart_id_t port, unsigned char data) {
	/* Validate the input parameter port */
	if (MML_UART_DEV_COUNT <= port) {
		return COMMON_ERR_OUT_OF_RANGE;
	}
	/* Check UART port is initialized */
	else if (!mml_uart_context.port[port].reg) {
		return COMMON_ERR_NOT_INITIALIZED;
	} else {
		/* Check if the TX FIFO is full */
		if (!(mml_uart_context.port[port].reg->STAT & UARTn_STAT_TXFULL_Msk)) {
			mml_uart_context.port[port].reg->DATA = data;
			/* Wait EOT */
			while (!(mml_uart_context.port[port].reg->STAT & UARTn_STAT_TXEMPTY_Msk)
					|| (mml_uart_context.port[port].reg->STAT & UARTn_STAT_TXBUSY_Msk));
			return NO_ERROR;
		} else {
			return COMMON_ERR_RUNNING;
		}
	}
}

int mml_uart_flush(mml_uart_id_t port, unsigned int mode) {
	/* Validate the input parameter port */
	if (MML_UART_DEV_COUNT <= port) {
		return COMMON_ERR_OUT_OF_RANGE;
	}
	/* Check UART port is initialized */
	else if (!mml_uart_context.port[port].reg) {
		return COMMON_ERR_NOT_INITIALIZED;
	} else {
		mml_uart_flush_raw(port, mode);
		return NO_ERROR;
	}
}

int mml_uart_port_status(mml_uart_id_t port, int *status) {
	/* Validate the input parameter port */
	if (MML_UART_DEV_COUNT <= port) {
		return COMMON_ERR_OUT_OF_RANGE;
	} else if (!status) {
		return COMMON_ERR_NULL_PTR;
	}
	/* Check UART port is initialized */
	else if (!mml_uart_context.port[port].reg) {
		return COMMON_ERR_NOT_INITIALIZED;
	} else {
		*status = mml_uart_context.port[port].reg->STAT;
		return NO_ERROR;
	}
}

int mml_uart_port_interrupt_status(mml_uart_id_t port, int *status) {
	/* Validate the input parameter port */
	if (MML_UART_DEV_COUNT <= port) {
		return COMMON_ERR_OUT_OF_RANGE;
	} else if (!status) {
		return COMMON_ERR_NULL_PTR;
	}
	/* Check UART port is initialized */
	else if (!mml_uart_context.port[port].reg) {
		return COMMON_ERR_NOT_INITIALIZED;
	} else {
		*status = mml_uart_context.port[port].reg->INT_STAT;
		return NO_ERROR;
	}
}

int mml_uart_deinit(mml_uart_id_t port) {
	int result = NO_ERROR;

	/* Validate the input parameter port */
	if (MML_UART_DEV_COUNT <= port) {
		result = COMMON_ERR_OUT_OF_RANGE;
	}
	/* Check UART port is initialized */
	else if (!mml_uart_context.port[port].reg) {
		result = COMMON_ERR_NOT_INITIALIZED;
	} else {
		unsigned int clock1;
		unsigned int mask;

		mml_uart_flush_raw(port,
				(UARTn_CTRL_TXFLUSH_Msk | UARTn_CTRL_RXFLUSH_Msk));
		/* Disable the UART interrupts */
		mml_uart_context.port[port].reg->INT_EN = 0;
		switch (port) {
		case MML_UART_DEV0:
			/* Disable the UART0 clock */
			clock1 = MML_PERCKCN_DEV_UART0;
			/* Disable the alternate function for UART0 GPIO0[31:28] */
			mask = MML_UART0_GPIO_CFG_MASK;
			break;
		case MML_UART_DEV1:
			/* Disable the UART1 clock */
			clock1 = MML_PERCKCN_DEV_UART1;
			/* Disable the alternate function for UART1 GPIO0[27:24] */
			mask = MML_UART1_GPIO_CFG_MASK;
			break;
		default:
			result = COMMON_ERR_INVAL;
			goto mml_uart_deinit_out;
		}
		GCR->PERCKCN |= (1 << clock1);
		GPIO0->EN |= mask;
		GPIO0->EN1 |= mask;
		mml_uart_context.first_init = 0;
	}
	mml_uart_deinit_out: return result;
}

void mml_uart_flush_raw(mml_uart_id_t port, unsigned int mode) {
	mml_uart_context.port[port].reg->CTRL |= mode;
	while (mml_uart_context.port[port].reg->CTRL & mode);
}

/* IRQ Management *************************************************************/

int mml_uart_callback_handler_register(mml_uart_id_t port,
		mml_uart_handler_t handler) {
	/* Check input parameter */
	if (MML_UART_DEV_COUNT <= port) {
		return COMMON_ERR_INVAL;
	}
	/* Check state */
	else if (!mml_uart_context.port[port].reg) {
		return MML_UART_ERR_NOT_INITIALIZED;
	} else if (!handler) {
		return COMMON_ERR_NULL_PTR;
	} else {
		/* Attach to platform handler */
		mml_uart_context.port[port].handler = handler;
		NVIC_EnableIRQ(mml_uart_context.port[port].irq);
		return NO_ERROR;
	}
}

int mml_uart_callback_handler_unregister(mml_uart_id_t port) {
	/* Check input parameter */
	if (MML_UART_DEV_COUNT <= port) {
		return COMMON_ERR_INVAL;
	}
	/* Check state */
	else if (!mml_uart_context.port[port].reg) {
		return MML_UART_ERR_NOT_INITIALIZED;
	} else {
		/* Detach from platform handler */
		mml_uart_context.port[port].handler = 0;
		return NO_ERROR;
	}
}

int mml_uart_interrupt_activate(mml_uart_id_t port, unsigned char activation) {
	/* Check input port */
	if (MML_UART_DEV_COUNT <= port) {
		return COMMON_ERR_INVAL;
	}

	if (activation) {
		/* Enable UART's irq */
		NVIC_EnableIRQ(mml_uart_context.port[port].irq);
	} else {
		/* Disable UART's irq */
		NVIC_DisableIRQ(mml_uart_context.port[port].irq);
	}
	return NO_ERROR;
}

extern inline void mml_uart_interrupt_clear(mml_uart_id_t port,
		unsigned int uart_irq) {
	/* Clear the source level interrupt */
	mml_uart_context.port[port].reg->INT_STAT &= ~(uart_irq);
}

extern inline void mml_uart_interrupt_set(mml_uart_id_t port,
		unsigned int uart_irq) {
	/* Clear status ... just in case */
	mml_uart_context.port[port].reg->INT_STAT &= ~(uart_irq);
	/* Set the source level interrupt */
	mml_uart_context.port[port].reg->INT_EN |= uart_irq;
}

extern inline void mml_uart_interrupt_ack(mml_uart_id_t port) {
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(mml_uart_context.port[port].irq);
	return;
}


void mml_uart_irq_handler(mml_uart_id_t port) {
	switch (port) {
	case MML_UART_DEV0:
		if (mml_uart_context.port[port].handler) {
			mml_uart_context.port[port].handler();
		}
		break;
	case MML_UART_DEV1:
		if (mml_uart_context.port[port].handler) {
			mml_uart_context.port[port].handler();
		}
		break;
	default:
		break;
	}
}

void UART0_IRQHandler(void) {
	mml_uart_irq_handler(MML_UART_DEV0);
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(UART0_IRQn);
}

void UART1_IRQHandler(void) {
	mml_uart_irq_handler(MML_UART_DEV1);
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(UART1_IRQn);
}

const char * mml_uart_get_version(void) {
	return MML_UART_VERSION_STRING;
}

int mml_uart_init_context_scpa (mml_uart_id_t port) {
	/* First initialization */
	if (!mml_uart_context.first_init) {
		/* Assign interface base address */
		mml_uart_context.port[MML_UART_DEV0].reg = UART0;
		mml_uart_context.port[MML_UART_DEV1].reg = UART1;
		/* To be done once only */
		mml_uart_context.first_init = 1;
	}
	/* Validate the input parameter port */
	switch (port) {
		case MML_UART_DEV0:
			/* UART base assignment */
			mml_uart_context.port[port].reg = UART0;
			mml_uart_context.port[port].irq = UART0_IRQn;
			break;
		case MML_UART_DEV1:
			/* UART base assignment */
			mml_uart_context.port[port].reg = UART1;
			mml_uart_context.port[port].irq = UART1_IRQn;
			break;
		default:
			return COMMON_ERR_INVAL;
	}

	return NO_ERROR;
}

#endif //_MML_UART_H_

/* EOF */
