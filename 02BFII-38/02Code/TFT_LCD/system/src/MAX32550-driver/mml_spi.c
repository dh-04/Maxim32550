/*
 * @file mml_spi.c MML SPI Driver C File
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

#ifdef _MML_SPI_H_

#include <errors.h>

__attribute__((section(".bss")))         mml_spi_context_t spi_context;

void mml_spi_init_ssel(unsigned int ssel, unsigned int miso, unsigned int mosi,
		unsigned int sclk, unsigned int ssel0, unsigned int ssel1,
		unsigned int ssel2, unsigned int ssel3) {
	unsigned int i, dev, pin, mask0, mask1;
	unsigned int ssel_tab[7] = { miso, mosi, sclk, ssel0, ssel1, ssel2, ssel3 };

	mask0 = 0;
	mask1 = 0;
	/* Set mask for set  MISO, MOSI and SCLK pin */
	for (i = 0; i < 3; i++) {
		dev = GPIO_DEV(ssel_tab[i]);
		pin = GPIO_NUM(ssel_tab[i]);
		switch (dev) {
		case 0:
			mask0 |= (1 << pin);
			break;
		case 1:
			mask1 |= (1 << pin);
			break;
		}
	}
	/* Set mask for slave select */
	for (i = 0; i < 4; i++) {
		dev = GPIO_DEV(ssel_tab[i + 3]);
		pin = GPIO_NUM(ssel_tab[i + 3]);
		switch (dev) {
		case 0:
			if ((1 << i) & ssel)
				mask0 |= (1 << pin);
			break;
		case 1:
			if ((1 << i) & ssel)
				mask1 |= (1 << pin);
			break;
		}
	}
	/* Set GPIO pin */
	if (mask0) {
		GPIO0->EN &= ~mask0;
		GPIO0->EN1 &= ~mask0;
		/* Check if we wanted to set SSEL2_3 */
		if ((ssel & 0b1000) && (ssel3 == SSEL2_3)) {
			/* Set to 1 P0.14 to use secondary function (see schematics) */
			GPIO0->EN1 |= (1 << GPIO_NUM(ssel_tab[6]));
		}
		GPIO0->PAD_CFG1 &= ~mask0;
		GPIO0->PAD_CFG2 &= ~mask0;
	}
	if (mask1) {
		GPIO1->EN &= ~mask1;
		GPIO1->EN1 &= ~mask1;
		GPIO1->PAD_CFG1 &= ~mask1;
		GPIO1->PAD_CFG2 &= ~mask1;
	}
}

int mml_spi_init(mml_spi_dev_t devnum, mml_spi_params_t *pparams) {
	int result = COMMON_ERR_UNKNOWN;

	/* Call of set configuration function */
	result = mml_spi_set_config(devnum, pparams);
	if (NO_ERROR == result) {
		spi_context.port[devnum].initialized = 1;

		/* Set related SPI Port */
		switch (devnum) {
		case MML_SPI_DEV0:
			mml_spi_init_ssel(pparams->ssel, MISO0, MOSI0, SCLK0, SSEL0_0, SSEL0_1, SSEL0_2, SSEL0_3);
			break;
		case MML_SPI_DEV1:
			mml_spi_init_ssel(pparams->ssel, MISO1, MOSI1, SCLK1, SSEL1_0, SSEL1_1, SSEL1_2, SSEL1_3);
			break;
		case MML_SPI_DEV2:
			mml_spi_init_ssel(pparams->ssel, MISO2, MOSI2, SCLK2, SSEL2_0, SSEL2_1, SSEL2_2, SSEL2_3);
			break;
		default:
			return COMMON_ERR_OUT_OF_RANGE;
		}
	}

	return result;
}

