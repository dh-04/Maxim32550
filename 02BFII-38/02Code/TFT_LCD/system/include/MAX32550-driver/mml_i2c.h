/*
 * @file mml_i2c.h MML I2C Driver Header File
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

#ifndef _MML_I2C_H_
#define _MML_I2C_H_

/* Global includes */
#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_I2C I2C Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */


#define xstr(s) str(s)
#define str(s) #s
#define MML_I2C_VERS_MAJOR	3
#define MML_I2C_VERS_MINOR	0
#define MML_I2C_VERS_PATCH	2
#define MML_I2C_VERSION_STRING	"v" xstr(MML_I2C_VERS_MAJOR) "." xstr(MML_I2C_VERS_MINOR) "." xstr(MML_I2C_VERS_PATCH)

/* Defines *******************************************************************/
#define	K_COBRA_I2C_WAIT_LOOP				500000
#define	MML_I2C_MIN_BAUD_RATE			0
#define	MML_I2C_MAX_BAUD_RATE 			400000
#define	MML_I2C_MIN_RXFIFO_THRESHOLD	0
#define	MML_I2C_MAX_RXFIFO_THRESHOLD	8

/* GPIO port and pin for I2C0 */
#define SDA0	GPIO(0,23)	/**< SDA pin for I2C0 */
#define SCL0	GPIO(0,24)	/**< SCL pin for I2C0 */

/* Enumerations **************************************************************/

typedef enum {
	MML_I2C_DEV_MIN = 0,
	MML_I2C_DEV0 = MML_I2C_DEV_MIN, /**< Select I2C */
	MML_I2C_DEV_MAX = MML_I2C_DEV0,
	MML_I2C_DEV_COUNT
} mml_i2c_dev_t;

/** I2C errors list */
typedef enum {
	MML_I2C_ERR_MIN = COBRA_I2C_BASE_ERR,
	MML_I2C_ERR_INIT, /**< Error Code: I2C not initialized */
	MML_I2C_ERR_READY, /**< Error Code: I2C not ready*/
	MML_I2C_ERR_BUSY, /**< Error Code: I2C busy */
	MML_I2C_ERR_INVALID_CMD, /**< Error Code: Invalid command */
	MML_I2C_ERR_NOCONFIG, /**< Error Code: No configuration */
	MML_I2C_ERR_INVALID_BAUD, /**< Error Code: Baud rate invalid*/
	MML_I2C_ERR_INVALID_RXTHRD, /**< Error Code:  */
	MML_I2C_ERR_INVALID_ADDRESS, /**< Error Code: Invalid address */
	MML_I2C_ERR_INVALID_PARAM, /**< Error Code: Invalid parameter*/
	MML_I2C_ERR_GPIO_CONFIG /**< Error Code: GPIO configuration failed*/
} mml_i2c_errors_t;

/**
 * I2C command list
 */
typedef enum mml_i2c_cmd_id {
	MML_I2C_SET_FREEZE = 0, /**< Command to freeze I2C bus */
	MML_I2C_CLEAR_FREEZE, /**< Command to defreeze I2C bus */
	MML_I2C_TXFLUSH, /**< Command to flush TX FIFO*/
	MML_I2C_RXFLUSH, /**< Command to flush RX FIFO */
	MML_I2C_SET_READCOUNT, /**< Command to defreeze I2C bus */
	MML_I2C_CLEAR_READCOUNT, /**< Command to clear RCR register */
	MML_I2C_SET_AUTOSTART, /**< Command to start automatically I2C when a word is placed in the transmit FIFO */
	MML_I2C_CLEAR_AUTOSTART, /**< Command to clear autostart */
	MML_I2C_SET_AFREAD, /**< Command to set auto read */
	MML_I2C_CLEAR_AFREAD /**< Command to clear auto read */

} mml_i2c_cmd_t;

/**
 * I2C auto-start
 */
typedef enum mml_i2c_autost {
	MML_I2C_AUTOSTART_ENABLE = MML_I2C_SET_AUTOSTART, /**< Enable auto-start */
	MML_I2C_AUTOSTART_DISABLE /**< Disable auto-start */
} mml_i2c_autost_t;

