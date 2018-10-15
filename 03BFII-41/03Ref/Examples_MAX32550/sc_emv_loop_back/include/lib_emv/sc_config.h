/*
 * @file sc_config.h
 *
 * Smartcard driver configuration file
 *
 ******************************************************************************
 * Copyright (C) 2015-2017 Maxim Integrated Products, Inc., All rights Reserved.
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

/** @defgroup DRIVER_CONFIGURATION    Smartcard driver configuration file
 *
 * @ingroup SMARTCARD_DRIVER
 *
 *
 * @{
 */

//#define __MAX32550				/**< uncomment this definition to use with the MAX32550 */

/** @def    MAX32550_B1
 *  @brief  MAX32550-B1 MCU type
 */
//#define MAX32550_B1           /**< uncomment this definition to use with the MAX32550 B1 */
#ifdef MAX32550_B1
# undef __MAX32550              /* undefine the Compiler defined target (this apply only to the smartcard stack)*/
#endif

//#define __MAX32552				/**< uncomment this definition to use with the MAX32552 */

//#define __MAX32555				/**< uncomment this definition to use with the MAX32555 */

//#define __MAX32560				/**< uncomment this definition to use with the MAX32560 */

//#define __MAX32565				/**< uncomment this definition to use with the MAX32565 */

/** @def    CARD_PRESENCE_ON_LOGIC_LOW
 *  @brief  When set, a Low logic level on PRES is interpreted as a card present
 */
//#define CARD_PRESENCE_ON_LOGIC_LOW

#ifdef CARD_PRESENCE_ON_LOGIC_LOW
#if defined(__MAX32550) || defined(MAX32550_B1) || defined(MAX32555)
#define CARD_PRESENCE_ON_LOGIC_LOW_SC0
#elif defined (__MAX32552) || defined (__MAX32560) || defined (__MAX32565)
//#define CARD_PRESENCE_ON_LOGIC_LOW_SC0 /**< uncomment this definition to use Smart Card interface 0 on logic low */
//#define CARD_PRESENCE_ON_LOGIC_LOW_SC1 /**< uncomment this definition to use Smart Card interface 1 on logic low */
#if !(defined(CARD_PRESENCE_ON_LOGIC_LOW_SC0)) && (!defined(CARD_PRESENCE_ON_LOGIC_LOW_SC1))
#warning "You defined CARD_PRESENCE_ON_LOGIC_LOW but CARD_PRESENCE_ON_LOGIC_LOW_SC0 or CARD_PRESENCE_ON_LOGIC_LOW_SC1 isn't defined"
#endif
#endif
#endif //CARD_PRESENCE_ON_LOGIC_LOW


/** @def    BAREMETAL_EMV_LOOPBACK
 *  @brief  When set, We run the EMV loopback application (from the main.c)
 */
#ifdef BAREMETAL_EMV_LOOPBACK
# undef USE_FREERTOS
#endif

/** @def        USE_C4C8_FOR_SMARTCARD
 *  @brief      Enable C4/C8 for the smartcard interface
 *
 *
 * By defining USE_C4C8_FOR_SMARTCARD, the C4/C8 pin will be use by the smartcard interface.
 */
//#define USE_C4C8_FOR_SMARTCARD    /**< uncomment this definition to use the MAX32552 with an additional SAM*/

/** @def        HIGHLEVEL_FOR_C4C8
 *  @brief      select high level for C4/C8
 *
 *
 * By defining HIGHLEVEL_FOR_C4C8, the C4/C8 has configured to high level.
 */
//#define HIGHLEVEL_FOR_C4C8    /**< uncomment this definition to use the MAX32552 with an additional SAM*/


/** @def    CETECOM_FIME_EMVCO_1CF_126_0y_COMPLIANCE
 *  @brief  avoid to send a IFS(Request) after a resynch
 *
 * for CETECOM and FIME Labs,
 * if we received 3 wrong answers to a IFS(request),
 * we do a resynch, then we restart with the select
 * (ie not with a new IFS(Request)\n
 *
 * Note: this is not specified by the EMV, for the
 * EMVCo ref. 1CF.126.0y test, the pass criteria is to
 * receive a S(Resynch) request.
 */
 #define CETECOM_FIME_EMVCO_1CF_126_0y_COMPLIANCE

#if defined(__MAX32550) || defined(MAX32550_B1)
#define CHIP MAX32550
#define MAX325XX_H_PATH "../system/include/cmsis/Maxim/MAX325xx.h"
#define GCR_H_PATH "../system/include/MAX32550-driver/mml_gcr.h"
#define INTC_H_PATH "../system/include/MAX32550-driver/mml_intc.h"
#define SC_H_PATH "../system/include/MAX32550-driver/mml_sc.h"
#elif defined(__MAX32552)
#define CHIP MAX32552
#define MAX325XX_H_PATH "MAX325xx.h"
#define GCR_H_PATH "../system/include/MAX32552-driver/mml_gcr.h"
#define INTC_H_PATH "../system/include/MAX32552-driver/mml_intc.h"
#define SC_H_PATH "../system/include/MAX32552-driver/mml_sc.h"
#elif defined (__MAX32555)
#define CHIP MAX32555
#define MAX325XX_H_PATH "../system//include/cmsis/Maxim/MAX325xx.h"
#define GCR_H_PATH "../system/include/MAX32555-driver/mml_gcr.h"
#define INTC_H_PATH "../system/include/MAX32555-driver/mml_intc.h"
#define SC_H_PATH "../system/include/MAX32555-driver/mml_sc.h"
#elif defined (__MAX32560)
#define CHIP MAX32560
#define MAX325XX_H_PATH "../system/include/cmsis/Maxim/MAX325xx.h"
#define GCR_H_PATH "../system/include/MAX32560-driver/mml_gcr.h"
#define INTC_H_PATH "../system/include/MAX32560-driver/mml_intc.h"
#define SC_H_PATH "../system/include/MAX32560-driver/mml_sc.h"
#elif defined (__MAX32565)
#define CHIP MAX32565
#define MAX325XX_H_PATH "../system/include/cmsis/Maxim/MAX325xx.h"
#define GCR_H_PATH "../system/include/MAX32565-driver/mml_gcr.h"
#define INTC_H_PATH "../system/include/MAX32565-driver/mml_intc.h"
#define SC_H_PATH "../system/include/MAX32565-driver/mml_sc.h"
#else
#error	"Unkown chip. (you must define either __MAX32550, MAX32550_B1, __MAX32555, __MAX32552, __MAX32560 or __MAX32565)"
#endif


 /** @} */ /*defgroup*/
/** @} */ /*file*/
