/*
 * @file mml_gpio.h MML GPIO Driver Header File
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

#ifndef _MML_GPIO_H_
#define _MML_GPIO_H_

/* Global includes */
#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_GPIO GPIO Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_GPIO_VERS_MAJOR	3
#define MML_GPIO_VERS_MINOR	0
#define MML_GPIO_VERS_PATCH	5
#define MML_GPIO_VERSION_STRING	"v" xstr(MML_GPIO_VERS_MAJOR) "." xstr(MML_GPIO_VERS_MINOR) "." xstr(MML_GPIO_VERS_PATCH)

/* Defines ********************************************************************/
#define	K_COBRA_RESET_WAIT_LOOP_MAX				100000
#define MML_GPIO_BIT_RANGE_MIN 					0
#define MML_GPIO_BIT_RANGE_MAX 					31
#define	MML_GPIO_BIT_RANGE_NB					( MML_GPIO_BIT_RANGE_MAX + 1 )

/* Macros *********************************************************************/
/** COBRA adaptation */
#define	MML_GPIO_BASE_ERR						COBRA_GPIO_BASE_ERR
#define COBRA_PADSMODE_GPIO 					1
#define COBRA_PADSMODE_DEV  					0

/** Maccro to determine port and pin of GPIO */
#define GPIO(x,y)								((x << 5) + y)
#define GPIO_DEV(n)								(n >> 5)
#define GPIO_NUM(n)								(n & 0x1F)

/* Enumerations ***************************************************************/
/** GPIO errors list */
typedef enum {
	MML_GPIO_ERR_MIN = MML_GPIO_BASE_ERR, MML_GPIO_ERR_NOT_INITIALIZED, /**< Error Code: GPIO not initialized */
	MML_GPIO_ERR_INVALID_DEVICE, /**< Invalid GPIO device ID */
	MML_GPIO_ERR_OUT_OF_RANGE, /**< Invalid parameter or value */
	MML_GPIO_ERR_INVALID, /**< Error Code: Invalid operation */
	MML_GPIO_ERR_BAD_CONFIG,/**< Error Code : GPIO Pin is not correctly configured for the action requested */
	MML_GPIO_ERR_UNKNOWN, MML_GPIO_ERR_MAX = MML_GPIO_ERR_UNKNOWN /**< Error Code: Generic error for unknown behavior */
} mml_gpio_errors_t;

/** GPIO device identification numbers (id's) */
typedef enum {
	MML_GPIO_DEV_MIN = 0, MML_GPIO_DEV0 = MML_GPIO_DEV_MIN, /**< GPIO0 */
	MML_GPIO_DEV1, /**< GPIO1 */
	MML_GPIO_DEV2, /**< GPIO2 */
	MML_GPIO_DEV_MAX = MML_GPIO_DEV2, MML_GPIO_DEV_COUNT
} mml_gpio_id_t;

/** GPIO pin input/output data logic '0' or logic '1' */
typedef enum {
	MML_GPIO_OUT_LOGIC_ZERO, /**< Drive logic '0' on GPIO  */
	MML_GPIO_OUT_LOGIC_ONE, /**< Drive logic '1' on GPIO */
} mml_gpio_pin_data_t;

/** GPIO direction configuration for input or output */
typedef enum {
	MML_GPIO_DIR_IN, /**< direction input */
	MML_GPIO_DIR_OUT /**< direction output */
} mml_gpio_direction_t;

/** Open drain configuration for GPIO0 port only */
typedef enum {
	MML_GPIO_OPEN_DRAIN_MIN = 0,
	MML_GPIO_OPEN_DRAIN_NORMAL_MODE = MML_GPIO_OPEN_DRAIN_MIN, /**< applies to GPIO0 pins only */
	MML_GPIO_OPEN_DRAIN_OUTPUT_MODE, /**< Open Drain output mode: only applies to GPIO0 port pins */
	MML_GPIO_OPEN_DRAIN_MAX = MML_GPIO_OPEN_DRAIN_OUTPUT_MODE,
	MML_GPIO_OPEN_DRAIN_COUNT
} mml_gpio_open_drain_t;

/** Defines GPIO functionality for normal or alternate functions */
typedef enum {
	MML_GPIO_NORMAL_FUNCTION, /**< GPIO function enabled */
	MML_GPIO_SECONDARY_ALT_FUNCTION, /**< Secondary alternate function enabled */
	MML_GPIO_TERTIARY_ALT_FUNCTION /**< Tertiary alternate function enabled */
} mml_gpio_function_t;

