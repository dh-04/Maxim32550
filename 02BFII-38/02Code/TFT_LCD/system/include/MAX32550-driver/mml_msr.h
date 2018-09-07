/*
 * @file mml_msr.h MML MSR Driver Header File
 * @brief    Magnetic Stripe Reader driver
 * @details  This driver can be used to configure and operate the Magnetic Stripe
 *           Reader. It reads and decodes magnetic stripe data that is encoded
 *           according to the ISO/IEC standard 7811.
 * @details  This file defines the driver API including data types and function
 *           prototypes.
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

#ifndef _MML_MSR_H_
#define _MML_MSR_H_

#include <stdint.h>
#include <mml_dma.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_MSR MSR Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_MSR_VERS_MAJOR	3
#define MML_MSR_VERS_MINOR	0
#define MML_MSR_VERS_PATCH	1
#define MML_MSR_VERSION_STRING	"v" xstr(MML_MSR_VERS_MAJOR) "." xstr(MML_MSR_VERS_MINOR) "." xstr(MML_MSR_VERS_PATCH)

/* Definitions */
#ifndef MSR_NUM_TRACKS
/** Number of tracks on magstripe */
#define MSR_NUM_TRACKS      3
#endif /* MSR_NUM_TRACKS */

#define ADC_DSP_FREQ      			(4000000 / 3) /**< ADC frequency to perform magnetic stripe read */
#define MSR_MAX_SAMPLES				1536 /**< Maximum number of samples that can be collected per MML spec */
#define MSR_MAX_RAW_LEN_BITS		(709) /**< Maximum number of bits on a 3.375 inch, 210 bpi magstripe */
#define MSR_MAX_RAW_LEN_BYTES     	((MSR_MAX_RAW_LEN_BITS + 7) / 8) /**< Maximum number of bytes on a 3.375 inch, 210 bpi magstripe */
#define MSR_MAX_DEC_LEN   			(MSR_MAX_RAW_LEN_BITS / 5) /**< Maximum size in bytes of decoded track characters (5-bit min to 8-bit max) */
#define MSR_FORWARD     			0 /**< Swipe direction: Forward */
#define MSR_REVERSE     			1 /**< Swipe direction: Reverse */

/*  ADCCFG1  ***************************************************************  */
#define MML_MSR_ADCCFG1_PUADC_POS     (0)
#define MML_MSR_ADCCFG1_PUADC         (0x00000001UL << MML_MSR_ADCCFG1_PUADC_POS)
#define MML_MSR_ADCCFG1_VREF_SEL_POS  (2)
#define MML_MSR_ADCCFG1_VREF_SEL      (0x00000003UL << MML_MSR_ADCCFG1_VREF_SEL_POS)
#define MML_MSR_ADCCFG1_VREF_SEL_1P0V   (0x00000000UL << MML_MSR_ADCCFG1_VREF_SEL_POS)
#define MML_MSR_ADCCFG1_VREF_SEL_0P8V   (0x00000001UL << MML_MSR_ADCCFG1_VREF_SEL_POS)
#define MML_MSR_ADCCFG1_VREF_SEL_0P6V   (0x00000002UL << MML_MSR_ADCCFG1_VREF_SEL_POS)
#define MML_MSR_ADCCFG1_VREF_SEL_0P5V   (0x00000003UL << MML_MSR_ADCCFG1_VREF_SEL_POS)
#define MML_MSR_ADCCFG1_OFSAZ_POS     (4)
#define MML_MSR_ADCCFG1_OFSAZ         (0x00000007UL << MML_MSR_ADCCFG1_OFSAZ_POS)
#define MML_MSR_ADCCFG1_OFSAZ_GND0    (0x00000001UL << MML_MSR_ADCCFG1_OFSAZ_POS)
#define MML_MSR_ADCCFG1_OFSAZ_GND1    (0x00000002UL << MML_MSR_ADCCFG1_OFSAZ_POS)
#define MML_MSR_ADCCFG1_OFSAZ_GND2    (0x00000004UL << MML_MSR_ADCCFG1_OFSAZ_POS)
#define MML_MSR_ADCCFG1_RCR1_POS      (8)
#define MML_MSR_ADCCFG1_RCR1          (0x00000003UL << MML_MSR_ADCCFG1_RCR1_POS)
#define MML_MSR_ADCCFG1_RCR1_SHORT    (0x00000000UL << MML_MSR_ADCCFG1_RCR1_POS)
#define MML_MSR_ADCCFG1_RCR1_0P5KOHM  (0x00000001UL << MML_MSR_ADCCFG1_RCR1_POS)
#define MML_MSR_ADCCFG1_RCR1_1P0KOHM  (0x00000002UL << MML_MSR_ADCCFG1_RCR1_POS)
#define MML_MSR_ADCCFG1_RCR1_2P0KOHM  (0x00000003UL << MML_MSR_ADCCFG1_RCR1_POS)
#define MML_MSR_ADCCFG1_RCR2_POS      (10)
#define MML_MSR_ADCCFG1_RCR2          (0x00000007UL << MML_MSR_ADCCFG1_RCR2_POS)
#define MML_MSR_ADCCFG1_RCR2_OPEN     (0x00000000UL << MML_MSR_ADCCFG1_RCR2_POS)
#define MML_MSR_ADCCFG1_RCR2_1P5KOHM  (0x00000004UL << MML_MSR_ADCCFG1_RCR2_POS)
#define MML_MSR_ADCCFG1_RCR2_3P0KOHM  (0x00000005UL << MML_MSR_ADCCFG1_RCR2_POS)
#define MML_MSR_ADCCFG1_RCR2_6P0KOHM  (0x00000006UL << MML_MSR_ADCCFG1_RCR2_POS)
#define MML_MSR_ADCCFG1_RCR2_12KOHM   (0x00000007UL << MML_MSR_ADCCFG1_RCR2_POS)
#define MML_MSR_ADCCFG1_EXT_REF_POS   (16)
#define MML_MSR_ADCCFG1_EXT_REF       (0x00000001UL << MML_MSR_ADCCFG1_EXT_REF_POS)
#define MML_MSR_ADCCFG1_INT_REF_POS   (17)
#define MML_MSR_ADCCFG1_INT_REF       (0x00000001UL << MML_MSR_ADCCFG1_INT_REF_POS)

