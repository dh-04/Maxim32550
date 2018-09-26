/*
 * @file mml_uart.h MML UART Driver Header File
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

#ifndef _MML_UART_H_
#define _MML_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_UART UART Driver (Serial Port)
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_UART_VERS_MAJOR	3
#define MML_UART_VERS_MINOR	0
#define MML_UART_VERS_PATCH	2
#define MML_UART_VERSION_STRING	"v" xstr(MML_UART_VERS_MAJOR) "." xstr(MML_UART_VERS_MINOR) "." xstr(MML_UART_VERS_PATCH)

/* Global includes */

#include <errors.h>
/* Other includes */

/* Defines ********************************************************************/
#define	MML_UART_BASE_ERROR						COBRA_UART_BASE_ERR
#define	MML_UART_BRR_DIV_FACTOR					128
#define MML_UART_BAUD_RATE_MIN					1200
#define MML_UART_BAUD_RATE_MAX					921600

#define	MML_UARTx_GPIO_CFG_MASK_NOOFST			0xf

#define	MML_UART0_GPIO_CFG_OFFSET				8
#define	MML_UART0_GPIO_CFG_MASK					( MML_UARTx_GPIO_CFG_MASK_NOOFST << MML_UART0_GPIO_CFG_OFFSET )

#define	MML_UART1_GPIO_CFG_OFFSET				12
#define	MML_UART1_GPIO_CFG_MASK					( MML_UARTx_GPIO_CFG_MASK_NOOFST << MML_UART1_GPIO_CFG_OFFSET )

/* COBRA adaptation */
#define UARTB_CR_SIZE_8BIT       				0x00000c00

/* Nominal Frequency @192MHz */
#ifndef NOMINAL_FREQ_48MHZ
#define MML_BR_9600 							{156, 32}
#define MML_BR_57600       						{26, 5}
#define MML_BR_115200      						{13, 5}
#else
#define MML_BR_9600 							{78, 16}
#define MML_BR_57600 							{6, 65}
#define MML_BR_115200							{3, 32}
#endif/* NOMINAL_FREQ_48MHZ */

/* Macros *********************************************************************/

/* Enumerations ***************************************************************/

/** UART errors list */
typedef enum {
	MML_UART_ERR_MIN = MML_UART_BASE_ERROR,
	MML_UART_ERR_NOT_INITIALIZED, /**< Error Code: UART port not initialized */
	MML_UART_ERR_NOT_AVAILABLE, /**< Error Code: Invalid port or UART port not available */
	MML_UART_ERR_TX, /**< Error Code: UART TX error */
	MML_UART_ERR_RX, /**< Error Code: UART RX error */
	MML_UART_ERR_IRQ_SET, /**< Error Code: Interrupt can not be initialized */
	MML_UART_ERR_UNKNOWN, MML_UART_ERR_MAX = MML_UART_ERR_UNKNOWN /**< Error Code: Generic error for unknown behavior */
} mml_uart_errors_t;

/* UART EVEN or ODD parity */
/* NOTE: Don't interchange the values */
typedef enum {
	MML_UART_PARITY_EVEN = 0, /**< Parity even */
	MML_UART_PARITY_ODD = 1, /**< Parity odd */
	MML_UART_PARITY_NONE = 2 /**< Parity none */
} mml_uart_parity_t;

/** UART parity mode i.e. the parity is based on 0's or 1's in the frame */
typedef enum {
	MML_UART_PARITY_MODE_ONES = 0x00, /**< Parity calculation is based on number of 1s in frame */
	MML_UART_PARITY_MODE_ZEROS = 0x01, /**< Parity calculation is based on number of 0s in frame */
} mml_uart_parity_mode_t;

/**  UART FSM */
typedef enum {
	MML_UART_STATE_MIN = 0,
	MML_UART_STATE_NOT_INITIALIZED = MML_UART_STATE_MIN, /**< UART not initialized */
	MML_UART_STATE_INITIALIZED, /**< UART initialized */
	MML_UART_STATE_CLOSED, /**< UART closed */
	MML_UART_STATE_MAX = MML_UART_STATE_CLOSED,
	MML_UART_STATE_COUNT
} mml_uart_state_t;

/** UART device id's */
typedef volatile enum {
	MML_UART_DEV_MIN = 0,
	MML_UART_DEV0 = MML_UART_DEV_MIN, /**< UART0 */
	MML_UART_DEV1, /**< UART1 */
	MML_UART_DEV_MAX = MML_UART_DEV1,
	MML_UART_DEV_COUNT
} mml_uart_id_t;

/* Structures *****************************************************************/
typedef void (*mml_uart_handler_t)(void);