/** Interrupt mode */
typedef enum {
	MML_GPIO_INT_MODE_LEVEL_TRIGGERED, /**< Interrupt on Level */
	MML_GPIO_INT_MODE_EDGE_TRIGGERED /**< Interrupt on Edge */
} mml_gpio_intr_mode_t;

/** GPIO pin interrupt polarity */
typedef enum {
	MML_GPIO_INT_POL_RAISING, /**< raising edge */
	MML_GPIO_INT_POL_FALLING, /**< falling edge */
	MML_GPIO_INT_POL_HIGH, /**< high level triggering */
	MML_GPIO_INT_POL_LOW, /**< low level triggering */
	MML_GPIO_INT_POL_DUAL_EDGE /**< Dual edge [raising edge or falling edge] */
} mml_gpio_intr_polarity_t;

/** GPIO pad configuration information */
typedef enum {
	MML_GPIO_PAD_NORMAL = 0, /**< Pad in Normal Mode */
	MML_GPIO_PAD_PULLUP, /**< Pad in Pull-Up Mode */
	MML_GPIO_PAD_PULLDOWN, /**< Pad in Pull-Down Mode */
	MML_GPIO_PAD_WEAK_LATCH /**< Pad in Weak-Latch Mode */
} mml_gpio_pad_config_t;

/**
 * This is the GPIO configuration data structure
 */
typedef struct {
	mml_gpio_direction_t gpio_direction; /**< GPIO direction  */
	mml_gpio_function_t gpio_function; /**< GPIO function: primary, alternate function */
	mml_gpio_intr_mode_t gpio_intr_mode; /**< GPIO interrupt mode : level triggered or edge triggered */
	mml_gpio_intr_polarity_t gpio_intr_polarity; /**< GPIO interrupt polarity */
	mml_gpio_pad_config_t gpio_pad_config; /**< GPIO pad mode */
} mml_gpio_config_t;

/** GPIO initialization state */
typedef enum {
	MML_GPIO_STATE_NOT_INITIALIZED = 0, /**< GPIO Not Initialized */
	MML_GPIO_STATE_INITIALIZED /**< GPIO Initialized */
} mml_gpio_state_t;

/* Structures *****************************************************************/
typedef void (*mml_gpio_handler_t)(void *);

typedef struct {
	mml_gpio_state_t 		state; 				/**< Internal module state */
	mml_gpio_handler_t 		tab_handler[32];	/**< IRQ handlers for all gpio pins of the GPIO dev */
	void 					*tab_param[32]; 	/**< Params for IRQ handlers */
	GPIOn_Type 				*reg; 				/**< GPIO device Register base addresses */
	int 					vector; 			/**<  Interrupt number*/
} mml_gpio_port_conf_t;

/** Context structure related to only one GPIO port */
typedef struct {
	/* First init */
	unsigned int first_init;
	/* GPIO interface */
	mml_gpio_port_conf_t port[MML_GPIO_DEV_COUNT];
} mml_gpio_context_t;

/**
 * Return the version of the GPIO driver.
 * 
 */
const char * mml_gpio_get_version(void);

/**
 * Configure GPIOs interrupt
 */
void mml_gpio_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] vector						IRQ Number corresponding to the GPIO vector to set
 * @param[in] priority						Interrupt priority to set to the vector
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for vector or priority
 */
int mml_gpio_set_interrupt_priority(int vector, mml_intc_prio_t priority);

/**
 * The function is used to initialize and configure the consecutive 'n' GPIO
 * pins starting from the 'offset' position.
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * 											starting from 'offset' position.
 * @param[in] config						GPIO configuration
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid parameter
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_init(mml_gpio_id_t dev_id, int offset, int bits_count,
		mml_gpio_config_t config);

/**
 * The function is used to reset all gpio interface
 * @retval NO_ERROR 				No Error
 * @retval COMMON_ERR_IN_PROGRESS	Reset is still in progress
 */
int mml_gpio_reset_interface(void);

