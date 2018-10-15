/*******************************************************************************
 *  (C) Copyright 2009 Molisys Solutions Co., Ltd. , All rights reserved       *
 *                                                                             *
 *  This source code and any compilation or derivative thereof is the sole     *
 *  property of Molisys Solutions Co., Ltd. and is provided pursuant to a      *
 *  Software License Agreement.  This code is the proprietary information      *
 *  of Molisys Solutions Co., Ltd and is confidential in nature.  Its use and  *
 *  dissemination by any party other than Molisys Solutions Co., Ltd is        *
 *  strictly limited by the confidential information provisions of the         *
 *  Agreement referenced above.                                                *
 ******************************************************************************/

/**
 * @defgroup mlsHttps.h	mlsHttps.h
 * @brief mlsHttps.h
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mlsHttps.h
 *
 * @date 	30 Apr 2018
 * @author	stylvn008
 */

#ifndef SOURCE_STYL_MLSHTTPS_INC_MLSHTTPS_H_
#define SOURCE_STYL_MLSHTTPS_INC_MLSHTTPS_H_


#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include "mlsInclude.h"
/********** Constant  and compile switch definition section *******************/
#define HTTPS_RESP_SIZE	1024*2

/********** Type definition section *******************************************/

/********** Macro definition section*******************************************/

/********** Function declaration section **************************************/
extern char bufferData[HTTPS_RESP_SIZE];

/**
 * @brief This function is to transmit https request and receive https response
 * @param[in] caCert: CA certification
 * @param[in] ip: IP address
 * @param[in] port:
 * @param[in] httpRequest: The raw https request message
 * @param[out]httpsResponse: The raw https response
 * @param[in] responseSize
 * @param[in] timeOutMs
 * @return error code. MLS_SUCCESS if transfer is successful
 */
mlsErrorCode_t mlsHttpsTransfer(char *caCertTypePem, char *ip, char *port,
								char *httpRequest, char *httpsResponse,
								int responseSize, int timeOutMs);

/**
 * @brief This function is to transmit https request and receive https response
 * @param[in] caCert: CA certification
 * @param[in] ip: IP address
 * @param[in] port:
 * @param[in] httpRequest: The raw https request message
 * @param[out]httpsResponse: The raw https response
 * @param[in] responseSize
 * @param[in] timeOutMs
 * @return error code. MLS_SUCCESS if transfer is successful
 */
mlsErrorCode_t mlsHttpsNFPSettlementTransfer(char *caCertTypePem, char *ip, char *port,
								char *httpRequest, char *httpsResponse,
								int responseSize, int timeOutMs);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_STYL_MLSHTTPS_INC_MLSHTTPS_H_ */

/**@}*/
