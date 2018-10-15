/*
 * @file mml_i2c.c MML I2C Driver C File
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

#ifdef _MML_I2C_H_

#include <errors.h>

/** Variables */
__attribute__((section(".bss")))                                                mml_i2c_context_t mml_i2c_context;


/** I2C IRQ handler */
static void mml_i2c_irq_handler(mml_i2c_dev_t devnum) {
	if (mml_i2c_context.port[devnum].irq_handler) {
		mml_i2c_context.port[devnum].irq_handler();
	}
}

void mml_i2c_pre_init(void) {

	/* Attach vector */
	NVIC_SetVector(I2C_IRQn, (uint32_t)I2C_IRQHandler);

	/* Set priority level for interrupt */
	NVIC_SetPriority(I2C_IRQn, priority_table[I2C_IRQn]);
}

int mml_i2c_set_interrupt_priority(int vector, mml_intc_prio_t priority) {
	/* First check if vector is in correct range */
	if(vector != I2C_IRQn)
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

int mml_i2c_init(mml_i2c_dev_t devnum, mml_i2c_config_t *config) {
	int result = COMMON_ERR_UNKNOWN;

	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	}

	if (!mml_i2c_context.first_init[devnum]) {
		if (!config) {
			result = COMMON_ERR_NULL_PTR;
		} else {
			switch (devnum) {
			case MML_I2C_DEV0:
				/* Enable I2C clock */
				GCR->PERCKCN &= ~(GCR_PERCKCN_I2CD_Msk);
#ifdef _I2C_RESET_AT_INIT_
				/* Reset the I2C block */
				GCR->RSTR |= GCR_RSTR_I2C_Msk;
				/* Wait for I2C reset to be done */
				while( GCR->RSTR & GCR_RSTR_I2C_Msk );
#endif /* _I2C_RESET_AT_INIT_ */
				/* Assign interface base address */
				mml_i2c_context.port[MML_I2C_DEV0].reg = I2C;
				/* Initialization to be finished */
				mml_i2c_context.first_init[devnum] = 1;
				/* Set configuration */
				result = mml_i2c_set_configuration(devnum, config);
				if (result) {
					/* Close port */
					mml_i2c_close(devnum);
				}
				break;
			default:
				return COMMON_ERR_INVAL;
			}
		}
	}
	return result;
}

int mml_i2c_set_configuration(mml_i2c_dev_t devnum, mml_i2c_config_t *config) {
	int result = COMMON_ERR_UNKNOWN;

	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!mml_i2c_context.first_init[devnum]) {
		return MML_I2C_ERR_INIT;
	} else if (!config) {
		return COMMON_ERR_NULL_PTR;
	}
	/* Maximum baud rate = 400 kbps */
	if (( MML_I2C_MIN_BAUD_RATE >= config->baudrate)
			|| ( MML_I2C_MAX_BAUD_RATE < config->baudrate)) {
		return MML_I2C_ERR_INVALID_BAUD;
	}
	/* RX FIFO threshold (1 to 8) */
	if (( MML_I2C_MIN_RXFIFO_THRESHOLD >= config->rxfifo_thr)
			|| ( MML_I2C_MAX_RXFIFO_THRESHOLD < config->rxfifo_thr)) {
		return MML_I2C_ERR_INVALID_RXTHRD;
	}

	unsigned int freq;
	mml_gpio_config_t gpio_config;

	mml_i2c_context.port[devnum].reg->CR |= (I2C_CR_TXFLUSH_Msk
			| I2C_CR_RXFLUSH_Msk);
	mml_i2c_context.port[devnum].reg->CR |= config->rxfifo_thr;
	/* Set baud rate */
	mml_get_system_frequency(&freq);
	mml_i2c_context.port[devnum].reg->BRR = freq / (32 * config->baudrate);
	/* Auto-start asked */
	if (config->flags & MML_I2C_AUTOSTART_ENABLE) {
		mml_i2c_context.port[devnum].reg->CR |= I2C_CR_AUTOSTT_Msk;
	} else {
		mml_i2c_context.port[devnum].reg->CR &= ~I2C_CR_AUTOSTT_Msk;
	}

	switch (devnum) {
	case MML_I2C_DEV0:
		/* Configure interrupt */
		if (config->irq_handler) {
			mml_i2c_context.port[devnum].irq_handler = config->irq_handler;
			/* Enable the interrupt */
			NVIC_EnableIRQ(I2C_IRQn);
		}
		/* Configure GPIO multiplexed lines of I2C */
		/* Initialize GPIO for I2C interface SDA I2C Data, SCL I2C Clock */
		gpio_config.gpio_direction = MML_GPIO_DIR_OUT;
		gpio_config.gpio_function = MML_GPIO_SECONDARY_ALT_FUNCTION;
		gpio_config.gpio_intr_mode = 0;
		gpio_config.gpio_intr_polarity = 0;
		gpio_config.gpio_pad_config = MML_GPIO_PAD_NORMAL;
		result = mml_gpio_init(MML_GPIO_DEV0, GPIO_NUM(SDA0), 1, gpio_config); /* Initialize SDA0 P0.23 */
		if (result) {
			result = MML_I2C_ERR_GPIO_CONFIG;
		}
		if (result == NO_ERROR) {
			result = mml_gpio_init(MML_GPIO_DEV0, GPIO_NUM(SCL0), 1, gpio_config); /* Initialize SCL0 P0.24 */
			if (result) {
				result = MML_I2C_ERR_GPIO_CONFIG;
			}
		}
		break;
	default:
		return COMMON_ERR_INVAL;
	}
	if (result) {
		result = MML_I2C_ERR_GPIO_CONFIG;
	} else
		result = NO_ERROR;

	return result;
}