int mml_spi_shutdown(mml_spi_dev_t devnum) {
	if (MML_SPI_DEV_COUNT > devnum) {
		spi_context.port[devnum].reg->CNTL &= ~SPIn_CNTL_SPIEN_Msk;
		return NO_ERROR;
		/* Check and select the device */
		switch (devnum) {
		case MML_SPI_DEV0:
			/* Disable SPI0 port */
			GCR->PERCKCN |= (1 << MML_PERCKCN_DEV_SPI0);
			break;
		case MML_SPI_DEV1:
			/* Disable SPI1 port */
			GCR->PERCKCN |= (1 << MML_PERCKCN_DEV_SPI1);
			break;
		case MML_SPI_DEV2:
			/* Disable SPI2 port */
			GCR->PERCKCN |= (1 << MML_PERCKCN_DEV_SPI2);
			break;
		default:
			return COMMON_ERR_INVAL;
			break;
		}
	} else
		return COMMON_ERR_OUT_OF_RANGE;
}

int mml_spi_read_write(mml_spi_dev_t devnum, unsigned char *data_in,
						unsigned char *data_out, unsigned int length)
{
	unsigned int	size = length;
	unsigned char 	*ptr = (unsigned char *)data_in;
	unsigned char 	*ptr_out = (unsigned char *)data_out;


	while( size )
	{
		/** Send data */
		spi_context.port[devnum].reg->DATA = (*ptr) << 8;
		/** Wait 'til it's done */
		while( spi_context.port[devnum].reg->STATUS & SPIn_STATUS_TXST_Msk );

		/** Retrieve data */
		if(data_out == NULL)
		{
			(void) (spi_context.port[devnum].reg->DATA & spi_context.port[devnum].ws_mask);
		}
		else
		{
			*ptr_out = spi_context.port[devnum].reg->DATA & spi_context.port[devnum].ws_mask;
			ptr_out++;
		}

		ptr++;
		size--;
	}
	while( spi_context.port[devnum].reg->STATUS & SPIn_STATUS_TXST_Msk );

	return NO_ERROR;
}


int mml_spi_reset_interface(void) {
	unsigned int loop = K_COBRA_RESET_WAIT_LOOP_MAX;

	/* Launch all SPI reset */
	GCR->RSTR |= (GCR_RSTR_SPI0_Msk | GCR_RSTR_SPI1_Msk | GCR_RSTR_SPI2_Msk);
	/* Wait until it's done */
	while ((GCR->RSTR
			& (GCR_RSTR_SPI0_Msk | GCR_RSTR_SPI1_Msk | GCR_RSTR_SPI2_Msk))
			&& loop--);
	if (loop == 0)
		return COMMON_ERR_IN_PROGRESS;
	return NO_ERROR;
}