/* Enumerations ***************************************************************/
/** MSR errors list */
typedef enum {
	MML_MSR_ERRORS_MSR_ERR_OK = 0x00, /**< Error Code: No error */
	MML_MSR_ERRORS_MSR_ERR_BAD_LEN = 0x01, /**< Error Code: Invalid length parameter */
	MML_MSR_ERRORS_MSR_ERR_START_SEN = 0x02, /**< Error Code: Start sentinel was not found */
	MML_MSR_ERRORS_MSR_ERR_END_SEN = 0x04, /**< Error Code: End sentinel was not found */
	MML_MSR_ERRORS_MSR_ERR_OUTLIER = 0x08, /**< Error Code: Invalid sample value */
	MML_MSR_ERRORS_MSR_ERR_PARAM = 0x10, /**< Error Code: Invalid parameter */
	MML_MSR_ERRORS_MSR_ERR_LRC = 0x40, /**< Error Code: Invalid LRC (LRC != 0) */
	MML_MSR_ERRORS_MSR_ERR_PARITY = 0x80 /**< Error Code: Parity error */
} mml_msr_errors_t;

/* Structures *****************************************************************/
/** Structure to contain result of a track decode */
typedef struct {
	uint8_t error_code; /**< Error code value */
	uint8_t parity_errs; /**< Number of characters with parity errors */
	uint8_t lrc; /**< LRC check value. A value of '0' indicates a
	 successful LRC check. Any other value should be
	 considered a failure. */
	uint8_t direction; /**< Swipe direction determined from decode */
	uint8_t len; /**< Number or decoded characters. This does not include
	 the sentinels or the LRC. */
	uint16_t speed; /**< Approximate swipe rate in unit of in/sec * 10 */
	uint8_t data[MSR_MAX_DEC_LEN]; /**< The decoded data */
} mml_msr_decoded_track_t;

/** MSR sample fields */
typedef union {
	struct {
		uint16_t time :9;
		uint16_t amp :7;
	};
	uint16_t value;
} msr_sample_t;

/** Structure to contain raw MSR samples */
typedef struct {
	uint16_t len;
	msr_sample_t data[MSR_MAX_SAMPLES];
} msr_samples_t;

/** Track parameters */
typedef struct {
	uint8_t startsen; /**< start sentinel */
	uint8_t endsen; /**< end sentinel */
	uint8_t nbits; /**< # of bits per char */
	uint8_t density; /**< bit density used for swipe rate calculation (Note: Track 2 is 2x normal density) */
	const uint8_t *charset; /**< character set */
} trackpar_t;

typedef enum {
	MML_MSR_DMA_STATUS_NOT_USE, /**< DMA status not use */
	MML_MSR_DMA_STATUS_INIT, /**< DMA status not initialize */
	MML_MSR_DMA_STATUS_TRANSFERED, /**< DMA status transfered done */
	MML_MSR_DMA_STATUS_COMPLETE /**< DMA status  transfered complete */
} mml_msr_dma_status;

