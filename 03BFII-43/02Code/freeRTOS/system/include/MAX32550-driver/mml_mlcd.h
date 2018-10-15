/*
 * @file mml_mcld.h MML MLCD Driver Header File
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

#ifndef _MML_MLCD_H_
#define _MML_MLCD_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_MLCD MLCD Driver ( Mono LCD )
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_MLCD_VERS_MAJOR	2
#define MML_MLCD_VERS_MINOR	0
#define MML_MLCD_VERS_PATCH	3
#define MML_MLCD_VERSION_STRING	"v" xstr(MML_MLCD_VERS_MAJOR) "." xstr(MML_MLCD_VERS_MINOR) "." xstr(MML_MLCD_VERS_PATCH)

/* Enumerations ***************************************************************/

/**
 * Definition of Control register
 */
typedef enum {
	MML_MLCD_CN_REGISTER_TXIE,		/**< Select TXIE bit of CN register */
	MML_MLCD_CN_REGISTER_RXIE,		/**< Select RXIE bit of CN register */
	MML_MLCD_CN_REGISTER_AE,		/**< Select AE bit of CN register */
	MML_MLCD_CN_REGISTER_NIBBLE,	/**< Select NIBBLE bit of CN register */
	MML_MLCD_CN_REGISTER_RS,		/**< Select RS bit of CN register */
	MML_MLCD_CN_REGISTER_RW,		/**< Select RW bit of CN register */
	MML_MLCD_CN_REGISTER_ENB,		/**< Select ENB bit of CN register */
	MML_MLCD_CN_REGISTER_READ,		/**< Select READ bit of CN register */
	MML_MLCD_CN_REGISTER_MIN = MML_MLCD_CN_REGISTER_TXIE,
	MML_MLCD_CN_REGISTER_MAX = MML_MLCD_CN_REGISTER_READ
} mml_lcd_cn_register;

typedef enum {
	MML_MLCD_SR_REGISTER_TXIS, 		/**< Select TXIS bit of SR register */
	MML_MLCD_SR_REGISTER_RXIS, 		/**< Select RXIS bit of SR register */
	MML_MLCD_SR_REGISTER_MIN = MML_MLCD_SR_REGISTER_TXIS,
	MML_MLCD_SR_REGISTER_MAX = MML_MLCD_SR_REGISTER_RXIS
} mml_mlcd_sr_register;

/* Structures *****************************************************************/

/**
 * Control Register Configuration structure
 */
typedef struct {
	uint8_t txie; 			/** Value for TXIE bit (0 or 1) */
	uint8_t rxie; 			/** Value for RXIE bit (0 or 1) */
	uint8_t ae; 			/** Value for AE bit (0 or 1) */
	uint8_t nibble; 		/** Value for NIBBLE bit (0 or 1) */
	uint8_t rs; 			/** Value for RS bit (0 or 1) */
	uint8_t rw; 			/** Value for RW bit (0 or 1) */
	uint8_t enb; 			/** Value for ENB bit (0 or 1) */
} mml_lcd_cn_config;

/**
 * MLCD configuration structure
 */
typedef struct {
	mml_lcd_cn_config cn_config; 	/** Configuration of control register */
	uint8_t epr; 					/** Number of system clock cycles for an access */
} mml_lcd_config;

/**
 * Return the version of the MLCD driver.
 * 
 */
const char * mml_mlcd_get_version(void);

/**
 * This function is used to configure MLCD
 * @param[in] config					Configuration to set
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Niddle bit must be 0 when MLCD is configured in normal mode
 * @retval COMMON_ERR_UNKNOWN			Error unknown
 */
int mml_mlcd_config(mml_lcd_config config);

/**
 * This function is used to configure control register
 * @param[in] config					Configuration to set
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Invalid range for config
 * @retval COMMON_ERR_UNKNOWN			Error unknown
 */
int mml_mlcd_cn_config_f(mml_lcd_cn_config config);

/**
 * This function is used to read control register
 * @param[out] status					Pointer on the value of control register
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_UNKNOWN			Error unknown
 */
int mml_mlcd_read_cn(unsigned int *status);

/**
 * This function is used to configure EPR register
 * @param[in] value						Number of system clock cycles for an access.
 * 										It is twice the asserted time of the enable signal (LCD_EN pin).
 * 										This register has a minimum value of 4. Any values less than 4 will be set to 4
 */
void mml_mlcd_config_epr(unsigned char value);

/**
 * This function is used to read the Enable Period
 * @return the Enable period
 */
unsigned int mml_mlcd_read_epr(void);

/**
 * This function is used to clear flag of SR register
 * @param[in] sr_clear					Bits to clear
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Invalid range for sr_clear
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 */
int mml_mlcd_sr_clear_flag(mml_mlcd_sr_register sr_clear);

/**
 * This function is used to read SR register
 * @return SR register value
 */
unsigned int mml_mlcd_read_sr(void);

/**
 * This function is used to write data in DATA register in automatic mode
 * @param[in] data 						DATA to write in DATA register
 */
void mml_mlcd_write_data_automatic(unsigned char data);
/**
 * This function is used to write command in DATA register in automatic mode
 * @param[in] cmd 						Command to write in DATA register
 */
void mml_mlcd_write_cmd_automatic(unsigned char cmd);
/**
 * This function is used to get data in automatic mode
 * @retval								Return input value of RS pin
 */
unsigned char mml_mlcd_get_data_automatic(void);

/**
 * This function is used to set RS bit in control register
 * param[in] value						Value to set
 */
void mml_mlcd_cn_set_rs(uint32_t value);

/**
 * This function is used to set ENB bit in control register
 * param[in] value						Value to set
 */
void mml_mlcd_cn_set_enb(uint32_t value);

/**
 * This function is used to set RW bit in control register
 * param[in] value						Value to set
 */
void mml_mlcd_cn_set_rw(uint32_t value);

/**
 * This function is used to write data in DATA register in manual mode
 * @param[in] data 						DATA to write in DATA register
 */
void mml_mlcd_write_data_manual(uint32_t data);

/**
 * This function is used to get data in manual mode
 * @retval								Return input value of RS pin
 */
unsigned char mml_mlcd_get_data_manual(void);

/** @} *//* @defgroup MML_MLCD */

#ifdef __cplusplus
}
#endif

#endif /* _MML_MLCD_H_ */

/******************************************************************************/
/* EOF */