/** I2C events */
typedef enum mml_i2c_events {
	MML_I2C_EVENT_LOST = I2C_IER_LOSTIE_Msk, /**< Loss of transfer interrupt enable */
	MML_I2C_EVENT_NOANS = I2C_IER_NOANSIE_Msk, /**< Address not acknowledge interrupt enable */
	MML_I2C_EVENT_COMEN = I2C_IER_COMENDIE_Msk, /**< Communication end interrupt enable */
	MML_I2C_EVENT_RDYRD = I2C_IER_RDYRDIE_Msk, /**< Read ready interrupt enable */
	MML_I2C_EVENT_FFRX = I2C_IER_FFRXIE_Msk, /**< Receive FIFO threshold interrupt enable */
	MML_I2C_EVENT_FFTX_ONE = I2C_IER_FFTXOIE_Msk, /**< Transmit FIFO last word interrupt enable */
	MML_I2C_EVENT_FFTX_HALFEMPTY = I2C_IER_FFTXHIE_Msk, /**< Transmit FIFO last four words interrupt enable */
} mml_i2c_events_t;

/** I2C Interrupt Status */
typedef enum {
	MML_I2C_INTERRUPT_STATUS_LOST = I2C_IER_LOSTIE_Msk, /**< Loss of transfer status */
	MML_I2C_INTERRUPT_STATUS_NOANS = I2C_IER_NOANSIE_Msk, /**< Address not acknowledge status */
	MML_I2C_INTERRUPT_STATUS_COMEN = I2C_IER_COMENDIE_Msk, /**< Communication end status */
	MML_I2C_INTERRUPT_STATUS_RDYRD = I2C_IER_RDYRDIE_Msk, /**< Read ready status */
	MML_I2C_INTERRUPT_STATUS_FFRX = I2C_IER_FFRXIE_Msk, /**< Receive FIFO threshold status */
	MML_I2C_INTERRUPT_STATUS_FFTX_ONE = I2C_IER_FFTXOIE_Msk, /**< Transmit FIFO last word status */
	MML_I2C_INTERRUPT_STATUS_FFTX_HALFEMPTY = I2C_IER_FFTXHIE_Msk /**< Transmit FIFO last four words status */
} mml_i2c_interrupt_status_t;

/** I2C initialization state */
typedef enum {
	MML_I2C_STATE_NOT_INITIALIZED = 0, 	/**< I2C Not Initialized */
	MML_I2C_STATE_INITIALIZED		/**< I2C Initialized */

} mml_i2c_state_t;


/** I2C IRQ handler function */
typedef void (*mml_i2c_handler_t)(void);

/** Structures ****************************************************************/
/**
 * I2C configure structure
 */
typedef struct {
	unsigned int baudrate; /**< Baud rate in bps */
	unsigned int rxfifo_thr; /**< RX FIFO threshold ( 1 to 8)*/
	unsigned int flags; /**< autostart, restart */
	mml_i2c_handler_t irq_handler; /**< IRQ handler function */
} mml_i2c_config_t;

/**
 * I2C port structure
 */
typedef struct {
	mml_i2c_state_t state;		/**< Internal module state */
	I2C_Type *reg;			/**< I2C device Register base addresses */
	mml_i2c_handler_t irq_handler; /**< IRQ handler function */
} mml_i2c_port_conf_t;

/**
 * I2C context structure
 */
typedef struct {
	unsigned int first_init[MML_I2C_DEV_COUNT];	/**< Flag to know if first init is done */
	mml_i2c_port_conf_t port[MML_I2C_DEV_COUNT]; /**< I2C port (I2C0 or I2C1) */
} mml_i2c_context_t;









/**
 * Return the version of the I2C driver.
 * 
 */
const char * mml_i2c_get_version( void );

/**
 * Configure I2Cs interrupt
 */
void mml_i2c_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] vector						IRQ Number corresponding to the I2C vector to set
 * @param[in] priority						Interrupt priority to set to the vector
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for vector or priority
 */
int mml_i2c_set_interrupt_priority(int vector, mml_intc_prio_t priority);

/**
 * Function to initialize and configure the I2C interface
 * @param[in] devnum					Select the I2C device
 * @param[in] config					I2C configuration parameter
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Invalid range for vector
 * @retval COMMON_ERROR_UNKNOWN 		Unknown error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed
 * @retval MML_I2C_ERR_INVALID_BAUD		Baud rate is out of range
 * @retval MML_I2C_ERR_INVALID_RXTHRD	Receive FIFO threshold is invalid
 * @retval MML_I2C_ERR_GPIO_CONFIG		GPIO initialization error
 */
