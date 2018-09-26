/*
 * @file mml_msr.c MML MSR Driver C File
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

/**
 * WARNINGS : Use of mml_dma.. functions in this driver is specific to MSR driver due to DMA complexity
 */

#include <MAX325xx.h>

#ifdef _MML_MSR_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>


/* User swipe complete callback */
static void (*complete_callback)(void);

/* Decode parameters */
static uint8_t thr_md = 184; /* equal cost threshold x2^8 */
static uint8_t thr_up = 215; /* 8:1 cost threshold x2^8 */
static uint8_t thr_dn = 157; /* 1:8 cost threshold x2^8 */
static uint8_t narb = 5; /* # of steps in arbitration */
static uint8_t pairing = 1; /* enable bit-pairing, force even # of ones (0=pairing disable) */

/* Track parameters */
static const trackpar_t trackpars[] =
		{
				{ 0x45, /* start sentinel */
				0x1F, /* end sentinel */
				7, /* # of bits per char */
				210, /* bit density used for swipe rate calculation */
						/*                00 000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555555566666666666666667777777777777 777 */
						/*                01 23456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABC DEF */
						(const uint8_t *) "~!\"~$~~`(~~+~-.~0~~3~56~~9:~<~~?@~~C~EF~~IJ~L~~O~QR~T~~WX~~[~]^~ ~~#~%&~~)*~,~~/~12~4~~78~~;~=>~~AB~D~~GH~~K~MN~P~~S~UV~~YZ~\\~~_" },
				{ 0x0B, /* start sentinel */
				0x1F, /* end sentinel */
				5, /* # of bits per char */
				150, /* bit density used for swipe rate calculation (Note: Track 2 is 2x normal density) */
				/*                00000000000000001111111111111111 */
				/*                0123456789ABCDEF0123456789ABCDEF */
				(const uint8_t *) "~12~4~~78~~;~=>~0~~3~56~~9:~<~~?" }, { 0x0B, /* start sentinel */
				0x1F, /* end sentinel */
				5, /* # of bits per char */
				210, /* bit density used for swipe rate calculation (Note: Track 2 is 2x normal density) */
				/*                00000000000000001111111111111111 */
				/*                0123456789ABCDEF0123456789ABCDEF */
				(const uint8_t *) "~12~4~~78~~;~=>~0~~3~56~~9:~<~~?" } };

/***** Function Prototypes *****/
/*
 *  DMA specific interrupt handler for MSR application
 */
void mml_MSR_DMA_IRQHandler(mml_dma_channel_t ch);


__attribute__((section(".bss")))  									     mml_msr_context_t mml_msr_context[MSR_NUM_TRACKS];

void remove_false_peaks(uint16_t * samples, unsigned int len,
		unsigned int thr, uint16_t * samplesf, unsigned int * lenf) {
	unsigned int ii, iout;
	uint16_t smpl, peak, up, dn, ppt, threshold;
	unsigned int iup, idn;

	/* init */
	up = dn = ppt = 0;
	iup = idn = 0;
	iout = 0;
	for (ii = 0; ii < len; ii++) {
		smpl = (samples[ii] & 0x01FF);
		peak = (samples[ii] & 0xFE00) >> 5; /* scaled <<4 */
		/* establish top and bottom levels */
		if (!iup && (peak >= 1064)) {
			iup = 1;
			up = peak;
		}
		if (!idn && iup && (peak < 984)) {
			idn = 1;
			dn = peak;
		}
		if (iup && idn && (ii > 0) && (ii < (len - 1))) {
			/* set threshold */
			threshold = ((up - dn) * thr) >> 4;
			/* check for false peak */
			if (abs(peak - ((samples[ii - 1] & 0xFE00) >> 5)) < threshold
					|| abs(peak - ((samples[ii + 1] & 0xFE00) >> 5))
							< threshold) {
				ppt += smpl;
			} else {
				if (peak > ((up + dn) >> 1)) {
					/* update up */
					up = (up + peak) >> 1;
					samplesf[iout++] = (smpl + ppt);
					ppt = 0;
				} else {
					/* update dn */
					dn = (dn + peak) >> 1;
					samplesf[iout++] = (smpl + ppt);
					ppt = 0;
				}
			}
		}
	}
	*lenf = iout;
}

