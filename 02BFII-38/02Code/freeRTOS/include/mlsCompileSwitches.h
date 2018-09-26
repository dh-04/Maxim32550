/**
 * @mainpage	SURISDK APIs document
 *
 * @section	intro_sec	Introduction
 *
 * This document describes APIs in source code XMSDK
 *
 * @section	license_sec	License
 *
 * (C) Copyright 2008 STYL Solutions Pte., Ltd. , All rights reserved
 *
 * This source code and any compilation or derivative thereof is the sole
 * property of STYL Solutions Pte., Ltd. and is provided pursuant to a
 * Software License Agreement.  This code is the proprietary information
 * of STYL Solutions Pte., Ltd and is confidential in nature.  Its use and
 * dissemination by any party other than STYL Solutions Pte., Ltd is
 * strictly limited by the confidential information provisions of the
 * Agreement referenced above.
 *
 * @section	contactInfo_sec	Contact Information
 *
 * sales@styl.com.sg
 *
 */

/**
 * @defgroup mlsCompileSwitch	Common header
 * @brief Common header file
 *
 * Define  define preprocessors and definition that affect project-wide
 * @{
 *
 */
#ifndef SRC_INCLUDE_MLSCOMPILESWITCHES_H_
#define SRC_INCLUDE_MLSCOMPILESWITCHES_H_

#define FIRMWARE_DATE	__DATE__
#define FIRMWARE_TIME	__TIME__

#define FIRMWARE_PROVIDER				"STYL"

#define	FIRMWARE_VERSION_MAJOR	        1
#define FIRMWARE_VERSION_MINOR	        0
#define FIRMWARE_REVISION               0

#define FIRMWARE_VERSION_MAJOR_REPORT   1
#define FIRMWARE_VERSION_MINOR_REPORT   0
#define FIRMWARE_REVISION_REPORT        0

#define FIRMWARE_BOOTLOADER_OWNER_ID	'S' //Bootloader firmware owner: 'S': STYL
#define FIRMWARE_SURISDK_OWNER_ID		'S' //surisdk firmware owner: 'S': STYL

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#define FIRMWARE_VER	"S" STRINGIZE(FIRMWARE_VERSION_MAJOR) "." STRINGIZE(FIRMWARE_VERSION_MINOR) "." STRINGIZE(FIRMWARE_REVISION)
#define FIRMWARE_VER_FULL	FIRMWARE_VER" "FIRMWARE_DATE" "FIRMWARE_TIME" "FIRMWARE_PROVIDER

/* define the firmware base version for EMVL2 */
#define EMV_FIRMWARE_VERSION_MAJOR 			1
#define EMV_FIRMWARE_VERSION_MINOR 			0
#define EMV_FIRMWARE_REVISION 				0
#define EMV_FIRMWARE_OWNER_ID				' '
#define EMV_FIRMWARE_VER	STRINGIZE(EMV_FIRMWARE_VERSION_MAJOR) "." STRINGIZE(EMV_FIRMWARE_VERSION_MINOR) "." STRINGIZE(EMV_FIRMWARE_REVISION)

#define PLATFORM_SEL_LINUX 		0
#define PLATFORM_SEL_WINDOWS 	0
#define PLATFORM_SEL_MCU_M3 	1
#if (PLATFORM_SEL_LINUX + \
	PLATFORM_SEL_WINDOWS + \
	PLATFORM_SEL_MCU_M3  != 1 )
#error "Only support one platform at a time !"
#endif

#define BUILD_MODE_HOST						0
#define BUILD_MODE_SLAVER 					1
#define BUILD_MODE_SLAVER_ON_RF_PROCESSOR 	0
#if (BUILD_MODE_HOST + \
	BUILD_MODE_SLAVER != 1)
#error "Only support one build mode!"
#endif

#define BUILD_ENV_LINUX_PC 		0
#define BUILD_ENV_LINUX_SIRIUS 	1
#if (BUILD_ENV_LINUX_PC + \
	BUILD_ENV_LINUX_SIRIUS != 1)
#error "Only one environment is allowed!"
#endif


#define BUILD_HARDWARE_SIRIUS_3PCB		0
#define BUILD_HARDWARE_SIRIUS_P1		0
#define BUILD_HARDWARE_SIRIUS_P2		1
#if (BUILD_HARDWARE_SIRIUS_3PCB + \
	BUILD_HARDWARE_SIRIUS_P1 + \
	BUILD_HARDWARE_SIRIUS_P2 != 1)
#error "Only one hardware platform is allowed!"
#endif


/** Enable one of these compile switches to select RTOS*/
#define MLS_OSAL_FREE_RTOS
//#define MLS_OSAL_UCOS_III

#if defined MLS_OSAL_FREE_RTOS && \
	defined MLS_OSAL_UCOS_III
#error "Only support one OS at a time!"
#endif

#if !defined MLS_OSAL_FREE_RTOS && \
	!defined MLS_OSAL_UCOS_III
#error "OS error!"
#endif

//#define MLS_EMV_CERTIFY

#define MLS_FEATURESEL_CCID    0
#define MLS_FEATURESEL_EMV1_TA 1

#if (defined(MLS_EMV_CERTIFY) || (MLS_FEATURESEL_EMV1_TA == 1))
#define MLS_FEATURESEL_WDT     0
#else
#define MLS_FEATURESEL_WDT     1
#endif

#define MLS_FEATURESEL_FELICA  0
#define MLS_FEATURESEL_MFC     0
#define MLS_FEATURESEL_MFDF    0
#define MLS_FEATURESEL_NFC     0
#define MLS_FEATURESEL_VICARD  0
#define MLS_FEATURESEL_NXPLOG  0

#define MLS_NXPLOG_SKIP_CONFIG 1
#define MLS_NXPLOG_ISO14443    0
#define MLS_NXPLOG_ISO14443A   0
#define MLS_NXPLOG_ISO14443B   0
#define MLS_NXPLOG_ISO144434   0
#define MLS_NXPLOG_MFC         0
#define MLS_NXPLOG_MFUL        0
#define MLS_NXPLOG_HWAL        0
#define MLS_NXPLOG_DISCLOOP    0


#endif /* SRC_INCLUDE_MLSCOMPILESWITCHES_H_ */

/*@}*/