/* MSR context */
typedef struct {
	unsigned int first_init; /**< First initialization done ? */
	unsigned char dma_channel; /**< DMA Channel */
	mml_msr_dma_status dma_status; /**< DMA Status */
	mml_dma_config_t dma_config; /**< DMA config */
	mml_dma_addr_t dma_addr; /**< DMA address */
	uint16_t raw_data[MSR_MAX_SAMPLES]; /**< Variable where data read from magnetic stripe reader will be stored */
	unsigned int raw_len; /**< Length of data read*/
	unsigned int raw_idx;
} mml_msr_context_t;

/***** Function Prototypes *****/

/**
 * This functions attempts to remove false peaks from samples
 * @param[in]	samples[]   sample data from DSP
 * @param[in]	len         length of sample[]
 * @param[in]	thr         threshold
 * @param[out]	samplesf[]  sample data with false peaks removed
 * @param[out]	len         length of samplesf[]
 * NOTE: Internal function
 * */
void remove_false_peaks(uint16_t * samples, unsigned int len,
		unsigned int thr, uint16_t * samplesf, unsigned int * lenf);

/**
 * This function parses the raw track bits into characters using ISO/IEC standard 7811
 *   Inputs:
 * @param[in]	bits        raw track bits
 * @param[in]	nbits       number of bits in bits[]
 * @param[in]	trackpar    track-specific parameters
 * @param[in]	direction   parse direction; 0=forward, 1=backwards
 * @param[out]	parsed      parsed characters array
 * @return					number of valid characters parsed, including sentinels and LRC
 * NOTE: Internal function
 */
unsigned int parse_track(uint8_t * bits, unsigned int nbits,
		const trackpar_t * trackpar, unsigned int direction,
		mml_msr_decoded_track_t * parsed);
/**
 * Description: Decodes the samples into the raw track bits
 * @param[in]	samples[]   	sample data from DSP
 * @param[in]	len         	length of sample[]
 * @param[in]	trackpar    	track-specific parameters
 * @param[in]	direction   	decode direction
 *   Outputs:
 * @param[out]	bits        	raw track bits
 * @param[out]	nbits       	number of bits in bits[]
 * @param[out]	speed       	approximate swipe speed
 * @retval		NO_ERROR		no error
 * @retval		MML_MSR_ERRORS_MSR_ERR_BAD_LEN	bad length
 * @retval		MML_MSR_ERRORS_MSR_ERR_OUTLIER	invalid sample value
 * NOTE: Internal function
 */
uint8_t decode_track(uint16_t * samples, unsigned int len,
		const trackpar_t * trackpar, unsigned int direction, uint8_t * bits,
		unsigned int * nbits, unsigned int * speed);

/**
 * Return the version of the MSR driver.
 * 
 */
const char * mml_msr_get_version(void);

/**
 * Configure MSR interrupt
 */
void mml_msr_pre_init(void);

/**
 * Set interrupt priority
 * @param[in] priority						Interrupt priority to set
 *
 * @retval NO_ERROR							No error
 * @retval COMMON_ERR_INVAL					Invalid value for priority
 */
int mml_msr_set_interrupt_priority(mml_intc_prio_t priority);

/**
 *   \brief    Initializes magnetic stripe reader hardware
 */
int mml_msr_init(void);

/**
 *   \brief    Reinitialize internal structure for next swipe
 *   NOTE: This function replace mml_init_track
 */
void mml_msr_restart(void);

/**
 *   \brief    Enables magnetic stripe reader
 *   \pre      The reader should be initialized by calling msr_init() and then
 *             waiting at least 100 us before calling this function.
 */
void mml_msr_enable(void);

/**
 *   \brief    Disables magnetic stripe reader
 */
void mml_msr_disable(void);

/**
 *   \brief		Decodes the specified track of data
 *   \param[in]	track           track number (1 to 3)
 *   \param[in]  decoded_track   track decode results
 *   \returns  	number of characters decoded
 *   \note     	This function has significant stack usage.
 */
unsigned int mml_msr_track_decode(unsigned int track,
		mml_msr_decoded_track_t * decoded_track);

/**
 *   \brief    	Registers an application callback function
 *   \details  	The callback function will be called after completion of the read
 *             	of all enabled tracks
 *   \details  	Unregistering of the callback can be performed by calling this
 *   			function function with a NULL parameter.
 *   \param[in]	func  application callback function
 */
void mml_msr_set_complete_callback(void (*func)(void));

/**
 *   \brief   	Retrieves the raw (undecoded) sample data for the specified track
 *            	of data
 *   \param[in]	track       track number (1 to 3)
 *   \param[in]	samples     pointer to where the sample data will be copied
 *   \returns 	number of samples retrieved
 */
unsigned int mml_msr_get_track_samples(unsigned int track,
		msr_samples_t * samples);

/** @} *//* @defgroup MML_MSR */

#ifdef __cplusplus
}
#endif

#endif /* _MML_MSR_H_ */

/*****************************************************************************/
/* EOF */
