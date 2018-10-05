/*
 * @file mml_smon.h MML SMON Driver Header File
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

#ifndef _MML_SMON_H_
#define _MML_SMON_H_

/* Global includes */
#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_SMON SMON Driver ( Security Monitor )
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_SMON_VERS_MAJOR	3
#define MML_SMON_VERS_MINOR	0
#define MML_SMON_VERS_PATCH	4
#define MML_SMON_VERSION_STRING	"v" xstr(MML_SMON_VERS_MAJOR) "." xstr(MML_SMON_VERS_MINOR) "." xstr(MML_SMON_VERS_PATCH)

/* Defines ********************************************************************/
#define	MML_SMON_BASE_ERR						COBRA_SMON_BASE_ERR

/* Macros *********************************************************************/
#define	M_MML_SMON_KEY_WIPE()					( ((volatile mml_smon_regs_t*)MML_SEC_MON_IOBASE)->secalm |= MML_SMON_SECALM_KEYWIPE_MASK )

/* Enumerations ***************************************************************/
/**
 * INTSCN configuration
 */
typedef enum {
	MML_SMON_INTSCN_CONFIG_SHIELD_EN = 0x00000001,						/**< Enable SHIELD_EN SMON INTSCN register */
	MML_SMON_INTSCN_CONFIG_MIN = MML_SMON_INTSCN_CONFIG_SHIELD_EN,
	MML_SMON_INTSCN_CONFIG_TEMP_EN = 0x00000002,						/**< Enable TEMP_EN of SMON INTSCN register */
	MML_SMON_INTSCN_CONFIG_VBAT_EN = 0x00000004,						/**< Enable VBAT_EN of SMON INTSCN register */
	MML_SMON_INTSCN_CONFIG__LOTEMP_SEL = 0x00010000,					/**< Enable LOTEMP_SEL of SMON INTSCN register */
	MML_SMON_INTSCN_CONFIG_VCORELOEN  = 0x00040000,						/**< Enable VCORELOEN of SMON INTSCN register */
	MML_SMON_INTSCN_CONFIG_VCOREHIEN = 0x00080000,						/**< Enable VCOREHIEN of SMON INTSCN register */
	MML_SMON_INTSCN_CONFIG_VDDLOEN = 0x00100000,						/**< Enable VDDLOEN of SMON INTSCN register */
	MML_SMON_INTSCN_CONFIG_VDDHIEN = 0x00200000,						/**< Enable VDDHIEN of SMON INTSCN register */
	MML_SMON_INTSCN_CONFIG_VGLEN = 0x00400000,							/**< Enable VGLEN of SMON INTSCN register */
	MML_SMON_INTSCN_CONFIG_LOCK = 0x80000000,							/**< Enable LOCK of SMON INTSCN register */
	MML_SMON_INTSCN_CONFIG_MAX = MML_SMON_INTSCN_CONFIG_LOCK
} mml_smon_intscn_config;

/**
 *SECALM configuration
 */
typedef enum {
	MML_SMON_SECALM_CONFIG_INITIATE_DRS,								/**< Initiate DRS */
	MML_SMON_SECALM_CONFIG_MIN = MML_SMON_SECALM_CONFIG_INITIATE_DRS,
	MML_SMON_SECALM_CONFIG_INITIATE_KEYWIPE,							/**< Initiate Key wipe */
	MML_SMON_SECALM_CONFIG_MAX = MML_SMON_SECALM_CONFIG_INITIATE_KEYWIPE
} mml_smon_secalm_config;

/**
 * Pair of external sensor device
 */
typedef enum {
	MML_SMON_EXT_SENSOR0 = 0x01,	/**< External sensor pair 0 */
	MML_SMON_EXT_SENSOR1 = 0x02,	/**< External sensor pair 1 */
	MML_SMON_EXT_SENSOR2 = 0x04,	/**< External sensor pair 2 */
	MML_SMON_EXT_SENSOR3 = 0x08,	/**< External sensor pair 3 */
	MML_SMON_EXT_SENSOR4 = 0x10,	/**< External sensor pair 4 */
	MML_SMON_EXT_SENSOR5 = 0x20,	/**< External sensor pair 5 */
	MML_SMON_EXT_ALL = 0x3F,		/**< All external sensor pair */
} mml_smon_ext_sensor;

/* Structures *****************************************************************/

