/*
 * @file mml_trng.h MML TRNG Driver Header File
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
 ****
 */

#ifndef _MML_TRNG_H_
#define _MML_TRNG_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_TRNG TRNG Driver (True Random Number Generator)
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_TRNG_VERS_MAJOR	3
#define MML_TRNG_VERS_MINOR	0
#define MML_TRNG_VERS_PATCH	2
#define MML_TRNG_VERSION_STRING	"v" xstr(MML_TRNG_VERS_MAJOR) "." xstr(MML_TRNG_VERS_MINOR) "." xstr(MML_TRNG_VERS_PATCH)

#define MML_TRNG_BASE_ERR COBRA_TRNG_BASE_ERR

typedef enum {
	TRNG_ERROR_READ_NOT_READY = MML_TRNG_BASE_ERR /**< Data is not ready to be read */
} mml_trng_error;


/**
 * Return the version of the TRNG driver.
 * 
 */
const char * mml_trng_get_version( void );

/**
 *This is the initialize function
 *@retval NO_ERROR					No error
 */
int mml_trng_init(void);

/**
 * This is the read function used to read TRNG_DATA register
 * @param[out] *data_reg				Pointer on the return value of TRNG_DATA register
 * @retval NO_ERROR  					No error
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 * @retval TRNG_ERROR_READ_NOT_READY	Data is not ready to be read
 */
int mml_trng_read_trng_data(unsigned int *data_reg);

/**
 * This function is used to configure interrupt
 * @param[in] config					Configuration to set
 * @retval NO_ERROR						No error
 * @retval COMMON_ERR_INVAL				Config is in invalid range
 * @retval COMMON_ERR_UNKNOWN			Unknown error
 */

int mml_trng_configure_interrupt(TRNG_CN_RNG_IE_Type config);

/**
 * This function is used to read control register
 * @return					The value of TRNG_CN register
 */
uint32_t mml_trng_read_cn(void);

/**
 * This function is used to initiate AES key generation
 */
void mml_trng_initiate_aes_key_generation(void);

/** @} *//* @defgroup MML_TRNG */

#ifdef __cplusplus
}
#endif

#endif /* _MML_TRNG_H_ */

/******************************************************************************/
/* EOF */
