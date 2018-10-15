/*
 * @file MAX325xx_afe_private.h
 *
 * MAX32550, MAX32552, MAX32555, MAX32560 & MAX32565 on-SoC Analog Front End - driver private functions
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

#ifndef _AFE_MAX325xx_PRIVATE_H_
#define _AFE_MAX325xx_PRIVATE_H_

#include <stdint.h>
#include "sc_errors.h"
#include "slot.h"
#include "sc_states.h"
#include "sc_config.h"

/** @defgroup AFE_MAX325xx_PRIVATE Analog Front End driver private functions for the MAX32550, MAX32552, MAX32555, MAX32560 & MAX32565
 *
 * @ingroup ICC_ABSTRACTION
 *
 * This file defines AFE access functions for direct calls from the abstraction layer.\n
 * This is not intended to be directly used by the user.
 *
 * @{
 */
#if defined (__MAX32550)
/** @def  RESET_ACTIVE    Smartcard RST signal active level
 */
#define RESET_ACTIVE                    0 /**< Smartcard Reset is active LOW*/
#elif defined(MAX32550_B1) || defined(__MAX32555)
/** @def  RESET_ACTIVE    Smartcard RST signal active level
 */
#define RESET_ACTIVE                    0 /**< Smartcard Reset is active LOW*/

/** @def MAX325xx_BYP_CLK_PORT         Clock pin (CLK) port for the bypass bus
 */
# define MAX325xx_SC0_BYP_CLK_PORT         MML_GPIO_DEV1   /**< SC_BYP_CLK is on P1.1*/

/** @def MAX325xx_SC0_BYP_CLK              Clock pin (CLK) number for the bypass bus
 */
# define MAX325xx_SC0_BYP_CLK              1                /**< SC_BYP_CLK is on P1.1*/

/** @def MAX325xx_SC0_BYP_IO_PORT          I/O pin (IO) port for the bypass bus
 */
# define MAX325xx_SC0_BYP_IO_PORT          MML_GPIO_DEV0   /**< SC_BYP_IO is on P0.21*/

/** @def MAX325xx_SC0_BYP_IO               I/O pin (IO) number for the bypass bus
 */
# define MAX325xx_SC0_BYP_IO               21               /**< SC_BYP_IO is on P0.21*/

/** @def MAX325xx_SC0_BYP_RST_PORT         RESET pin (RST) port for the bypass bus
 */
# define MAX325xx_SC0_BYP_RST_PORT         MML_GPIO_DEV0   /**< SC_BYP_RST is on P0.22*/

/** @def MAX325xx_SC0_BYP_RST              RESET pin (RST) number for the bypass bus
 */
# define MAX325xx_SC0_BYP_RST              22               /**< SC_BYP_RST is on P0.22*/


/** @def MAX325xx_SC0_BYP_SC_DETECT_PORT   SC Detect pin port for the bypass bus
 */
# define MAX325xx_SC0_BYP_SC_DETECT_PORT   MML_GPIO_DEV1   /**< SC__DETECT_BYP is on P1.0*/

/** @def MAX325xx_SC0_BYP_SC_DETECT        SC_DETECT_BYP pin number for the bypass bus
 */
# define MAX325xx_SC0_BYP_SC_DETECT        0               /**< SC_DETECT_BYP is on P1.0*/

/*  C4/C8_BYP GPIO port & pin */
/** @def MAX325xx_SC0_BYP_C4_C8_PORT		SC C4/C8 pins port for the bypass bus
*/
# define MAX325xx_SC0_BYP_C4_C8_PORT		MML_GPIO_DEV0   /**< SC0_BYP_C4_C8 is on P0.10*/

/** @def MAX325xx_SC0_BYP_C4_C8        		SC C4/C8 pins pin number for the bypass bus
*/
# define MAX325xx_SC0_BYP_C4_C8		        10               /**< SC0_DETECT_BYP is on P0.10*/


#elif defined(__MAX32560) || defined(__MAX32552)
/** @def  RESET_ACTIVE    Smartcard RST signal active level
 */
#define RESET_ACTIVE                    0 /**< Smartcard Reset is active LOW*/


/* SC0 interface */

/* CLK_BYP GPIO port & pin */
/** @def MAX325xx_SC0_BYP_CLK_PORT			Clock pin (CLK) port for the bypass bus
 */
	# define MAX325xx_SC0_BYP_CLK_PORT			MML_GPIO_DEV1   /**< SC0_BYP_CLK is on P1.1*/
	/** @def MAX325xx_SC0_BYP_CLK				Clock pin (CLK) number for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_CLK				1                /**< SC0_BYP_CLK is on P1.1*/

