/*
 * @file mml_gcr.h MML GCR Driver Header File
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

#ifndef _MML_GCR_H_
#define _MML_GCR_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_GCR GCR Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

/* Defines ********************************************************************/
#define xstr(s) str(s)
#define str(s) #s
#define MML_GCR_VERS_MAJOR	3
#define MML_GCR_VERS_MINOR	0
#define MML_GCR_VERS_PATCH	4
#define MML_GCR_VERSION_STRING	"v" xstr(MML_GCR_VERS_MAJOR) "." xstr(MML_GCR_VERS_MINOR) "." xstr(MML_GCR_VERS_PATCH)

/** RSTR devices
 *	Value to set in RSTR register in order to reset the corresponding device
 */
typedef enum {
	/* Device of RSTR Register */
	MML_RSTR_DEV_MIN = 0,
	MML_RSTR_DEV_DMA = MML_RSTR_DEV_MIN, /**< DMA */
	MML_RSTR_DEV_WDT, /**< Watchdog */
	MML_RSTR_DEV_GPIO0, /**< GPIO0 */
	MML_RSTR_DEV_GPIO1, /**< GPIO1 */
	MML_RSTR_DEV_GPIO2, /**< GPIO2 */
	MML_RSTR_DEV_TIMER0, /**< TIMER0 */
	MML_RSTR_DEV_TIMER1, /**< TIMER1 */
	MML_RSTR_DEV_TIMER2, /**< TIMER2 */
	MML_RSTR_DEV_TIMER3, /**< TIMER3 */
	MML_RSTR_DEV_TIMER4, /**< TIMER4 */
	MML_RSTR_DEV_TIMER5, /**< TIMER5 */
	MML_RSTR_DEV_UART0, /**< UART0 */
	MML_RSTR_DEV_UART1, /**< UART1 */
	MML_RSTR_DEV_SPI0, /**< SPI0 */
	MML_RSTR_DEV_SPI1, /**< SPI1 */
	MML_RSTR_DEV_SPI2, /**< SPI2 */
	MML_RSTR_DEV_I2C0, /**< I2C0 */
	MML_RSTR_DEV_RTC, /**< RTC */
	MML_RSTR_DEV_CRYPTO, /**< CRYPTO */
	MML_RSTR_DEV_MAGDSP, /**< Magnetic stripe reader */
	MML_RSTR_DEV_SC0, /**< Smart card 0 */
	MML_RSTR_DEV_KBD, /**< Keyboard */
	MML_RSTR_DEV_MLCD, /**< MLCD */
	MML_RSTR_DEV_USB_BD, /**< USB_BD */
	MML_RSTR_DEV_ADC = 25, /**< ADC */
	MML_RSTR_DEV_DAC, /**< DAC */
	MML_RSTR_DEV_SRST = 29, /**< Soft reset */
	MML_RSTR_DEV_PRST, /**< Peripheral reset */
	MML_RSTR_DEV_ALL, /**< System reset */
	MML_RSTR_DEV_MAX = MML_RSTR_DEV_ALL,
	MML_RSTR_DEV_COUNT
} mml_rstr_dev_t;

/** Peripheral Clock Disable Register devices
 *	Value to set in PERCKCN register in order to disable clock of the corresponding device
 */
typedef enum {
	MML_PERCKCN_DEV_MIN = 0, //!< MML_PERCKCN_DEV_MIN
	MML_PERCKCN_DEV_GPIO0 = MML_PERCKCN_DEV_MIN, //! Disables the GPIO0 clock.
	MML_PERCKCN_DEV_GPIO1, //! Disables the GPIO1 clock.
	MML_PERCKCN_DEV_GPIO2, //! Disables the GPIO2 clock.
	MML_PERCKCN_DEV_USB, //! Disables the USB clock.
	MML_PERCKCN_DEV_CLCDD, //! Disables the CLCDD clock.
	MML_PERCKCN_DEV_DMA, //! Disables the DMA clock.
	MML_PERCKCN_DEV_SPI0, //! Disables the SPI0 clock.
	MML_PERCKCN_DEV_SPI1, //! Disables the SPI1 clock.
	MML_PERCKCN_DEV_SPI2, //! Disables the SPI2 clock.
	MML_PERCKCN_DEV_UART0, //! Disables the UART0 clock.
	MML_PERCKCN_DEV_UART1, //! Disables the UART1 clock.
	MML_PERCKCN_DEV_MAGDSP, //! Disables the MAGDSP clock.
	MML_PERCKCN_DEV_SC, //! Disables the SC clock.
	MML_PERCKCN_DEV_I2C, //! Disables the I2C clock.
	MML_PERCKCN_DEV_CRYPTO, //! Disables the Crypto clock.
	MML_PERCKCN_DEV_T0, //! Disables the Timer 0 clock.
	MML_PERCKCN_DEV_T1, //! Disables the Timer 1 clock.
	MML_PERCKCN_DEV_T2, //! Disables the Timer 2 clock.
	MML_PERCKCN_DEV_T3, //! Disables the Timer 3 clock.
	MML_PERCKCN_DEV_T4, //! Disables the Timer 4 clock.
	MML_PERCKCN_DEV_T5, //! Disables the Timer 5 clock.
	MML_PERCKCN_DEV_MLCD, //! Disables the MLCD clock.
	MML_PERCKCN_DEV_KBD, //! Disables the KBD clock.
	MML_PERCKCN_DEV_ADC, //! Disables the ADC clock.
	MML_PERCKCN_DEV_DAC, //! Disables the DAC clock.
	MML_PERCKCN_DEV_MAX = MML_PERCKCN_DEV_DAC, //!< MML_PERCKCN_DEV_MAX
	MML_PERCKCN_DEV_COUNT //!< MML_PERCKCN_DEV_COUNT
} mml_perckcn_dev_t;