/**
 * This function closes the GPIO device ID also resets and disables
 * the clock for the GPIO device ID.
 * @param[in] dev_id						The GPIO device identifier
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid parameter
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_close(mml_gpio_id_t dev_id);

/**
 * The function re-configures an already initialized GPIO device ID with new configuration
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * 											starting from 'offset' position.
 * @param[in] config						GPIO configuration
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE	Invalid GPIO ID
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid parameter
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_set_config(mml_gpio_id_t dev_id, int offset, int bits_count,
		mml_gpio_config_t config);

/**
 * The function configures the GPIO PAD configuration information
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] config						GPIO pad configuration information
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * 											starting from 'offset' position.
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_pad_configure(mml_gpio_id_t dev_id, mml_gpio_pad_config_t config,
		int offset, int bits_count);

/**
 * This function configures the GPIO functionality i.e. GPIO in normal function mode
 * or alternate functions for the specified range of pins.
 * NOTE: All the specified GPIO pins are configured for only one GPIO functionality
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] function						GPIO functionality configuration information
 * 											i.e. GPIO alternate function or normal function.
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * 											starting from 'offset' position.
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_configure_function(mml_gpio_id_t dev_id,
		mml_gpio_function_t function, int offset, int bits_count);

/**
 * This function enables a range of consecutive GPIO pins for the output mode.
 * The valid GPIO pins 'offset' range is between 0 to 31.
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * 											starting from 'offset' position.
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_enable_output(mml_gpio_id_t dev_id, int offset, int bits_count);

/**
 * This function enables a range of consecutive GPIO pins interrupt.
 * The valid GPIO pins 'offset' range is between 0 to 31.
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * 											starting from 'offset' position.
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_enable_interrupt(mml_gpio_id_t dev_id, int offset, int bits_count);

/**
 * This function disables a range of consecutive GPIO pins interrupt.
 * The valid GPIO pins 'offset' range is between 0 to 31.
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * 											starting from 'offset' position.
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_disable_interrupt(mml_gpio_id_t dev_id, int offset, int bits_count);

/**
 * This function clears a range of consecutive GPIO pins interrupt flags.
 * The valid GPIO pins 'offset' range is between 0 to 31.
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * 											starting from 'offset' position.
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_clear_interrupt(mml_gpio_id_t dev_id, int offset, int bits_count);

/**
 * This function gets GPIO port interrupt flags.
 * @param[in] dev_id						The GPIO device identifier
 * @param[out] status						Pointer on the return interrupt status
 *
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval COMMON_ERR_INVAL					Invalid value, if status is NULL
 */
int mml_gpio_get_interrupt_status(mml_gpio_id_t dev_id, unsigned int *status);

/**
 * This function disables a range of consecutive GPIO pins from output mode.
 * The valid GPIO pins range is between 0 to 31.
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * 											starting from 'offset' position.
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_disable_output(mml_gpio_id_t dev_id, int offset, int bits_count);

/**
 * This function configures the consecutive GPIO0 pins in the open drain output mode.
 * NOTE: This function applies only to GPIO0.
 * @param[in] dev_id						The GPIO device identifier and
 * 											it should be MML_GPIO_DEV0 only.
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * 											starting from 'offset' position.
 * @param[in] config						Open drain configuration
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid device ID or invalid parameter
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_config_open_drain(mml_gpio_id_t dev_id, const unsigned int offset,
		unsigned int bits_count, mml_gpio_open_drain_t config);

/**
 * This function writes 32-bit data to the GPIO DATA_OUT register
 * @param[in] dev_id						The GPIO device identifier.
 * @param[in] data							Data word for output
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid GPIO device
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_write_dataout(mml_gpio_id_t dev_id, unsigned int data);

/**
 * This function reads 32-bit data from the GPIO DATA_OUT register
 * @param[in] dev_id						The GPIO device identifier.
 * @param[out] data							Received data word is stored
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_read_dataout(mml_gpio_id_t dev_id, unsigned int *data);

/**
 * This function reads 32-bit data word from the GPIO DATA_IN register
 * @param[in] dev_id						The GPIO device identifier.
 * @param[out] data							Received data word is stored
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid GPIO device
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_read_datain(mml_gpio_id_t dev_id, unsigned int *data);

/**
 * This function transmits specified number of data words stored in the buffer to the GPIO device
 * @param[in] dev_id						The GPIO device identifier.
 * @param[in] buffer						buffer pointer from where the data to be transmitted
 * @param[in] length						Specifies number of 32-bit data words
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device
 * @retval COMMON_ERR_NULL_PTR				buffer pointer is NULL
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_write_buffer(mml_gpio_id_t dev_id, unsigned int *buffer,
		int length);

/**
 * This function receives specified number of data words from the GPIO device and stores in buffer
 * @param[in] dev_id						The GPIO device identifier.
 * @param[in] buffer						buffer pointer where to store the received data
 * @param[in] length						Specifies number of 32-bit data words
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device
 * @retval COMMON_ERR_NULL_PTR				buffer pointer is NULL
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_read_buffer(mml_gpio_id_t dev_id, unsigned int *buffer, int length);

/**
 * Writes a consecutive bit pattern stored in the 'data' word in a specified
 * group of pins with in a GPIO port.
 * Example: Input parameters(gpio_id, offset = 3, bits_count = 3, data[31:0])
 * Then the 3-bit pattern is stored as gpio_id[5:3] = data[2:0];
 * @param[in] dev_id						The GPIO device identifier.
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * @param[in] data							LSB adjusted n-bit field of x that begins
 * 											at position 'offset'.
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid parameters
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_write_bit_pattern(mml_gpio_id_t dev_id, int offset, int bits_count,
		unsigned int data);

/**
 * Reads bit pattern from consecutive GPIO pins and left adjusts this pattern and
 * stores it in LSB of 'data' word.
 * Example: Input parameters(gpio_id, offset = 5, bits_count = 6, data_p[31:0])
 * Then the 6-bit pattern is read is stored in data_p[5:0] = gpio_id[10:5];
 * @param[in] dev_id						The GPIO device identifier.
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * @param[in] data							Where the LSB adjusted bit pattern is stored
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid parameters
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_read_bit_pattern(mml_gpio_id_t dev_id, int offset, int bits_count,
		unsigned int *data);

/**
 * This is the output function used to set/clear logic '1' or '0' to a GPIO pin
 * without affecting the other pins.
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] output						Specifies the output value logic '0' or '1'
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_pin_output(mml_gpio_id_t dev_id, const unsigned int offset,
		mml_gpio_pin_data_t output);

/**
 * This is the input function it can be used to read data from a GPIO pin.
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] input							input value either logic '0' or '1'
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_pin_input(mml_gpio_id_t dev_id, const unsigned int offset,
		mml_gpio_pin_data_t *input);

/**
 * This is the check function used to check if device id is correct
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_check(mml_gpio_id_t dev_id, int offset, int bits_count);

/**
 * This is the clear function used to disable corresponding interruption and disable PMU wake for GPIO pins
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 */
void mml_gpio_clear_out_int_wake(mml_gpio_id_t dev_id, int offset,
		int bits_count);

