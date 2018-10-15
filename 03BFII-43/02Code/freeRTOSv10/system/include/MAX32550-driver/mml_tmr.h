/*
 * @file mml_tmr.h MML TMR Driver Header File
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

#ifndef _MML_TMR_H_
#define _MML_TMR_H_

/* Global includes */
#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_TMR Timer Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_TMR_VERS_MAJOR	3
#define MML_TMR_VERS_MINOR	0
#define MML_TMR_VERS_PATCH	2
#define MML_TMR_VERSION_STRING	"v" xstr(MML_TMR_VERS_MAJOR) "." xstr(MML_TMR_VERS_MINOR) "." xstr(MML_TMR_VERS_PATCH)

/* Defines ********************************************************************/

#define MML_GCR_RSTR_TIMERS_MASK (GCR_RSTR_TIMER0_Msk|GCR_RSTR_TIMER1_Msk | GCR_RSTR_TIMER2_Msk | GCR_RSTR_TIMER2_Msk | GCR_RSTR_TIMER3_Msk | GCR_RSTR_TIMER4_Msk | GCR_RSTR_TIMER5_Msk)

/* Macros *********************************************************************/
#define	MML_TIMER_BASE_ERR						COBRA_TIMER_BASE_ERR

#define	MML_TMR_RSTR_OFST						5 //!< Offset for Timer reset [10:5] PWMx reset
#define	MML_TMR_ACTIVE_OFST						15 //!< offset for clock enable [20:15] PWMx: PWM/timers

#define MML_TMR_CN_TEN_ENABLE					1 //!< Enable timer

/* Enumerations ***************************************************************/
/** Timer errors list */
typedef enum {
	MML_TMR_ERR_MIN = MML_TIMER_BASE_ERR, MML_TMR_ERR_NOT_INITIALIZED, /**< Error Code: Timer not initialized */
	MML_TMR_ERR_INVALID, /**< Error Code: Invalid operation */
	MML_TMR_ERR_IRQ_SET, /**< Error Code: Interrupt can not be initialized */
	MML_TMR_ERR_NOT_ALLOWED, /**< Error Code: Operation not allowed */
	MML_TMR_ERR_ENABLE, /**< Error Code: Timer is not disable */
	MML_TMR_ERR_NO_TIMER_AVAILABLE, /**< Error Code: No timer available */
	MML_TMR_ERR_UNKNOWN, MML_TMR_ERR_MAX = MML_TMR_ERR_UNKNOWN /**< Error Code: Generic error for unknown behavior */
} mml_tmr_errors_t;

/** Timer device identification numbers (id's) */
typedef enum {
	MML_TMR_DEV_MIN = 0, MML_TMR_DEV0 = MML_TMR_DEV_MIN, /**< Timer 0 */
	MML_TMR_DEV1, /**< Timer 1 */
	MML_TMR_DEV2, /**< Timer 2 */
	MML_TMR_DEV3, /**< Timer 3 */
	MML_TMR_DEV4, /**< Timer 4 */
	MML_TMR_DEV5, /**< Timer 5 */
	MML_TMR_DEV_MAX = MML_TMR_DEV5, MML_TMR_DEV_COUNT
} mml_tmr_id_t;

/** Timer IOCTL commands */
typedef enum {
	MML_TMR_CMD_SET_PRESCALE, /**< Set prescale */
	MML_TMR_CMD_SET_MODE, /**< Set mode */
	MML_TMR_CMD_SET_IO_POLARITY /**< Set IO polarity */
} mml_tmr_ioctl_t;

/** Timer initialization state */
typedef enum {
	MML_TMR_STATE_NOT_INITIALIZED = 0, /**< State not initialized */
	MML_TMR_STATE_INITIALIZED /**< State initialized */
} mml_tmr_state_t;

/** Timer status values */
typedef enum {
	MML_TMR_STATUS_DISABLED = 0, /**< Status disabled */
	MML_TMR_STATUS_ENABLED /**< Status enabled */
} mml_tmr_status_t;

/* Structures *****************************************************************/
typedef void (*mml_tmr_handler_t)(void);