int mml_spi_set_config(mml_spi_dev_t devnum, mml_spi_params_t *pparams) {
	int result = COMMON_ERR_UNKNOWN;
	unsigned int tmp;
	unsigned int i;
	unsigned int freq_apb = 0;

	/* Check input parameters */
	if (MML_SPI_DEV_MAX < devnum) {
		return COMMON_ERR_INVAL;
	} else if (!pparams) {
		return COMMON_ERR_NULL_PTR;
	} else if (((pparams->baudrate > SPI_MAX_BAUD_RATE)
			|| (pparams->baudrate < SPI_MIN_BAUD_RATE))
			&& (pparams->mode == SPIn_CNTL_MMEN_master)) {
		return COMMON_ERR_INVAL;
	} else if ((pparams->word_size > SPI_WORDSIZE_MAX)
			|| (pparams->word_size < SPI_WORDSIZE_MIN)) {
		return COMMON_ERR_INVAL;
	}

#ifdef _SPIn_RESET_AT_INIT_
	/*  */
	if ( !spi_context.first_init )
	{
		/* Clock up all SPI ports ... */
		GCR->PERCKCN &= ~( GCR_PERCKCN_SPI0D_Msk | GCR_PERCKCN_SPI1D_Msk | GCR_PERCKCN_SPI2D_Msk );
		/* ... then reset ALL SPI interfaces in a row ... */
		GCR->RSTR |= MML_GCR_RSTR_SPI_MASK;
		while( GCR->RSTR & MML_GCR_RSTR_SPI_MASK );
		/* ... finish with clocking down all interfaces */
		GCR->PERCKCN |= ( GCR_PERCKCN_SPI0D_Msk | GCR_PERCKCN_SPI1D_Msk | GCR_PERCKCN_SPI2D_Msk );
		/* Only once ;) */
		spi_context.first_init = 1;
	}
#endif /* _SPIn_RESET_AT_INIT_ */

	/* Select the device */
	switch (devnum) {
	case MML_SPI_DEV0:
		spi_context.port[devnum].reg = SPI0;
		/* Enable SPI0 port */
		GCR->PERCKCN &= ~(1 << MML_PERCKCN_DEV_SPI0);
		break;
	case MML_SPI_DEV1:
		spi_context.port[devnum].reg = SPI1;
		/* Enable SPI1 port */
		GCR->PERCKCN &= ~(1 << MML_PERCKCN_DEV_SPI1);
		break;
	case MML_SPI_DEV2:
		spi_context.port[devnum].reg = SPI2;
		/* Enable SPI2 port */
		GCR->PERCKCN &= ~(1 << MML_PERCKCN_DEV_SPI2);
		break;
	default:
		return COMMON_ERR_OUT_OF_RANGE;
	}
	/* Retrieve APB frequency */
	result = mml_get_apb_frequency(&freq_apb);
	if (result) {
		return result;
	}

	/* Baud Rate register ****************************************************/
	if (SPIn_CNTL_MMEN_master == pparams->mode) {
		spi_context.port[devnum].reg->BRG = (unsigned int) (freq_apb
				/ (2 * (pparams->baudrate)));
	} else {
		spi_context.port[devnum].reg->BRG = SPI_DFLT_BAUD_RATE;
	}

	/* Mode register *********************************************************/
	tmp = spi_context.port[devnum].reg->MOD;
	tmp &= ~SPIn_MOD_NUMBITS_Msk;
	if ( SPI_WORDSIZE_MAX == pparams->word_size) {
		spi_context.port[devnum].ws = 0;
	} else {
		spi_context.port[devnum].ws = (MML_SPI_MR_NUMBITS_MASK_NOOFST
				& pparams->word_size);
	}
	/* Compute word size mask for further process */
	spi_context.port[devnum].ws_mask = 0;
	for (i = 0; i < spi_context.port[devnum].ws; i++) {
		spi_context.port[devnum].ws_mask |= (unsigned short) (1 << i);
	}
	/* Re-introduce word size */
	tmp |= (spi_context.port[devnum].ws << SPIn_MOD_NUMBITS_Pos);
	/* Remove previous SSV related values */
	tmp &= ~(SPIn_MOD_SSV_Msk | SPIn_MOD_SSIO_Msk);
	/* Set SSV depending on SPI master mode */
	if (SPIn_CNTL_MMEN_master == pparams->mode) {
		/* Slave Select Value */
		if (SPIn_MOD_SSV_hi == pparams->ssv) {
			tmp |= SPIn_MOD_SSV_Msk;
		} else {
			/* No need to do that for now, but just in case */
			tmp &= ~SPIn_MOD_SSV_Msk;
		}
		/* Slave Select I/O */
		if (SPIn_MOD_SSIO_output == pparams->ssio) {
			tmp |= SPIn_MOD_SSIO_Msk;
		} else {
			/* No need to do that for now, but just in case */
			tmp &= ~SPIn_MOD_SSIO_Msk;
		}
	} else if (SPIn_MOD_SSIO_output == pparams->ssio) {
		return COMMON_ERR_INVAL;
	} else {
		tmp &= ~SPIn_MOD_SSIO_Msk;
	}
	/* Remove previous Transmit Left Justify value */
	tmp &= ~SPIn_MOD_TX_LJ_Msk;
	if (SPIn_MOD_TX_LJ_enable == pparams->tlj)
		tmp |= SPIn_MOD_TX_LJ_Msk;
	/* Set new value */
	spi_context.port[devnum].reg->MOD = tmp;

	/* FIFO RX Threshold setting *********************************************/
	tmp = spi_context.port[devnum].reg->DMA_REG;
	if (SPIn_DMA_REG_DMA_EN_enable == pparams->dma_tx.active) {
		/* First erase related bits ... */
		tmp &= ~(SPIn_DMA_REG_TX_DMA_EN_Msk | SPIn_DMA_REG_TX_FIFO_CNT_Msk
				| SPIn_DMA_REG_TX_FIFO_LEVEL_Msk);
		/* ... add clear FIFO bit */
		tmp |= SPIn_DMA_REG_TX_FIFO_CLEAR_Msk;
		/* ... then set new ones */
		tmp |= ((pparams->dma_tx.lvl & MML_SPI_DMAR_TX_FIFO_LVL_MASK_NOOFST)
				<< SPIn_DMA_REG_TX_FIFO_CNT_Pos);
		tmp |= ((pparams->dma_tx.cnt & MML_SPI_DMAR_TX_FIFO_CNT_MASK_NOOFST)
				<< SPIn_DMA_REG_TX_FIFO_CNT_Pos);
		tmp |= SPIn_DMA_REG_TX_DMA_EN_Msk;
	} else
		tmp &= ~SPIn_DMA_REG_TX_DMA_EN_Msk;

	if (SPIn_DMA_REG_DMA_EN_enable == pparams->dma_rx.active) {
		/* First erase related bits ... */
		tmp &= ~(SPIn_DMA_REG_RX_DMA_EN_Msk | SPIn_DMA_REG_RX_FIFO_CNT_Msk
				| SPIn_DMA_REG_RX_FIFO_LEVEL_Msk);
		/* ... add clear FIFO bit */
		tmp |= SPIn_DMA_REG_RX_FIFO_CLEAR_Msk;
		/* ... then set new ones */
		tmp |= ((pparams->dma_rx.lvl & MML_SPI_DMAR_RX_FIFO_LVL_MASK_NOOFST)
				<< SPIn_DMA_REG_RX_FIFO_CNT_Pos);
		tmp |= ((pparams->dma_rx.cnt & MML_SPI_DMAR_RX_FIFO_CNT_MASK_NOOFST)
				<< SPIn_DMA_REG_RX_FIFO_CNT_Pos);
		tmp |= SPIn_DMA_REG_RX_DMA_EN_Msk;
	} else
		tmp &= ~SPIn_DMA_REG_RX_DMA_EN_Msk;
	spi_context.port[devnum].reg->DMA_REG = tmp;

	/* Control register setting **********************************************/
	tmp = spi_context.port[devnum].reg->CNTL;
	if (SPIn_CNTL_MMEN_master == pparams->mode)
		tmp |= SPIn_CNTL_MMEN_Msk;
	else
		tmp &= ~SPIn_CNTL_MMEN_Msk;
	/* WOR */
	if (SPIn_CNTL_WOR_enable == pparams->wor)
		tmp |= SPIn_CNTL_WOR_Msk;
	else
		tmp &= ~SPIn_CNTL_WOR_Msk;
	/* SPI clock polarity */
	if (SPIn_CNTL_CLKPOL_idleHi == pparams->clk_pol)
		tmp |= SPIn_CNTL_CLKPOL_Msk;
	else
		tmp &= ~SPIn_CNTL_CLKPOL_Msk;
	/* SPI clock phase */
	if (SPIn_CNTL_PHASE_inactiveEdge == pparams->phase)
		tmp |= SPIn_CNTL_PHASE_Msk;
	else
		tmp &= ~SPIn_CNTL_PHASE_Msk;
	/* BRG Timer interrupt */
	/* Remove value first ... */
	tmp &= ~SPIn_CNTL_BIRQ_Msk;
	if (SPIn_CNTL_BIRQ_enable == pparams->brg_irq)
		tmp |= SPIn_CNTL_BIRQ_Msk;
	/* Set value */
	spi_context.port[devnum].reg->CNTL = tmp;
	/* No error */
	result = NO_ERROR;
	return result;
}

