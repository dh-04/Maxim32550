/*
 * @file mml_dac.h MML DAC Driver Header File
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

#ifndef _MML_DAC_H_
#define _MML_DAC_H_

#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_DAC DAC Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_DAC_VERS_MAJOR	2
#define MML_DAC_VERS_MINOR	0
#define MML_DAC_VERS_PATCH	6
#define MML_DAC_VERSION_STRING	"v" xstr(MML_DAC_VERS_MAJOR) "." xstr(MML_DAC_VERS_MINOR) "." xstr(MML_DAC_VERS_PATCH)

/* Enumerations ***************************************************************/

/**
 * Description of DAC CTRL register
 */
typedef enum {
	MML_DAC_FLAG_CTRL_REGISTER_DACFAECNT,
	MML_DAC_FLAG_CTRL_REGISTER_MIN = MML_DAC_FLAG_CTRL_REGISTER_DACFAECNT,
	MML_DAC_FLAG_CTRL_REGISTER_DACFAF,
	MML_DAC_FLAG_CTRL_REGISTER_DACFAE,
	MML_DAC_FLAG_CTRL_REGISTER_DACINTERPMODE,
	MML_DAC_FLAG_CTRL_REGISTER_DACFFCNT,
	MML_DAC_FLAG_CTRL_REGISTER_DACSTMODE,
	MML_DAC_FLAG_CTRL_REGISTER_CPUDACST,
	MML_DAC_FLAG_CTRL_REGISTER_DACMODE,
	MML_DAC_FLAG_CTRL_REGISTER_DACPU,
	MML_DAC_FLAG_CTRL_REGISTER_RST,
	MML_DAC_FLAG_CTRL_REGISTER_MAX = MML_DAC_FLAG_CTRL_REGISTER_RST
} mml_dac_flag_ctrl_register;

/**
 * Description of DAC RATE register
 */
typedef enum {
	MML_DAC_FLAG_RATE_REGISTER_DACRATECNT,
	MML_DAC_FLAG_RATE_REGISTER_DACSAMPLECNT,
	MML_DAC_FLAG_RATE_REGISTER_MIN = MML_DAC_FLAG_RATE_REGISTER_DACRATECNT,
	MML_DAC_FLAG_RATE_REGISTER_MAX = MML_DAC_FLAG_RATE_REGISTER_DACSAMPLECNT
} mml_dac_flag_rate_register;

typedef enum {
	MML_DAC_INTERRUPT_MIN = 0,
	MML_DAC_INTERRUPT_DONE = MML_DAC_INTERRUPT_MIN, /**< Done interrupt */
	MML_DAC_INTERRUPT_FUFI, /**< FIFO underflow interrutp */
	MML_DAC_INTERRUPT_FAE /**< FIFO almost empty interrupt */
}mml_dac_interupt_t;

/**
 * Description of interrupt register
 */
typedef enum {
	MML_DAC_INT_REGISTER_DACDONEISR,
	MML_DAC_INT_REGISTER_DACFUFISR,
	MML_DAC_INT_REGISTER_DACFAEISR,
	MML_DAC_INT_REGISTER_FUFUST,
	MML_DAC_INT_REGISTER_DACDONEIE,
	MML_DAC_INT_REGISTER_DACFUFIE,
	MML_DAC_INT_REGISTER_DACFAEIE,
	MML_DAC_INT_REGISTER_MIN = MML_DAC_INT_REGISTER_DACDONEISR,
	MML_DAC_INT_REGISTER_MAX = MML_DAC_INT_REGISTER_DACFAEIE,
} mml_dac_int_register;

/**
 * Structure for configuration of mml_dac_ctrl_config
 */
typedef struct {
	int dacfaecnt;
	DAC_CTRL_DACINTERPMODE_Type interpolation_mode;
	int dacffcnt;
	int dacstmode;
	DAC_CTRL_DACMODE_Type dacmode;
	int dacrst;
} mml_dac_ctrl_config;

/**
 *
 */
typedef struct {
	int dacratecnt;
	int dacsamplecnt;
} mml_dac_rate_config;

/**
 * Structure for configuration of DAC
 */
typedef struct {
	mml_dac_ctrl_config dac_ctrl_config;
	mml_dac_rate_config dac_rate_config;
} mml_dac_config;

/**
 * Structure for DAC context
 */