/* IO_BYP GPIO port & pin */
	/** @def MAX325xx_SC0_BYP_IO_PORT			I/O pin (IO) port for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_IO_PORT			MML_GPIO_DEV0   /**< SC0_BYP_IO is on P0.21*/
	/** @def MAX325xx_SC0_BYP_IO				I/O pin (IO) number for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_IO				21               /**< SC0_BYP_IO is on P0.21*/

/* RST_BYP GPIO port & pin */
	/** @def MAX325xx_SC0_BYP_RST_PORT			RESET pin (RST) port for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_RST_PORT			MML_GPIO_DEV0   /**< SC0_BYP_RST is on P0.22*/
	/** @def MAX325xx_SC0_BYP_RST				RESET pin (RST) number for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_RST				22               /**< SC0_BYP_RST is on P0.22*/

/* DETECT_BYP GPIO port & pin */
	/** @def MAX325xx_SC0_BYP_SC_DETECT_PORT	SC Detect pin port for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_SC_DETECT_PORT	MML_GPIO_DEV1   /**< SC0__DETECT_BYP is on P1.0*/

	/** @def MAX325xx_SC0_BYP_SC_DETECT        SC_DETECT_BYP pin number for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_SC_DETECT        0               /**< SC0_DETECT_BYP is on P1.0*/

/*  C4/C8_BYP GPIO port & pin */
	/** @def MAX325xx_SC0_BYP_C4_C8_PORT		SC C4/C8 pins port for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_C4_C8_PORT		MML_GPIO_DEV0   /**< SC0_BYP_C4_C8 is on P0.10*/

	/** @def MAX325xx_SC0_BYP_C4_C8        		SC C4/C8 pins pin number for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_C4_C8		        10               /**< SC0_DETECT_BYP is on P0.10*/


/* SC1 interface */

/* CLK_BYP GPIO port & pin */
/** @def MAX325xx_SC1_BYP_CLK_PORT			Clock pin (CLK) port for the bypass bus
 */
	# define MAX325xx_SC1_BYP_CLK_PORT			MML_GPIO_DEV0   /**< SC0_BYP_CLK is on P1.1*/
	/** @def MAX325xx_SC1_BYP_CLK				Clock pin (CLK) number for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_CLK				29               /**< SC1_BYP_CLK is on P0.29*/

/* IO_BYP GPIO port & pin */
	/** @def MAX325xx_SC1_BYP_IO_PORT			I/O pin (IO) port for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_IO_PORT			MML_GPIO_DEV0   /**< SC1_BYP_IO is on P0.27*/
	/** @def MAX325xx_SC1_BYP_IO				I/O pin (IO) number for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_IO				27               /**< SC1_BYP_IO is on P0.27*/

/* RST_BYP GPIO port & pin */
	/** @def MAX325xx_SC1_BYP_RST_PORT			RESET pin (RST) port for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_RST_PORT			MML_GPIO_DEV0   /**< SC1_BYP_RST is on P0.28*/
	/** @def MAX325xx_SC1_BYP_RST				RESET pin (RST) number for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_RST				28              /**< SC1_BYP_RST is on P0.28*/

/* DETECT_BYP GPIO port & pin */
	/** @def MAX325xx_SC1_BYP_SC_DETECT_PORT	SC Detect pin port for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_SC_DETECT_PORT	MML_GPIO_DEV0   /**< SC1__DETECT_BYP is on P0.0*/

	/** @def MAX325xx_SC1_BYP_SC_DETECT        SC_DETECT_BYP pin number for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_SC_DETECT        20               /**< SC0_DETECT_BYP is on P0.20*/

/*  C4/C8_BYP GPIO port & pin */
	/** @def MAX325xx_SC1_BYP_C4_C8_PORT		SC C4/C8 pins port for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_C4_C8_PORT		MML_GPIO_DEV0   /**< SC1_BYP_C4_C8 is on P0.25*/

	/** @def MAX325xx_SC1_BYP_C4_C8        		SC C4/C8 pins pin number for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_C4_C8		        25               /**< SC1_DETECT_BYP is on P0.25*/

#elif defined(__MAX32565)
/** @def  RESET_ACTIVE    Smartcard RST signal active level
 */
#define RESET_ACTIVE                    0 /**< Smartcard Reset is active LOW*/

/* SC0 interface */

/* CLK_BYP GPIO port & pin */
/** @def MAX325xx_SC0_BYP_CLK_PORT			Clock pin (CLK) port for the bypass bus
 */
	# define MAX325xx_SC0_BYP_CLK_PORT			MML_GPIO_DEV1   /**< SC0_BYP_CLK is on P1.22*/
	/** @def MAX325xx_SC0_BYP_CLK				Clock pin (CLK) number for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_CLK				22                /**< SC0_BYP_CLK is on P1.22*/