int mml_spi_transmit(mml_spi_dev_t devnum, unsigned char *data,
		unsigned int length) {
	volatile unsigned int tx;
	unsigned char wordsize;
	unsigned char offset;
	unsigned int size;
	volatile unsigned char *ptr;

	/* Check and select the device */
	if (MML_SPI_DEV_COUNT <= devnum) {
		return COMMON_ERR_OUT_OF_RANGE;
	} else if (0 == spi_context.port[devnum].initialized) {
		return COMMON_ERR_NOT_INITIALIZED;
	} else if (!data) {
		return COMMON_ERR_NULL_PTR;
	} else if (length == 0) {
		return COMMON_ERR_OUT_OF_RANGE;
	}

	/* Retrieve word length */
	if (!spi_context.port[devnum].ws) {
		wordsize = 16;
	} else {
		wordsize = spi_context.port[devnum].ws;
	}

	/* Pointer is aligned on beginning of data
	 * therefore 'offset' is null at first */
	offset 	= 0;
	size 	= length;
	ptr 	= (volatile unsigned char*) data;
	/* Asserts nSS I/O */
	spi_context.port[devnum].reg->MOD &= ~SPIn_MOD_SSV_Msk;
	/* Loop until all data is transmitted */
	while (size) {
		/* Process depending on word size */
		if (8 >= wordsize) {
			/* Prepare data to be sent */
			tx = *ptr;
			tx |= (*(ptr + 1) << 8);
			tx = (tx >> offset);
			tx &= spi_context.port[devnum].ws_mask;
			tx = (tx << (16 - wordsize));
			/* Then removing data to be sent */
			*ptr &=
					~((unsigned char) spi_context.port[devnum].ws_mask << offset);
			if (8 < (wordsize + offset)) {
				*(ptr + 1) &= ~((unsigned char) spi_context.port[devnum].ws_mask
						<< (8 - offset));
			}
			/* Send data */
			spi_context.port[devnum].reg->DATA = tx;
			/* Wait 'til it's done */
			while (spi_context.port[devnum].reg->STATUS & SPIn_STATUS_TXST_Msk);
			/* Retrieve data */
			tx = spi_context.port[devnum].reg->DATA
					& spi_context.port[devnum].ws_mask;
			/* Fill buffer */
			*ptr |= ((unsigned char) tx << offset);
			if (8 < (wordsize + offset)) {
				*(ptr + 1) |= ((unsigned char) tx << (8 - offset));
			}
		} else {
			/* Prepare data to be sent */
			tx = *ptr;
			tx |= (*(ptr + 1) << 8);
			tx |= (*(ptr + 2) << 16);
			tx |= (*(ptr + 3) << 24);
			tx = (tx >> offset);
			tx &= spi_context.port[devnum].ws_mask;
			tx = (tx << (16 - wordsize));
			/* Then removing data to be sent */
			*ptr &= ~((unsigned short) spi_context.port[devnum].ws_mask
					<< offset);
			if (8 <= offset) {
				*(ptr + 1) &=
						~((unsigned short) spi_context.port[devnum].ws_mask
								<< (offset - 8));
			}
			if (16 < (wordsize + offset)) {
				*(ptr + 2) &=
						~((unsigned short) spi_context.port[devnum].ws_mask
								>> (16 - offset));
				*(ptr + 3) &=
						~((unsigned short) spi_context.port[devnum].ws_mask
								>> (24 - offset));
			}
			/* Send data */
			spi_context.port[devnum].reg->DATA = tx;
			/* Wait 'til it's done */
			while (spi_context.port[devnum].reg->STATUS & SPIn_STATUS_TXST_Msk);
			/* Retrieve data */
			tx = spi_context.port[devnum].reg->DATA
					& spi_context.port[devnum].ws_mask;
			/* Fill buffer */
			*ptr |= ((unsigned char) tx << offset);
			if (8 <= offset) {
				*(ptr + 1) |= ~((unsigned short) tx << (offset - 8));
			}
			if (16 < (wordsize + offset)) {
				*(ptr + 2) &= ~((unsigned int) tx >> (16 - offset));
				*(ptr + 3) &= ~((unsigned int) tx >> (24 - offset));
			}
		}
		/* Increment parameters */
		offset += wordsize;
		while (8 <= offset) {
			/* Increment pointer */
			ptr++;
			/* Decrement offset */
			offset -= 8;
			/* Decrement size */
			size--;
		}
	}
	/* Wait for data transmitting complete and then Deasserts nSS I/O */
	spi_context.port[devnum].reg->MOD |= SPIn_MOD_SSV_Msk;
	return NO_ERROR;
}

