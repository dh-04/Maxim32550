/*
 * @file mml_sc.h MML SC Driver Header File
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

#ifndef _MML_SC_H_
#define _MML_SC_H_

/* Global includes */
#include <errors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MML_SC Smart Card Driver
 *
 * @ingroup MML_Drivers
 *
 * @{
 */

/* Defines ********************************************************************/
#define xstr(s) str(s)
#define str(s) #s
#define MML_SC_VERS_MAJOR	2
#define MML_SC_VERS_MINOR	0
#define MML_SC_VERS_PATCH	1
#define MML_SC_VERSION_STRING	"v" xstr(MML_SC_VERS_MAJOR) "." xstr(MML_SC_VERS_MINOR) "." xstr(MML_SC_VERS_PATCH)

/* Enumerations ***************************************************************/
/** Smart card device identification numbers (id's) */
typedef enum {
	MML_SC_DEV_MIN = 0, MML_SC_DEV0 = MML_SC_DEV_MIN, /**< Smart Card 0 */
	MML_SC_DEV_MAX = MML_SC_DEV0,
	MML_SC_DEV_COUNT /**< Number of smart card devices */
} mml_sc_id_t;

/* Structures *****************************************************************/
/** This structure contains information about smart card */
typedef struct {
	SC_Type *reg_sc; /**< Timer physical base address */
} mml_sc_info_t;

/** Structure holds the smart card device context information */
typedef struct {
	unsigned char first_init; /**< First initialization */
	mml_sc_info_t sc[MML_SC_DEV_COUNT]; /**< Bunch of timers */
} mml_sc_context_t;

/**
 * This function is used to initialize smart card
 * @param[in] id				Smart card device to initialize
 * @retval NO_ERROR				No error
 * @retval COMMON_ERR_INVAL		Input parameter id is in invalid range
 */
int mml_sc_init(mml_sc_id_t id);

/**
 * Return the version of the SC driver.
 *
 */
const char * mml_sc_get_version(void);

/** @} *//* @defgroup MML_SC */

#ifdef __cplusplus
}
#endif

#endif /* _MML_SC_H_ */

/******************************************************************************/
/* EOF */
