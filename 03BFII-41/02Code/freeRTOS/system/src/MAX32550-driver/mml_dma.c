/*
 * @file mml_dma.c MML DMA Driver C File
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
 * trademarks, Mskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

#include <MAX325xx.h>

#ifdef _MML_DMA_H_

#include <errors.h>
#include <stddef.h>
#include <string.h>

/** DMA channel context info */
__attribute__((section(".bss")))                                         mml_dma_context_t mml_dma_context;


/* DMA IRQ handler */
static void mml_dma_irq_handler(mml_dma_channel_t ch);

uint32_t mml_dma_get_free_channel(void) {
	int i = 0;
	while (1) {
		switch (i % 4) {
		case 0:
			if (!(DMAC->DMA_CH[i].DMAn_CFG & DMAn_CFG_CHEN_Msk))
				return MML_DMA_CH0;
			break;

		case 1:
			if (!(DMAC->DMA_CH[i].DMAn_CFG & DMAn_CFG_CHEN_Msk))
				return MML_DMA_CH1;
			break;

		case 2:
			if (!(DMAC->DMA_CH[i].DMAn_CFG & DMAn_CFG_CHEN_Msk))
				return MML_DMA_CH2;
			break;

		case 3:
			if (!(DMAC->DMA_CH[i].DMAn_CFG & DMAn_CFG_CHEN_Msk))
				return MML_DMA_CH3;
			break;
		}
		i++;
	}
	return COMMON_ERR_UNKNOWN;
}

void mml_dma_pre_init(void) {

	/* Attach vector */
	NVIC_SetVector(DMA0_IRQn, (uint32_t)DMA0_IRQHandler);
	NVIC_SetVector(DMA1_IRQn, (uint32_t)DMA1_IRQHandler);
	NVIC_SetVector(DMA2_IRQn, (uint32_t)DMA2_IRQHandler);
	NVIC_SetVector(DMA3_IRQn, (uint32_t)DMA3_IRQHandler);

	/* Set priority level for interrupt */
	NVIC_SetPriority(DMA0_IRQn, priority_table[DMA0_IRQn]);
	NVIC_SetPriority(DMA1_IRQn, priority_table[DMA1_IRQn]);
	NVIC_SetPriority(DMA2_IRQn, priority_table[DMA2_IRQn]);
	NVIC_SetPriority(DMA3_IRQn, priority_table[DMA3_IRQn]);
}

