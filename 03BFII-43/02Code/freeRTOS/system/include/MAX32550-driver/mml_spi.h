/*
 * @file mml_spi.h MML SPI Driver Header File
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

#ifndef _MML_SPI_H_
#define _MML_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_SPI SPI Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_SPI_VERS_MAJOR	3
#define MML_SPI_VERS_MINOR	0
#define MML_SPI_VERS_PATCH	2
#define MML_SPI_VERSION_STRING	"v" xstr(MML_SPI_VERS_MAJOR) "." xstr(MML_SPI_VERS_MINOR) "." xstr(MML_SPI_VERS_PATCH)

/* Defines ********************************************************************/
#define MML_GCR_RSTR_SPI_MASK				(GCR_RSTR_SPI0_Msk | GCR_RSTR_SPI1_Msk | GCR_RSTR_SPI2_Msk)
#define SPI_MAX_BAUD_RATE					12000000
#define SPI_MIN_BAUD_RATE					732
#define SPI_DFLT_BAUD_RATE					0x0000ffff
#define	SPI_SYSTEM_FREQ_MAX					108000000
#define SPI_BLOC_FREQUENCY					54000000
#define SPI_WORDSIZE_MAX					16
#define SPI_WORDSIZE_MIN					1
#define MML_SPI_MR_SSV_MASK_NOOFST				0x1 //!< SSV mask no offset
#define MML_SPI_MR_SSIO_MASK_NOOFST				0x1 //!< SSIO mask no offset
#define MML_SPI_MR_SSLx_MASK					( SPIn_MOD_SSL1_Msk |\
													SPIn_MOD_SSL2_Msk |\
													SPIn_MOD_SSL3_Msk )
#define MML_SPI_MR_NUMBITS_MASK_NOOFST			0xf //!< NUMBITS mask no offset
#define MML_SPI_MR_NUMBITS_MASK					( MML_SPI_MR_NUMBITS_MASK_NOOFST << MML_SPI_MR_NUMBITS_OFST ) //!< NUMBITS mask
#define MML_SPI_DMAR_TX_FIFO_LVL_MASK_NOOFST	0x3 //!< TX_FIFO_LEVEL mask no offset
#define MML_SPI_DMAR_TX_FIFO_CNT_MASK_NOOFST	0x7 //!< TX_FIFO_CNT mask no offset
#define MML_SPI_DMAR_RX_FIFO_LVL_MASK_NOOFST	0x3 //!< RX_FIFO_LEVEL mask no offset
#define MML_SPI_DMAR_RX_FIFO_CNT_MASK_NOOFST	0x7 //!< RX_FIFO_CNT mask no offset

/* GPIO port and pin for SPI0 */
#define MISO0								GPIO(0,16) /**< MISO pin for SPI0 */
#define MOSI0								GPIO(0,17) /**< MOSI pin for SPI0 */
#define SCLK0								GPIO(0,18) /**< SCLK pin for SPI0 */
#define SSEL0_0								GPIO(0,19) /**< SSEL0 pin for SPI0 */
#define SSEL0_1								GPIO(0,20) /**< SSEL1 pin for SPI0 */
#define SSEL0_2								GPIO(0,21) /**< SSEL2 pin for SPI0 */
#define SSEL0_3								GPIO(0,22) /**< SSEL3 pin for SPI0 */

/* GPIO port and pin for SPI1 */
#define MISO1								GPIO(0,25) /**< MISO pin for SPI1 */
#define MOSI1								GPIO(0,26) /**< MOSI pin for SPI1 */
#define SCLK1								GPIO(0,27) /**< SCLK pin for SPI1 */
#define SSEL1_0								GPIO(0,28) /**< SSEL0 pin for SPI1 */
#define SSEL1_1								GPIO(0,29) /**< SSEL1 pin for SPI1 */
#define SSEL1_2								GPIO(0,30) /**< SSEL2 pin for SPI1 */
#define SSEL1_3								GPIO(0,31) /**< SSEL3 pin for SPI1 */

/* GPIO port and pin for SPI2 */
#define MISO2								GPIO(1,2)  /**< MISO pin for SPI2 */
#define MOSI2								GPIO(1,5)  /**< MOSI pin for SPI2 */
#define SCLK2								GPIO(1,7)  /**< SCLK pin for SPI2 */
#define SSEL2_0								GPIO(1,3)  /**< SSEL0 pin for SPI2 */
#define SSEL2_1								GPIO(1,6)  /**< SSEL1 pin for SPI2 */
#define SSEL2_2								GPIO(1,4)  /**< SSEL2 pin for SPI2 */
#define SSEL2_3								GPIO(0,14) /**< SSEL3 pin for SPI2 */

/* Macros *********************************************************************/
#define	M_MML_SPI_ENABLE(_devnum_)			mml_spi_ioctl(_devnum_, MML_SPI_IOCTL_ENABLE, NULL)
#define	M_MML_SPI_DISABLE(_devnum_)		mml_spi_ioctl(_devnum_, MML_SPI_IOCTL_DISABLE, NULL)

