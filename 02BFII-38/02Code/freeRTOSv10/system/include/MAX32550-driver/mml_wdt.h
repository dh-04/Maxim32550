/*
 * @file mml_wdt.h MML WDT Driver Header File
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

#ifndef _MML_WDT_H_
#define _MML_WDT_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_WDT WDT Driver (Watchdog Timer)
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_WDT_VERS_MAJOR	3
#define MML_WDT_VERS_MINOR	0
#define MML_WDT_VERS_PATCH	2
#define MML_WDT_VERSION_STRING	"v" xstr(MML_WDT_VERS_MAJOR) "." xstr(MML_WDT_VERS_MINOR) "." xstr(MML_WDT_VERS_PATCH)

/* Word for reset of watchdog timer */
#define	MML_WDT_RST_TRST_1ST_WORD				0xa5
#define	MML_WDT_RST_TRST_2ND_WORD				0x5a

/* Mask */
#define RESET_CONFIG_Msk									(WDT_CTRL_INT_PERIOD_Msk | WDT_CTRL_RST_PERIOD_Msk | WDT_CTRL_INT_EN_Msk | WDT_CTRL_RST_EN_Msk)

/* Enumerations ***************************************************************/

typedef enum {
	MML_WDT_INTERRUPT_FLAG_INT_FLAG = 0,
	MML_WDT_INTERRUPT_FLAG_RST_FLAG = 1,
} mml_wdt_interupt_flag_t;

/* Structures *****************************************************************/

/**
 * WDT configuration structure
 */
typedef struct {
	int initialize; 						/**< Initialize at least once */
	WDT_CTRL_INT_EN_Type int_en; 			/**< Interrupt enable		  */
	WDT_CTRL_RST_EN_Type rst_ien; 			/**< Reset enable			  */
	WDT_CTRL_INT_PERIOD_Type int_period; 	/**< Interrupt period		  */
	WDT_CTRL_RST_PERIOD_Type rst_period; 	/**< Reset period			  */
} mml_wdt_config;

/**
 * Return the version of the WDT driver.
 * 
 */
const char * mml_wdt_get_version(void);

/**
 * Configure WDT interrupt
 */
void mml_wdt_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] priority						Interrupt priority to set
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for priority
 */
int mml_wdt_set_interrupt_priority(mml_intc_prio_t priority);

/**
 * This function is use to initialize watchdog timer.
 * @param[in] config						Input configuration for watchdog timer.
 * @retval NO_ERROR							No error.
 * @retval COMMON_ERR_INVAL					Input parameter in invalid range.
 * @retval COMMON_ERR_UNKNOWN				Unknown error.
 */
int mml_wdt_set_configuration(mml_wdt_config config);

/**
 * This function is used to enable the watchdog.
 */
void mml_wdt_enable(void);

/**
 * This function is used to disable watchdog timer.
 */
void mml_wdt_disable(void);

/**
 * This function is used to set the reset period.
 * @param[in] rst_period				Reset period to set.
 * @retval NO_ERROR						No error.
 * @retval COMMON_ERR_INVAL				rst_period is in invalid range.
 * @retval COMMON_ERR_UNKNOWN			Unknown error.
 *
 */
int mml_wdt_set_rst_period(WDT_CTRL_RST_PERIOD_Type rst_period);

/**
 * This function is used to set the interrupt period.
 * @param[in] int_period				Interrupt period to set.
 * @retval NO_ERROR						No error.
 * @retval COMMON_ERR_INVAL				int_period is in invalid range.
 */
int mml_wdt_set_int_period(WDT_CTRL_INT_PERIOD_Type int_period);

/**
 * This function is used to enable interrupt from watchdog timer.
 */
void mml_wdt_enable_int(void);

/**
 * This function is used to disable interrupt from watchdog timer.
 */
void mml_wdt_disable_int(void);

/**
 * This function is used to enable Reset frm watchdog timer.
 */
void mml_wdt_enable_reset(void);

/**
 * This function is used to disable reset from watchdog timer.
 */
void mml_wdt_disable_reset(void);

/**
 * This function is used to read interrupt flag.
 * @param[in] flag						Flag to read.
 * @param[out] status					Pointer on the return value of flag to read.
 * @retval NO_ERROR						No error.
 * @retval COMMON_ERR_INVAL				flag is in invalid range.
 * @retval COMMON_ERR_NULL_PTR			status is NULL.
 */
int mml_wdt_read_flag(mml_wdt_interupt_flag_t flag, uint32_t *status);

/**
 * This function is used to clear interrupt flag.
 * @param[in] flag						Flag to clear.
 * @retval NO_ERROR						No error.
 * @retval COMMON_ERR_INVAL				flag is in invalid range.
 * @retval COMMON_ERR_UNKNOWN			Unknown error.
 */
int mml_wdt_clear_flag(mml_wdt_interupt_flag_t flag);

/**
 * This function is used to reset watchdog counter.
 */
void mml_wdt_counter_reset(void);

/** @} *//* @defgroup MML_WDT */

#ifdef __cplusplus
}
#endif

#endif /* _MML_WDT_H_ */

/******************************************************************************/
/* EOF */