int mml_i2c_write_start(mml_i2c_dev_t devnum, unsigned short address,
		const unsigned char *data, unsigned int *plength) {
	int result = NO_ERROR;
	unsigned int len = 0;
	unsigned int len_temp;
	register unsigned int loop = K_COBRA_I2C_WAIT_LOOP;

	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!mml_i2c_context.first_init[devnum]) {
		return  MML_I2C_ERR_INIT;
	} else if (!data || !plength) {
		return  COMMON_ERR_NULL_PTR;
	}
	/* Check if address is one of 10 bit OR 7 bit pattern */
	if (((address & 0xf900) != 0xf000) && ((address & 0xff01) != 0x0000)) {
		return  MML_I2C_ERR_INVALID_ADDRESS;
	}

	len_temp = *plength;

	/* Wait for TX/RX FIFO to be empty ... just in case */
	while (!(mml_i2c_context.port[devnum].reg->SR & I2C_SR_TXEMPTY_Msk) && loop--);
	if (loop ==0)
		return MML_I2C_ERR_READY;
	/* De-assert RESTART & STOP bits */
	mml_i2c_context.port[devnum].reg->CR &= (~I2C_CR_RESTART_Msk | I2C_CR_STOP_Msk);
	/* Write address AND R/W = 0 */
	if ((address & 0xf900) == 0xf000) {
		/* 10-bit address */
		mml_i2c_context.port[devnum].reg->DR = (address >> 8);
	} else {
		mml_i2c_context.port[devnum].reg->DR = (address & 0x00ff);
	}
	/* Condition to see if we want a read after mml_write_start */
	if(*plength == 1) {
		/* Send data */
		do {
			mml_i2c_context.port[devnum].reg->DR = *data++;
			(*plength)--;
			len++;
			if (!(*plength)) {
				break;
			}
		} while (!(mml_i2c_context.port[devnum].reg->SR & I2C_SR_TXFULL_Msk));
		/*Start I2C transmission */
		mml_i2c_context.port[devnum].reg->CR |= (I2C_CR_START_Msk | I2C_CR_RESTART_Msk);
		*plength = len;
		/* Enable auto-read in order to use function mml_read_start to start a read */
		mml_i2c_context.port[devnum].reg->CR |= I2C_CR_AFREAD_Msk;
	} else {
		if (*plength > 7) {
			/* Freeze I2C bus when TX FIFO is empty in write mode. */
			mml_i2c_context.port[devnum].reg->CR |= I2C_CR_FREEZE_Msk;
		}
		/* Send data */
		do {
			mml_i2c_context.port[devnum].reg->DR = *data++;
			(*plength)--;
			len++;
			if (!(*plength)) {
				break;
			}
		} while (!(mml_i2c_context.port[devnum].reg->SR & I2C_SR_TXFULL_Msk));
		mml_i2c_context.port[devnum].reg->CR |= (I2C_CR_START_Msk | I2C_CR_RESTART_Msk);
		*plength = len_temp - len;
	}
	return result;
}

int mml_i2c_write(mml_i2c_dev_t devnum, unsigned char *data, unsigned int *plength) {
	unsigned int len = 0;
	int ret;

	if (!data || !plength) {
		return COMMON_ERR_NULL_PTR;
	}

	ret = mml_i2c_write_attended(devnum, data, *plength, &len);
	*plength = len;

	return ret;
}