/* Enumerations ***************************************************************/
typedef enum {
	MML_SPI_DEV_MIN = 0, MML_SPI_DEV0 = MML_SPI_DEV_MIN, /**< SPI0 */
	MML_SPI_DEV1, /**< SPI1 */
	MML_SPI_DEV2, /**< SPI2 */
	MML_SPI_DEV_MAX = MML_SPI_DEV2, MML_SPI_DEV_COUNT
} mml_spi_dev_t;

/**
 * The control enumeration defines the type of control on the SPI interface
 */
typedef enum {
	MML_SPI_IOCTL_ENABLE, /**< Enable the SPI module */
	MML_SPI_IOCTL_DISABLE, /**< Disable the SPI module */
	MML_SPI_IOCTL_MODE_MASTER, /**< Master mode selected */
	MML_SPI_IOCTL_MODE_SLAVE, /**< Slave mode selected */
	MML_SPI_IOCTL_OPEN_DRAIN_ENABLE, /**< SPI pins not configured for open drain */
	MML_SPI_IOCTL_OPEN_DRAIN_DISABLE, /**< SPI pins configured for open drain */
	MML_SPI_IOCTL_RESET_TX_FIFO, /**< Reset the transmit FIFO */
	MML_SPI_IOCTL_RESET_RX_FIFO, /**< Reset the receive FIFO */
	MML_SPI_IOCTL_CLKPOL_HIGH, /**< Clock idles low */
	MML_SPI_IOCTL_CLKPOL_LOW, /**< CLock idles high */
	MML_SPI_IOTCL_PHASE_HIGH, /**< Received/Transmit data on the rising/falling edge of the clock */
	MML_SPI_IOCTL_PHASE_LOW, /**< Received/transmitted data on the rising/falling edge of the clock */
	MML_SPI_IOCTL_RESET_ERROR, /**< Reset the error */
	MML_SPI_IOCTL_SSV, /**< Slave Select Value */
	MML_SPI_IOCTL_SSIO, /**< Slave Select IO */
	MML_SPI_IOCTL_SSLx /**< Slave Select 1, 2 & 3 - Master mode only */
} mml_spi_config_t;

/* Structures *****************************************************************/
/**
 * SPI configuration information
 */
typedef struct {
	SPIn_DMA_REG_DMA_EN_Type active; /**< DMA request */
	SPIn_DMA_REG_FIFO_LEVEL_Type lvl; /**< Transmit/Receive FIFO level */
	SPIn_DMA_REG_FIFO_CNT_Type cnt; /**< Transmit/Receive FIFO count*/
} mml_spi_dma_conf_t;

/**
 *  SPI configuration information
 */
typedef struct {
	unsigned int baudrate; /**< Baud rate */
	unsigned int ssel; /**< IO configuration:
	 	 	 	 	 	* bit0 - SSELx0
	 	 	 	 	 	* bit1 - SSELx1
	 	 	 	 	 	* bit2 - SSELx2
	 	 	 	 	 	* bit3 - SSELx3
	 	 	 	 	 	* bit4 - bit31 should be null
	 	 	 	 	 	* Set bit to '1' to set it to SSELx
	 	 	 	 	 	* Set bit to '0' to leave it as it is */
	unsigned char word_size; 		/**< Word size */
	SPIn_CNTL_MMEN_Type mode; 		/**< Select the mode of communication */
	SPIn_CNTL_WOR_Type wor; 		/**< SPI Wired OR (open drain) Enable */
	SPIn_CNTL_CLKPOL_Type clk_pol;	/**< SPI Clock polarity */
	SPIn_CNTL_PHASE_Type phase; 	/**< SPI Phase select */
	SPIn_CNTL_BIRQ_Type brg_irq; 	/**< SPI BRG Timer Interrupt Request */
	SPIn_MOD_SSV_Type ssv; 			/**< SPI Slave select value */
	SPIn_MOD_SSIO_Type ssio; 		/**< Slave select pin configuration (input or output) */
	SPIn_MOD_TX_LJ_Type tlj; 		/**< TLJ */
	mml_spi_dma_conf_t dma_tx; 		/**< Structure to configure transmit FIFO level, transmit fifo count */
	mml_spi_dma_conf_t dma_rx; 		/**< Structure to configure receive FIFO level, receive fifo count */
} mml_spi_params_t;

/* Private structures *********************************************************/
/**
 * SPI conf structure
 */
typedef struct {
	unsigned char initialized; 	/**< Device initialized */
	unsigned char ws; 			/**< Word size */
	unsigned short ws_mask; 	/**< Word size mask */
	volatile SPIn_Type *reg; 	/**< Base address */
} mml_spi_conf_t;

/**
 * SPI context structure
 */
typedef struct {
	unsigned char first_init; 				/**< First initialization */
	mml_spi_conf_t port[MML_SPI_DEV_COUNT]; /**< Word size mask */
} mml_spi_context_t;

/**
 * Return the version of the SPI driver.
 * 
 */
const char * mml_spi_get_version(void);