int mml_i2c_init(mml_i2c_dev_t devnum, mml_i2c_config_t *config);

/**
 * Function to configure the I2C interface which is already been initialized
 * @param[in] devnum					Select the I2C device
 * @param[in] config					I2C configuration parameters
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Devnum is in invalid range
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed
 * @retval MML_I2C_ERR_INVALID_BAUD		Baud rate is out of range
 * @retval MML_I2C_ERR_INVALID_RXTHRD	Receive FIFO threshold is invalid
 * @retval MML_I2C_ERR_GPIO_CONFIG		GPIO initialization error
 */
int mml_i2c_set_configuration(mml_i2c_dev_t devnum, mml_i2c_config_t *config);

/**
 * Function initializes the I2C write operation and transmits. this function send only 7bytes of data and one byte of address (slave address)
 * @param[in] devnum					Select the I2C device
 * @param[in] address					I2C Device address
 * 										10 bit address pattern = 1111 0xx0 xxxx xxxx
 * 										7 bit address pattern  = 0000 0000 xxxx xxx0
 * @param[in] data						Memory address from where the data to be transmitted
 * @param[in/out] plength				Memory address pointing to the 'length' number of bytes to transmit.
 * 										If the number of bytes exceed 7 you should use function mml_i2c_write just after
 * 										mml_i2c_write_start to transmit the last bytes.
 * 										At the end the *plength is the number of bytes non transmitted.
 * 										Example: if you want to transmit 8bytes of data. The first 8bytes you will
 * 												 have the slave address and the next seven bytes are the first seven bytes of data,
 * 												 the new value of *plength is 1 because one bytes was not transmitted.
 * 												 You must use again mml_i2c_write to transmit the last byte.
 * @retval NO_ERROR						No error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval MML_I2C_ERR_READY			TX FIFO is not empty
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 * @retval MML_I2C_ERR_INVALID_ADDRESS	Address is not one of 10 bit OR 7 bit pattern
 */
int mml_i2c_write_start(mml_i2c_dev_t devnum, unsigned short address,
		const unsigned char *data, unsigned int *plength);

