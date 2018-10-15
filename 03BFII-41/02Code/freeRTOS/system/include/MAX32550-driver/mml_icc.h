/*
 * @file mml_icc.h MML ICC Driver Header File
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

#ifndef _MML_ICC_H_
#define _MML_ICC_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_ICC ICC Driver ( Intsruction Cache Controller )
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_ICC_VERS_MAJOR	2
#define MML_ICC_VERS_MINOR	0
#define MML_ICC_VERS_PATCH	1
#define MML_ICC_VERSION_STRING	"v" xstr(MML_ICC_VERS_MAJOR) "." xstr(MML_ICC_VERS_MINOR) "." xstr(MML_ICC_VERS_PATCH)

/**
 * Return the version of the ICC driver.
 * 
 */
const char * mml_icc_get_version(void);

/**
 * This function is used to get cache size.
 * @retval				Return total size in Kbytes of cache
 */
unsigned int mml_icc_get_cache_size(void);

/**
 * This function is used to get the total size
 * @retval				Return total size in units of 128 kbytes, of code memory accessible to the cache controller.
 */
unsigned int mml_icc_get_main_memory_size(void);

/**
 * This function is used to enable cache
 * Changing the state of this bit will cause the instruction cache to be flushed and its contents invalidated
 */
void mml_icc_enable_cache(void);

/**
 * This function is used to disable cache
 * Changing the state of this bit will cause the instruction cache to be flushed and its contents invalidated
 */
void mml_icc_disable_cache(void);

/**
 * This function is used to read ICC cache controller register
 * @param[out] status		Pointer on the return value of ICC cache controller value
 */
void mml_get_icc_cn(unsigned int *status);

/** @} *//* @defgroup MML_ICC */

#ifdef __cplusplus
}
#endif

#endif /* _MML_ICC_H_ */

/******************************************************************************/
/* EOF */
