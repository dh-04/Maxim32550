/*
 * @file mml_rtc.c MML RTC Driver C File
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

#ifdef _MML_RTC_H_
#include <errors.h>

__attribute__((section(".bss")))  mml_rtc_context_t mml_rtc_context;

/* */
#define WAIT_WHILE_RTC_BUSY()  while ((RTC->CN & RTC_CN_BUSY_Msk) != 0);

void mml_rtc_pre_init(void) {

	/* Attach vector */
	NVIC_SetVector(RTC_IRQn, (uint32_t)RTC_IRQHandler);

	/* Set priority level for interrupt */
	NVIC_SetPriority(RTC_IRQn, priority_table[RTC_IRQn]);
}

int mml_rtc_set_interrupt_priority(mml_intc_prio_t priority) {
	/* Check if priority is in correct range */
	if (priority > MML_INTC_PRIO_7)
		return COMMON_ERR_INVAL;

	/* Then update priority_table */
	priority_table[RTC_IRQn] = priority;
	/* then set interrupt priority */
	NVIC_SetPriority(RTC_IRQn, priority);

	return NO_ERROR;
}

int mml_rtc_set_time_of_day_alrm(uint32_t days, uint32_t hours,
		uint32_t minutes, uint32_t seconds) {
	int result = NO_ERROR;

	uint32_t seconde_days = 24 * 3600 * days;
	uint32_t seconde_hours = 3600 * hours;
	uint32_t seconds_minutes = 60 * minutes;
	uint32_t val_rtc_current_time = RTC->SEC;
	uint32_t val_set_rtc_ras = seconde_days + seconde_hours + seconds_minutes
			+ seconds + val_rtc_current_time;
	/* Check if alarm is not superior at 12days from current time */
	if (val_set_rtc_ras > (MML_RTC_RAS_12_DAYS + val_rtc_current_time)) {
		result = MML_RTC_RAS_ERROR_SUPPERIOR_12_DAYS;
	}
	/* Check if alarm is not inferior to 1s from current time */
	else if (val_set_rtc_ras < (1 + val_rtc_current_time)) {
		result = MML_RTC_RAS_ERROR_INFERIOR_1_SECOND;
	} else {
		/* Enable write enable */
		RTC->CN |= RTC_CN_WE_Msk;
		while(!(RTC->CN & RTC_CN_WE_Msk));
		/* Disable RTC */
		mml_rtc_disable();
		/* Disable Time of Day Alarm */
		result = mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_ADE_DISABLE);
		WAIT_WHILE_RTC_BUSY();
		/* Set val_set_rtc_ras in RTC_RAS register */
		if ((RTC->CN & RTC_CN_ADE_Msk) == 0) {
			WAIT_WHILE_RTC_BUSY();
			/* Clear RAS */
			RTC->RAS = 0;
			WAIT_WHILE_RTC_BUSY();
			/*Set new value */
			RTC->RAS |= val_set_rtc_ras;
			WAIT_WHILE_RTC_BUSY();
		}
		/* Disable write enable */
		RTC->CN &= ~(RTC_CN_WE_Msk);
		while(RTC->CN & RTC_CN_WE_Msk);
	}
	return result;
}

int mml_rtc_set_sub_second_alarm(uint32_t seconds, uint32_t sub_seconds) {
	int result = COMMON_ERR_UNKNOWN;
	uint32_t interval;
	uint32_t interval_seconds;

	/* Calculate number of clock tick to have seconds */
	interval_seconds = seconds * MML_RTC_SSEC_FREQ;
	/* Calculate number of total clock tick to have value of alarm */
	interval = interval_seconds + (sub_seconds / MML_RTC_SSEC_FREQ);
	if (interval + 1 > MML_RTC_RASS_16s) {
		result = MML_RTC_RSSA_ERROR_SUPPERIOR_16_SECOND;
		return result;
	}
	/* Enable write enable */
	RTC->CN |= RTC_CN_WE_Msk;

	/* Disable RTC */
	mml_rtc_disable();

	/* Disable sub-second Interrupt*/
	result = mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_ASE_DISABLE);
	WAIT_WHILE_RTC_BUSY();

	if ((RTC->CN & RTC_CN_ASE_Msk) == 0) {
		WAIT_WHILE_RTC_BUSY();
		/* Clear RSSA */
		RTC->RSSA = 0;
		WAIT_WHILE_RTC_BUSY();
		/* Set reload value */
		RTC->RSSA |= 0xFFFF - interval + 1;
	}
	return result;
}