/**
 * Function to transmit the data to the I2C device, this function should be called after the
 * communication has started.
 * Pre-condition : mml_i2c_write_start() to be called before calling this function.
 * NOTE: this functions is used to transmit data only !
 * @param[in] devnum					Select the I2C device
 * @param[in] data						Memory address from where the data to be transmitted
 * @param[in] plength					Memory address pointing to the 'length' number of bytes to transmit.
 * 										If the number of bytes exceed 7 you should use again function mml_i2c_write
 * 										in order to transmit the last bytes.
 * @retval NO_ERROR						No error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_write(mml_i2c_dev_t devnum, unsigned char *data,
		unsigned int *plength);

/**
 * Function to transmit the data to the I2C device, this function should be called after the communication has started.
 * Pre-condition : mml_write_start() to be called before calling this function.
 * NOTE : this function is used to transmit data only.
 * @param[in] devnum					Select the I2C device
 * @param[in] data						Memory address from where the data to be transmitted
 * @param[in] len_requested				Number of bytes to write.
 * 										If the number of bytes exceed 7 you should use again function mml_i2c_write
 * 										in order to transmit the last bytes.
 * @param[out] plen_writed				Memory address pointing to the number of bytes written.
 * @retval NO_ERROR						No error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_write_attended(mml_i2c_dev_t devnum, unsigned char *data, unsigned int len_requested, unsigned int *plen_written);

/**
 * Function to initiate the I2C Start condition for read operation.
 * Pre-condition : mml_i2c_write_start() to be called before calling this function.
 * No data is read from the I2C slave device but it just initializes the I2C slave device for reading.
 * @param[in] devnum					Select the I2C device
 * @param[in] address					I2C Device address
 * 										10-bit address pattern = 1111 0xx0 xxxx xxxx
 * 										7-bit address pattern  = 0000 0000 xxxx xxx0
 * @retval NO_ERROR						No error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval MML_I2C_ERR_INVALID_ADDRESS	Invalid device address
 * @retval MML_I2C_ERR_READY			TX FIFO empty
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_read_start(mml_i2c_dev_t devnum, unsigned short address);

/**
 * Function to receive the data from the I2C device, this function should be called after the
 * communication has started i.e. mml_i2c_read_start() should be invoked before calling this function.
 * @param[in] devnum					Select the I2C device
 * @param[out] data						Memory address where the read data to be stored (maximum = 8).
 * @param[in/out] plength				Memory address pointing to the 'length' number of bytes to receive.
 * 										If the number of bytes exceed 7 you should use again function mml_i2c_read
 * 										At the end the *plength is the number of bytes non received.
 * 										Example: if you want to receive 9bytes of data. You must use mml_i2c_read two times
 * 												 You must use again mml_i2c_read to transmit the last byte.
 * @retval NO_ERROR						No error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_read(mml_i2c_dev_t devnum, unsigned char *data,
		unsigned int *plength);

/**
 * Function to receive the data from the I2C device, this function should be called after the
 * communication has started i.e. mml_i2c_read_start() should be invoked before calling this function.
 * @param[in] devnum					Select the I2C device
 * @param[out] data						Memory address where the read data to be stored (maximum = 8).
 * @param[in] len_requested				Number of bytes supposed to be received.
 * 										If the number of bytes exceed 7 you should use again function mml_i2c_read
 * @param[out] plen_received			Memory address pointing to the number of bytes receive.
 * 										Example: if you want to receive 9bytes of data. You must use mml_i2c_read two times
 * 												 You must use again mml_i2c_read to read the last byte.
 * @retval NO_ERROR						No error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_read_attended(mml_i2c_dev_t devnum, unsigned char *data, unsigned int len_requested, unsigned int *plen_received);

/**
 * I2C Input/output command control function
 * @param[in] devnum					Select the I2C device
 * @param[in] command					IOCTL commands
 * @param[in] data						Data if present for the IOCTL command else
 * 										This parameter is ignored.
 * @retval NO_ERROR						No error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_NULL_PTR			NULL pointer parameter passed
 * @retval COMMON_ERR_UNKNOWN			Unknown error *
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_ioctl(mml_i2c_dev_t devnum, int command, void *data);

/**
 * Function to stop the current read communication from the I2C device.
 * @param[in] devnum					Select the I2C device
 * @retval NO_ERROR						No error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_stop(mml_i2c_dev_t devnum);

/**
 * Close the I2C Device and uninstall the I2C IRQ handler if installed.
 * @param[in] devnum					Select the I2C device
 * @retval NO_ERROR						No error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_close(mml_i2c_dev_t devnum);

/**
 * Function to enable the desired I2C interrupt event/s
 * @param[in] devnum					Select the I2C device
 * @param[in] events					I2C Interrupt events to be enabled
 * @retval NO_ERROR						No error
 * @retval MML_I2C_ERR_INIT				Device is not initialized
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_enable_interrupt_events(mml_i2c_dev_t devnum, unsigned int events);

/**
 * Function to disable the desired I2C interrupt event/s
 * @param[in] devnum					Select the I2C device
 * @param[in] events					I2C Interrupt events to be disabled
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_disable_interrupt_events(mml_i2c_dev_t devnum, unsigned int events);

/**
 * Function to acknowledge/clear the desired I2C interrupt status
 * @param[in] devnum					Select the I2C device
 * @param[in] status					interrupt status flag/s to be cleared
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_interrupt_status(mml_i2c_dev_t devnum, unsigned int *status);

/**
 * Function to read the current interrupt status flags
 * @param[in] devnum				Select the I2C device
 * @param[out] status				Pointer to interrupt status flag
 * @retval NO_ERROR					No error
 * @retval COMMON_ERR_INVAL			No such device or value is not appropriate
 */
int mml_i2c_clear_interrupt_status(mml_i2c_dev_t devnum, unsigned int status);

/**
 * Get the I2C bus status
 * @param[in] devnum					Select the I2C device
 * @param[out] status					I2C bus status
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				No such device or value is not appropriate
 */
int mml_i2c_bus_status(mml_i2c_dev_t devnum, unsigned int *status);

/** @} *//* @defgroup MML_I2C */

#ifdef __cplusplus
}
#endif

#endif /* _MML_I2C_H_ */

/******************************************************************************/
/* EOF */