/* IO_BYP GPIO port & pin */
	/** @def MAX325xx_SC0_BYP_IO_PORT			I/O pin (IO) port for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_IO_PORT			MML_GPIO_DEV1   /**< SC0_BYP_IO is on P1.20*/
	/** @def MAX325xx_SC0_BYP_IO				I/O pin (IO) number for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_IO				20               /**< SC0_BYP_IO is on P1.20*/

/* RST_BYP GPIO port & pin */
	/** @def MAX325xx_SC0_BYP_RST_PORT			RESET pin (RST) port for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_RST_PORT			MML_GPIO_DEV1   /**< SC0_BYP_RST is on P1.21*/
	/** @def MAX325xx_SC0_BYP_RST				RESET pin (RST) number for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_RST				21               /**< SC0_BYP_RST is on P1.21*/

/* DETECT_BYP GPIO port & pin */
	/** @def MAX325xx_SC0_BYP_SC_DETECT_PORT	SC Detect pin port for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_SC_DETECT_PORT	MML_GPIO_DEV1   /**< SC0__DETECT_BYP is on P1.28*/

	/** @def MAX325xx_SC0_BYP_SC_DETECT        SC_DETECT_BYP pin number for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_SC_DETECT        28               /**< SC0_DETECT_BYP is on P1.28*/

/*  C4/C8_BYP GPIO port & pin */
	/** @def MAX325xx_SC0_BYP_C4_C8_PORT		SC C4/C8 pins port for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_C4_C8_PORT		MML_GPIO_DEV1   /**< SC0_BYP_C4_C8 is on P1.18*/

	/** @def MAX325xx_SC0_BYP_C4_C8        		SC C4/C8 pins pin number for the bypass bus
	 */
	# define MAX325xx_SC0_BYP_C4_C8		        18               /**< SC0_DETECT_BYP is on P1.18*/


/* SC1 interface */

/* CLK_BYP GPIO port & pin */
/** @def MAX325xx_SC1_BYP_CLK_PORT			Clock pin (CLK) port for the bypass bus
 */
	# define MAX325xx_SC1_BYP_CLK_PORT			MML_GPIO_DEV1   /**< SC0_BYP_CLK is on P1.27*/
	/** @def MAX325xx_SC1_BYP_CLK				Clock pin (CLK) number for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_CLK				27               /**< SC1_BYP_CLK is on P1.27*/

/* IO_BYP GPIO port & pin */
	/** @def MAX325xx_SC1_BYP_IO_PORT			I/O pin (IO) port for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_IO_PORT			MML_GPIO_DEV1   /**< SC1_BYP_IO is on P1.25*/
	/** @def MAX325xx_SC1_BYP_IO				I/O pin (IO) number for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_IO				25               /**< SC1_BYP_IO is on P0.25*/

/* RST_BYP GPIO port & pin */
	/** @def MAX325xx_SC1_BYP_RST_PORT			RESET pin (RST) port for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_RST_PORT			MML_GPIO_DEV1   /**< SC1_BYP_RST is on P1.26*/
	/** @def MAX325xx_SC1_BYP_RST				RESET pin (RST) number for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_RST				26              /**< SC1_BYP_RST is on P1.26*/

/* DETECT_BYP GPIO port & pin */
	/** @def MAX325xx_SC1_BYP_SC_DETECT_PORT	SC Detect pin port for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_SC_DETECT_PORT	MML_GPIO_DEV1   /**< SC1__DETECT_BYP is on P1.29*/

	/** @def MAX325xx_SC1_BYP_SC_DETECT        SC_DETECT_BYP pin number for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_SC_DETECT        29               /**< SC0_DETECT_BYP is on P0.20*/

/*  C4/C8_BYP GPIO port & pin */
	/** @def MAX325xx_SC1_BYP_C4_C8_PORT		SC C4/C8 pins port for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_C4_C8_PORT		MML_GPIO_DEV0   /**< SC1_BYP_C4_C8 is on P0.25*/

	/** @def MAX325xx_SC1_BYP_C4_C8        		SC C4/C8 pins pin number for the bypass bus
	 */
	# define MAX325xx_SC1_BYP_C4_C8		        25               /**< SC1_DETECT_BYP is on P0.25*/
#endif

/** @} */ /*defgroup*/
/** @} */ /*file*/


#endif /* _AFE_MAX325xx_PRIVATE_H_*/