static uint8_t get_char(uint8_t * bits, unsigned int nbits, unsigned int ii,
		int direction, uint8_t char_Msk) {
	uint8_t ch;
	uint16_t tmp;
	unsigned int ip;

	if (direction == MSR_FORWARD) {
		ip = (ii >> 3);
		tmp = ((uint16_t) bits[ip + 1] << 8) + bits[ip];
		ch = (uint8_t) (tmp >> (ii & 0x7)) & char_Msk;
	} else {
		ip = ((nbits - ii) >> 3);
		tmp = ((uint16_t) bits[ip] << 8) + bits[ip - 1];
		tmp = tmp >> (((nbits - ii) & 0x7) + 1);
		ch = (tmp & 0x80) ? 1 : 0;
		ch += (((tmp & 0x40) ? 1 : 0) << 1);
		ch += (((tmp & 0x20) ? 1 : 0) << 2);
		ch += (((tmp & 0x10) ? 1 : 0) << 3);
		ch += (((tmp & 0x08) ? 1 : 0) << 4);
		ch += (((tmp & 0x04) ? 1 : 0) << 5);
		ch += (((tmp & 0x02) ? 1 : 0) << 6);
		ch = ch & char_Msk;
	}
	return ch;
}

unsigned int parse_track(uint8_t * bits, unsigned int nbits,
		const trackpar_t * trackpar, unsigned int direction,
		mml_msr_decoded_track_t * parsed) {
	unsigned int ii, ii1;
	unsigned int parity_errs, valid_cnt;
	uint8_t lrc, ch;
	uint8_t char_Msk;
	int oneflag;

	/* Initialize output */
	memset(parsed, 0, sizeof(mml_msr_decoded_track_t));
	parsed->lrc = 0xFF; /* invalid */
	parsed->error_code = MML_MSR_ERRORS_MSR_ERR_START_SEN
			| MML_MSR_ERRORS_MSR_ERR_END_SEN;
	/* check length */
	if ((nbits == 0) || (nbits > MSR_MAX_RAW_LEN_BITS)) {
		parsed->error_code |= MML_MSR_ERRORS_MSR_ERR_BAD_LEN;
		return 0;
	}
	/* init */
	ii = parity_errs = lrc = 0;
	oneflag = 0;
	char_Msk = (1 << trackpar->nbits) - 1;
	valid_cnt = 0;
	/* Search bit-wise for start sentinel */
	while (1) {
		if ((ii + trackpar->nbits) >= nbits) {
			return valid_cnt;
		}
		ch = get_char(bits, nbits, ii, direction, char_Msk);
		if (ch != 0) {
			if (!oneflag) {
				/* remember the 1-st one */
				oneflag = 1;
				ii1 = ii;
			}
			if (ch == trackpar->startsen) {
				parsed->error_code &= ~MML_MSR_ERRORS_MSR_ERR_START_SEN; /* clear the sentinel error */
				valid_cnt++;
				lrc = (lrc ^ ch);
				ii += trackpar->nbits; /* skip past the sentinel */
				break;
			}
			/* allow no more than 2 charaters without start sentinel */
			if ((ii - ii1) > ((unsigned int) trackpar->nbits << 1)) {
				return valid_cnt;
			}
		}
		ii++;
	}
	/* Parse data and search for end sentinel */
	while (parsed->len < (MSR_MAX_DEC_LEN - 1)) {
		if ((ii + (trackpar->nbits << 1)) >= nbits) {
			parsed->error_code = MML_MSR_ERRORS_MSR_ERR_END_SEN;
			return valid_cnt;
		}
		ch = get_char(bits, nbits, ii, direction, char_Msk);
		lrc = (lrc ^ ch);
		/* check for end sentinel */
		if (ch != trackpar->endsen) {
			/* record the character, check parity & lrc */
			if ((parsed->data[parsed->len++] = trackpar->charset[ch]) == '~')
				parity_errs++;
			else
				valid_cnt++;
		} else {
			parsed->error_code &= ~MML_MSR_ERRORS_MSR_ERR_END_SEN; /* clear the sentinel error */
			valid_cnt++;
			ii += trackpar->nbits; /* skip past the sentinel */
			ch = get_char(bits, nbits, ii, direction, char_Msk); /* get LRC */
			lrc = (lrc ^ ch) & (char_Msk >> 1);
			/* check parity & lrc */
			if (trackpar->charset[ch] == '~')
				parity_errs++;
			else
				valid_cnt++;
			break;
		}
		ii += trackpar->nbits;
	}
	/* output and exit */
	if (parity_errs > 0)
		parsed->error_code |= MML_MSR_ERRORS_MSR_ERR_PARITY;
	if (lrc != 0)
		parsed->error_code |= MML_MSR_ERRORS_MSR_ERR_LRC;
	parsed->lrc = lrc;
	parsed->parity_errs = parity_errs;
	return valid_cnt;
}