/** This structure contains the Timer device configuration parameters */
typedef struct {
	unsigned int timeout; 		/**< Compare value which is used to set the maximum count value to initiate a realod of the timer to 0x0001 */
	unsigned int count; 		/**< Value set in TMR_CNT register */
	unsigned int pwm_value; 	/**< PWM value. pwm_value is value compared to the current timer count (store in TMR_CNT) */
	TMRn_CN_PRES_Type clock; 	/**< Prescaler */
	TMRn_CN_TMODE_Type mode; 	/**< Timer mode */
	TMRn_CN_TPOL_Type polarity; /**< Timer polarity */
	volatile mml_tmr_handler_t handler; /**< IRQ handler */
} mml_tmr_config_t;

/** This structure contains information about timer */
typedef struct {
	TMRn_Type *reg_tmr; /**< Timer physical base address */
	mml_tmr_state_t state; /**< Timer state */
	mml_tmr_status_t status; /**< Timer status */
	mml_tmr_handler_t handler; /**< Timer interrupt handler */
} mml_tmr_info_t;

/** Structure holds the Timer device context information */
typedef struct {
	unsigned char first_init; /**< First initialization */
	mml_tmr_info_t tmr[MML_TMR_DEV_COUNT]; /**< Bunch of timers */
} mml_tmr_context_t;

/**
 * Return the version of the TMR driver.
 * 
 */
const char * mml_tmr_get_version(void);

/**
 * Configure TMRs interrupt
 */
void mml_tmr_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] vector						IRQ Number corresponding to the TMR vector to set
 * @param[in] priority						Interrupt priority to set to the vector
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for vector or priority
 */
int mml_tmr_set_interrupt_priority(int vector, mml_intc_prio_t priority);

/**
 * This function is used to get the first free timer: timer not enabled and not initialized.
 * @param[out]id							The first free timer id.
 * @note 									Do not forget to close the timer after utilization
 *											(mml_tmr_close) otherwise this timer could not be re-used
 * 											by another process (important in OS context).
 */
int mml_tmr_get_free_timer(mml_tmr_id_t *id);

/**
 * This function initializes specified timer ID with the configuration parameters.
 * @param[in] id							Timer device ID
 * @param[in] config						Points to configuration parameters
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid timer ID
 * @retval COMMON_ERR_NULL_PTR				configuration parameter is NULL
 * @retval COMMON_ERR_BAD_STATE				Timer already initialized
 * @retval MML_TMR_ERR_IRQ_SET				Interrupt can not be initialized
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 * @note 									Do not forget to close the timer after utilization
 *											(mml_tmr_close) otherwise this timer could not be re-used
 * 											by another process (important in OS context).
 */
int mml_tmr_init(mml_tmr_id_t id, mml_tmr_config_t *config);

/**
 * This function resets and closes the specified timer ID
 * @param[in] id							Timer ID
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid timer ID
 * @retval COMMON_ERR_BAD_STATE				Timer not initialized or timer is enabled
 * @retval MML_TMR_ERR_NOT_ALLOWED			Operation not allowed
 */
int mml_tmr_close(mml_tmr_id_t id);

/** This function resets all timer interfaces
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_IN_PROGRESS			Reset is in progress
 */
int mml_tmr_reset_interface(void);

/**
 * This function configures the Timer ID with the new configuration values.
 * Timer must not be running(enabled) before calling this function.
 * @param id								Timer device id
 * @param config							Points to configuration parameters
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid timer ID
 * @retval COMMON_ERR_NULL_PTR				configuration parameter is NULL
 * @retval MML_TMR_ERR_NOT_INITIALIZED		Timer ID is not initialized or Timer is enabled
 * @retval MML_TMR_ERR_NOT_ALLOWED			Operation not allowed
 * @retval MML_TMR_ERR_IRQ_SET				Interrupt can not be initialized
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_tmr_config(mml_tmr_id_t id, mml_tmr_config_t *config);

/**
 * This function enables (starts) the timer ID
 * @param id								Timer device id
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid timer ID
 * @retval MML_TMR_ERR_NOT_INITIALIZED		Timer ID is not initialized
 * 											or Timer is already enabled
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 * @retval MML_TMR_ERR_NOT_ALLOWED			Operation not allowed
 */
int mml_tmr_enable(mml_tmr_id_t id);

/**
 * This function disables (stops) the timer channel
 * @param[in] id							Timer device id
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid timer ID
 * @retval MML_TMR_ERR_NOT_INITIALIZED		Timer ID is not initialized
 * 											or Timer is already in disabled status
 * @retval MML_TMR_ERR_UNKNOWN				Unknown error occurred
 */