/** Smart Card Clock Frequencies
 *
 */
typedef enum {
	MML_GCR_SCFREQ_96MHZ = 0x1,
	MML_GCR_SCFREQ_48MHZ,
	MML_GCR_SCFREQ_32MHZ,
	MML_GCR_SCFREQ_24MHZ,
	MML_GCR_SCFREQ_19_2MHZ,
	MML_GCR_SCFREQ_16MHZ,
	MML_GCR_SCFREQ_3MHZ		= 32,
	MML_GCR_SCFREQ_1_5MHZ	= 64
} mml_gcr_sc_freq_t;

/** Memory Zeroise device
 *
 */
typedef enum {
	MML_GCR_MEMZ_SRAM0 = 0x0, /**< System RAM block 0 */
	MML_GCR_MEMZ_SRAM1, /**< System RAM block 1 */
	MML_GCR_MEMZ_ICACHE = 0x3, /**< Instruction Cache */
	MML_GCR_MEMZ_CRYPTO = 0x5, /**< Crypto (MAA) Memory */
	MML_GCR_MEMZ_SRAM2 /**< System RAM block 2 */
} mml_gcr_memz_t;

/**
 * Return the version of the GCR driver.
 * 
 */
const char * mml_gcr_get_version(void);

/** Reset devices from RSTR0.
 *
 * @param[in] devs					Device id to reset
 *
 */
void mml_gcr_reset(unsigned int devs);

/** Reset devices from RSTR1.
 *
 * @param[in] devs					Device id to reset
 *
 */
void mml_gcr_reset1(unsigned int devs);

/** Reset devices from RSTR0 or RSTR1.
 *
 * @param[in] dev					Device id to reset
 *
 */
void mml_gcr_dev_reset(mml_rstr_dev_t dev);

/** Enable device clock
 *
 * @param[in] dev					Devices id to enable
 * 									Example: if you want to disable GPIO1D and USBD
 * 											 dev must be equal to dev = 0x0A
 *
 */
void mml_gcr_enable_clk_dev(unsigned int dev);

/** Disable device clock
 *
 * @param[in] dev					Devices id to disable
 * 									Example: if you want to disable GPIO0D and USBD
 * 											 dev must be equal to dev = 0x09
 *
 */
void mml_gcr_disable_clk_dev(unsigned int dev);

/** Enable device STANDBY mode
 *
 * @param[in] dev					Device id to enable standby mode
 * 									Example: if you want to enable standby mode of OSCPD and PLL01PD
 * 											 value of dev must be equal to dev = 0x3000
 *
 */
void mml_gcr_enable_in_stdbmode(unsigned int dev);

/** Disable device STANDBY mode
 *
 * @param[in] dev					Device id to disable standby mode
 * 									Example: if you want to disable standby mode of OSCPD and COPD
 * 											 value of dev must be equal to dev = 0x5000
 *
 */
void mml_gcr_disable_in_stdbmode(unsigned int dev);

/**
 * This function is used to select source for the system clock
 * @param[in] clksrc				Source to select
 */
void mml_gcr_set_clk_src(GCR_CLKCN_CLKSEL_Type clksrc);

/** Set System Frequency.
 *
 * @param[in] freq					Frequency to set
 * @retval NO_ERROR					No Error
 * @retval COMMON_ERR_INVAL			Value is not appropriate
 * @retval COMMON_ERR_UNKNOWN		Generic error for unknown behavior
 */
int mml_set_system_frequency(mml_system_frequency freq);

/** Get System Frequency.
 *
 * @param[out] p_freq				Pointer to return frequency
 * @retval NO_ERROR					No Error
 * @retval Pointer is null			Pointer is null
 * @retval COMMON_ERR_UNKNOWN		Generic error for unknown behavior
 * @retval COMMON_ERR_OUT_OF_RANGE	Freq is out of expected range
 *
 */