uint8_t decode_track(uint16_t * samples, unsigned int len,
		const trackpar_t * trackpar, unsigned int direction, uint8_t * bits,
		unsigned int * nbits, unsigned int * speed) {
	unsigned int ii, jj, kk;
	unsigned int oneflag;
	unsigned int onecnt, onecnt0, zcnt;
	unsigned int lpfc;
	uint16_t avg, avg0;
	uint16_t smpl, smpl0, an, da, corr;
	uint32_t cost, cost0_34, cost1_34, cost0_14;
	unsigned int bit;
	uint32_t avgsum;
	unsigned int avgsum_cnt;

	/* check data length */
	if ((len <= 5) || (len > MSR_MAX_SAMPLES))
		return MML_MSR_ERRORS_MSR_ERR_BAD_LEN;
	/* init */
	memset(bits, 0, MSR_MAX_RAW_LEN_BYTES);
	*nbits = 0;
	avg = 0;
	avgsum = 0;
	avgsum_cnt = 0;
	oneflag = 0;
	onecnt = 0;
	zcnt = 0;
	/* decode loop */
	for (ii = 0;
			(ii < len)
					&& !((zcnt > (((unsigned int) trackpar->nbits - 1) * 2))
							&& oneflag) && (*nbits < (MSR_MAX_RAW_LEN_BITS - 8));
			ii++) {
		lpfc = 3;
		/* new sample */
		kk = (direction == MSR_REVERSE) ? len - 1 - ii : ii;
		smpl = (samples[kk] & 0x1FF) << 3;
		/* average first 4 without decoding */
		if (ii == 4) {
			/* establish avg */
			avg = avgsum >> 2;
		}
		if (ii > 3) {
			/* normal pass */
			/* check for outliers */
			if (smpl > ((avg * 384 + 128) >> 8))
				return MML_MSR_ERRORS_MSR_ERR_OUTLIER;
			/* make decision 0 (short) or 1 (long) ? */
			if (((onecnt & 1) && pairing)
					|| (smpl <= ((avg * thr_dn + 128) >> 8))) {
				/* certain bit 1 detected */
				bit = 1;
			} else if ((smpl >= (avg * thr_up + 128) >> 8)) {
				/* certain bit 0 detected */
				bit = 0;
			} else {
				/* uncertain bit */
				/* start arbitration procedure */
				/* save initial state */
				avg0 = avg;
				smpl0 = smpl;
				onecnt0 = onecnt;
				/* try bit 0 */
				cost = ((uint32_t) (smpl - avg) * (uint32_t) (smpl - avg)) >> 1;
				an = (smpl + avg) >> 1; /* new avg target */
				da = ((an - avg) * lpfc + 2) >> 2;
				avg += da; /* filter avg */
				onecnt = 0;
				corr = da;
				/* collect cost of forward ppts */
				for (jj = 1; (jj <= narb) && (ii + jj < len); jj++) {
					kk = (direction == MSR_REVERSE) ?
							len - 1 - (ii + jj) : ii + jj;
					smpl = ((samples[kk] & 0x1FF) << 3) + corr;
					if (((onecnt & 1) && pairing)
							|| (smpl <= ((avg * thr_md + 128) >> 8))) {
						/* set 1 */
						cost += ((uint32_t) (2 * smpl - avg)
								* (uint32_t) (2 * smpl - avg)) / 5;
						an = (2 * ((uint32_t) smpl + 2 * (uint32_t) avg)) / 5; /* new avg target */
						da = ((an - avg) * lpfc + 4) >> 3;
						avg += da; /* filter avg */
						onecnt++;
						corr = 2 * da;
					} else {
						/* set 0 */
						cost += ((uint32_t) (smpl - avg)
								* (uint32_t) (smpl - avg)) >> 1;
						an = (smpl + avg) >> 1; /* new avg target */
						da = ((an - avg) * lpfc + 2) >> 2;
						avg += da; /* filter avg */
						onecnt = 0;
						corr = da;
					}
				}
				cost0_34 = cost; /* total cost of bit 0 */
				/* restore initial state */
				avg = avg0;
				smpl = smpl0;
				onecnt = onecnt0;
				/* try bit 1 */
				cost = ((uint32_t) (2 * smpl - avg)
						* (uint32_t) (2 * smpl - avg)) / 5;
				an = (2 * ((uint32_t) smpl + 2 * (uint32_t) avg)) / 5; /* new avg target */
				da = ((an - avg) * lpfc + 4) >> 3;
				avg += da; /* filter avg */
				onecnt++;
				corr = 2 * da;
				/* collect cost of forward ppts */
				for (jj = 1; (jj <= narb) && (ii + jj < len); jj++) {
					kk = (direction == MSR_REVERSE) ?
							len - 1 - (ii + jj) : ii + jj;
					smpl = ((samples[kk] & 0x1FF) << 3) + corr;
					if (((onecnt & 1) && pairing)
							|| (smpl <= ((avg * thr_md + 128) >> 8))) {
						/* set 1 */
						cost += ((uint32_t) (2 * smpl - avg)
								* (uint32_t) (2 * smpl - avg)) / 5;
						an = (2 * ((uint32_t) smpl + 2 * (uint32_t) avg)) / 5; /* new avg target */
						da = ((an - avg) * lpfc + 4) >> 3;
						avg += da; /* filter avg */
						onecnt++;
						corr = 2 * da;
					} else {
						/* set 0 */
						cost += ((uint32_t) (smpl - avg)
								* (uint32_t) (smpl - avg)) >> 1;
						an = (smpl + avg) >> 1; /* new avg target */
						da = ((an - avg) * lpfc + 2) >> 2;
						avg += da; /* filter avg */
						onecnt = 0;
						corr = da;
					}
				}
				cost1_34 = cost; /* total cost of bit 1 */
				/* set slow lpf */
				lpfc = 1;
				/* restore initial state */
				avg = avg0;
				smpl = smpl0;
				onecnt = onecnt0;
				/* try bit 0 */
				cost = ((uint32_t) (smpl - avg) * (uint32_t) (smpl - avg)) >> 1;
				an = (smpl + avg) >> 1; /* new avg target */
				da = ((an - avg) * lpfc + 2) >> 2;
				avg += da; /* filter avg */
				onecnt = 0;
				corr = da;
				/* collect cost of forward ppts */
				for (jj = 1; (jj <= narb) && (ii + jj < len); jj++) {
					kk = (direction == MSR_REVERSE) ?
							len - 1 - (ii + jj) : ii + jj;
					smpl = ((samples[kk] & 0x1FF) << 3) + corr;
					if (((onecnt & 1) && pairing)
							|| (smpl <= ((avg * thr_md + 128) >> 8))) {
						/* set 1 */
						cost += ((uint32_t) (2 * smpl - avg)
								* (uint32_t) (2 * smpl - avg)) / 5;
						an = 2 * ((uint32_t) smpl + 2 * (uint32_t) avg) / 5; /* new avg target */
						da = ((an - avg) * lpfc + 4) >> 3;
						avg += da; /* filter avg */
						onecnt++;
						corr = 2 * da;
					} else {
						/* set 0 */
						cost += ((uint32_t) (smpl - avg)
								* (uint32_t) (smpl - avg)) >> 1;
						an = (smpl + avg) >> 1; /* new avg target */
						da = ((an - avg) * lpfc + 2) >> 2;
						avg += da; /* filter avg */
						onecnt = 0;
						corr = da;
					}
				}
				cost0_14 = cost; /* total cost of bit 0 */
				/* restore initial state */
				avg = avg0;
				smpl = smpl0;
				onecnt = onecnt0;
				/* try bit 1 */
				cost = ((uint32_t) (2 * smpl - avg)
						* (uint32_t) (2 * smpl - avg)) / 5;
				an = (2 * ((uint32_t) smpl + 2 * (uint32_t) avg) / 5); /* new avg target */
				da = ((an - avg) * lpfc + 4) >> 3;
				avg += da; /* filter avg */
				onecnt++;
				corr = 2 * da;
				/* collect cost of forward ppts */
				for (jj = 1; (jj <= narb) && (ii + jj < len); jj++) {
					kk = (direction == MSR_REVERSE) ?
							len - 1 - (ii + jj) : ii + jj;
					smpl = ((samples[kk] & 0x1FF) << 3) + corr;
					if (((onecnt & 1) && pairing)
							|| (smpl <= ((avg * thr_md + 128) >> 8))) {
						/* set 1 */
						cost += ((uint32_t) (2 * smpl - avg)
								* (uint32_t) (2 * smpl - avg)) / 5;
						an = (2 * ((uint32_t) smpl + 2 * (uint32_t) avg)) / 5; /* new avg target */
						da = ((an - avg) * lpfc + 4) >> 3;
						avg += da; /* filter avg */
						onecnt++;
						corr = 2 * da;
					} else {
						/* set 0 */
						cost += ((uint32_t) (smpl - avg)
								* (uint32_t) (smpl - avg)) >> 1;
						an = (smpl + avg) >> 1; /* new avg target */
						da = ((an - avg) * lpfc + 2) >> 2;
						avg += da; /* filter avg */
						onecnt = 0;
						corr = da;
					}
				}
				/* restore initial state */
				avg = avg0;
				smpl = smpl0;
				onecnt = onecnt0;
				/* select the try with smallest cost */
				bit = 1; /* will be bit selection */
				if (cost > cost0_14) {
					cost = cost0_14;
					bit = 0;
				}
				if (cost > cost1_34) {
					cost = cost1_34;
					bit = 1;
					lpfc = 3;
				}
				if (cost > cost0_34) {
					cost = cost0_34;
					bit = 0;
					lpfc = 3;
				}
				/* end of arbitration */
			}
			if (bit) {
				/* set 1 */
				an = (2 * ((uint32_t) smpl + 2 * (uint32_t) avg)) / 5; /* new avg target */
				avg = avg + (((an - avg) * lpfc + 4) >> 3); /* filter avg */
				if (onecnt & 1) {
					bit = 1 << (*nbits & 0x7);
					bits[*nbits >> 3] |= bit;
					(*nbits)++;
				}
				onecnt++;
				zcnt = 0;
				oneflag = 1;
			} else {
				/* set 0 */
				an = (smpl + avg) >> 1; /* new avg target */
				avg = avg + (((an - avg) * lpfc + 2) >> 2); /* filter avg */
				(*nbits)++;
				onecnt = 0;
				zcnt++;
			}
		} else
			avg = smpl;

		/* sum avg */
		avgsum += (uint32_t) avg;
		avgsum_cnt++;
	}
	*nbits += 8; /* add trailing 0s */
	/* Calculate the approximate swipe speed */
	*speed = ((ADC_DSP_FREQ * 10) >> 4)
			/ (((avgsum / avgsum_cnt) * trackpar->density) >> 4);

	return NO_ERROR;
}