typedef struct {
	unsigned int brr;
	unsigned int rcr;
} mml_uart_brr_t;

/** UART configuration information */
typedef struct {
	UARTn_CTRL_SIZE_Type data_bits; /**< Data bits */
	mml_uart_parity_t parity; /**< UART Parity */
	mml_uart_parity_mode_t parity_mode; /**< Parity Mode based on 0's or 1's in the frame */
	UARTn_CTRL_STOP_Type stop_bits; /**< Stop bits */
	unsigned int baudrate; /**< Baud rate */
	volatile mml_uart_handler_t handler; /**< IRQ handler */
	UARTn_CTRL_RTSCTSF_Type flwctrl; /**< Hardware Flow Control bit */
	UARTn_PIN_RTS_Type rts_ctl; /**< UART RTS IO level */
} mml_uart_config_t;

/** UART port configuration information */
typedef struct {
	unsigned int irq; /**< Interrupt number */
	volatile UARTn_Type *reg; /**< Base address of the interface */
	mml_uart_config_t config; /**< UART port parameters */
	mml_uart_handler_t handler; /**< UART interrupt handler */
} mml_uart_port_conf_t;

/** UART port context information */
typedef struct {
	unsigned int first_init; /**< First initialization */
	mml_uart_port_conf_t port[MML_UART_DEV_COUNT]; /**< UART ports */
} mml_uart_context_t;


/**
 * Return the version of the UART driver.
 * 
 */
const char * mml_uart_get_version( void );

/**
 * Configure UARTs interrupt
 */
void mml_uart_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] vector						IRQ Number corresponding to the UART vector to set
 * @param[in] priority						Interrupt priority to set to the vector
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for vector or priority
 */
int mml_uart_set_interrupt_priority(int vector, mml_intc_prio_t priority);

/**
 * The function is used to initialize the UART port.
 * @param[in] port							The UART interface identifier
 * @param[in] config						Configuration structure which specifies transfer size
 * 											(byte width), baud rate, parity, parity mode,
 * 											stops bits and flow control
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number
 * @retval COMMON_ERR_NULL_PTR				NULL pointer parameter passed
 * @retval COMMON_ERR_BAD_STATE				UART port is not initialized
 * @retval COMMON_ERR_INVAL					Invalid configuration parameter
 * @retval COMMON_ERR_RUNNING				UART is currently busy with transmitting or receiving the data
 * @retval MML_UART_ERR_IRQ_SET				Interrupt can not be initialized
 * @retval MML_GPIO_ERR_INVALID_DEVICE		Invalid GPIO device identifier
 * @retval MML_GPIO_ERR_OUT_OF_RANGE		Invalid parameter
 */
int mml_uart_init(mml_uart_id_t port, mml_uart_config_t config);

/**
 * This function is use to reset all UART interface
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_IN_PROGRESS			Reset is in progress
 */
int mml_uart_reset_interface(void);

/**
 * The function is used to configure the communication parameters of UART Interface.
 * @param[in] port							The UART interface identifier
 * @param[in] config						Configuration structure which specifies transfer
 * 											size(data byte width), baud rate, parity, parity mode,
 * 											stops bits and flow control
 * 											NOTE: structure must be allocated and filled by the
 * 											caller
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number
 * @retval COMMON_ERR_NULL_PTR				NULL pointer parameter passed
 * @retval COMMON_ERR_BAD_STATE				UART port is not initialized
 * @retval COMMON_ERR_INVAL					Invalid configuration parameter
 * @retval COMMON_ERR_RUNNING				UART is currently busy with transmitting or receiving the data
 * @retval MML_UART_ERR_IRQ_SET				Interrupt can not be initialized
 */
int mml_uart_set_config(mml_uart_id_t port, mml_uart_config_t *config);

/**
 * The function is used to receive a character data from the UART Interface.
 * @param[in] port							The UART interface identifier
 * @param[out] data							Pointer to the character data where
 * 											the received data to be stored
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_NULL_PTR				Parameter passed is NULL pointer
 * @retval COMMON_ERR_BAD_STATE				UART port is not initialized
 * @retval COMMON_ERR_RUNNING				UART RX FIFO is empty (try later)
 * @retval COMMON_ERR_NOT_INITIALIZED		UART not initialized
 * @retval COMMON_ERR_NO_MATCH				Parity error
 */
int mml_uart_read_char(mml_uart_id_t port, unsigned char *data);

/**
 * The function is used to transmit a character data from the UART Interface.
 * @param[in] port							The UART interface identifier
 * @param[in] data							The character data to transmit
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_BAD_STATE				UART port is not initialized
 * @retval COMMON_ERR_RUNNING				UART TX FIFO is full and try later
 * @retval COMMON_ERR_NOT_INITIALIZED		UART not initialized
 */
