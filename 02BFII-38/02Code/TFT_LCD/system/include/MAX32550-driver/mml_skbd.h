/*
 * @file mml_skbd.h MML SKBD Driver Header File
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

#ifndef _MML_SKBD_H_
#define _MML_SKBD_H_

/* Global includes */
#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_SKBD SKBD Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */


#define xstr(s) str(s)
#define str(s) #s
#define MML_SKBD_VERS_MAJOR	3
#define MML_SKBD_VERS_MINOR	0
#define MML_SKBD_VERS_PATCH	3
#define MML_SKBD_VERSION_STRING	"v" xstr(MML_SKBD_VERS_MAJOR) "." xstr(MML_SKBD_VERS_MINOR) "." xstr(MML_SKBD_VERS_PATCH)

/* Macros Defines ********************************************************************/
/* COBRA adaptation */
#define	MML_KEYPAD_BASE_ERR				COBRA_KEYPAD_BASE_ERR
/* Number of key registers present in the keypad interface */
#define	MML_SKBD_TOTAL_KEY_REGS					4
/* Flag to indicate the key press/release event */
#define	MML_SKBD_FLAG_KEY_PRESS			MML_SKBD_KEY_PRESS_FLAG
/* Key pressed flag */
#define	MML_SKBD_KEY_PRESS_FLAG					( 0x1 << 8 )
#define MML_SKBD_CR1_CLEAR_MASK_NOOFST			0x1

/* Enumerations ***************************************************************/
/** Keypad errors list */
typedef enum {
	MML_SKBD_ERR_MIN = MML_KEYPAD_BASE_ERR, MML_SKBD_ERR_NOT_INITIALIZED, /**< Error Code: Keypad not initialized */
	MML_SKBD_ERR_ALREAD_INITIALIZED, /**< Error Code: Keypad already initialized */
	MML_SKBD_ERR_INVALID_OPERATION, /**< Error Code: Invalid keypad operation */
	MML_SKBD_ERR_OUT_OF_RANGE, /**< Error Code: Invalid parameter or value */
	MML_SKBD_ERR_OVERRUN, /**< Error Code: Keypad Over run error */
	MML_SKBD_ERR_IRQ, /**< Error Code: IRQ setup error */
	MML_SKBD_ERR_IRQ_NULL, /**< Error Code: NULL IRQ handler */
	MML_SKBD_ERR_INVALID_PIN_CONFIGURATION, /**< Error Code: One or more keypad I/O pins are overlapped or  input/output pin configurations are invalid */
	MML_SKBD_ERR_BUSY, /**< Error Code: Keypad is busy */
	MML_SKBD_ERR_UNKNOWN, /**< Error Code: Generic error for unknown behavior */
	MML_SKBD_ERR_MAX = MML_SKBD_ERR_UNKNOWN
} mml_skbd_errors_t;

/**  Keypad initialization state FSM */
typedef enum {
	MML_SKBD_STATE_MIN = 0,
	MML_SKBD_STATE_NOT_INITIALIZED = MML_SKBD_STATE_MIN, /**< State not initialized */
	MML_SKBD_STATE_INITIALIZED, /**< State initialized */
	MML_SKBD_STATE_CLOSED, /**< State closed */
	MML_SKBD_STATE_MAX = MML_SKBD_STATE_CLOSED,
	MML_SKBD_STATE_COUNT
} mml_skbd_state_t;

/** Keypad events */
typedef enum {
	MML_SKBD_EVENT_PUSH = SKBD_IER_PUSHIE_Msk, /**< Push Event */
	MML_SKBD_EVENT_RELEASE = SKBD_IER_RELEASEIE_Msk, /**< Release Event */
	MML_SKBD_EVENT_OVERRUN = SKBD_IER_OVERIE_Msk /**< Overrun Event */
} mml_skbd_events_t;

/** Keypad Interrupt Status */
typedef enum {
	MML_SKBD_INTERRUPT_STATUS_PUSHIS = SKBD_ISR_PUSHIS_Msk, /**< Push Interupt flag */
	MML_SKBD_INTERRUPT_STATUS_RELEASEIS = SKBD_ISR_RELEASEIS_Msk, /**< Release Interupt flag */
	MML_SKBD_INTERRUPT_STATUS_OVERIS = SKBD_ISR_OVERIS_Msk /**< Overrun Interupt flag */
} mml_interrupt_status_t;

