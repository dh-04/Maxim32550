/*
 * @file mml_gcr.c MML GCR Driver C File
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

#ifdef _MML_GCR_H_

#include <errors.h>

/* get mask value between start and stop bits */
#define GET_BIT_RANGE_MASK(s, e)  ( ( (1 << (e-s)) - 1) << s)

void mml_gcr_reset(unsigned int devs) {
	unsigned int loop = K_COBRA_RESET_WAIT_LOOP_MAX;

	/* Reset */
	GCR->RSTR = devs;
	/* Wait until resets end */
	while ((GCR->RSTR & devs) && loop--);
}

void mml_gcr_dev_reset(mml_rstr_dev_t dev) {
	if (dev < 32) {
		GCR->RSTR = (1 << dev);
	}
}

void mml_gcr_enable_clk_dev(unsigned int dev) {
	/* Enable dev(s) clock */
	GCR->PERCKCN &= ~dev;
}

void mml_gcr_disable_clk_dev(unsigned int dev) {
	/* Disable dev(s) clock */
	GCR->PERCKCN |= dev;
}

void mml_gcr_enable_in_stdbmode(unsigned int dev) {

	dev &= GET_BIT_RANGE_MASK(12, 15);
	GCR->PM |= dev;
}

void mml_gcr_disable_in_stdbmode(unsigned int dev) {

	dev &= GET_BIT_RANGE_MASK(12, 15);
	GCR->PM &= ~dev;
}

void mml_gcr_set_clk_src(GCR_CLKCN_CLKSEL_Type clksrc) {
	GCR->CLKCN = (clksrc << GCR_CLKCN_CLKSEL_Pos) & GCR_CLKCN_CLKSEL_Msk;
	while (!( GCR_CLKCN_CKRDY_Msk & GCR->CLKCN));
}

int mml_set_system_frequency(mml_system_frequency freq) {
	register unsigned int tmp;
	GCR_CLKCN_PSC_Type prescale_value;

	/* Check parameters */
	switch (freq) {
	case MML_SYSTEM_FREQUENCY_108_MHZ:
		prescale_value = GCR_CLKCN_PSC_div1;
		break;
	case MML_SYSTEM_FREQUENCY_54_MHz:
		prescale_value = GCR_CLKCN_PSC_div2;
		break;
	case MML_SYSTEM_FREQUENCY_27_MHz:
		prescale_value = GCR_CLKCN_PSC_div4;
		break;
	case MML_SYSTEM_FREQUENCY_13_5_MHz:
		prescale_value = GCR_CLKCN_PSC_div8;
		break;
	case MML_SYSTEM_FREQUENCY_6_75_MHz:
		prescale_value = GCR_CLKCN_PSC_div16;
		break;
	case MML_SYSTEM_FREQUENCY_3_375_MHz:
		prescale_value = GCR_CLKCN_PSC_div32;
		break;
	case MML_SYSTEM_FREQUENCY_1_6875_MHz:
		prescale_value = GCR_CLKCN_PSC_div64;
		break;
	case MML_SYSTEM_FREQUENCY_0_84375_MHz:
		prescale_value = GCR_CLKCN_PSC_div128;
		break;
	default:
		return COMMON_ERR_INVAL;
		break;
	}
	/* Set PLL enable and DLL enable bits ... */
	GCR->PLL0CN |= GCR_PLLCN_PLLn_EN_Msk;
	/* Wait PLLs are locked - lock bits shall be set to '1' when ready */
	while ((( GCR_PLLCN_PLLn_LOCK_Msk | GCR_PLLCN_PLLn_EN_Msk) & GCR->PLL0CN)
			!= ( GCR_PLLCN_PLLn_LOCK_Msk | GCR_PLLCN_PLLn_EN_Msk));
	/* ... for PLL1CN too ! */
	GCR->PLL1CN |= GCR_PLLCN_PLLn_EN_Msk;
	/* Wait PLLs are locked - lock bits shall be set to '1' when ready */
	while ((( GCR_PLLCN_PLLn_LOCK_Msk | GCR_PLLCN_PLLn_EN_Msk) & GCR->PLL1CN)
			!= ( GCR_PLLCN_PLLn_LOCK_Msk | GCR_PLLCN_PLLn_EN_Msk));
	/* Remove previous prescaler and clock source values */
	tmp = GCR->CLKCN;
	tmp &= ~( GCR_CLKCN_PSC_Msk | GCR_CLKCN_CLKSEL_Msk);
	tmp |= ((prescale_value << GCR_CLKCN_PSC_Pos)
			| (GCR_CLKCN_CLKSEL_pll_Output  << GCR_CLKCN_CLKSEL_Pos));
	/* ... then set prescaler and clock source selection */
	GCR->CLKCN = tmp;
	/* Wait that it's done */
	while (!( GCR_CLKCN_CKRDY_Msk & GCR->CLKCN));

	return NO_ERROR;
}

