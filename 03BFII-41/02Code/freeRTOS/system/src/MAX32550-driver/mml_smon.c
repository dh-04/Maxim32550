/*
 * @file mml_smon.c MML SMON Driver C File
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

#include<MAX325xx.h>

#ifdef _MML_SMON_H_

#include <errors.h>

/* */
#define WAIT_FOR_ACCESS(mask)  while((SMON->SECST & (mask)) != 0);
#define WAIT_UNTIL_UPDATE_DONE(reg, val)  for(int counter = 0; counter < 10000; counter++) {  \
												if ( (reg) == (val) ) break; 				  \
												else reg = val;								  \
										  }

int mml_smon_set_config(mml_smon_config_t config) {
	int result = NO_ERROR;

	if ( (config.ext_sensor == 0) && (config.intscn_config == 0)) {
		return COMMON_ERR_INVAL;
	}

	/* Configuration of EXTSCN Register */
	/* Enable pair of sensor */
	if (config.ext_sensor != 0) {
		/* even if external sensors will be 0000 after BOR reset, check wrong usage condition */
		if ( (SMON->EXTSCN & MML_SMON_EXT_ALL) != 0 ){
			mml_smon_disable_external_sensor(MML_SMON_EXT_ALL);
		}
		/* Enable external sensors  */
		result = mml_smon_enable_external_sensor(config.ext_sensor);
		if(result)
			return result;
		/* Set number of external sensor accepted mismatches */
		result = mml_smon_set_extcnt(config.extcnt);
		if(result)
			return result;
		/* Select frequency which the external sensor are clocked */
		result = mml_smon_set_extfrq(config.ext_divider);
		if(result)
			return result;
		/* Select divider for input 8kHz clock*/
		result = mml_smon_set_divclk(config.divider_divclk);
		if(result)
			return result;
		/* Locked EXTSNC Register */
		if (config.ext_lock == SMON_EXTSCN_LOCK_locked) {
			mml_smon_set_extscn_lock();
		}
	}

	if (config.intscn_config != 0) {
		/* Configuration of INTSCN Register */
		result = mml_smon_intscn_config_f(config.intscn_config);
		if(result)
			return result;
		/* Locked EXTSNC Register */
		if (config.int_lock == SMON_INTSCN_LOCK_locked) {
			mml_smon_set_intscn_lock();
		}
	}

	if (config.secalm_config <= MML_SMON_SECALM_CONFIG_MAX) {
		/* Configuration of SECALM Register */
		result = mml_smon_secalm_config_f(config.secalm_config);
	}

	return result;
}

void mml_smon_write_intscn_register(unsigned int value){

	WAIT_FOR_ACCESS(SMON_SECST_INTSRS_Msk);
	SMON->INTSCN = value;
	WAIT_FOR_ACCESS(SMON_SECST_INTSRS_Msk);

	WAIT_UNTIL_UPDATE_DONE(SMON->INTSCN, value);
}

int mml_smon_intscn_config_f(mml_smon_intscn_config config) {

	config &= (	MML_SMON_INTSCN_CONFIG_SHIELD_EN 	|
				MML_SMON_INTSCN_CONFIG_TEMP_EN   	|
				MML_SMON_INTSCN_CONFIG_VBAT_EN	 	|
				MML_SMON_INTSCN_CONFIG__LOTEMP_SEL  |
				MML_SMON_INTSCN_CONFIG_VCORELOEN   	|
				MML_SMON_INTSCN_CONFIG_VCOREHIEN    |
				MML_SMON_INTSCN_CONFIG_VDDLOEN      |
				MML_SMON_INTSCN_CONFIG_VDDHIEN	    |
				MML_SMON_INTSCN_CONFIG_VGLEN		|
				MML_SMON_INTSCN_CONFIG_LOCK
			 );

	if (config == 0){
		return COMMON_ERR_INVAL;
	}

	mml_smon_write_intscn_register(config);

	if (config != SMON->INTSCN ) {
		return COMMON_ERR_UNKNOWN;
	} else {
		return NO_ERROR;
	}
}

