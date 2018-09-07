/*
 * @file mml_dma.h MML DMA Driver Header File
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

#ifndef _MML_DMA_H_
#define _MML_DMA_H_

/* Global includes */
#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_DMA DMA Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_DMA_VERS_MAJOR	3
#define MML_DMA_VERS_MINOR	0
#define MML_DMA_VERS_PATCH	2
#define MML_DMA_VERSION_STRING	"v" xstr(MML_DMA_VERS_MAJOR) "." xstr(MML_DMA_VERS_MINOR) "." xstr(MML_DMA_VERS_PATCH)

/* Macros Defines ********************************************************************/
/* COBRA adaptation */
#define	MML_DMA_BASE_ERR						COBRA_DMA_BASE_ERR

#define MML_DMA_CN_CHIEN_ENABLE					0x1 //!< CHIEN enable
#define MML_DMA_CN_CHIEN_DISABLE				0x0 //!< CHIEN disable

/* Enumerations ***************************************************************/
/** DMA errors list */
typedef enum {
	MML_DMA_ERR_MIN = MML_DMA_BASE_ERR, MML_DMA_ERR_NOT_INITIALIZED, /**< Error Code: DMA not initialized */
	MML_DMA_ERR_INVALID_DEVICE, /**< Invalid DMA channel ID */
	MML_DMA_ERR_OUT_OF_RANGE, /**< Invalid parameter or value */
	MML_DMA_ERR_INVALID, /**< Error Code: Invalid operation */
	MML_DMA_ERR_RELOAD_NOT_CONFIGURED, /**< DMA channel is not configured for Re-Load/Chain transfer */
	MML_DMA_ERR_IRQ_SET, /**< IRQ setup error */
	MML_DMA_ERR_BUSY, /**< DMA is busy */
	MML_DMA_ERR_INTERRUPT_PENDING, /**< Interrupt is pending */
	MML_DMA_ERR_UNKNOWN, /**< Error Code: Generic error for unknown behavior */
	MML_DMA_ERR_MAX = MML_DMA_ERR_UNKNOWN
} mml_dma_errors_t;

/** DMA device id's */
typedef volatile enum {
	MML_DMA_CH_MIN = 0, MML_DMA_CH0 = MML_DMA_CH_MIN, /**< DMA channel 0 */
	MML_DMA_CH1, /**< DMA channel 1 */
	MML_DMA_CH2, /**< DMA channel 2 */
	MML_DMA_CH3, /**< DMA channel 3 */
	MML_DMA_CH_MAX = MML_DMA_CH3, MML_DMA_CH_COUNT
} mml_dma_channel_t;

/** DMA Request Wait Enable */
typedef enum {
	MML_DMA_START_TIMER_NORMAL = 0, /**< Start timer normally */
	MML_DMA_DELAY_TIMER_START = 1 /**< Start timer with delay */
} mml_dma_req_t;

/**  DMA initialization state FSM */
typedef enum {
	MML_DMA_STATE_MIN = 0, MML_DMA_STATE_NOT_INITIALIZED = MML_DMA_STATE_MIN, /**< DMA is not initialized */
	MML_DMA_STATE_INITIALIZED, /**< DMA is initialized */
	MML_DMA_STATE_CLOSED, /**< DMA is closed */
	MML_DMA_STATE_MAX = MML_DMA_STATE_CLOSED, MML_DMA_STATE_COUNT
} mml_dma_state_t;

/** DMA channel current status */
typedef enum {
	MML_DMA_STATUS_MIN = 0, MML_DMA_STATUS_NOT_INITIALIZED = MML_DMA_STATUS_MIN, /**< DMA channel is not initialized */
	MML_DMA_STATUS_IDLE, /**< DMA channel is in idle state */
	MML_DMA_STATUS_BUSY, /**< DMA channel is busy */
	MML_DMA_STATUS_NORMAL_CONFIGURATION, /**< DMA channel is in normal configuration */
	MML_DMA_STATUS_RELOAD_CONFIGURATION, /**< DMA channel is in reload configuration */
} mml_dma_channel_status_t;

typedef enum {
	MML_DMA_STATUS_DISABLED = 0, /**< DMA disable */
	MML_DMA_STATUS_ENABLED /**< DMA enable */

} mml_dma_status_t;

typedef void (*mml_dma_handler_t)(mml_dma_channel_t ch);

/** Structures *****************************************************************/
/**
 * DMA Configuration structure
 */