int mml_get_system_frequency(unsigned int *p_freq) {
	GCR_CLKCN_PSC_Type prescale_value;

	/* Check input pointer */
	if (!p_freq)
		return COMMON_ERR_NULL_PTR;
	prescale_value = ((GCR->CLKCN) & GCR_CLKCN_PSC_Msk) >> GCR_CLKCN_PSC_Pos;
	if (GCR_CLKCN_PSC_div128 >= prescale_value) {
		switch (prescale_value) {
		case GCR_CLKCN_PSC_div1:
			*p_freq = MML_SYSTEM_FREQUENCY_108_MHZ;
			break;
		case GCR_CLKCN_PSC_div2:
			*p_freq = MML_SYSTEM_FREQUENCY_54_MHz;
			break;
		case GCR_CLKCN_PSC_div4:
			*p_freq = MML_SYSTEM_FREQUENCY_27_MHz;
			break;
		case GCR_CLKCN_PSC_div8:
			*p_freq = MML_SYSTEM_FREQUENCY_13_5_MHz;
			break;
		case GCR_CLKCN_PSC_div16:
			*p_freq = MML_SYSTEM_FREQUENCY_6_75_MHz;
			break;
		case GCR_CLKCN_PSC_div32:
			*p_freq = MML_SYSTEM_FREQUENCY_3_375_MHz;
			break;
		case GCR_CLKCN_PSC_div64:
			*p_freq = MML_SYSTEM_FREQUENCY_1_6875_MHz;
			break;
		case GCR_CLKCN_PSC_div128:
			*p_freq = MML_SYSTEM_FREQUENCY_0_84375_MHz;
			break;
		}
		return NO_ERROR;
	} else
		return COMMON_ERR_OUT_OF_RANGE;
}

int mml_get_ahb_frequency(unsigned int *p_freq) {
	/* Retrieve AHB divider - same frequency as core one */
	return mml_get_system_frequency(p_freq);
}

int mml_get_apb_frequency(unsigned int *p_freq) {
	int result = COMMON_ERR_UNKNOWN;

	/* Retrieve APB divider - Half of AHB frequency */
	result = mml_get_system_frequency(p_freq);
	if (NO_ERROR == result) {
		/* Apply divider */
		*p_freq >>= 1;
	}
	return result;
}

void mml_gcr_flip_iflash(void) {
	/* Activate flip bit - Bottom half mapped to logical top half and vice versa*/
	GCR->SCON |= GCR_SCON_FLASH_PAGE_FLIP_Msk;
}

void mml_gcr_unflip_iflash(void) {
	/* De-activate flip bit - Physical layout matches logical layout */
	GCR->SCON &= ~GCR_SCON_FLASH_PAGE_FLIP_Msk;
}

unsigned char mml_gcr_get_flip_state(void) {
	return (GCR->SCON & GCR_SCON_FLASH_PAGE_FLIP_Msk)
			>> GCR_SCON_FLASH_PAGE_FLIP_Pos;
}