int mml_uart_write_char(mml_uart_id_t port, unsigned char data);

/**
 * The function is used to flush the bytes left out in the transmit FIFO of the UART Interface.
 * @param[in] port							The UART interface identifier
 * @param[in] mode							UART control register value
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_NOT_INITIALIZED		UART not initialized
 */
int mml_uart_flush(mml_uart_id_t port, unsigned int mode);

/**
 * The function is used to retrieve the state of an UART Interface.
 * @param[in] port							The UART interface identifier
 * @param[out] status						Current status of the UART Interface.
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number
 * @retval COMMON_ERR_NULL_PTR				NULL pointer passed
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_NOT_INITIALIZED		UART not initialized
 */
int mml_uart_port_status(mml_uart_id_t port, int *status);

/**
 * The function is used to retrieve the interrupt status of an UART Interface.
 * @param[in] port							The UART interface identifier
 * @param[out] status						Current interrupt status of the UART Interface.
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number
 * @retval COMMON_ERR_NULL_PTR				NULL pointer passed
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_NOT_INITIALIZED		UART not initialized
 */
int mml_uart_port_interrupt_status(mml_uart_id_t port, int *status);

/**
 * The function is used to shutdown the UART Interface.
 * @param[in] port							The UART interface identifier
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_OUT_OF_RANGE			Invalid UART port number
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_NOT_INITIALIZED		UART not initialized
 */
int mml_uart_deinit(mml_uart_id_t port);

/**
 * The function is used to flush bytes left out in the transmit FIFO of the UART Interface
 * @param[in] port							The UART interface identifier
 * @param[in] mode							UART control register value
 */
void mml_uart_flush_raw(mml_uart_id_t port, unsigned int mode);

/*
 * This function is used to attach an UART handler to an interruption
 * @param[in] port							The UART interface identifier
 * @param[in] handler						Handler for UART interruption
 * 											the received data to be stored
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Port is in invalid range
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_NULL_PTR				Parameter passed is NULL pointer
 * @retval MML_UART_ERR_NOT_INITIALIZED		UART port is not initialized
 */
int mml_uart_callback_handler_register(mml_uart_id_t port,
		mml_uart_handler_t handler);

/*This function is used to detach an UART handler to an interruption
 *
 * @param[in] port							The UART interface identifier
 * @param[in] handler						Handler for UART interruption
 * 											the received data to be stored
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_INVAL					Invalid UART port number
 * @retval COMMON_ERR_NULL_PTR				Parameter passed is NULL pointer
 * @retval MML_UART_ERR_NOT_INITIALIZED		UART port is not initialized
 */
int mml_uart_callback_handler_unregister(mml_uart_id_t port);

/**
 * This function is used to enable UART interruption in NVIC
 * @param[in] port						The UART interface identifier
 * @param[in] activation				Select state of interrupt:
 * 										activation = 0 enable UART's irq
 * 										activation = 1 disable UART's irq
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval COMMON_ERR_INVAL				Invalid UART port number
 */
int mml_uart_interrupt_activate(mml_uart_id_t port, unsigned char activation);

/**
 * This function is used to clear UART interruption status register
 * @param[in] port							The UART interface identifier
 * @param[in] uart_irq						value to set in UART interruption status register
 * @return
 */
void mml_uart_interrupt_clear(mml_uart_id_t port, unsigned int uart_irq);

/**
 * This function is used to set an interrupt by cleaning the interruption status register and setting interruption enable register
 * @param port							UART device id
 * @param uart_irq						Value to set in UART interruption status register
 */
void mml_uart_interrupt_set(mml_uart_id_t port, unsigned int uart_irq);

/**
 * This function is used to clear pending interrupt
 * @param[in]	port						UART device id
 */
void mml_uart_interrupt_ack(mml_uart_id_t port);

/**
 *  Uart interrupt handler for all uart port
 *  @param[in] port							UART device id
 *  NOTE: this handler calls your mml_uart_handler_t
 */
void mml_uart_irq_handler (mml_uart_id_t port);

/**
 * This function is used to save UART context in SCPA.
 * @param[in] port							The UART interface identifier
 * @retval NO_ERROR							No error.
 * @retval COMMON_ERR_INVAL					port is in invalid range.
 */
int mml_uart_init_context_scpa (mml_uart_id_t port);

/** @} *//* @defgroup MML_UART */

#ifdef __cplusplus
}
#endif

#endif /* _MML_UART_H_ */

/******************************************************************************/
/* EOF */
