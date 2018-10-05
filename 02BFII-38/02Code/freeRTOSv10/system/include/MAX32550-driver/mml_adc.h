/*
 * @file mml_adc.h MML ADC Driver Header File
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

#ifndef _MML_ADC_H_
#define _MML_ADC_H_

#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_ADC ADC Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_ADC_VERS_MAJOR	2
#define MML_ADC_VERS_MINOR	0
#define MML_ADC_VERS_PATCH	1
#define MML_ADC_VERSION_STRING	"v" xstr(MML_ADC_VERS_MAJOR) "." xstr(MML_ADC_VERS_MINOR) "." xstr(MML_ADC_VERS_PATCH)

/* Enumerations ***************************************************************/

/**
 * ADC error list
 */
typedef enum {
	MML_ADC_ERRORS_INTERNAL_TEMP_SENSOR_NOT_SELECTED_AS_INPUT_CHANNEL = COBRA_ADC_BASE_ERR, /**< Internal temperature sensor not selected as input channel */
	MML_ADC_ERRORS_NOT_ENABLED, /**< ADC not enabled */
	MML_ADC_ERRORS_CONVERSION_NOT_AVAILABLE /**< ADC conversion not available */
} mml_adc_errors_t;

/**
 * Status Register
 */
typedef enum {
	MML_ADC_STATUS_REGISTER_ADCONV,
	MML_ADC_STATUS_REGISTER_ADDAI,
	MML_ADC_STATUS_REGISTER_ADOVF,
	MML_ADC_STATUS_REGISTER_ADLTIF,
	MML_ADC_STATUS_REGISTER_ADHTIF,
	MML_ADC_STATUS_REGISTER_MIN = MML_ADC_STATUS_REGISTER_ADCONV,
	MML_ADC_STATUS_REGISTER_MAX = MML_ADC_STATUS_REGISTER_ADHTIF
} mml_adc_flag_status_register_t;

/**
 *	IOL selection for io configuration
 */
typedef enum {
	MML_ADC_INTERRUPT_CONFIG_ENABLE_ADDAIE = 0,
	MML_ADC_INTERRUPT_CONFIG_MIN = MML_ADC_INTERRUPT_CONFIG_ENABLE_ADDAIE,
	MML_ADC_INTERRUPT_CONFIG_DISABLE_ADDAIE,
	MML_ADC_INTERRUPT_CONFIG_ENABLE_ADLTIE,
	MML_ADC_INTERRUPT_CONFIG_DISABLE_ADLTIE,
	MML_ADC_INTERRUPT_CONFIG_ENABLE_ADHTIE,
	MML_ADC_INTERRUPT_CONFIG_DISABLE_ADHTIE,
	MML_ADC_INTERRUPT_CONFIG_MAX = MML_ADC_INTERRUPT_CONFIG_DISABLE_ADHTIE
} mml_adc_interrupt_config_t;

typedef enum {
	MML_ADC_CLEAR_INTERRUPT_ADDAI,
	MML_ADC_CLEAR_INTERRUPT_ADOVF,
	MML_ADC_CLEAR_INTERRUPT_ADLTIF,
	MML_ADC_CLEAR_INTERRUPT_ADHTIF
} mml_adc_clear_interrupt_t;

/* Structure *****************************************************************/

/**
 * This structure is used to configure ADC.
 * Set alignment, input channel, select scale, reference and clock
 */
typedef struct {
	ADC_CN_ADALGN_Type alignment; /**< Select data alignment */
	ADC_CN_ADCH_Type channel; /**< Select channel*/
	ADC_CN_ADSCL_Type scale; /**< Select scale */
	ADC_CN_ADREF_Type reference; /**< Select reference */
	ADC_CN_ADCCLK_Type clock; /**< Select clock */
} mml_adc_config_t;

/**
 * Structure for ADC context
 */
typedef struct {
	unsigned char interrupt_enable;
} mml_adc_context_t;

/**
 * Return the version of the ADC driver.
 * 
 */
const char * mml_adc_get_version(void);

/**
 * Configure ADC interrupt
 */
void mml_adc_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] priority						Interrupt priority to set
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for priority
 */
int mml_adc_set_interrupt_priority(mml_intc_prio_t priority);

/**
 * This function is used to initialize ADC
 * @param[in] config														Configuration to set
 * @retval NO_ERROR															No error
 * @retval COMMON_ERR_INVAL													Channel is in invalid range
 * @retval COMMON_ERR_UNKNOWN												Unknown error
 */
int mml_adc_init(mml_adc_config_t config);

/**
 * This function is used to configure ADC
 * @param[in] config														Configuration to set
 * @retval NO_ERROR															No error
 * @retval COMMON_ERR_INVAL													Channel is in invalid range
 * @retval COMMON_ERR_UNKNOWN												Unknown error
 */
int mml_adc_set_config(mml_adc_config_t config);

