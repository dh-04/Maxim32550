/**********************************************************************************
 *  (C) Copyright 2009 Molisys Solutions Co., Ltd. , All rights reserved          *
 *                                                                                *
 *  This source code and any compilation or derivative thereof is the sole        *
 *  property of Molisys Solutions Co., Ltd. and is provided pursuant to a         *
 *  Software License Agreement.  This code is the proprietary information         *
 *  of Molisys Solutions Co., Ltd and is confidential in nature.  Its use and     *
 *  dissemination by any party other than Molisys Solutions Co., Ltd is           *
 *  strictly limited by the confidential information provisions of the            *
 *  Agreement referenced above.                                                   *
 **********************************************************************************/

/**
 * @addtogroup mlsErrors Reader Result Codes
 * @brief Reader Result Codes
 * @{
 */

/**
 * @file mlsErrors.h
 * @brief	Error code definitions
 *
 * This file contains error code definitions which are used by all function that
 * return error code in project
 * @author	mlsvnusr001
 */

#ifndef __MLSERRORS_H__
#define __MLSERRORS_H__

#include "../include/mlsTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef Int32 mlsErrorCode_t;

/*---------------------------------------------------
 * Common error codes
 * --------------------------------------------------*/
#define MLS_SUCCESS                    		0 	/** Global success return status   */
#define MLS_ERROR							-1	/** Global error return status*/
#define MLS_ERR_LIMITED						-3 	/** return when over Max value*/
#define MLS_ERR_NONE_DATA					-4 	/** return when don't have response data*/
#define MLS_ERR_BAD_LC						-6  /** return when length of command incorrect*/
#define MLS_ERR_BAD_LE						-7	/** return when response data > expected data*/
#define MLS_TIME_ERR						-9	//
#define MLS_ERR_ICC_MUTE					-10	//

#define MLS_ERR_BAD_PARAMETER				-109 /** Invalid input parameter      */
#define MLS_ERR_NO_RESOURCES				-112 /** Resource is not available    */
#define MLS_ERR_NOT_INITIALIZED				-117 /** Component is not initialized */
#define MLS_ERR_NOT_SUPPORTED				-119 /** Function is not supported    */
#define MLS_ERR_TIMEOUT						-131 /** Timeout error                */
#define MLS_ERR_BAD_FORMAT					-192 /** Bad format                   */
#define MLS_ERR_REFERENCE_DATA				-216

#define MLS_ERR_GPRS						-400 /** GPRS general error           */


/** Entry point error code (-100  to -199) */
#define MLS_ERRORCODE_ENTRYPOINT_BASE			-100
#define MLS_ERR_EP_PROTOCOL_ACTIVATION			-101

#define MLS_ERR_K2_RECOVER_AC_RESULT			-201


/** Ez SAM error */

/** Nets SAM error */

/** Lta SAM error */

/** define iso 7816 Response status SW error code */

/**CASIO error code*/
#define MLS_ERR_NFC_ACTIVATE                                    -2100
#define MLS_ERR_NFC_PRESENCE_CHECK                              -2101
#define MLS_ERR_NFC_CARD_REMOVED                                -2102
#define MLS_ERR_NFC_TRANSCEIVE_ERROR                            -2103
#define MLS_ERR_NFC_NOT_SUPPORTED                               -2104
#define MLS_ERR_NFC_RATS_ERROR                                  -2105
#define MLS_ERR_NFC_TRANSCEI_TIMEOUT                            -2105

#ifdef __cplusplus
}
#endif

#endif /**__MLSERRORS_H__*/


/*@}*/