int mml_i2c_write_attended(mml_i2c_dev_t devnum, unsigned char *data, unsigned int len_requested, unsigned int *plen_written) {
	int result = NO_ERROR;
	unsigned int len = len_requested;

	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!mml_i2c_context.first_init[devnum]) {
		return MML_I2C_ERR_INIT;
	} else	if (!data || !plen_written) {
		return COMMON_ERR_NULL_PTR;
	}

	*plen_written = 0;
	/* Check number of byte to transmit */
	if (len_requested > 7) {
		len = 7;
	}
	/* Wait untill there is enough available bytes in TX FIFO to write all your data (maximum 7bytes) */
	while(((mml_i2c_context.port[devnum].reg->SR & I2C_SR_TXELT_Msk)>>I2C_SR_TXELT_Pos) > (8-len));
	while (!(mml_i2c_context.port[devnum].reg->SR & I2C_SR_TXFULL_Msk) && (len != *plen_written)) {
		mml_i2c_context.port[devnum].reg->DR = *data++;
		(*plen_written)++;
	}
	if (len == *plen_written) {
		mml_i2c_context.port[devnum].reg->CR &= ~I2C_CR_START_Msk;
		mml_i2c_context.port[devnum].reg->CR &= ~I2C_CR_FREEZE_Msk;
	}

	return result;
}

int mml_i2c_read_start(mml_i2c_dev_t devnum, unsigned short address) {
	int result = NO_ERROR;
	register unsigned int loop = K_COBRA_I2C_WAIT_LOOP;

	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!mml_i2c_context.first_init[devnum]) {
		return MML_I2C_ERR_INIT;
	}

	/* Check if address is one of 10 bit OR 7 bit pattern */
	if (((address & 0xf900) != 0xf000) && ((address & 0xff01) != 0x0000)) {
		return MML_I2C_ERR_INVALID_ADDRESS;
	}
	/* Wait for TX/RX FIFO to be empty ... just in case */
	while (!(mml_i2c_context.port[devnum].reg->SR & I2C_SR_TXEMPTY_Msk)
			&& loop--);
	if (loop == 0)
		return MML_I2C_ERR_READY;
	/* De assert RESTART & STOP bits */
	mml_i2c_context.port[devnum].reg->CR &= (~I2C_CR_RESTART_Msk | I2C_CR_STOP_Msk);
	/* Write address AND R/W = 1*/
	if ((address & 0xf900) == 0xf000) {
		/* 10-bit address */
		mml_i2c_context.port[devnum].reg->DR = (address >> 8) | 0x01;
	}
	mml_i2c_context.port[devnum].reg->DR = (address & 0x00ff) | 0x01;

	return result;
}

int mml_i2c_read(mml_i2c_dev_t devnum, unsigned char *data, unsigned int *plength) {
	unsigned int len = 0;
	int ret;

	if (!data || !plength) {
		return COMMON_ERR_NULL_PTR;
	}

	ret = mml_i2c_read_attended(devnum, data, *plength, &len);
	*plength = len;

	return ret;
}

int mml_i2c_read_attended(mml_i2c_dev_t devnum, unsigned char *data, unsigned int len_requested, unsigned int *plen_received) {
	int result = NO_ERROR;

	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else	if (!mml_i2c_context.first_init[devnum]) {
		return MML_I2C_ERR_INIT;
	} else if (!data || !plen_received) {
		return COMMON_ERR_NULL_PTR;
	} else {
		*plen_received = 0;
		if (len_requested > 8) {
			/* Enable freeze when RX FIFO will be full */
			mml_i2c_context.port[devnum].reg->CR |= I2C_CR_FREEZE_Msk;
		}
		mml_i2c_context.port[devnum].reg->RCR = len_requested;
		/* Read data untill RX FIFO is empty */
		while (!(mml_i2c_context.port[devnum].reg->SR & I2C_SR_RXEMPTY_Msk) && (len_requested != *plen_received)) {
			*data++ = mml_i2c_context.port[devnum].reg->DR;
			(*plen_received)++;
		}
		if (len_requested == *plen_received) {
			mml_i2c_context.port[devnum].reg->CR &= ~I2C_CR_FREEZE_Msk;
			mml_i2c_context.port[devnum].reg->RCR = 0;
		}
	}

	return result;
}

