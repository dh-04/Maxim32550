/*
 * @file mml_rtc.h MML RTC Driver Header File
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

#ifndef _MML_RTC_H_
#define _MML_RTC_H_

#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_RTC RTC Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_RTC_VERS_MAJOR	2
#define MML_RTC_VERS_MINOR	0
#define MML_RTC_VERS_PATCH	4

#define MML_RTC_VERSION_STRING	"v" xstr(MML_RTC_VERS_MAJOR) "." xstr(MML_RTC_VERS_MINOR) "." xstr(MML_RTC_VERS_PATCH)

#define MML_RTC_RAS_12_DAYS		12 * 3600 * 24
#define MML_RTC_RASS_16s 		0xFFFF
#define MML_RTC_SSEC_FREQ		256

/* Enumerations ***************************************************************/

/**
 * RTC errors list
 */
typedef enum {
	MML_RTC_RAS_ERROR_SUPPERIOR_12_DAYS = COBRA_RTC_BASE_ERR,	/**< RTC error: value set in SEC register is superior at 12days 	*/
	MML_RTC_RAS_ERROR_INFERIOR_1_SECOND,						/**< RTC error: value set in SEC register is less than 1second 		*/
	MML_RTC_RSSA_ERROR_SUPPERIOR_16_SECOND,						/**< RTC error: value set in SSEC register is superior at 16seconds */
	MML_RTC_RAS_ERROR_INFERIOR_244_US							/**< RTC error: value set in SSEC register is less than 244us 		*/
} mml_rtc_errors_t;

/**
 * Configuration of interrupt
 */
typedef enum {
	MML_RTC_INTERRUPT_CONFIG_ADE_ENABLE,  						/**< Enable time of day alarm interrupt  */
	MML_RTC_INTERRUPT_CONFIG_ADE_DISABLE, 						/**< Disable time of day alarm interrupt */
	MML_RTC_INTERRUPT_CONFIG_ASE_ENABLE,  						/**< Enable sub second alarm interrupt   */
	MML_RTC_INTERRUPT_CONFIG_ASE_DISABLE, 						/**< Disable sub second alarm interrupt  */
	MML_RTC_INTERRUPT_CONFIG_RDYE_ENABLE, 						/**< Enable RTC ready interrupt          */
	MML_RTC_INTERRUPT_CONFIG_RDYE_DISABLE 						/**< Disable RTC ready interrupt         */
} mml_rtc_interrupt_config_t;

/**
 * Interrupt Flag
 */
typedef enum {
	MML_RTC_INTERRUPT_FLAG_ALDF = 1, 	  						/**< Time of day alarm flag */
	MML_RTC_INTERRUPT_FLAG_ALSF, 	 	  						/**< Sub second alarm flag  */
} mml_rtc_interrupt_flag_t;

/** Structures ****************************************************************/
/**
 * RTC context structure
 */
typedef struct {
	unsigned int first_init;
} mml_rtc_context_t;

/**
 * Return the version of the RTC driver.
 * 
 */
const char * mml_rtc_get_version(void);

/**
 * Configure RTC interrupt
 */
void mml_rtc_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] priority						Interrupt priority to set
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for priority
 */
int mml_rtc_set_interrupt_priority(mml_intc_prio_t priority);

/**
 * This function is used to set the time of day alarm.
 * @param[in] days									Number of days before alarm.
 * @param[in] hours									Number of hours before alarm.
 * @param[in] minutes								Number of minutes before alarm.
 * @param[in] seconds								Number of seconds before alarm.
 * @retval NO_ERROR									No error.
 * @retval COMMON_ERR_UNKNOWN						Unknown error.
 * @retval MML_RTC_RAS_ERROR_SUPPERIOR_12_DAYS		All your parameters are superior to 12 days from your current time.
 * @retval MML_RTC_RAS_ERROR_INFERIOR_1_SECOND		All your parameters are inferior to 1 second from your current time.
 * @note This function first disable rtc and then set alarm but didn't enable rtc after. You must call mml_rtc_enable or mml_rtc_start_up
 */
int mml_rtc_set_time_of_day_alrm(uint32_t days, uint32_t hours,
		uint32_t minutes, uint32_t secondes);

/**
 * This function is used to set the sub second alarm.
 * @param[in] seconds								Number of seconds before alarm.
 * @param[in] sub_seconds							Number of sub_seconds before alarm.
 * @retval NO_ERROR									No error.
 * @retval COMMON_ERR_UNKNOWN						Unknown error.
 * @retval MML_RTC_RAS_ERROR_SUPPERIOR_16_SECOND	Your total time is superior as 16s.
 * @note This function first disable rtc and then set alarm but didn't enable rtc after. You must call mml_rtc_enable or mml_rtc_start_up
 */
int mml_rtc_set_sub_second_alarm(uint32_t seconds, uint32_t sub_seconds);

/**
 * This function is used to allowed write in RTCE and RTC_TRIM.
 */
void rtc_enable_we(void);

/**
 * This function is used to ignored write in RTCE and RTC_TRIM.
 */
void rtc_disable_we(void);

/**
 * This function is used to enable RTC.
 */
void mml_rtc_enable(void);

/**
 * This function is used to disable RTC.
 */
void mml_rtc_disable(void);

/**
 * This function is used to configure RTC interruption.
 * You must configure interrupt one by one.
 * @param[in] config								Configuration to set.
 * @retval NO_ERROR									No error.
 * @retval COMMON_ERR_INVAL							Config is in invalid range.
 */
int mml_rtc_configure_interrupt(mml_rtc_interrupt_config_t config);

/**
 * This function is used to clear interrupt flag.
 * @param[in] flag									Flag to clear.
 * @retval NO_ERROR									No error.
 * @retval COMMON_ERR_INVAL							Flag is in invalid range.
 */
int mml_rtc_clear_interrupt(mml_rtc_interrupt_flag_t flag);

/**
 * This function is used to read interrupt status of RTC.
 * @return											The value of the RTC interrupt flag ALDF and ALSF.
 * 													Example : the return value will be 0bxy where x = value of ALSF and y value of ALDF.
 */
uint32_t mml_rtc_read_interrupt(void);

/**
 * This function is used to set SQE bit of control register.
 * @param[in] value									Value to set.
 * 													0: Square wave output disabled.
 * 													1: Square wave specified by FT is outputted on the SQW pin.
 */
void mml_rtc_set_sqe(RTC_CN_SQE_Type value);

/**
 * This function is used to set FT bit of control register.
 * @param[in] value									Value to set.
 * 													00: 1Hz (Compensated).
 * 													01: 512Hz.
 * 													1x: RTC_Input_Clock / 8.
 */
void mml_rtc_set_ft(RTC_CN_FT_Type value);

/**
 * This function is used to configure Oscillator Mode.
 * @param[in] value									Value to set.
 * @retval NO_ERROR									No error.
 * @retval COMMON_ERR_INVAL							value is in invalid range.
 * @retval COMMON_ERR_UNKNOWN						Unknown error.
 */
int mml_rtc_config_x32kmd(RTC_CN_X32KMD_Type value);

/**
 * This function is used to reset RTC
 */
void mml_rtc_reset(void);

/**
 * This function is used to start-up the RTC.
 * NOTE: the function clears all counter register (SEC and SSEC).
 */
void mml_rtc_start_up(void);

/** @} *//* @defgroup MML_RTC */

#ifdef __cplusplus
}
#endif

#endif /* _MML_RTC_H_ */

/******************************************************************************/
/* EOF */