typedef struct {
	DMAn_CFG_PRI_Type priority;
	DMAn_CFG_REQSEL_Type request;
	mml_dma_req_t req_wait;
	DMAn_CFG_TOSEL_Type timeout;
	DMAn_CFG_PSSEL_Type prescale;
	DMAn_CFG_SRCWD_Type src_width;
	int src_increment;
	DMAn_CFG_DSTWD_Type dst_width;
	int dst_increment;
	int burst_size; /** Note : valid range for busrt_size is 1 to 32*/
	mml_dma_handler_t handler;
} mml_dma_config_t;

/** DMA address configuration */
typedef struct {
	unsigned int src;
	unsigned int dst;
	unsigned int count;
} mml_dma_addr_t;

/** DMA callback function for reload configuration, invoked from the DMA interrupt context */
typedef void (*mml_dma_realod_callback_t)(mml_dma_addr_t *rld_addr,
		int *next_rld);

/** DMA channel structure */
typedef struct {
	mml_dma_state_t state; /** DMA channel initialization state */
	mml_dma_channel_status_t status; /** DMA channel status */
	mml_dma_realod_callback_t rld_callback; /** DMA reload callback function */
	mml_dma_handler_t handler; /** DMA interrupt handler */
} mml_dma_channel_struct_t;

/** DMA channel context information */
typedef struct {
	unsigned int first_init;
	mml_dma_channel_struct_t channel[MML_DMA_CH_COUNT]; /** DMA Channel structure */
} mml_dma_context_t;

/**
 * Return the version of the DMA driver.
 * 
 */
const char * mml_dma_get_version(void);

/**
 * This function is used to get free dma channel
 * @retval MML_DMA_CH0						DMA channel 0 is free
 * @retval MML_DMA_CH1						DMA channel 1 is free
 * @retval MML_DMA_CH2						DMA channel 2 is free
 * @retval MML_DMA_CH3						DMA channel 3 is free
 * @retval MML_DMA_ERR_BUSY					All DMA channel busy
 */
uint32_t mml_dma_get_free_channel(void);

/**
 * Configure DMAs interrupt
 */
void mml_dma_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] vector						IRQ Number corresponding to the DMA vector to set
 * @param[in] priority						Interrupt priority to set to the vector
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for vector or priority
 */
int mml_dma_set_interrupt_priority(int vector, mml_intc_prio_t priority);

/**
 * The function is used to initialize the DMA channel. It will initialize the DMA driver internal
 * data structures and setups the interrupt handler and enables the channel IRQ.
 * @param[in] ch							DMA channel number
 * @param[in] config						DMA configuration information.
 * @param[in] addr							DMA source and destination address for data transfer and byte count specifies
 * 											the number of bytes to transfer.
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for burst size
 * 											Note: value of burst size is maximum 32
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval MML_DMA_ERR_INVALID_DEVICE		Invalid DMA channel number passed.
 * @retval COMMON_ERR_BAD_STATE				DMA channel already been initialized.
 * @retval MML_DMA_ERR_IRQ_SET				IRQ setup error
 */
int mml_dma_init_without_irq(mml_dma_channel_t ch, mml_dma_config_t config,
		mml_dma_addr_t addr);

/**
 *
 * The function is used to initialize the DMA channel. It will initialize the DMA driver internal
 * data structures and setups the interrupt handler and enables the channel IRQ.
 * NOTE: this function is only here for legacy.
 * 		 There is a bug because irq number should be equal to channel (ch) number
 * 		 Prefered method is now mml_dma_init_without_irq
 * @param[in] ch							DMA channel number
 * @param[in] irq							DMA channel IRQ number.
 * @param[in] config						DMA configuration information.
 * @param[in] addr							DMA source and destination address for data transfer and byte count specifies
 * 											the number of bytes to transfer.
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for burst size
 * 											Note: value of burst size is maximum 32
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval MML_DMA_ERR_INVALID_DEVICE		Invalid DMA channel number passed.
 * @retval COMMON_ERR_BAD_STATE				DMA channel already been initialized.
 * @retval MML_DMA_ERR_OUT_OF_RANGE			IRQ number is out of range
 * 											NOTE: User must pass the appropriate IRQ number to the respective DMA channel.
 * 											Meaning that the IRQ number 28 must be passed for the DMA Channel-0 initialization.
 * @retval MML_DMA_ERR_IRQ_SET				IRQ setup error
 */
int mml_dma_init(mml_dma_channel_t ch, int irq, mml_dma_config_t config,
		mml_dma_addr_t addr);