void mml_msr_pre_init(void) {

	/* Attach vector */
	NVIC_SetVector(MSR_IRQn, (uint32_t)MSR_DSP_IRQHandler);

	/* Set priority level for interrupt */
	NVIC_SetPriority(MSR_IRQn, priority_table[MSR_IRQn]);
}

int mml_msr_set_interrupt_priority(mml_intc_prio_t priority) {
	/* Check if priority is in correct range */
	if (priority > MML_INTC_PRIO_7)
		return COMMON_ERR_INVAL;

	/* Then update priority_table */
	priority_table[MSR_IRQn] = priority;
	/* then set interrupt priority */
	NVIC_SetPriority(MSR_IRQn, priority);

	return NO_ERROR;
}

int mml_msr_init(void) {
	unsigned int apb_freq;
	int i;

	for (i = 0; i < MSR_NUM_TRACKS; i++){
		if(!(mml_msr_context[i].first_init)) {
			mml_msr_context[i].first_init = 1;
		}
	}
	if ( (mml_msr_context[0].first_init) & (mml_msr_context[1].first_init) & (mml_msr_context[2].first_init) ){
		/* Enable Magnetic stripe reader clock */
		GCR->PERCKCN &= ~(GCR_PERCKCN_MAGDSPD_Msk);
	}
	mml_get_apb_frequency(&apb_freq);
	MSR->DSPCTRL = (apb_freq / ADC_DSP_FREQ) << MSR_DSPCTRL_DIV_RATIO_Pos;
	MSR->DSPCTRL |= MSR_DSPCTRL_DSP_PKDETECT_Msk;
	/* Configure the DSP settings */
	MSR->DSPADDR = MSR_DSPADDR_ADDR_t13ZctFast;
	MSR->DSPDATA = 0x01F4;
	MSR->DSPADDR = MSR_DSPADDR_ADDR_t13ZctMid;
	MSR->DSPDATA = 0x00FA;
	MSR->DSPADDR = MSR_DSPADDR_ADDR_t13ZctSlow;
	MSR->DSPDATA = 0x007D;
	MSR->DSPADDR = MSR_DSPADDR_ADDR_t2ZctFast;
	MSR->DSPDATA = 0x0190;
	MSR->DSPADDR = MSR_DSPADDR_ADDR_t2ZctMid;
	MSR->DSPDATA = 0x00FA;
	MSR->DSPADDR = MSR_DSPADDR_ADDR_t2ZctSlow;
	MSR->DSPDATA = 0x0064;
	MSR->DSPADDR = MSR_DSPADDR_ADDR_startPcnt;
	MSR->DSPDATA = 2;
	/* Configure and power up the A/D converters */
	MSR->DSPADDR = MSR_DSPADDR_ADDR_adcCfg;
	MSR->DSPDATA = MML_MSR_ADCCFG1_INT_REF | MML_MSR_ADCCFG1_PUADC
			| MML_MSR_ADCCFG1_VREF_SEL_0P5V | MML_MSR_ADCCFG1_RCR2_12KOHM;
	/* Initialize MSR context */
	for (i = 0; i < MSR_NUM_TRACKS; i++) {
		/* Clear track state */
		memset(mml_msr_context[i].raw_data, 0,
				sizeof(mml_msr_context[i].raw_data));
		mml_msr_context[i].raw_len = 0;
		mml_msr_context[i].raw_idx = 0;
		mml_msr_context[i].dma_channel = mml_dma_get_free_channel();
		mml_msr_context[i].dma_addr.dst =
				(uint32_t) mml_msr_context[i].raw_data;
		mml_msr_context[i].dma_addr.count = 8; /* bytes */
		mml_msr_context[i].dma_config.burst_size = 8;
		mml_msr_context[i].dma_config.dst_increment = 1;
		mml_msr_context[i].dma_config.dst_width = DMAn_CFG_DSTWD_word;
		mml_msr_context[i].dma_config.src_width = DMAn_CFG_SRCWD_halfWord;
		mml_msr_context[i].dma_config.request = DMAn_CFG_REQSEL_msrT1Rx + i;
		mml_msr_context[i].dma_config.handler = mml_MSR_DMA_IRQHandler;
		mml_dma_init_without_irq(mml_msr_context[i].dma_channel,
				mml_msr_context[i].dma_config, mml_msr_context[i].dma_addr);
		mml_msr_context[i].dma_status = MML_MSR_DMA_STATUS_INIT;
		mml_dma_enable(mml_msr_context[i].dma_channel);
	}
	/* Setup the interrupt handler */
	NVIC_EnableIRQ(MSR_IRQn);

	return NO_ERROR;
}