typedef struct {
	mml_smon_ext_sensor ext_sensor; /**< External sensor to enable. */
	unsigned char extcnt; /**< Number of external sensor accepted mismatches */
	SMON_EXTSCN_EXTFRQ_Type ext_divider; /**< frequency which the external sensor are clocked */
	SMON_EXTSCN_DIVCLK_Type divider_divclk;/**< Divider for input 8kHz clock */
	SMON_EXTSCN_LOCK_Type ext_lock; /**< Locked or don't locked EXTSCN register */
	SMON_INTSCN_LOCK_Type int_lock; /**< Locked or don't locked INTSCN register */
	mml_smon_intscn_config intscn_config; /**< INTSCN configuration */
	SMON_INTSCN_LOTEMP_SEL_Type select_lowtemp; /**< Select low temperature for INTSCN */
	mml_smon_secalm_config secalm_config; /**< SECALM configuration */
} mml_smon_config_t;

/**
 * Return the version of the SMON driver.
 * 
 */
const char * mml_smon_get_version( void );

/**
 * This function is used to configure SMON
 * @param[in] config						Configuration to set
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 */
int mml_smon_set_config(mml_smon_config_t config);

/**
 * This function is used to write value in INTSCN register
 * @param[in] value							Value to write in extscn register
 */
void mml_smon_write_intscn_register(unsigned int value);

/**
 * This function is used to configure INTSCN register
 * @param[in] config						Configuration to set
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid range
 */
int mml_smon_intscn_config_f(unsigned int config);

/**
 * This function is used to enable a pair of external sensor
 * @param[in] pair							Pairs of external sensor to enable
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Pair of external sensor are not in valid range
 * @retval COMMON_ERR_UNKNOWN				Error unknown
 */
int mml_smon_enable_external_sensor(unsigned int pair);

/**
 * This function is used to disable a pair of external sensor
 * @param[in] pair							Pairs of external sensor to enable
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Pair of external sensor are not in valid range
 * @retval COMMON_ERR_UNKNOWN				Error unknown
 */
int mml_smon_disable_external_sensor(unsigned int pair);

/**
 * This function is used to set the number of external sensor accepted mismatched that have
 * to occur within a single bit period before an external sensor alarm is triggered
 * @param [in] value					Number of accepted mismatches
 * 										Value must be inferior to 31
 * @return NO_ERROR						No error
 * @return COMMON_ERR_INVAL				value is superior at 31
 * @retval COMMON_ERR_UNKNOWN			Error unknown
 */
int mml_smon_set_extcnt(unsigned int value);

/**
 * This function is used to set divider for external sensor frequency
 * param[in] divider					Divider to set
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Divider is not in a valid range
 * @retval COMMON_ERR_UNKNOWN			Error unknown
 */
int mml_smon_set_extfrq(SMON_EXTSCN_EXTFRQ_Type divider);

/**
 * This function is used to divides the 8kHz input clock
 * @param[in] divider
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Freq is not in a valid range
 * @retval COMMON_ERR_UNKNOWN			Error unknown
 */
int mml_smon_set_divclk(SMON_EXTSCN_DIVCLK_Type divider);

/**
 * This function is used to set and locked LOCK register of External Sensor Control Register.
 * Once locked the EXTSCN register can no longer be modified.
 * Only battery disconnect will clear this bit.
 */
void mml_smon_set_extscn_lock(void);

/**
 * This function is used to set and locked LOCK register of Internal Sensor Control Register.
 * Once locked the INTSCN register can no longer be modified.
 * Only battery disconnect will clear this bit.
 */
void mml_smon_set_intscn_lock(void);

/**
 * This function is used to write value in SECALM register
 * @param[in] value							Value to write in SECALM register
 */
void mml_smon_write_secalm(unsigned int value);

/**
 * This function is used to configure SECALM register
 * @param[in] config							Configuration to set
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid range for config
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 */
int mml_smon_secalm_config_f(mml_smon_secalm_config config);

/**
 * This function is used to read DRS log RTC value.
 * This register contains the 32bit value in the RTC second register when the last DRS event occurred
 * @return							Return register value
 */
int mml_smon_read_dlrtc(void);

/** Set value to SECALM register
 * @param[in] reg	Value to set in SECALM register
 */
void mml_smon_setsecalm(unsigned int reg);

/** Write value to SECALM register
 * @param[in] reg	Value of SECALM register to be written
 */
void mml_smon_writesecalm(unsigned int reg);

/** Read value from SECALM register
 * @param *reg	Pointer on read SECALM register value
 */
void mml_smon_readsecalm(unsigned int *reg);

/** Clear value from SECALM register
 * @param reg	Value to remove from SECALM register
 */
void mml_smon_clearsecalm(unsigned int reg);

/** Read value from SECDIAG register
 * @param *reg	Pointer on read SECDIAG register value
 */
void mml_smon_readsecdiag(unsigned int *reg);

/** @} *//* @defgroup MML_SMON */

#ifdef __cplusplus
}
#endif

#endif /* _MML_SMON_H_ */

/******************************************************************************/
/* EOF */