void mml_rtc_enable(void) {

	RTC->CN |= RTC_CN_WE_Msk;
	WAIT_WHILE_RTC_BUSY();

	/* Set RTCE bits to 1 */
	RTC->CN |= RTC_CN_RTCE_Msk;

	/* Disable write enable */
	RTC->CN &= ~(RTC_CN_WE_Msk);
}

void mml_rtc_disable(void) {
	RTC->CN |= RTC_CN_WE_Msk;
	WAIT_WHILE_RTC_BUSY();

	/* Set RTCE bits to 0 */
	RTC->CN &= ~(RTC_CN_RTCE_Msk);

	/* Disable write enable */
	RTC->CN &= ~(RTC_CN_WE_Msk);
}

int mml_rtc_configure_interrupt(mml_rtc_interrupt_config_t config) {
	switch (config) {
	case MML_RTC_INTERRUPT_CONFIG_ADE_ENABLE:
		WAIT_WHILE_RTC_BUSY();
		RTC->CN |= RTC_CN_ADE_Msk;
		break;
	case MML_RTC_INTERRUPT_CONFIG_ADE_DISABLE:
		WAIT_WHILE_RTC_BUSY();
		RTC->CN &= ~(RTC_CN_ADE_Msk);
		break;
	case MML_RTC_INTERRUPT_CONFIG_ASE_ENABLE:
		WAIT_WHILE_RTC_BUSY();
		RTC->CN |= RTC_CN_ASE_Msk;
		break;
	case MML_RTC_INTERRUPT_CONFIG_ASE_DISABLE:
		WAIT_WHILE_RTC_BUSY();
		RTC->CN &= ~(RTC_CN_ASE_Msk);
		break;
	case MML_RTC_INTERRUPT_CONFIG_RDYE_ENABLE:
		WAIT_WHILE_RTC_BUSY();
		RTC->CN |= RTC_CN_RDYE_Msk;
		break;
	case MML_RTC_INTERRUPT_CONFIG_RDYE_DISABLE:
		WAIT_WHILE_RTC_BUSY();
		RTC->CN &= ~(RTC_CN_RDYE_Msk);
		break;
	default:
		return COMMON_ERR_INVAL;
		break;
	}
	WAIT_WHILE_RTC_BUSY();

	if(RTC->CN & (RTC_CN_ADE_Msk | RTC_CN_ASE_Msk | RTC_CN_RDYE_Msk))
		NVIC_EnableIRQ(RTC_IRQn);
	else
		NVIC_DisableIRQ(RTC_IRQn);

	return NO_ERROR;
}

int mml_rtc_clear_interrupt(mml_rtc_interrupt_flag_t flag){
	switch(flag){
	case MML_RTC_INTERRUPT_FLAG_ALDF:
		WAIT_WHILE_RTC_BUSY();
		RTC->CN &= ~RTC_CN_ALDF_Msk;
		WAIT_WHILE_RTC_BUSY();
		break;
	case MML_RTC_INTERRUPT_FLAG_ALSF:
		WAIT_WHILE_RTC_BUSY();
		RTC->CN &= ~RTC_CN_ALSF_Msk;
		WAIT_WHILE_RTC_BUSY();
		break;
	default:
		return COMMON_ERR_INVAL;
	}

	return NO_ERROR;
}

uint32_t mml_rtc_read_interrupt(void) {
	/* Read Time-of-Day Alarm interrupt flag */
	return ((RTC->CN & (RTC_CN_ALDF_Msk | RTC_CN_ALSF_Msk))>>RTC_CN_ALDF_Pos);
}

void mml_rtc_set_sqe(RTC_CN_SQE_Type value) {
	WAIT_WHILE_RTC_BUSY();
	RTC->CN	= (RTC->CN & ~RTC_CN_SQE_Msk) | ((value << RTC_CN_SQE_Pos) & RTC_CN_SQE_Msk);
	WAIT_WHILE_RTC_BUSY();
}

void mml_rtc_set_ft(RTC_CN_FT_Type value) {
	WAIT_WHILE_RTC_BUSY();
	RTC->CN	= (RTC->CN & ~RTC_CN_FT_Msk) | ((value << RTC_CN_FT_Pos) & RTC_CN_FT_Msk);
	WAIT_WHILE_RTC_BUSY();
}