int mml_dma_set_interrupt_priority(int vector, mml_intc_prio_t priority) {
	/* First check if vector is in correct range */
	if((vector > DMA3_IRQn) || (vector < DMA0_IRQn))
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

int mml_dma_init_without_irq(mml_dma_channel_t ch, mml_dma_config_t config,
		mml_dma_addr_t addr) {
	int result = NO_ERROR;

	if (!mml_dma_context.first_init) {
		/* Enable DMA clock */
		GCR->PERCKCN &= ~(GCR_PERCKCN_DMAD_Msk);
		/* Memset like procedure */
		memset((unsigned char*) &mml_dma_context, 0x00,
				sizeof(mml_dma_context_t));
#ifdef _DMA_RESET_AT_INIT_
		/* Reset DMA interface */
		GCR->RSTR |= GCR_RSTR_DMA_Msk;
		/* Wait until DMA reset completes */
		while( GCR_RSTR_DMA_Msk & GCR->RSTR );
#endif /* _DMA_RESET_AT_INIT_ */
		/* To be done once only */
		mml_dma_context.first_init = 1;
	}
	/* Validate the input parameters */
	if (MML_DMA_CH_COUNT <= ch) {
		result = MML_DMA_ERR_INVALID_DEVICE;
	} /* Check if the DMA channel is already been initialized */
	else if (MML_DMA_STATE_INITIALIZED == mml_dma_context.channel[ch].state) {
		result = COMMON_ERR_BAD_STATE;
	} else if (config.burst_size > 32) {
		result = COMMON_ERR_INVAL;
	} else {
		volatile unsigned int dma_cfg = 0;

		/* Read-modify-write */
		dma_cfg = DMAC->DMA_CH[ch].DMAn_CFG;
		dma_cfg |= ((config.priority << DMAn_CFG_PRI_Pos) & DMAn_CFG_PRI_Msk);
		dma_cfg |= ((config.request << DMAn_CFG_REQSEL_Pos)
				& DMAn_CFG_REQSEL_Msk);
		dma_cfg |= ((config.req_wait << DMAn_CFG_REQWAIT_Pos)
				& DMAn_CFG_REQWAIT_Msk);
		dma_cfg |=
				((config.timeout << DMAn_CFG_TOSEL_Pos) & DMAn_CFG_TOSEL_Msk);
		dma_cfg |=
				((config.prescale << DMAn_CFG_PSSEL_Pos) & DMAn_CFG_PSSEL_Msk);
		dma_cfg |= ((config.src_width << DMAn_CFG_SRCWD_Pos)
				& DMAn_CFG_SRCWD_Msk);
		dma_cfg |= ((config.src_increment << DMAn_CFG_SRCINC_Pos)
				& DMAn_CFG_SRCINC_Msk);
		dma_cfg |= ((config.dst_width << DMAn_CFG_DSTWD_Pos)
				& DMAn_CFG_DSTWD_Msk);
		dma_cfg |= ((config.dst_increment << DMAn_CFG_DSTINC_Pos)
				& DMAn_CFG_DSTINC_Msk);
		dma_cfg |= (((config.burst_size - 1) << DMAn_CFG_BRST_Pos)
				& DMAn_CFG_BRST_Msk);
		/* Write to CFG register */
		DMAC->DMA_CH[ch].DMAn_CFG |= dma_cfg;
		/* Configure the DMA source and destination registers */
		DMAC->DMA_CH[ch].DMAn_SRC = addr.src;
		DMAC->DMA_CH[ch].DMAn_DST = addr.dst;
		DMAC->DMA_CH[ch].DMAn_CNT = addr.count;
		/* Enable DMA channel interrupt */
		DMAC->CN |= ( MML_DMA_CN_CHIEN_ENABLE << ch);
		/* Normal mode without buffer chaining (RLDEN = 0) */
		DMAC->DMA_CH[ch].DMAn_CFG |= DMAn_CFG_CHDIEN_Msk;
		/* Enable DMA channel interrupt */
		mml_dma_context.channel[ch].status =
				MML_DMA_STATUS_NORMAL_CONFIGURATION;
		mml_dma_context.channel[ch].rld_callback = NULL;
		mml_dma_context.channel[ch].state = MML_DMA_STATE_INITIALIZED;
		mml_dma_context.channel[ch].handler = config.handler;

		switch (ch) {
		case MML_DMA_CH0:
			DMAC->CN |= DMAC_CN_CH0_IEN_Msk;
			/* Setup DMA channel IRQ */
			NVIC_EnableIRQ(DMA0_IRQn);
			break;
		case MML_DMA_CH1:
			DMAC->CN |= DMAC_CN_CH1_IEN_Msk;
			/* Setup DMA channel IRQ */
			NVIC_EnableIRQ(DMA1_IRQn);
			break;
		case MML_DMA_CH2:
			DMAC->CN |= DMAC_CN_CH2_IEN_Msk;
			/* Setup DMA channel IRQ */
			NVIC_EnableIRQ(DMA2_IRQn);
			break;
		case MML_DMA_CH3:
			DMAC->CN |= DMAC_CN_CH3_IEN_Msk;
			/* Setup DMA channel IRQ */
			NVIC_EnableIRQ(DMA3_IRQn);
			break;
		default:
			result = MML_DMA_ERR_IRQ_SET;
		}
	}
	return result;
}
/*
 * NOTE: this function is only here for legacy.
 * 		 There is a bug because irq number should be equal to channel (ch) number
 * 		 Prefered method is now mml_dma_init_without_irq
 */
int mml_dma_init(mml_dma_channel_t ch, int irq, mml_dma_config_t config,
		mml_dma_addr_t addr) {
	int result = NO_ERROR;

	if (!mml_dma_context.first_init) {
		/* Enable DMA clock */
		GCR->PERCKCN &= ~(GCR_PERCKCN_DMAD_Msk);
		/* Memset like procedure */
		memset((unsigned char*) &mml_dma_context, 0x00,
				sizeof(mml_dma_context_t));
#ifdef _DMA_RESET_AT_INIT_
		/* Reset DMA interface */
		GCR->RSTR |= GCR_RSTR_DMA_Msk;
		/* Wait until DMA reset completes */
		while( GCR_RSTR_DMA_Msk & GCR->RSTR );
#endif /* _DMA_RESET_AT_INIT_ */
		/* To be done once only */
		mml_dma_context.first_init = 1;
	}
	/* Validate the input parameters */
	if (MML_DMA_CH_COUNT <= ch) {
		result = MML_DMA_ERR_INVALID_DEVICE;
	} /* Check if the DMA channel is already been initialized */
	else if (MML_DMA_STATE_INITIALIZED == mml_dma_context.channel[ch].state) {
		result = COMMON_ERR_BAD_STATE;
	} else if ((DMA0_IRQn > irq) || (DMA3_IRQn < irq)) {
		result = MML_DMA_ERR_OUT_OF_RANGE;
	} else if (config.burst_size > 32) {
		result = COMMON_ERR_INVAL;
	} else {
		volatile unsigned int dma_cfg = 0;

		/* Make sure irq number corresponds to the channel */
		if (ch == MML_DMA_CH0) {
			if(irq != DMA0_IRQn)
				return COMMON_ERR_INVAL;
		}
		if (ch == MML_DMA_CH1) {
			if(irq != DMA1_IRQn)
				return COMMON_ERR_INVAL;
		}
		if (ch == MML_DMA_CH2) {
			if(irq != DMA2_IRQn)
				return COMMON_ERR_INVAL;
		}
		if (ch == MML_DMA_CH3) {
			if(irq != DMA3_IRQn)
				return COMMON_ERR_INVAL;
		}
		/* Read-modify-write */
		dma_cfg = DMAC->DMA_CH[ch].DMAn_CFG;
		dma_cfg |= ((config.priority << DMAn_CFG_PRI_Pos) & DMAn_CFG_PRI_Msk);
		dma_cfg |= ((config.request << DMAn_CFG_REQSEL_Pos)
				& DMAn_CFG_REQSEL_Msk);
		dma_cfg |= ((config.req_wait << DMAn_CFG_REQWAIT_Pos)
				& DMAn_CFG_REQWAIT_Msk);
		dma_cfg |=
				((config.timeout << DMAn_CFG_TOSEL_Pos) & DMAn_CFG_TOSEL_Msk);
		dma_cfg |=
				((config.prescale << DMAn_CFG_PSSEL_Pos) & DMAn_CFG_PSSEL_Msk);
		dma_cfg |= ((config.src_width << DMAn_CFG_SRCWD_Pos)
				& DMAn_CFG_SRCWD_Msk);
		dma_cfg |= ((config.src_increment << DMAn_CFG_SRCINC_Pos)
				& DMAn_CFG_SRCINC_Msk);
		dma_cfg |= ((config.dst_width << DMAn_CFG_DSTWD_Pos)
				& DMAn_CFG_DSTWD_Msk);
		dma_cfg |= ((config.dst_increment << DMAn_CFG_DSTINC_Pos)
				& DMAn_CFG_DSTINC_Msk);
		dma_cfg |= (((config.burst_size - 1) << DMAn_CFG_BRST_Pos)
				& DMAn_CFG_BRST_Msk);
		/* Write to CFG register */
		DMAC->DMA_CH[ch].DMAn_CFG |= dma_cfg;
		/* Configure the DMA source and destination registers */
		DMAC->DMA_CH[ch].DMAn_SRC = addr.src;
		DMAC->DMA_CH[ch].DMAn_DST = addr.dst;
		DMAC->DMA_CH[ch].DMAn_CNT = addr.count;
		/* Enable DMA channel interrupt */
		DMAC->CN |= ( MML_DMA_CN_CHIEN_ENABLE << ch);
		/* Normal mode without buffer chaining (RLDEN = 0) */
		DMAC->DMA_CH[ch].DMAn_CFG |= DMAn_CFG_CHDIEN_Msk;
		/* Enable DMA channel interrupt */
		mml_dma_context.channel[ch].status =
				MML_DMA_STATUS_NORMAL_CONFIGURATION;
		mml_dma_context.channel[ch].rld_callback = NULL;
		mml_dma_context.channel[ch].state = MML_DMA_STATE_INITIALIZED;
		mml_dma_context.channel[ch].handler = config.handler;

		switch (ch) {
		case MML_DMA_CH0:
			DMAC->CN |= DMAC_CN_CH0_IEN_Msk;
			/* Setup DMA channel IRQ */
			NVIC_EnableIRQ(DMA0_IRQn);
			break;
		case MML_DMA_CH1:
			DMAC->CN |= DMAC_CN_CH1_IEN_Msk;
			/* Setup DMA channel IRQ */
			NVIC_EnableIRQ(DMA1_IRQn);
			break;
		case MML_DMA_CH2:
			DMAC->CN |= DMAC_CN_CH2_IEN_Msk;
			/* Setup DMA channel IRQ */
			NVIC_EnableIRQ(DMA2_IRQn);
			break;
		case MML_DMA_CH3:
			DMAC->CN |= DMAC_CN_CH3_IEN_Msk;
			/* Setup DMA channel IRQ */
			NVIC_EnableIRQ(DMA3_IRQn);
			break;
		default:
			result = MML_DMA_ERR_IRQ_SET;
		}
	}
	return result;
}

int mml_dma_set_configuration(mml_dma_channel_t ch, mml_dma_config_t config) {
	int result = NO_ERROR;

	/* Validate the input parameters */
	if (MML_DMA_CH_COUNT <= ch) {
		result = MML_DMA_ERR_INVALID_DEVICE;
	} /* Check if the DMA channel is initialized or not */
	else if (MML_DMA_STATE_INITIALIZED != mml_dma_context.channel[ch].state) {
		result = COMMON_ERR_BAD_STATE;
	} else if (config.burst_size > 32) {
		result = COMMON_ERR_INVAL;
	}
	/* Is DMA channel disabled ? */
	else if (!(DMAC->DMA_CH[ch].DMAn_ST & DMAn_ST_CH_ST_Msk)) {
		unsigned int dma_cfg = 0;
		DMAC->DMA_CH[ch].DMAn_CFG = 0;
		dma_cfg |= ((config.priority << DMAn_CFG_PRI_Pos) & DMAn_CFG_PRI_Msk);
		dma_cfg |= ((config.request << DMAn_CFG_REQSEL_Pos)
				& DMAn_CFG_REQSEL_Msk);
		dma_cfg |= ((config.req_wait << DMAn_CFG_REQWAIT_Pos)
				& DMAn_CFG_REQWAIT_Msk);
		dma_cfg |=
				((config.timeout << DMAn_CFG_TOSEL_Pos) & DMAn_CFG_TOSEL_Msk);
		dma_cfg |=
				((config.prescale << DMAn_CFG_PSSEL_Pos) & DMAn_CFG_PSSEL_Msk);
		dma_cfg |= ((config.src_width << DMAn_CFG_SRCWD_Pos)
				& DMAn_CFG_SRCWD_Msk);
		dma_cfg |= ((config.src_increment << DMAn_CFG_SRCINC_Pos)
				& DMAn_CFG_SRCINC_Msk);
		dma_cfg |= ((config.dst_width << DMAn_CFG_DSTWD_Pos)
				& DMAn_CFG_DSTWD_Msk);
		dma_cfg |= ((config.dst_increment << DMAn_CFG_DSTINC_Pos)
				& DMAn_CFG_DSTINC_Msk);
		dma_cfg |= (((config.burst_size - 1) << DMAn_CFG_BRST_Pos)
				& DMAn_CFG_BRST_Msk);
		/*
		 * Write to CFG register
		 * Clear the Count-to-zero interrupt(CTZIEN)
		 * Enable only Channel Disable interrupt (CHDIEN) for normal operation recommended
		 */
		DMAC->DMA_CH[ch].DMAn_CFG |= (dma_cfg & ~DMAn_CFG_CTZIEN_Msk)
				| DMAn_CFG_CHDIEN_Msk;
		/* Set the DMA channel status */
		mml_dma_context.channel[ch].status =
				MML_DMA_STATUS_NORMAL_CONFIGURATION;
	} else {
		result = MML_DMA_ERR_BUSY;
	}
	return result;
}

int mml_dma_get_configuration(mml_dma_channel_t ch, mml_dma_config_t *config) {
	int result = NO_ERROR;
	unsigned int dma_cfg = 0;

	/* Validate the input parameters */
	if (MML_DMA_CH_COUNT <= ch) {
		return MML_DMA_ERR_INVALID_DEVICE;
	} else if (config == NULL){
		return COMMON_ERR_NULL_PTR;
	}

	dma_cfg = DMAC->DMA_CH[ch].DMAn_CFG;
	config->priority = (dma_cfg & DMAn_CFG_PRI_Msk) >> DMAn_CFG_PRI_Pos;
	config->request =
			(dma_cfg & DMAn_CFG_REQSEL_Msk) >> DMAn_CFG_REQSEL_Pos;
	config->req_wait = (dma_cfg & DMAn_CFG_REQWAIT_Msk)
			>> DMAn_CFG_REQWAIT_Pos;
	config->timeout = (dma_cfg & DMAn_CFG_TOSEL_Msk) >> DMAn_CFG_TOSEL_Pos;
	config->prescale = (dma_cfg & DMAn_CFG_PSSEL_Msk) >> DMAn_CFG_PSSEL_Pos;
	config->src_width =
			(dma_cfg & DMAn_CFG_SRCWD_Msk) >> DMAn_CFG_SRCWD_Pos;
	config->src_increment = (dma_cfg & DMAn_CFG_SRCINC_Msk)
			>> DMAn_CFG_SRCINC_Pos;
	config->dst_width =
			(dma_cfg & DMAn_CFG_DSTWD_Msk) >> DMAn_CFG_DSTWD_Pos;
	config->dst_increment = (dma_cfg & DMAn_CFG_DSTINC_Msk)
			>> DMAn_CFG_DSTINC_Pos;
	config->burst_size = (dma_cfg & DMAn_CFG_BRST_Msk) >> DMAn_CFG_BRST_Pos;

	return result;
}

int mml_dma_set_address(mml_dma_channel_t ch, mml_dma_addr_t addr) {
	int result = NO_ERROR;

	/* Validate the input parameters */
	if (MML_DMA_CH_COUNT <= ch) {
		result = MML_DMA_ERR_INVALID_DEVICE;
	}
	/* Check if the DMA channel is initialized or not */
	else if (MML_DMA_STATE_INITIALIZED != mml_dma_context.channel[ch].state) {
		result = COMMON_ERR_BAD_STATE;
	}
	/* Is DMA channel disabled? */
	else if (!(DMAC->DMA_CH[ch].DMAn_ST & DMAn_ST_CH_ST_Msk)) {
		/* Configure the DMA source and destination registers */
		DMAC->DMA_CH[ch].DMAn_SRC = addr.src;
		DMAC->DMA_CH[ch].DMAn_DST = addr.dst;
		DMAC->DMA_CH[ch].DMAn_CNT = addr.count & DMAn_CNT_CNT_Msk;
	} else
		result = MML_DMA_ERR_BUSY;
	return result;
}

int mml_dma_set_chaintransfer(mml_dma_channel_t ch, mml_dma_addr_t reload_addr,
		mml_dma_realod_callback_t rld_callback) {
	int result = NO_ERROR;

	/* Validate the input parameters */
	if (MML_DMA_CH_COUNT <= ch) {
		result = MML_DMA_ERR_INVALID_DEVICE;
	}
	/* Check if the DMA channel is initialized or not */
	else if (MML_DMA_STATE_INITIALIZED != mml_dma_context.channel[ch].state) {
		result = COMMON_ERR_BAD_STATE;
	}
	/* Is DMA channel disabled? */
	if (!(DMAC->DMA_CH[ch].DMAn_ST & DMAn_ST_CH_ST_Msk)) {
		/* Configure the DMA source and destination registers */
		DMAC->DMA_CH[ch].DMAn_SRC_RLD = reload_addr.src
				& DMAn_SRC_RLD_SRC_RLD_Msk;
		DMAC->DMA_CH[ch].DMAn_DST_RLD = reload_addr.dst
				& DMAn_DST_RLD_DST_RLD_Msk;
		DMAC->DMA_CH[ch].DMAn_CNT_RLD = reload_addr.count
				& DMAn_CNT_RLD_CNT_RLD_Msk;
		mml_dma_context.channel[ch].rld_callback = rld_callback;
		/* Set the DMA channel status */
		mml_dma_context.channel[ch].status =
				MML_DMA_STATUS_RELOAD_CONFIGURATION;
	} else
		result = MML_DMA_ERR_BUSY;
	return result;
}

int mml_dma_enable(mml_dma_channel_t ch) {
	int result = NO_ERROR;

	/* Validate the input parameters */
	if (MML_DMA_CH_COUNT <= ch) {
		result = MML_DMA_ERR_INVALID_DEVICE;
	} else if (MML_DMA_STATE_INITIALIZED != mml_dma_context.channel[ch].state) {
		result = COMMON_ERR_BAD_STATE;
	} else {
		/* Chained buffer transfer */
		if (MML_DMA_STATUS_RELOAD_CONFIGURATION
				== mml_dma_context.channel[ch].status) {
			/* Enable CHDIEN and CTZIEN interrupts */
			DMAC->DMA_CH[ch].DMAn_CFG |= (DMAn_CFG_CHDIEN_Msk
					| DMAn_CFG_CTZIEN_Msk);
			/* Must be the last step */
			DMAC->DMA_CH[ch].DMAn_CFG |=
					(DMAn_CFG_CHEN_Msk | DMAn_CFG_RLDEN_Msk);

		} else {
			/* Normal DMA transfer */
			volatile unsigned int dma_cfg = 0;

			dma_cfg = DMAC->DMA_CH[ch].DMAn_CFG;
			/* Enable CHDIEN interrupt */
			DMAC->DMA_CH[ch].DMAn_CFG = (dma_cfg & ~DMAn_CFG_CTZIEN_Msk)
					| DMAn_CFG_CHDIEN_Msk;
			/* Must be the last step */
			DMAC->DMA_CH[ch].DMAn_CFG |= DMAn_CFG_CHEN_Msk;
		}
	}
	return result;
}

int mml_dma_disable(mml_dma_channel_t ch) {
	int result = NO_ERROR;

	/* Validate the input parameters */
	if (MML_DMA_CH_COUNT <= ch) {
		result = MML_DMA_ERR_INVALID_DEVICE;
	} else if (MML_DMA_STATE_INITIALIZED != mml_dma_context.channel[ch].state) {
		result = COMMON_ERR_BAD_STATE;
	} else if (DMAC->INT & (0x01 << ch)) {
		/* Interrupt is pending for the channel */
		result = MML_DMA_ERR_INTERRUPT_PENDING;
	} else {
		/* Chained buffer transfer */
		if (MML_DMA_STATUS_RELOAD_CONFIGURATION
				== mml_dma_context.channel[ch].status) {
			/* Disable the CHDIEN and CTZIEN interrupts */
			DMAC->DMA_CH[ch].DMAn_CFG &= ~(DMAn_CFG_CHDIEN_Msk
					| DMAn_CFG_CTZIEN_Msk);
			/* Must be the last step */
			DMAC->DMA_CH[ch].DMAn_CFG &= ~(DMAn_CFG_CHEN_Msk
					| DMAn_CFG_RLDEN_Msk);
		} else {
			/* Normal DMA transfer */
			/* Enable CHDIEN interrupt */
			DMAC->DMA_CH[ch].DMAn_CFG &= ~(DMAn_CFG_CTZIEN_Msk
					| DMAn_CFG_CHDIEN_Msk);
			/* Must be the last step */
			DMAC->DMA_CH[ch].DMAn_CFG &= ~DMAn_CFG_CHEN_Msk;
		}
	}
	return result;
}

int mml_dma_get_status(mml_dma_channel_t ch, mml_dma_status_t *status) {
	int result = NO_ERROR;

	if (MML_DMA_CH_COUNT <= ch) {
		return MML_DMA_ERR_INVALID_DEVICE;
	} else if (status == NULL){
		return COMMON_ERR_NULL_PTR;
	}

	if (DMAn_ST_CH_ST_Msk & DMAC->DMA_CH[ch].DMAn_ST) {
		*status = MML_DMA_STATUS_ENABLED;
	} else {
		*status = MML_DMA_STATUS_DISABLED;
	}

	return result;
}

static void mml_dma_irq_handler(mml_dma_channel_t ch) {
	volatile unsigned int status;
	volatile unsigned int irq_src = 0;

	status = DMAC->DMA_CH[ch].DMAn_ST;
	/* Count-to-Zero interrupt has occurred */
	if ( DMAn_ST_CTZ_ST_Msk & status) {
		/* Save the source level interrupt */
		irq_src |= DMAn_ST_CTZ_ST_Msk;
	}
	if (((DMAn_ST_RLD_ST_Msk | DMAn_ST_CTZ_ST_Msk) & status)
			&& (MML_DMA_STATUS_RELOAD_CONFIGURATION
					== mml_dma_context.channel[ch].status)) {
		if (mml_dma_context.channel[ch].rld_callback) {
			int reload;
			mml_dma_addr_t rld_addr;

			mml_dma_context.channel[ch].rld_callback(&rld_addr, &reload);
			if (reload) {
				DMAC->DMA_CH[ch].DMAn_SRC_RLD = rld_addr.src
						& DMAn_SRC_RLD_SRC_RLD_Msk;
				DMAC->DMA_CH[ch].DMAn_DST_RLD = rld_addr.dst
						& DMAn_DST_RLD_DST_RLD_Msk;
				DMAC->DMA_CH[ch].DMAn_CNT_RLD = rld_addr.count
						& DMAn_CNT_RLD_CNT_RLD_Msk;
				/* NOTE: write in two steps */
				DMAC->DMA_CH[ch].DMAn_CNT_RLD |= DMAn_CNT_RLD_RLDEN_Msk;
			}
		}
		/* Save the source level interrupt */
		irq_src |= DMAn_ST_RLD_ST_Msk;
	}
	if ( DMAn_ST_BUS_ERR_Msk & status) {
		/* irq_src the Bus Error interrupt */
		irq_src |= DMAn_ST_BUS_ERR_Msk;
	}
	if ( DMAn_ST_TO_ST_Msk & status) {
		irq_src |= DMAn_ST_TO_ST_Msk;
	}
	/* Clear the source level interrupt */
	DMAC->DMA_CH[ch].DMAn_ST |= (irq_src
			& ( DMAn_ST_CTZ_ST_Msk | DMAn_ST_RLD_ST_Msk | DMAn_ST_BUS_ERR_Msk
					| DMAn_ST_TO_ST_Msk));
	if (irq_src & DMAn_ST_CTZ_ST_Msk)
		if (mml_dma_context.channel[ch].handler)
			mml_dma_context.channel[ch].handler(ch);
}

void DMA0_IRQHandler(void) {
	if ( DMAC->INT & DMAC_INT_CH0_IPEND_Msk) {
		mml_dma_irq_handler(MML_DMA_CH0);
	}
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(DMA0_IRQn);
}

void DMA1_IRQHandler(void) {
	if ( DMAC->INT & DMAC_INT_CH1_IPEND_Msk) {
		mml_dma_irq_handler(MML_DMA_CH1);
	}
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(DMA1_IRQn);
}

void DMA2_IRQHandler(void) {
	if ( DMAC->INT & DMAC_INT_CH2_IPEND_Msk) {
		mml_dma_irq_handler(MML_DMA_CH2);
	}
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(DMA2_IRQn);
}

void DMA3_IRQHandler(void) {

	if ( DMAC->INT & DMAC_INT_CH3_IPEND_Msk) {
		mml_dma_irq_handler(MML_DMA_CH3);
	}
	/* Acknowledge interrupt at platform level */
	NVIC_ClearPendingIRQ(DMA3_IRQn);
}

const char * mml_dma_get_version(void) {
	return MML_DMA_VERSION_STRING;
}

#endif //_MML_DMA_H_

/******************************************************************************/
/* EOF */