/** Keypad I/O's IOSEL */
typedef enum {
	MML_SKBD_KBDIO0 = (0x01 << 0), /**< SKBD pin 0 */
	MML_SKBD_KBDIO1 = (0x01 << 1), /**< SKBD pin 1 */
	MML_SKBD_KBDIO2 = (0x01 << 2), /**< SKBD pin 2 */
	MML_SKBD_KBDIO3 = (0x01 << 3), /**< SKBD pin 3 */
	MML_SKBD_KBDIO4 = (0x01 << 4), /**< SKBD pin 4 */
	MML_SKBD_KBDIO5 = (0x01 << 5), /**< SKBD pin 5 */
	MML_SKBD_KBDIO6 = (0x01 << 6), /**< SKBD pin 6 */
	MML_SKBD_KBDIO7 = (0x01 << 7), /**< SKBD pin 7 */
} mml_skbd_io_pins_t;

/** Keypad IRQ handler function */
typedef void (*irq_handler_t)(void);

/* Structures *****************************************************************/
/** Keypad configuration structure */
typedef struct {
	unsigned short ioselect; /**< I/O pin direction selection for the corresponding keypad pins */
	unsigned int reg_erase; /**< key register erase flag on key is released */
	int outputs; /**< Specifies the keypad pins to be configured as output */
	int inputs; /**< Specifies the keypad pins to be configured as input */
	SKBD_CR1_DBTM_Type debounce; /**< Keypad Debouncing Time */
	irq_handler_t irq_handler; /**< IRQ handler */
} mml_skbd_config_t;

/** Keypad channel context information */
typedef struct {
	unsigned int first_init;
	unsigned int irq; /**< Interrupt request(IRQ) number */
	irq_handler_t irq_handler; /**< IRQ handler */
	mml_skbd_state_t state; /**< keypad initialization state */
} mml_skbd_context_t;

/** Keyboard Key's scan codes */
typedef struct {
	/*
	 * key scan code format as follows
	 *  	key(x) bits[3-0] : Input scan code
	 *  	key(x) bits[7-4] : Output scan code
	 * 		key(x) bit[8]	 : Next Key Flag
	 */
	unsigned short key0; /**< Key0 scan code */
	unsigned short key1; /**< Key1 scan code */
	unsigned short key2; /**< Key2 scan code */
	unsigned short key3; /**< Key3 scan code */
} mml_skbd_keys_t;

/** Functions ******************************************************************/

/**
 * Return the version of the SKBD driver.
 * 
 */
const char * mml_skbd_get_version( void );

/**
 * Configure Keypad interrupt
 */
void mml_keypad_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] priority						Interrupt priority to set
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for priority
 */
int mml_keypad_set_interrupt_priority(mml_intc_prio_t priority);

/** The function is used to initialize the keypad controller
 * @param[in] config								Configuration to set
 * @retval NO_ERROR									No error
 * @retval COMMON_ERR_UNKNOWN						Unknown error
 * @retval MML_SKBD_ERR_IRQ_NULL					Handler is null
 * @retval MML_SKBD_ERR_INVALID_PIN_CONFIGURATION	One or more keypad I/O pins are overlapped or
 * 													input/output pin configurations are invalid
 * @retval MML_SKBD_ERR_ALREAD_INITIALIZED			SKBD is already initialized
 */
int mml_keypad_init(mml_skbd_config_t config);

/** Function is used to enable the interrupt events
 * @param[in] events								Input interrupt events to set
 * @retval NO_ERROR									No error
 * @retval COMMON_ERR_UNKNOWN						Unknown error
 * @retval MML_SKBD_ERR_NOT_INITIALIZED				SKBD not initialized
 */
int mml_keypad_enable_interrupt_events(unsigned int events);

/** Function to disable the interrupt events
 * @param[in] events								Input interrupt events to disable
 * @retval NO_ERROR									No error
 */
int mml_keypad_disable_interrupt_events(unsigned int events);

/** Function is used to clear the interrupt events
 * @param[in] status								Interrupt status to clear
 * @retval NO_ERROR									No error
 */
int mml_keypad_clear_interrupt_status(unsigned int status);

/** Function is used to read the interrupt status
 * @param[in] status								Interrupt status to clear
 * @retval NO_ERROR									No error
 */
int mml_keypad_interrupt_status(unsigned int *status);

/** Function to read the key scan codes
 * @param[in] keys									Pointer on the keyboard Key's scan codes
 * @retval NO_ERROR									No error
 */
int mml_keypad_read_keys(mml_skbd_keys_t *keys);

/** Function to close the keypad
 * @retval NO_ERROR									No error
 */
int mml_keypad_close(void);

/** @} *//* @defgroup MML_SKBD */

#ifdef __cplusplus
}
#endif

#endif /* _MML_SKBD_H_ */

/******************************************************************************/
/* EOF */