int mml_get_system_frequency(unsigned int *p_freq);

/** Get AHB Frequency.
 *
 * @param[out] p_freq				Pointer to return frequency
 * @retval NO_ERROR					No Error
 * @retval Pointer is null			Pointer is null
 * @retval COMMON_ERR_UNKNOWN		Generic error for unknown behavior
 * @retval COMMON_ERR_OUT_OF_RANGE	Value is out of expected range
 *
 */
int mml_get_ahb_frequency(unsigned int *p_freq);

/** Get APB Frequency.
 *
 * @param p_freq					Pointer to return frequency
 * @retval NO_ERROR					No Error
 * @retval COMMON_ERR_UNKNOWN		Generic error for unknown behavior
 * @retval Pointer is null			Pointer is null
 * @retval COMMON_ERR_OUT_OF_RANGE	Value is out of expected range
 *
 */
int mml_get_apb_frequency(unsigned int *p_freq);

/** Flip internal Flash Memory Addressing.
 * Bottom half mapped to logical top half and vice versa
 */
void mml_gcr_flip_iflash(void);

/**UnFlip internal Flash Memory Addressing.
 * Physical Layout matches Logical layout
 *
 */
void mml_gcr_unflip_iflash(void);

/**
 * Get the flip status of the internal Flash
 * @retval 0						Physical Layout matches Logical layout
 * @retval 1						Bottom half mapped to logical top half and vice versa
 *
 */
unsigned char mml_gcr_get_flip_state(void);

/**
 * Initiate the zeroize operation for the selected memory
 * @param[in] mem_to_zeroize		Memory Id to zeroize
 */
void mml_gcr_zeroize_memory(mml_gcr_memz_t mem_to_zeroize);

/**
 * Enable Boundary TAP Port
 */
void mml_gcr_enable_BSTAP(void);

/**
 * Disable Boundary Scan TAP Port
 */
void mml_gcr_disable_BSTAP(void);

/**
 * Enable CM3 Power Management
 */
void mml_gcr_enable_CM3_pm(void);

/**
 * Disable CM3 Power Management
 */
void mml_gcr_disable_CM3_pm(void);

/**
 * Compute and verify ROM Checksum
 * @retval 0						Pass
 * @retval 1 						Fail
 */
unsigned char mml_gcr_checkROM_checksum(void);

/**
 * Get Battery Area power supply source
 * @retval 0						VBAT is selected to supply battery Area
 * @retval 1						VMAIN is selected to supply battery Area
 */
unsigned char mml_gcr_get_battery_source(void);

/**
 * Enable wakeup source
 * @param[in] dev		 			Wake-up source to enable
 * 									Example: if you want to enable GPIO and RTC as wake up source
 * 											 dev must be equal to dev = 0x30
 */
void mml_gcr_enable_wakeup_src(unsigned int dev);

/**
 * Disable wakeup source
 * @param dev		 				Wake-up source to disable
 * 									Example: if you want to disable GPIO and USB as wake up source
 * 											 dev must be equal to dev = 0x50
 */
void mml_gcr_disable_wakeup_src(unsigned int dev);

/**
 * Enable PLL0N
 */
void mml_gcr_enable_pll0(void);

/**
 * Enable PLL1N
 */
void mml_gcr_enable_pll1(void);

/**
 * Disable PLL1N
 */
void mml_gcr_disable_pll1(void);

/**
 * Disable PLL0N
 */
void mml_gcr_disable_pll0(void);

/**
 * Get CM3WICK acknowledgement
 * @retval 						Value of the CM3WICACK bit of PM register
 */
unsigned char mml_gcr_get_CM3WIC_ack(void);

/**
 * Set frequency for the Smart Card Clock
 * @param[in] freq						Frequency
 */
void mml_gcr_set_sc_clk(mml_gcr_sc_freq_t freq);


/** Macros ********************************************************************/
#define	M_MML_GCR_MODE_SHUTDOWN()				( GCR->PM |= 0x3 )
#define	M_MML_GCR_MODE_STANDBY()				( GCR->PM |= 0x2 )
#define	M_MML_GCR_MODE_IDLE()					( GCR->PM |= 0x1 )
#define	M_MML_GCR_MODE_ACTIVE()					( GCR->PM |= 0x0 )
#define	M_MML_GCR_RST_SOFT()					( GCR->RSTR |= ( 1 << MML_RSTR_DEV_SRST ) )
#define	M_MML_GCR_RST_PERIPHERAL()				( GCR->RSTR |= ( 1 << MML_RSTR_DEV_PRST ) )
#define	M_MML_GCR_SYSTEM()						( GCR->RSTR |= ( 1 << MML_RSTR_DEV_ALL  ) )

/** @} *//* @defgroup MML_GCR */

#ifdef __cplusplus
}
#endif

#endif /* _MML_GCR_H_ */

/*****************************************************************************/
/* EOF */