void mml_gcr_zeroize_memory(mml_gcr_memz_t mem_to_zeroize) {
	GCR->MEMZCN |= (0x01 << mem_to_zeroize);
}

void mml_gcr_enable_BSTAP(void) {
	GCR->SCON |= GCR_SCON_BSTAPEN_Msk;
}

void mml_gcr_disable_BSTAP(void) {
	GCR->SCON &= ~GCR_SCON_BSTAPEN_Msk;
}

void mml_gcr_enable_CM3_pm(void) {
	GCR->PM |= GCR_PM_CM3PMUEN_Msk;
}

void mml_gcr_disable_CM3_pm(void) {
	GCR->PM &= ~GCR_PM_CM3PMUEN_Msk;
}

unsigned char mml_gcr_checkROM_checksum(void) {
	GCR->SCON |= GCR_SCON_CCHK_Msk;
	/* Check finish by polling Check Ready Bit (CHKRD) */
	while ((GCR->SCON & GCR_SCON_CHKRD_Msk) != GCR_SCON_CHKRD_Msk);

	return (GCR->SCON & GCR_SCON_CHKRES_Msk) >> GCR_SCON_CHKRES_Pos;
}

unsigned char mml_gcr_get_battery_source(void) {
	return (GCR->SCON & GCR_SCON_ONVMAIN_Msk) >> GCR_SCON_ONVMAIN_Pos;
}

void mml_gcr_enable_wakeup_src(unsigned int dev) {

	dev &= GET_BIT_RANGE_MASK(4, 8);
	GCR->PM |= dev;
}

void mml_gcr_disable_wakeup_src(unsigned int dev) {

	dev &= GET_BIT_RANGE_MASK(4, 8);
	GCR->PM &= ~dev;
}

void mml_gcr_enable_pll0(void) {
	GCR->PLL0CN |= GCR_PLLCN_PLLn_EN_Msk;
	/* Wait PLLs are locked - lock bits shall be set to '1' when ready */
	while ((( GCR_PLLCN_PLLn_LOCK_Msk | GCR_PLLCN_PLLn_EN_Msk) & GCR->PLL0CN)
			!= ( GCR_PLLCN_PLLn_LOCK_Msk | GCR_PLLCN_PLLn_EN_Msk));
}

void mml_gcr_enable_pll1(void) {
	GCR->PLL1CN |= GCR_PLLCN_PLLn_EN_Msk;
	/* Wait PLLs are locked - lock bits shall be set to '1' when ready */
	while ((( GCR_PLLCN_PLLn_LOCK_Msk | GCR_PLLCN_PLLn_EN_Msk) & GCR->PLL1CN)
			!= ( GCR_PLLCN_PLLn_LOCK_Msk | GCR_PLLCN_PLLn_EN_Msk));
}

void mml_gcr_disable_pll1(void) {
	GCR->PLL1CN &= ~GCR_PLLCN_PLLn_EN_Msk;
}

void mml_gcr_disable_pll0(void) {
	GCR->PLL0CN &= ~GCR_PLLCN_PLLn_EN_Msk;
}

unsigned char mml_gcr_get_CM3WIC_ack(void) {
	return ((GCR->PM & GCR_PM_CM3WICACK_Msk) >> GCR_PM_CM3WICACK_Pos);
}

void mml_gcr_set_sc_clk(mml_gcr_sc_freq_t freq) {
	unsigned int tmp;

	tmp  = GCR->SCCK;
	tmp &= ~GCR_SCCK_SC0FRQ_Msk;
	tmp |= (freq << GCR_SCCK_SC0FRQ_Pos);

	GCR->SCCK = tmp;
}

const char * mml_gcr_get_version(void) {
	return MML_GCR_VERSION_STRING;
}

#endif //_MML_GCR_H_

/*****************************************************************************/
/* EOF */