void mml_msr_restart(void) {
	unsigned int i;

	/* Initialize MSR context */
	for (i = 0; i < MSR_NUM_TRACKS; i++) {
		/* Clear track state */
		memset(mml_msr_context[i].raw_data, 0,
				sizeof(mml_msr_context[i].raw_data));
		mml_msr_context[i].raw_len = 0;
		mml_msr_context[i].raw_idx = 0;
		mml_msr_context[i].dma_addr.dst =
				(uint32_t) mml_msr_context[i].raw_data;
		mml_msr_context[i].dma_status = MML_MSR_DMA_STATUS_INIT;
		mml_msr_context[i].dma_addr.count = 8;
		mml_dma_set_address(mml_msr_context[i].dma_channel,
				mml_msr_context[i].dma_addr);
		mml_dma_enable(mml_msr_context[i].dma_channel);
	}
}

void mml_msr_enable(void) {
	MSR->DSPCTRL |= MSR_DSPCTRL_DSPEN_Msk;
}

void mml_msr_disable(void) {
	MSR->DSPCTRL &= ~MSR_DSPCTRL_DSPEN_Msk;
}

void mml_msr_set_complete_callback(void (*func)(void)) {
	complete_callback = func;
}

void MSR_DSP_IRQHandler(void) {
	unsigned int tidx;

	/* Clear the interrupt flag */
	MSR->DSPCTRL &= ~MSR_DSPCTRL_DSP_INTF_Msk;
	/* Disable to prevent corruption of captured data */
	mml_msr_disable();
	for (tidx = 0; tidx < MSR_NUM_TRACKS; tidx++) {
		mml_msr_context[tidx].dma_status = MML_MSR_DMA_STATUS_TRANSFERED;
		MSR->DSPADDR = MSR_DSPADDR_ADDR_t1DataCnt + tidx;
		mml_msr_context[tidx].raw_len = MSR->DSPDATA;
		mml_dma_disable(mml_msr_context[tidx].dma_channel);
	}
	/* Call the callback if it exists */
	if (complete_callback != NULL) {
		(*complete_callback)();
	}
}

