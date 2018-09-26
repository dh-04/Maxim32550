/*
 * @file mml_icc.c MML ICC Driver C File
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

#include <MAX325xx.h>

#ifdef _MML_ICC_H_

unsigned int mml_icc_get_cache_size(void) {
	return ((ICC->MEMCFG & ICC_MEMCFG_CCHSZ_Msk) >> ICC_MEMCFG_CCHSZ_Pos);
}

unsigned int mml_icc_get_main_memory_size(void) {
	return ((ICC->MEMCFG & ICC_MEMCFG_MEMSZ_Msk) >> ICC_MEMCFG_MEMSZ_Pos);
}

void mml_icc_enable_cache(void) {
	ICC->CACHE_CTRL |= ICC_CACHE_CTRL_CACHE_EN_Msk;
}

void mml_icc_disable_cache(void) {
	ICC->CACHE_CTRL &= ~(ICC_CACHE_CTRL_CACHE_EN_Msk);
}

void mml_get_icc_cn(unsigned int *status) {
	*status = ICC->CACHE_CTRL;
}

const char * mml_icc_get_version(void) {
	return MML_ICC_VERSION_STRING;
}

#endif //_MML_ICC_H_

/******************************************************************************/
/* EOF */