/**
 * The function configures the DMA channel which is already been initialized/configured.
 * This function can be used to re-configure an already initialized and/or configured DMA channel
 * with the new DMA channel configurations. If the DMA channel is busy then the function
 * returns error code.
 * @param[in] ch							DMA channel number
 * @param[in] config						Configuration structure which specifies
 * 											DMA configuration info.
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for burst size
 * 											Note: value of burst size is maximum 32
 * @retval MML_DMA_ERR_INVALID_DEVICE		Invalid DMA channel number passed.
 * @retval COMMON_ERR_BAD_STATE				DMA channel is not initialized.
 * @retval MML_DMA_ERR_BUSY					DMA is running.
 */
int mml_dma_set_configuration(mml_dma_channel_t ch, mml_dma_config_t config);

/**
 * This function reads the DMA channel current configuration.
 * @param[in] ch							DMA channel number
 * @param[in] *config						Pointer to the DMA channel configuration structure.
 * @retval NO_ERROR							No error
 * @retval MML_DMA_ERR_INVALID_DEVICE		Invalid DMA channel number passed.
 */
int mml_dma_get_configuration(mml_dma_channel_t ch, mml_dma_config_t *config);

/**
 * The function sets the DMA channel address for new buffer transfers.
 * If the DMA channel is busy then the function returns error.
 * @param[in] ch							DMA channel number
 * @param[in] addr							DMA address config structure
 * @retval NO_ERROR							No error
 * @retval MML_DMA_ERR_INVALID_DEVICE		Invalid DMA channel number passed.
 * @retval COMMON_ERR_BAD_STATE				DMA channel is not initialized.
 * @retval MML_DMA_ERR_BUSY					DMA is running.
 */
int mml_dma_set_address(mml_dma_channel_t ch, mml_dma_addr_t addr);

/**
 * This function sets the DMA channel reload addresses for the chained buffer transfers. Chained buffer
 * transfers will avoid the interrupt delays such as setting up of source and destination address information.
 * The DMA channel must have been initialized before calling this function for chained DMA transfers.
 *
 * @param[in] ch							DMA channel number
 * @param reload_addr						Reload DMA source and destination address and byte count
 * 											which specifies number of bytes to transfer.
 * @param rld_callback						callback function upon data transfer completion.
 * @retval NO_ERROR							No error
 * @retval MML_DMA_ERR_INVALID_DEVICE		Invalid DMA channel number passed.
 * @retval COMMON_ERR_BAD_STATE				DMA channel is not initialized.
 * @retval MML_DMA_ERR_BUSY					DMA is running.
 */
int mml_dma_set_chaintransfer(mml_dma_channel_t ch, mml_dma_addr_t reload_addr,
		mml_dma_realod_callback_t rld_callback);

/**
 * This function enables the DMA channel for data transfer.
 * @param ch								DMA channel number
 * @retval NO_ERROR							No error
 * @retval MML_DMA_ERR_INVALID_DEVICE		Invalid DMA channel number passed.
 * @retval COMMON_ERR_BAD_STATE				DMA channel is not initialized.
 */
int mml_dma_enable(mml_dma_channel_t ch);

/**
 * This function disables the DMA channel. If the interrupt is active/pending for the DMA channel,
 * the function returns appropriate error code and the application must wait till the pending
 * interrupts are processed/cleared.
 * @param ch								DMA channel number
 * @retval NO_ERROR							No error
 * @retval MML_DMA_ERR_INVALID_DEVICE		Invalid DMA channel number passed.
 * @retval COMMON_ERR_BAD_STATE				DMA channel is not initialized.
 * @retval MML_DMA_ERR_INTERRUPT_PENDING	DMA channel interrupt is pending
 */
int mml_dma_disable(mml_dma_channel_t ch);

/**
 * Function returns the DMA channel current status
 * @param[in] ch							DMA channel number
 * @param[out] status						DMA status pointer flag indicates whether the DMA channel
 * 											is enabled/disabled.
 * @retval NO_ERROR							No error
 * @retval MML_DMA_ERR_INVALID_DEVICE		Invalid DMA channel number passed.
 */
int mml_dma_get_status(mml_dma_channel_t ch, mml_dma_status_t *status);

/** @} *//* @defgroup MML_DMA */

#ifdef __cplusplus
}
#endif

#endif /* _MML_DMA_H_ */

/******************************************************************************/
/* EOF */