void mml_MSR_DMA_IRQHandler(mml_dma_channel_t ch) {
	unsigned int tidx;

	/* Check each track's DMA channel for completion */
	for (tidx = 0; tidx < MSR_NUM_TRACKS; tidx++) {
		if (mml_msr_context[tidx].dma_channel == ch) {
			break;
		}
	}
	mml_msr_context[tidx].raw_idx += 4;
	if ((mml_msr_context[tidx].raw_idx < MSR_MAX_SAMPLES)
			&& !(MSR->DSPCTRL & MSR_DSPCTRL_DSP_INTF_Msk)) {
		/* Setup channel for next transfer */
		mml_msr_context[tidx].dma_addr.dst =
				(unsigned int) &mml_msr_context[tidx].raw_data[mml_msr_context[tidx].raw_idx];
		mml_msr_context[tidx].dma_addr.count = 8;
		mml_dma_set_address(ch, mml_msr_context[tidx].dma_addr);
		mml_dma_enable(ch);
	}
}

unsigned int mml_msr_track_decode(unsigned int track,
		mml_msr_decoded_track_t * decoded_track) {
	unsigned int tidx, i, j, bitdir, chardir;
	uint16_t samplesf[MSR_MAX_SAMPLES], *samples;
	unsigned int len;
	uint8_t bits[MSR_MAX_RAW_LEN_BYTES]; /* decoded bit storage */
	unsigned int nbits; /* # of decoded bits */
	mml_msr_decoded_track_t best_decode;
	unsigned int best_cnt, cnt;
	unsigned int speed;

	/* Check for a valid track number */
	if ((track == 0) || (track > MSR_NUM_TRACKS)) {
		memset(decoded_track, 0, sizeof(mml_msr_decoded_track_t));
		decoded_track->lrc = 0xFF;
		decoded_track->error_code = MML_MSR_ERRORS_MSR_ERR_PARAM;
		return 0;
	}
	/* Track array index (0 to 2) is 1 less than the track number (1 to 3) */
	tidx = track - 1;
	/* Get the number of samples */
	MSR->DSPADDR = MSR_DSPADDR_ADDR_t1DataCnt + tidx;
	len = MSR->DSPDATA;
	mml_msr_context[tidx].raw_len = len;
	/* Check if any samples were acquired */
	if (len == 0) {
		memset(decoded_track, 0, sizeof(mml_msr_decoded_track_t));
		decoded_track->lrc = 0xFF;
		decoded_track->error_code = MML_MSR_ERRORS_MSR_ERR_BAD_LEN;
		return 0;
	}
	/* Initialize track data */
	memset(&best_decode, 0, sizeof(mml_msr_decoded_track_t));
	best_decode.lrc = 0xFF; /* invalid */
	best_decode.error_code = MML_MSR_ERRORS_MSR_ERR_START_SEN
			| MML_MSR_ERRORS_MSR_ERR_END_SEN;
	best_cnt = 0;
	/* Start with the raw samples */
	samples = mml_msr_context[tidx].raw_data;
	for (i = 0; i < 2; i++) {
		/* Increase arbitration steps in second pass */
		if (i == 0) {
			narb = 5;
		} else {
			narb = 11;
		}
		for (j = 0; j < 3; j++) {
			/* try false peaks removal in 2nd and 3rd pass */
			if (j == 1) {
				remove_false_peaks(mml_msr_context[tidx].raw_data,
						mml_msr_context[tidx].raw_len, 4, samplesf, &len);
				samples = samplesf;
			} else if (j == 2) {
				remove_false_peaks(mml_msr_context[tidx].raw_data,
						mml_msr_context[tidx].raw_len, 6, samplesf, &len);
				samples = samplesf;
			}
			/* Try decoding in both directions */
			for (bitdir = MSR_FORWARD; bitdir <= MSR_REVERSE; bitdir++) {
				/* Decode the samples into bits */
				if (decode_track(samples, len, &trackpars[tidx], bitdir, bits,
						&nbits, &speed) == MML_MSR_ERRORS_MSR_ERR_OK) {
					/* at this point PPTs are decoded into bits in array */
					/* bits[], containing nbits bits */
					/* Try parsing in both directions */
					for (chardir = MSR_FORWARD; chardir <= MSR_REVERSE;
							chardir++) {
						/* Parse the bits into characters */
						cnt = parse_track(bits, nbits, &trackpars[tidx],
								chardir, decoded_track);
						decoded_track->direction = bitdir ^ chardir;
						decoded_track->speed = speed;
						/* If there were no errors and at least the sentinels and LRC are found, the decode was successful */
						if ((decoded_track->error_code
								== MML_MSR_ERRORS_MSR_ERR_OK) && (cnt >= 3))
							return decoded_track->len;
						/* Select the decode with the most valid characters */
						if (cnt > best_cnt) {
							memcpy(&best_decode, decoded_track,
									sizeof(mml_msr_decoded_track_t));
							best_cnt = cnt;
						}
					}
				}
			}
		}
	}
	/* Decode and parsing failed, select the best */
	memcpy(decoded_track, &best_decode, sizeof(mml_msr_decoded_track_t));
	return decoded_track->len;
}

unsigned int mml_msr_get_track_samples(unsigned int track,
		msr_samples_t * samples) {
	unsigned int tidx;

	memset(samples, 0, sizeof(msr_samples_t));
	/* Check for a valid track number */
	if ((track == 0) || (track > MSR_NUM_TRACKS))
		return 0;
	/* Track array index (0 to 2) is 1 less than the track number (1 to 3) */
	tidx = track - 1;
	/* Copy the raw sample information */
	samples->len = mml_msr_context[tidx].raw_len;
	memcpy(samples->data, mml_msr_context[tidx].raw_data,
			mml_msr_context[tidx].raw_len
					* sizeof(mml_msr_context[tidx].raw_data[0]));

	return samples->len;
}

const char * mml_msr_get_version(void) {
	return MML_MSR_VERSION_STRING;
}

#endif //_MML_MSR_H_

/* EOF */