int mml_rtc_config_x32kmd(RTC_CN_X32KMD_Type value) {
	WAIT_WHILE_RTC_BUSY();
	switch (value) {
		case RTC_CN_X32KMD_noiseImmuneMode:
		case RTC_CN_X32KMD_quietMode:
		case RTC_CN_X32KMD_quietInStopWithWarmup:
		case RTC_CN_X32KMD_quietInStopNoWarmup:
			RTC->CN	= (RTC->CN & ~RTC_CN_X32KMD_Msk) | ((value << RTC_CN_X32KMD_Pos) & RTC_CN_X32KMD_Msk);
			break;
		default:
			return COMMON_ERR_INVAL; }
	return NO_ERROR;
}

void mml_rtc_reset(void) {
	/* Poll on RTC busy bit */
	while ((( RTC->CN & RTC_CN_BUSY_Msk) >> RTC_CN_BUSY_Pos) == 1);

	/* First disable RTC */
	mml_rtc_disable();

	/* Set RTC control write enable */
	RTC->CN |= RTC_CN_WE_Msk;
	/* Poll on RTC busy bit */
	while ((( RTC->CN & RTC_CN_BUSY_Msk) >> RTC_CN_BUSY_Pos) == 1);

	/* Reset RTC second register */
	RTC->SEC = 0;
	/* Poll on RTC busy bit */
	while ((( RTC->CN & RTC_CN_BUSY_Msk) >> RTC_CN_BUSY_Pos) == 1);

	/* Reset RTC sub-second register */
	RTC->SSEC = 0;
	/* poll on RTC busy bit */
	while ((( RTC->CN & RTC_CN_BUSY_Msk) >> RTC_CN_BUSY_Pos) == 1);


	/* Reset time of day alarm */
	/* First be sure time of day alarm interrupt is disable */
	if(RTC->CN & RTC_CN_ADE_Msk) {
		mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_ADE_DISABLE);
	}
	WAIT_WHILE_RTC_BUSY();
	/* Reset RAS register */
	RTC->RAS = 0;
	WAIT_WHILE_RTC_BUSY();

	/* Reset sub-second alarm */
	/* First be sure sub-second alarm interrupt is disable */
	if(RTC->CN & RTC_CN_ASE_Msk) {
		mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_ASE_DISABLE);
	}
	WAIT_WHILE_RTC_BUSY();
	/* Reset RSSA register */
	RTC->RSSA = 0;
	WAIT_WHILE_RTC_BUSY();

	/* Reset TRIM register */
	RTC->TRIM = 0;

	/* Disable write enable */
	RTC->CN &= ~(RTC_CN_WE_Msk);

	/* Disable all interrupts */
	mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_ADE_DISABLE);
	mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_ASE_DISABLE);
	mml_rtc_configure_interrupt(MML_RTC_INTERRUPT_CONFIG_RDYE_DISABLE);

	/* Clear all interrupt flags */
	mml_rtc_clear_interrupt(MML_RTC_INTERRUPT_FLAG_ALDF);
	mml_rtc_clear_interrupt(MML_RTC_INTERRUPT_FLAG_ALSF);

	/* Reset SQE */
	mml_rtc_set_sqe(RTC_CN_SQE_disable);

	/* Reset FT */
	mml_rtc_set_ft(RTC_CN_FT_freq1Hz);

	/* Reset X32KMD */
	mml_rtc_config_x32kmd(RTC_CN_X32KMD_noiseImmuneMode);
}

void mml_rtc_start_up(void) {
	/* Wait for the busy flag */
	while ((( RTC->CN & RTC_CN_BUSY_Msk) >> RTC_CN_BUSY_Pos) == 1);

	/* Define second value */
	RTC->SEC = 0x00000000;
	/* Wait for the busy flag */
	while ((( RTC->CN & RTC_CN_BUSY_Msk) >> RTC_CN_BUSY_Pos) == 1);

	/* Define second value */
	RTC->SSEC = 0x00000000;
	/* poll on RTC busy bit */
	while ((( RTC->CN & RTC_CN_BUSY_Msk) >> RTC_CN_BUSY_Pos) == 1);

	/* Enable RTC */
	mml_rtc_enable();
}

const char * mml_rtc_get_version(void) {
	return MML_RTC_VERSION_STRING;
}

#endif /* _MML_RTC_H_ */

/******************************************************************************/
/* EOF */