int mml_tmr_disable(mml_tmr_id_t id);

/**
 * This function reads the timer ID current value
 * @param[in] id							Timer device ID
 * @param[out] time							Timer current value
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_NULL_PTR				Pointer is null
 * @retval COMMON_ERR_INVAL					Invalid timer ID
 * @retval MML_TMR_ERR_NOT_INITIALIZED		Timer ID is not initialized
 * @retval MML_TMR_ERR_UNKNOWN				Unknown error occurred
 */
int mml_tmr_read(mml_tmr_id_t id, unsigned int *time);

/**
 * This function provides the IOCLT commands for the timer configuration.
 * @param[in] id							Timer device ID
 * @param[in] cmd							Timer ioctl commands to be performed
 * @param[in,out] data						Input/Output parameters
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_NULL_PTR				Pointer is null
 * @retval COMMON_ERR_INVAL					Invalid timer ID
 * @retval MML_TMR_ERR_NOT_INITIALIZED		Timer ID is not initialized
 * 											or Timer is in enabled status.
 * @retval COMMON_ERR_NO_MATCH				Invalid IOCTL command
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid timer configuration parameter
 * @retval MML_TMR_ERR_NOT_ALLOWED			Operation not allowed
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_tmr_ioctl(mml_tmr_id_t id, mml_tmr_ioctl_t cmd, void *data);

/**
 * This function registers the interrupt handler function for the
 * specified Timer ID device.
 * @param[in] id							Timer device ID
 * @param[in] handler						Interrupt handler to be registered
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid timer ID
 * @retval MML_TMR_ERR_NOT_INITIALIZED		Timer ID is not initialized
 * @retval COMMON_ERR_NULL_PTR				Null handler pointer passed
 * @retval MML_TMR_ERR_NOT_ALLOWED			Operation not allowed
 */
int mml_tmr_callback_handler_register(mml_tmr_id_t id,
		mml_tmr_handler_t handler);

/**
 * This function unregisters the interrupt handler function for the
 * specified Timer ID device if any.
 * @param[in] id							Timer device ID
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid timer ID
 * @retval MML_TMR_ERR_NOT_INITIALIZED		Timer ID is not initialized
 * @retval MML_TMR_ERR_NOT_ALLOWED			Operation not allowed
 */
int mml_tmr_callback_handler_unregister(mml_tmr_id_t id);

/**
 * This function enables interruption
 * @param[in] id							Timer device id
 * @retval NO_ERROR							No error
 * @retval MML_TMR_ERR_IRQ_SET				Interrupt can not be initialized
 */
int mml_tmr_interrupt_enable(mml_tmr_id_t id);

/**
 * This function disables interruption
 * @param[in] id							Timer device id
 * @retval NO_ERROR							No error
 */
int mml_tmr_interrupt_disable(mml_tmr_id_t id);

/**
 * This functions clears interrupt
 * @param[in] id	Timer device id
 */
void mml_tmr_interrupt_clear(mml_tmr_id_t id);

/**
 * This function is used to set a timeout.
 * @param[in] id				Timer device id
 * @param[in] timeout			Timeout value to set in microseconds.
 * @retval NO_ERROR				No error.
 * @retval COMMON_ERR_INVAL		Id is in invalid range or timeout can't be set with current APB frequency.
 * @retval COMMON_ERR_UNKNOWN	Unknown error.
 */
int mml_tmr_set_timeout_us(mml_tmr_id_t id, unsigned int timeout);

/**
 * This function configures the mode for the Timer
 * @param[in] id							Timer device id
 * @param config							Points to configuration parameters
 * @retval NO_ERROR							No error
 * @retval MML_TMR_ERR_ENABLE				Timer is not disable
 * @retval COMMON_ERR_NO_MATCH				Invalid PWM configuration
 */
int mml_tmr_mode_ctrl(mml_tmr_id_t id, mml_tmr_config_t config);

/**
 *  Timer interrupt handler for all timers
 *  @param[in] id							Timer device id
 *  NOTE: When you have define a mml_tmr_handler_t in mml_tmr_init, mml_tmr_config or mml_tmr_callback_handler_register, your handler will be called in this handler
 */
void mml_tmr_irq_handler(mml_tmr_id_t id);

/** @} *//* @defgroup MML_TMR */

#ifdef __cplusplus
}
#endif

#endif /* _MML_TMR_H_ */

/******************************************************************************/
/* EOF */