int mml_i2c_ioctl(mml_i2c_dev_t devnum, int command, void *data) {
	int result = NO_ERROR;

	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!mml_i2c_context.first_init[devnum]) {
		return MML_I2C_ERR_INIT;
	} else {
		switch (command) {
		case MML_I2C_SET_FREEZE:
			mml_i2c_context.port[devnum].reg->CR |= I2C_CR_FREEZE_Msk;
			break;
		case MML_I2C_CLEAR_FREEZE:
			mml_i2c_context.port[devnum].reg->CR &= ~I2C_CR_FREEZE_Msk;
			break;
		case MML_I2C_RXFLUSH:
			mml_i2c_context.port[devnum].reg->CR |= I2C_CR_RXFLUSH_Msk;
			break;
		case MML_I2C_TXFLUSH:
			mml_i2c_context.port[devnum].reg->CR |= I2C_CR_TXFLUSH_Msk;
			break;
		case MML_I2C_SET_READCOUNT:
			if (!data) {
				return COMMON_ERR_NULL_PTR;
			}
			mml_i2c_context.port[devnum].reg->RCR = *((unsigned int *) data);
			break;
		case MML_I2C_CLEAR_READCOUNT:
			mml_i2c_context.port[devnum].reg->RCR = 0;
			break;
		case MML_I2C_SET_AUTOSTART:
			mml_i2c_context.port[devnum].reg->CR |= I2C_CR_AUTOSTT_Msk;
			break;
		case MML_I2C_CLEAR_AUTOSTART:
			mml_i2c_context.port[devnum].reg->CR &= ~I2C_CR_AUTOSTT_Msk;
			break;
		case MML_I2C_SET_AFREAD:
			mml_i2c_context.port[devnum].reg->CR |= I2C_CR_AFREAD_Msk;
			break;
		case MML_I2C_CLEAR_AFREAD:
			mml_i2c_context.port[devnum].reg->CR &= ~I2C_CR_AFREAD_Msk;
			break;
		default:
			return COMMON_ERR_INVAL;
		}
	}
	return result;
}

int mml_i2c_stop(mml_i2c_dev_t devnum) {
	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!mml_i2c_context.first_init[devnum]) {
		return MML_I2C_ERR_INIT;
	}

	mml_i2c_context.port[devnum].reg->CR |= I2C_CR_STOP_Msk;

	return NO_ERROR;
}

int mml_i2c_close(mml_i2c_dev_t devnum) {
	int result = NO_ERROR;

	 if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!mml_i2c_context.first_init[devnum]) {
		return MML_I2C_ERR_INIT;
	} else {
		switch (devnum) {
		case MML_I2C_DEV0:
			/* Reset the I2C block */
			GCR->RSTR |= GCR_RSTR_I2C_Msk;
			while (GCR->RSTR & GCR_RSTR_I2C_Msk);
			/* Disable I2C clock */
			GCR->PERCKCN |= (GCR_PERCKCN_I2CD_Msk);
			/* Make available GPIO pins used by I2C */
			mml_gpio_clear_out_int_wake(MML_GPIO_DEV0, 23, 2); /*SDA0 P0.23, SCL0 P0.24 */
			mml_gpio_disable_output(MML_GPIO_DEV0, 23, 2); /* SDA0 P0.23, SCL0 P0.24 */
			mml_i2c_context.first_init[devnum] = 0;
			break;
		default:
			return COMMON_ERR_INVAL;
		}
	}
	return result;
}

int mml_i2c_enable_interrupt_events(mml_i2c_dev_t devnum, unsigned int events) {

	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!mml_i2c_context.first_init[devnum]) {
		return MML_I2C_ERR_INIT;
	} else {
		mml_i2c_context.port[devnum].reg->IER |= events;
		return NO_ERROR;
	}
}

extern inline int mml_i2c_disable_interrupt_events(mml_i2c_dev_t devnum,
		unsigned int events) {
	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else {
		mml_i2c_context.port[devnum].reg->IER &= ~events;
		return NO_ERROR;
	}
}

extern inline int mml_i2c_clear_interrupt_status(mml_i2c_dev_t devnum,
		unsigned int status) {
	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else {
		mml_i2c_context.port[devnum].reg->ISR &= ~status;
		return NO_ERROR;
	}
}

extern inline int mml_i2c_interrupt_status(mml_i2c_dev_t devnum,
		unsigned int *status) {
	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!status) {
		return COMMON_ERR_NULL_PTR;
	} else {
		*status = mml_i2c_context.port[devnum].reg->ISR;
		return NO_ERROR;
	}
}

int mml_i2c_bus_status(mml_i2c_dev_t devnum, unsigned int *status) {

	if (MML_I2C_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!status) {
		return COMMON_ERR_NULL_PTR;
	}

	*status = mml_i2c_context.port[devnum].reg->SR;

	return NO_ERROR;
}

void I2C_IRQHandler(void) {
	mml_i2c_irq_handler(MML_I2C_DEV0);
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(I2C_IRQn);
}

const char * mml_i2c_get_version(void) {
	return MML_I2C_VERSION_STRING;
}

#endif //_MML_I2C_H_

/******************************************************************************/
/* EOF */