/**
 * This function used to set GPIOs and unset GPIOs
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] value						The Value
 */
void mml_gpio_flip(mml_gpio_id_t dev_id, unsigned int value);

/**
 * This function is used to configure interrupt
 * @param[in] dev_id						The GPIO device identifier
 * @param[in] offset						Pin offset in the range of 0 to 31
 * @param[in] bits_count					Specifies number of consecutive pins
 * @param[in] int_mode						Interrupt mode to configure
 * @param[in] polarity						Interrupt polarity to configure
 * @retval NO_ERROR							No error
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		One or more parameters are invalid
 * @retval COMMON_ERR_UNKNOWN				Unknown error occurred
 */
int mml_gpio_configure_interrupt(mml_gpio_id_t dev_id, int offset,
		int bits_count, mml_gpio_intr_mode_t int_mode,
		mml_gpio_intr_polarity_t polarity);

/**
 * This function is used to attach a user handler to a specific pin
 * @param[in] id							The GPIO device identifier
 * @param[in] pin							GPIO pin to deattach user handler
 * @param[in] handler						Specific user handler to attach.
 * @retval	NO_ERROR						No error
 * @retval	COMMON_ERR_UNKNOWN				Unknown error
 * @retval	COMMON_ERR_NULL_PTR				handler is null
 * @retval	MML_GPIO_ERR_INT_PIN			GPIO pin has no interrupt enable in INT_EN register
 * @retval	MML_GPIO_ERR_INVALID_DEVICE		Id is in invalid range
 */
int mml_gpio_attach_handler(mml_gpio_id_t id, unsigned int pin, mml_gpio_handler_t handler, void *param);

/**
 * This function is used to deattach a user handler to a specific pin
 * @param[in] id							The GPIO device identifier
 * @param[in] pin							GPIO pin to deattach user handler
 * @retval	NO_ERROR						No error
 * @retval	MML_GPIO_ERR_INVALID_DEVICE		Id is in invalid range
 * @retval	MML_GPIO_ERR_NOT_INITIALIZED	GPIO port is not initialized
 */
int mml_gpio_deattach_handler(mml_gpio_id_t id, unsigned int pin);

/** @} *//* @defgroup MML_GPIO */

#ifdef __cplusplus
}
#endif

#endif /* _MML_GPIO_H_ */

/******************************************************************************/
/* EOF */