int mml_smon_enable_external_sensor(unsigned int pair) {
	unsigned int tmp;

	if ((pair < MML_SMON_EXT_SENSOR0) | (pair > MML_SMON_EXT_ALL)) {
		return COMMON_ERR_INVAL;
	}

	pair &= (	MML_SMON_EXT_SENSOR0 	|
				MML_SMON_EXT_SENSOR1   	|
				MML_SMON_EXT_SENSOR2	|
				MML_SMON_EXT_SENSOR3  	|
				MML_SMON_EXT_SENSOR4   	|
				MML_SMON_EXT_SENSOR5
			 );

	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);
	tmp  = SMON->EXTSCN;
	tmp |= pair;
	SMON->EXTSCN  = tmp;
	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);

	WAIT_UNTIL_UPDATE_DONE(SMON->EXTSCN, tmp);

	if (tmp != SMON->EXTSCN ) {
		return COMMON_ERR_UNKNOWN;
	} else {
		return NO_ERROR;
	}
}

int mml_smon_disable_external_sensor(unsigned int pair) {
	unsigned int tmp;

	if ((pair < MML_SMON_EXT_SENSOR0) | (pair > MML_SMON_EXT_ALL)) {
		return COMMON_ERR_INVAL;
	}

	pair &= (	MML_SMON_EXT_SENSOR0 	|
				MML_SMON_EXT_SENSOR1   	|
				MML_SMON_EXT_SENSOR2	|
				MML_SMON_EXT_SENSOR3  	|
				MML_SMON_EXT_SENSOR4   	|
				MML_SMON_EXT_SENSOR5
			 );

	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);
	tmp  = SMON->EXTSCN;
	tmp &= ~pair;
	SMON->EXTSCN = tmp;
	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);

	WAIT_UNTIL_UPDATE_DONE(SMON->EXTSCN, tmp);

	if (tmp != SMON->EXTSCN ) {
		return COMMON_ERR_UNKNOWN;
	} else {
		return NO_ERROR;
	}
}

int mml_smon_set_extcnt(unsigned int value) {
	unsigned int tmp;

	if (value > 31)
		return COMMON_ERR_INVAL;

	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);
		tmp  = SMON->EXTSCN;
		tmp &= ~SMON_EXTSCN_EXTCNT_Msk;
		tmp |= (value << SMON_EXTSCN_EXTCNT_Pos) & SMON_EXTSCN_EXTCNT_Msk;
		SMON->EXTSCN = tmp;
	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);

	WAIT_UNTIL_UPDATE_DONE(SMON->EXTSCN, tmp);

	if (tmp != SMON->EXTSCN ) {
		return COMMON_ERR_UNKNOWN;
	} else {
		return NO_ERROR;
	}
}

int mml_smon_set_extfrq(SMON_EXTSCN_EXTFRQ_Type divider) {
	unsigned int tmp;

	if (divider > SMON_EXTSCN_EXTFRQ_freq31Hz)
		return COMMON_ERR_INVAL;

	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);
		tmp  = SMON->EXTSCN;
		tmp &= ~SMON_EXTSCN_EXTFRQ_Msk;
		tmp |= (divider<<SMON_EXTSCN_EXTFRQ_Pos) & SMON_EXTSCN_EXTFRQ_Msk;
		SMON->EXTSCN = tmp;
	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);

	WAIT_UNTIL_UPDATE_DONE(SMON->EXTSCN, tmp);

	if (tmp != SMON->EXTSCN) {
		return COMMON_ERR_UNKNOWN;
	} else {
		return NO_ERROR;
	}
}