int mml_spi_get_status(mml_spi_dev_t devnum, unsigned int *pstatus) {
	int result = COMMON_ERR_UNKNOWN;
	
	if (!pstatus) {
	result = COMMON_ERR_NULL_PTR;
	}
	/* Check and select the device */
	else if (MML_SPI_DEV_COUNT <= devnum) {
		result = COMMON_ERR_OUT_OF_RANGE;
	} else if (0 == spi_context.port[devnum].initialized) {
		result = COMMON_ERR_NOT_INITIALIZED;
	} else {
		spi_context.port[devnum].reg->STATUS &= SPIn_STATUS_TXST_Msk;
		*pstatus = spi_context.port[devnum].reg->STATUS;
		result = NO_ERROR;
	}
	return result;
}

int mml_spi_ioctl(mml_spi_dev_t devnum, mml_spi_config_t cmd, void *p_data) {
	int result = COMMON_ERR_UNKNOWN;

	/* Check and select the device */
	if (MML_SPI_DEV_COUNT <= devnum) {
		result = COMMON_ERR_OUT_OF_RANGE;
	} else if (0 == spi_context.port[devnum].initialized) {
		result = COMMON_ERR_NOT_INITIALIZED;
	}
	/* Treat command */
	else {
		/* No error */
		result = NO_ERROR;
		switch (cmd) {
		case MML_SPI_IOCTL_ENABLE:
			spi_context.port[devnum].reg->CNTL |= SPIn_CNTL_SPIEN_Msk;
			break;
		case MML_SPI_IOCTL_DISABLE:
			spi_context.port[devnum].reg->CNTL &= ~SPIn_CNTL_SPIEN_Msk;
			break;
		case MML_SPI_IOCTL_MODE_MASTER:
			spi_context.port[devnum].reg->CNTL |= SPIn_CNTL_MMEN_Msk;
			break;
		case MML_SPI_IOCTL_MODE_SLAVE:
			spi_context.port[devnum].reg->CNTL &= ~SPIn_CNTL_MMEN_Msk;
			break;
		case MML_SPI_IOCTL_OPEN_DRAIN_ENABLE:
			spi_context.port[devnum].reg->CNTL |= SPIn_CNTL_WOR_Msk;
			break;
		case MML_SPI_IOCTL_OPEN_DRAIN_DISABLE:
			spi_context.port[devnum].reg->CNTL &= ~SPIn_CNTL_WOR_Msk;
			break;
		case MML_SPI_IOCTL_RESET_TX_FIFO:
			while (spi_context.port[devnum].reg->STATUS & SPIn_STATUS_TXST_Msk);
			/* Set RESET TX FIFO bit */
			spi_context.port[devnum].reg->DMA_REG |=
					SPIn_DMA_REG_TX_FIFO_CLEAR_Msk;
			break;
		case MML_SPI_IOCTL_RESET_RX_FIFO:
			while (spi_context.port[devnum].reg->DMA_REG
					& SPIn_DMA_REG_RX_FIFO_CNT_Msk);
			/* Set RESET RX FIFO bit */
			spi_context.port[devnum].reg->DMA_REG |=
					SPIn_DMA_REG_RX_FIFO_CLEAR_Msk;
			break;
		case MML_SPI_IOCTL_CLKPOL_HIGH:
			spi_context.port[devnum].reg->CNTL |= SPIn_CNTL_CLKPOL_Msk;
			break;
		case MML_SPI_IOCTL_CLKPOL_LOW:
			spi_context.port[devnum].reg->CNTL &= ~SPIn_CNTL_CLKPOL_Msk;
			break;
		case MML_SPI_IOTCL_PHASE_HIGH:
			spi_context.port[devnum].reg->CNTL |= SPIn_CNTL_PHASE_Msk;
			break;
		case MML_SPI_IOCTL_PHASE_LOW:
			spi_context.port[devnum].reg->CNTL &= ~SPIn_CNTL_PHASE_Msk;
			break;
		case MML_SPI_IOCTL_RESET_ERROR: {
			unsigned int tmp;

			tmp = spi_context.port[devnum].reg->STATUS;
			tmp |= (SPIn_STATUS_IRQ_Msk | SPIn_STATUS_TOVR_Msk
					| SPIn_STATUS_COL_Msk | SPIn_STATUS_ABT_Msk
					| SPIn_STATUS_ROVR_Msk | SPIn_STATUS_TUND_Msk);
			spi_context.port[devnum].reg->STATUS = tmp;
			break;
		}
		case MML_SPI_IOCTL_SSV: {
			unsigned int tmp;
			unsigned int reg;

			if (!p_data) {
				result = COMMON_ERR_NULL_PTR;
				break;
			} else {
				reg = (spi_context.port[devnum].reg->MOD & ~SPIn_MOD_SSV_Msk);
				tmp = *((unsigned int*) p_data);
				reg |= ((MML_SPI_MR_SSV_MASK_NOOFST & tmp) << SPIn_MOD_SSV_Pos);
				spi_context.port[devnum].reg->MOD = reg;
			}
			break;
		}
		case MML_SPI_IOCTL_SSIO: {
			unsigned int tmp;
			unsigned int reg;

			if (!p_data) {
				result = COMMON_ERR_NULL_PTR;
				break;
			} else {
				reg = (spi_context.port[devnum].reg->MOD & ~SPIn_MOD_SSIO_Msk);
				tmp = *((unsigned int*) p_data);
				reg |= ((MML_SPI_MR_SSIO_MASK_NOOFST & tmp)
								<< SPIn_MOD_SSIO_Pos);
				spi_context.port[devnum].reg->MOD = reg;
			}
			break;
		}
		case MML_SPI_IOCTL_SSLx: {
			unsigned int tmp;
			unsigned int reg;

			if (!p_data) {
				result = COMMON_ERR_NULL_PTR;
				break;
			} else {
				reg = (spi_context.port[devnum].reg->MOD
								& ~MML_SPI_MR_SSLx_MASK);
				tmp = *((unsigned int*) p_data);
				reg |= ((MML_SPI_MR_SSV_MASK_NOOFST & tmp) << SPIn_MOD_SSV_Pos);
				spi_context.port[devnum].reg->MOD = reg;
			}
			break;
		}
		default:
			result = COMMON_ERR_OUT_OF_RANGE;
			break;
		}
	}
	return result;
}

const char * mml_spi_get_version(void) {
	return MML_SPI_VERSION_STRING;
}

#endif //_MML_SPI_H_

/* EOF */
