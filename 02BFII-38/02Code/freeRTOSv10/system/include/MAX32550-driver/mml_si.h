/*
 * @file mml_si.h MML SI Driver Header File
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

#ifndef _MML_SI_H_
#define _MML_SI_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_SI SI Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

#define xstr(s) str(s)
#define str(s) #s
#define MML_SI_VERS_MAJOR	2
#define MML_SI_VERS_MINOR	0
#define MML_SI_VERS_PATCH	1
#define MML_SI_VERSION_STRING	"v" xstr(MML_SI_VERS_MAJOR) "." xstr(MML_SI_VERS_MINOR) "." xstr(MML_SI_VERS_PATCH)

/**
 * Return the version of the SI driver.
 * 
 */
const char * mml_si_get_version(void);

/** Return if the MAGIC word is set or not.
 * @retval 0 : Magic word was not set - OTP has not been initialized properly
 * @retval 1 : Magic word was set - OTP contains valid settings
 */
uint32_t mml_si_get_MAGIC(void);

/** Return if CRCERR word is set or not .
 * @retval 0 : No CRC errors occured during the read of the OTP memory block
 * @retval 1 : A CRC error occured while reading the OTP
 *
 */
uint32_t mml_si_get_CRCERR(void);

/** Return ERRADDR register value when CRCERR equals 1.
 * @retval return the failing address in OTP memory
 */
uint32_t mml_si_get_ERRADDR(void);

/** @} *//* @defgroup MML_SI */

#ifdef __cplusplus
}
#endif

#endif /* _MML_SI_H_ */

/*****************************************************************************/
/* EOF */