/**
 * This function is used to configure interrupt, this function configure interrupt one by one.
 * @param[in] iol_config													IO configuration to set
 * @retval NO_ERROR															No error
 * @retval COMMON_ERR_INVAL													iol_config is in invalid range
 */
int mml_adc_interrupt_config(mml_adc_interrupt_config_t iol_config);

/**
 * This function is used to clear interrupt flag
 * @param[in] flag					Flag to clear
 * @retval NO_ERROR					No error
 * @retval COMMON_ERR_INVAL			Flag is in invalid range
 */
int mml_adc_clear_interrupt(mml_adc_clear_interrupt_t flag);

/**
 * This function is used to reset ADC
 */
void mml_adc_reset(void);

/**
 * This function is used to select input channel
 * @param[in] channel				Selected channel
 * @retval NO_ERROR					No error
 * @retval COMMON_ERR_INVAL			Channel selected is not appropriate
 */
int mml_adc_select_input_channel(ADC_CN_ADCH_Type channel);

/**
 * This function is used to select the ADC input scale for AN0 or AN1
 * @param[in] scale_input			Selected scale
 * @retval NO_ERROR					No error
 * @retval COMMON_ERR_INVAL			Scale selected is not in valid range
 */
int mml_adc_select_scale_input(ADC_CN_ADSCL_Type scale_input);

/**
 * This function is used to select ADC reference
 * @param[in] reference				Selected reference
 * @retval NO_ERROR					No error
 * @retval COMMON_ERR_INVAL			Reference selected is not in valid range
 */
int mml_adc_select_reference(ADC_CN_ADREF_Type reference);

/**
 * This function is used to set ADC clock
 * @param[in] divider				Value of ADC prescaler
 * @retval NO_ERROR					No error
 * @retval COMMON_ERR_INVAL			Divider selected is not in valid range
 */
int mml_adc_set_adcclk(ADC_CN_ADCCLK_Type divider);

/**
 * This function is used to start conversion
 * NOTE: after a first conversion you should wait 1 ADC clock period before initiating another ADC conversion
 * @retval NO_ERROR						No error
 * @retval MML_ADC_ERRORS_NOT_ENABLED	ADC not enabled
 */
int mml_adc_start_conversion(void);

/**
 * This function is used to stop conversion
 * NOTE: The conversion time takes 1025 ADCCLK cycle to complete
 * @retval NO_ERROR						No error
 * @retval MML_ADC_ERRORS_NOT_ENABLED	ADC not enabled
 */
int mml_adc_stop_conversion(void);

/**
 * This function is used to read flag of ADC status register
 * @param[out] status					Pointer on the return value of the status register
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_NULL_PTR			Status is NULL
 */
int mml_adc_read_status(unsigned int * status);

/**
 * This function is used to get data after conversion
 * NOTE: The result will be aligned according to the ADALGN settings
 * @param[in] status								Pointer on the return value of the DATA register
 * @retval NO_ERROR									No error
 * @retval MML_ADC_ERRORS_CONVERSION_NOT_AVAILABLE	ADC Data is not available now
 * @retval COMMON_ERR_NULL_PTR						Status is NULL
 */
int mml_adc_get_data(unsigned int *status);

/**
 * This function is used to set the low threshold value
 * If ADC conversion result is less than low threshold value ADLTIF flag will be set
 * @param[in] value						Low threshold value to set
 * @param[in] channel					ADC channel where the threshold value will be set
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Channel is not appropriate
 */
int mml_adc_set_low_threshold(int value, ADC_CN_ADCH_Type channel);

/**
 * This function is used to get the value of low threshold
 * @param[in] channel					Select channel
 * @param[out] adlt						Pointer on the value of Low Threshold register
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Channel is not appropriate
 * @retval COMMON_ERR_NULL_PTR			adlt is NULL
 */
int mml_adc_get_low_threshold(ADC_CN_ADCH_Type channel, unsigned int *adlt);

/**
 * This function is used to set the high threshold value
 * If ADC conversion result is greater than high threshold value ADHTIF flag will be set
 * @param[in] value						Low threshold value to set
 * @param[in] channel					ADC channel where the threshold value will be set
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Channel is not appropriate
 */
int mml_adc_set_high_threshold(int value, ADC_CN_ADCH_Type channel);

/**
 * This function is used to get the value of low threshold
 * @param[in] channel					Select channel
 * @param[out] adht						Pointer on the value of High Threshold register
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Channel is not appropriate
 * @retval COMMON_ERR_NULL_PTR			adht is NULL
 */
int mml_adc_get_high_threshold(ADC_CN_ADCH_Type channel, unsigned int *adht);

/**
 * This function is used to enable/disable ADC operation
 * @param[in] config					Configuration to set
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Config is in invalid range
 */
int mml_adc_operation_config(ADC_CN_ADEN_Type config);

/** @} *//* @defgroup MML_ADC */

#ifdef __cplusplus
}
#endif

#endif /* _MML_ADC_H_ */

/******************************************************************************/
/* EOF */