typedef struct {
	unsigned int first_init;
	unsigned char interrupt_enable;
} mml_dac_context_t;

/**
 * Return the version of the DAC driver.
 * 
 */
const char * mml_dac_get_version(void);

/**
 * Configure DAC interrupt
 */
void mml_dac_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] priority						Interrupt priority to set
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for priority
 */
int mml_dac_set_interrupt_priority(mml_intc_prio_t priority);

/**
 * This function is used to initialize DAC
 */
void mml_dac_init(void);

/**
 * This function is used to reset DAC
 */
void mml_dac_reset(void);

/**
 * This function is used to configure DAC
 * @param[in] config			Configuration to set
 * @retval NO_ERROR				No error
 * @retval COMMON_ERR_INVAL		Invalid range for config
 * @retval COMMON_ERR_UNKNOWN	Unknown error
 */
int mml_dac_config_f(mml_dac_config config);

/**
 * This function is used to read flag of CTRL register
 * NOTE: you can read flag one by one
 * @param[in] ctrl				Bits to read
 * @param[out] flag				Pointer one the value of bit to read
 * @retval NO_ERROR				No error
 * @retval COMMON_ERR_INVAL		Invalid range for ctrl
 * @retval COMMON_ERR_NULL_PTR	when flag is NULL
 */
int mml_dac_read_ctrl_register(mml_dac_flag_ctrl_register ctrl,
		unsigned int *flag);

/**
 * This function is used to configure CTRL register
 * @param[in] config			Configuration to set
 * @retval NO_ERROR				No error
 * @retval COMMON_ERR_INVAL		Invalid range for config
 */
int mml_dac_config_ctrl_register(mml_dac_ctrl_config config);

/**
 * This function is used to power-up the DAC
 */
void mml_dac_power_up(void);

/**
 * This function is used to read DAC_RATE register
 * @param[in] dac_rate			Bits to read
 * @param[out] flag				Pointer on the value of bit to read
 * @retval NO_ERROR				No error
 * @retval COMMON_ERR_INVAL		Invalid range for dac_rate
 * @retval COMMON_ERR_NULL_PTR	when flag is NULL
 */
int mml_dac_read_dac_rate_register(mml_dac_flag_rate_register dac_rate,
		unsigned int *flag);

/**
 * This function is used to configure DAC RATE register
 * @param[in] config			Configuration to set
 * @retval NO_ERROR				No error
 * @retval COMMON_ERR_INVAL		Invalid range for config
 */
int mml_dac_config_dac_rate(mml_dac_rate_config config);

/**
 * This function is used to read interrupt register
 * @param[in] int_register		Bits to read
 * @param[out] flag				Pointer on the value of bit to read
 * @retval NO_ERROR				No error
 * @retval COMMON_ERR_INVAL		Invalid range for int_register
 * @retval COMMON_ERR_NULL_PTR	when flag is NULL
 */
int mml_dac_read_int_register(mml_dac_int_register int_register,
		unsigned int *flag);

/**
 * This function is used to set DAC FIFO
 * @param[in] value					Value to set
 */
void mml_dac_set_dacfifo(unsigned char value);

/**
 * This function is used to enable one interrupt
 * @param[in] interrupt				Interrupt to enable
 * @retval NO_ERROR					No error
 * @retval COMMON_ERR_INVAL			interrupt is in invalid range
 */
int mml_dac_interupt_enable(mml_dac_interupt_t interrupt);

/**
 * This function is used to disable one interrupt
 * @param[in] interrupt				Interrupt to disable
 * @retval NO_ERROR					No error
 * @retval COMMON_ERR_INVAL			interrupt is in invalid range
 */
int mml_dac_interupt_disable(mml_dac_interupt_t interrupt);

/**
 * This function is used to read one interrupt status
 * @param[in] interrupt				Interrupt to enable
 * @retval NO_ERROR					No error
 * @retval COMMON_ERR_INVAL			interrupt is in invalid range
 */
int mml_dac_read_interrupt(mml_dac_interupt_t interrupt);

/**
 * This function is used to disable DAC
 */
void mml_dac_deinit(void);

/** @} *//* @defgroup MML_DAC */

#ifdef __cplusplus
}
#endif

#endif /* _MML_DAC_H_ */

/******************************************************************************/
/* EOF */

