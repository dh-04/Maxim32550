/*
 * @file trng/src/trng.c
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

#include <trng.h>

int TRNG_read(unsigned int *p_random, unsigned int length) {
	int result = COMMON_ERR_UNKNOWN;
	unsigned int loop = 0;

	/* Input parameters have been checked upper */
	if (TRNG->CN & TRNG_CN_RNG_I4S_Msk) {
		/* Dummy read of previous 128bits */
		/* Read random number */
		for (loop = 0; loop < length; loop++) {
			p_random[loop] = TRNG->DATA;
		}
	}
	/* First clear RNG_I4S bit before asking for a new 128bits TRN */
	TRNG->CN |= TRNG_CN_RNG_ISC_Msk;
	loop = K_TRNG_MAX_WAIT_LOOP;
	/* Only 128bits number is read */
	while (!(TRNG->CN & TRNG_CN_RNG_I4S_Msk) && loop--);
	/* Check why loop ends */
	if (!loop && !(TRNG->CN & TRNG_CN_RNG_I4S_Msk)) {
		result = N_TRNG_ERR_FAILURE;
	} else {
		/* Read random number */
		for (loop = 0; loop < length; loop++) {
			p_random[loop] = TRNG->DATA;
		}
		result = NO_ERROR;
	}
	return result;
}

int usecase_trng_gen_aes_key(unsigned char *p_pucKey,
		unsigned int p_uiLength) {
	int result = COMMON_ERR_UNKNOWN;

	/* Check input parmaters */
	if (!p_pucKey) {
		result = COMMON_ERR_NULL_PTR;
		return result;
	}
	if ( K_COBRA_USECASE_TRNG_AES_KEY_SIZE != p_uiLength) {
		result = COMMON_ERR_INVAL;
		return result;
	}
	/* Enable TRNG */
	result = mml_trng_init();
	if (result) {
		return result;
	}
	/* Generate random number */
	result = TRNG_read((unsigned int*) p_pucKey,
			(p_uiLength / sizeof(unsigned int)));
	return result;
}