int mml_smon_set_divclk(SMON_EXTSCN_DIVCLK_Type divider) {
	unsigned int tmp;

	if (divider > SMON_EXTSCN_DIVCLK_div64)
		return COMMON_ERR_INVAL;

	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);
		tmp  = SMON->EXTSCN;
		tmp &= ~SMON_EXTSCN_DIVCLK_Msk;
		tmp |= (divider<<SMON_EXTSCN_DIVCLK_Pos) & SMON_EXTSCN_DIVCLK_Msk;
		SMON->EXTSCN = tmp;
	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);

	WAIT_UNTIL_UPDATE_DONE(SMON->EXTSCN, tmp);

	if (tmp != SMON->EXTSCN) {
		return COMMON_ERR_UNKNOWN;
	} else {
		return NO_ERROR;
	}
}

void mml_smon_set_extscn_lock(void) {
	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);
	SMON->EXTSCN |= SMON_EXTSCN_LOCK_Msk;
	WAIT_FOR_ACCESS(SMON_SECST_EXTSRS_Msk);

	WAIT_UNTIL_UPDATE_DONE(SMON->EXTSCN, (SMON->EXTSCN | SMON_EXTSCN_LOCK_Msk));
}

void mml_smon_set_intscn_lock(void) {
	WAIT_FOR_ACCESS(SMON_SECST_INTSRS_Msk);
	SMON->INTSCN |= SMON_INTSCN_LOCK_Msk;
	WAIT_FOR_ACCESS(SMON_SECST_INTSRS_Msk);

	WAIT_UNTIL_UPDATE_DONE(SMON->INTSCN, (SMON->INTSCN | SMON_INTSCN_LOCK_Msk));
}

void mml_smon_write_secalm(unsigned int value){
	mml_smon_writesecalm(value);
}

int mml_smon_secalm_config_f(mml_smon_secalm_config config) {
	if (config > MML_SMON_SECALM_CONFIG_MAX)
		return COMMON_ERR_INVAL;


	switch (config) {
	case MML_SMON_SECALM_CONFIG_INITIATE_DRS:
		WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);
		SMON->SECALM |= SMON_SECALM_DRS_Msk;
		WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);

		WAIT_UNTIL_UPDATE_DONE(SMON->SECALM, (SMON->SECALM | SMON_SECALM_DRS_Msk));
		break;
	case MML_SMON_SECALM_CONFIG_INITIATE_KEYWIPE:
		WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);
		SMON->SECALM |= SMON_SECALM_KEYWIPE_Msk;
		WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);

		WAIT_UNTIL_UPDATE_DONE(SMON->SECALM, (SMON->SECALM | SMON_SECALM_KEYWIPE_Msk));
		break;
	}

	return NO_ERROR;
}

int mml_smon_read_dlrtc(void) {
	return ((SMON->DLRTC));
}

extern inline void mml_smon_setsecalm(unsigned int reg) {
	WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);
	SMON->SECALM |= reg;
	WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);

	WAIT_UNTIL_UPDATE_DONE(SMON->SECALM, (SMON->SECALM | reg));
}

extern inline void mml_smon_writesecalm(unsigned int reg) {
	WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);
	SMON->SECALM = reg;
	WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);

	WAIT_UNTIL_UPDATE_DONE(SMON->SECALM, reg);
}

extern inline void mml_smon_readsecalm(unsigned int *reg) {
	/* Check input pointer */
	if (reg) {
		WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);
		*reg = SMON->SECALM;
		WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);
	}
}

extern inline void mml_smon_clearsecalm(unsigned int reg) {
	unsigned int tmp;

	WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);
	tmp = SMON->SECALM;
	tmp &= ~reg;
	SMON->SECALM &= tmp;
	WAIT_FOR_ACCESS(SMON_SECST_SECALRS_Msk);
}

extern inline void mml_smon_readsecdiag(unsigned int *reg) {
	/* Check input pointer */
	if (reg)
		*reg = SMON->SECDIAG;
}

const char * mml_smon_get_version( void ) {
    return MML_SMON_VERSION_STRING;
}

#endif //_MML_SMON_H_

/* EOF */
