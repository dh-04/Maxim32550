/*
 * @file trng/include/trng.h
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
 * trademarks, Mskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

/* Global includes */
#include <MAX325xx.h>
#include <errors.h>

/** Defines *******************************************************************/
#define TRNG_BASE_ERR						COBRA_TRNG_BASE_ERR
/* TRNG wait loops number for wanted bit to be released */
#define	K_TRNG_MAX_WAIT_LOOP				100000
#define	K_COBRA_USECASE_TRNG_AES_KEY_SIZE	0x10
#define K_AES_KEYS_NB 						3

/* Enumerations */
/* TRNG errors list */
typedef enum {
	N_TRNG_ERR_MIN = TRNG_BASE_ERR,
	N_TRNG_ERR_FAILURE, 									/**< Error Code: TRNG internal failure */
	N_TRNG_ERR_UNKNOWN, N_TRNG_ERR_MAX = N_TRNG_ERR_UNKNOWN	/**< Error Code: Generic error for unknown behavior */
} TRNG_errors_t;

int TRNG_read(unsigned int *p_random, unsigned int length);

int usecase_trng_gen_aes_key(unsigned char *p_pucKey,
		unsigned int p_uiLength);

/* EOF */