/**
 * The function is used to configure gpio pin of spi
 *
 * @param[in] ssel				IO configuration:
								bit0 - SSELx0
 								bit1 - SSELx1
 								bit2 - SSELx2
 								bit3 - SSELx3
 								bit4 - bit31 should be null
 								Set bit to '1' to set it to SSELx
 								Set bit to '0' to leave it as it is
 * @param[in] miso				Combination of port and pin for miso
 * @param[in] mosi				Combination of port and pin for mosi
 * @param[in] sclk				Combination of port and pin for sclk
 * @param[in] ssel0				Combination of port and pin for ssel0
 * @param[in] ssel1				Combination of port and pin for ssel1
 * @param[in] ssel2				Combination of port and pin for ssel2
 * @param[in] ssel3				Combination of port and pin for ssel3
 * NOTE: miso, mosi, sclk, ssel0, ssel1, ssel2, ssel3 are already defined in spi.h
 * 		 If you want to configure SPI0 you must use MISO0, MOSI0, SCLK0, SSEL0_0, SSEL0_1, SSEL0_2, SSEL0_3
 */
void mml_spi_init_ssel(unsigned int ssel, unsigned int miso, unsigned int mosi,
		unsigned int sclk, unsigned int ssel0, unsigned int ssel1,
		unsigned int ssel2, unsigned int ssel3);

/**
 * The function resets, activates and configures the SPI interface
 *
 * @param[in] devnum						Select the SPI device
 * @param[in] pparams						A pointer on SPI parameters structure allocated and
 * 											filled by the caller.
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					No such device or value is not appropriate
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_NULL_PTR				Parameter passed is NULL pointer
 */
int mml_spi_init(mml_spi_dev_t devnum, mml_spi_params_t *pparams);

/**
 * The function shutdown the SPI device selected
 *
 * @param[in] devnum						Select the SPI device
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_OUT_OF_RANGE			No such device or value is not appropriate
 */
int mml_spi_shutdown(mml_spi_dev_t devnum);

/**
 * The function resets all SPI interface
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_IN_PROGRESS			Reset still in progress
 */
int mml_spi_reset_interface(void);

/**
 * This function is used to configure the communication parameters of SPI Interface
 *
 * @param[in] devnum						Select the SPI device
 * @param[in] pparams						A pointer on SPI configuration structure allocated
 * 											and filled by the caller.
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					At least one parameter is in invalid range
 * @retval COMMON_ERR_NULL_PTR				Pointer is null
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_OUT_OF_RANGE  		No such device or value is not appropriate
 */
int mml_spi_set_config(mml_spi_dev_t devnum, mml_spi_params_t *pparams);

/**
 * The function transmits data on the SPI interface
 *
 * @param[in] devnum						Select the SPI device
 * @param[in] *data							Pointer on the data buffer
 * @param[in] length						Number of bytes in the buffer
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_OUT_OF_RANGE      	No such device or value is not appropriate
 * @retval COMMON_ERR_NOT_INITIALIZED		Module not initialized
 * @retval COMMON_ERR_NULL_PTR				Pointer is null
 */
int mml_spi_transmit(mml_spi_dev_t devnum, unsigned char *data,
		unsigned int length);

/**
 * The function is used to retrieve the status of SPI module
 *
 * @param[in]  devnum					 	Select the SPI device.
 * @param[out] pstatus       			 	A pointer on a variable allocated by the caller
 *                          			 	and filled by the function with the state of SPI
 *                           			 	module. It is a combination of the following
 *                           			 	values :
 *                          			 	\li #MML_SPI_STATE_TX_BUSY
 *                           			 	\li #MML_SPI_STATE_TX_EMPTY
 *                           			 	\li #MML_SPI_STATE_TX_FULL
 *                          			 	\li #MML_SPI_STATE_RX_EMPTY
 *                           			 	\li #MML_SPI_STATE_RX_FULL
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_NULL_PTR				Pointer is null
 * @retval COMMON_ERR_OUT_OF_RANGE      	No such device or value is not appropriate
 * @retval COMMON_ERR_NOT_INITIALIZED		Module not initialized
 *
 */
int mml_spi_get_status(mml_spi_dev_t devnum, unsigned int *pstatus);

/**
 * The function performs a specific control on the SPI Interface.
 *
 * @param[in] devnum						Select the SPI device
 * @param[in] cmd       					The control to be performed.
 * @param[in] p_data 							Pointer on the data buffer *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_UNKNOWN				Unknown error
 * @retval COMMON_ERR_OUT_OF_RANGE      	No such device or value is not appropriate
 * @retval COMMON_ERR_NULL_PTR				Parameter passed is NULL pointer
 * @retval COMMON_ERR_NOT_INITIALIZED		Module not initialized
 */
int mml_spi_ioctl(mml_spi_dev_t devnum, mml_spi_config_t cmd, void* p_data);

/** @} *//* @defgroup MML_SPI */

int mml_spi_read_write(mml_spi_dev_t devnum, unsigned char *data_in,
						unsigned char *data_out, unsigned int length);
#ifdef __cplusplus
}
#endif

#endif /* _MML_SPI_H_ */

/******************************************************************************/
/* EOF */
