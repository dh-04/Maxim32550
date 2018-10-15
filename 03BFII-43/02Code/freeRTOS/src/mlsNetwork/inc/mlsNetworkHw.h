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
 * @defgroup mlsNetworkHw.h	mlsNetworkHw.h
 * @brief mlsNetworkHw.h
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mlsNetworkHw.h
 *
 * @date 	8 Feb 2018
 * @author	stylvn008
 */

#ifndef SOURCE_STYL_MLSNETWORK_INC_MLSNETWORKHW_H_
#define SOURCE_STYL_MLSNETWORK_INC_MLSNETWORKHW_H_


#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include "mlsInclude.h"
/********** Constant  and compile switch definition section *******************/


/********** Type definition section *******************************************/
typedef Void (*fnNetworkProcessRsp) (UInt8 data);
/********** Macro definition section*******************************************/

/********** Function declaration section **************************************/

/**
 * @brief This function is to initialize network hardware
 * @return error code
 */
mlsErrorCode_t mlsNetworkHwInit(fnNetworkProcessRsp handler);

/**
 * @brief This function is to reset network hardware
 * @return error code
 */
mlsErrorCode_t mlsNetworkHwReset();

/**
 * @brief This function is to flush data of network hardware
 * @return error code
 */
mlsErrorCode_t mlsNetworkHwFlush();

/**
 * @brief This function is to write data to network hardware
 * @param[in] writeBuf is pointer to writing buffer
 * @param[in] writeBufLen is buffer length
 * @return error code
 */
mlsErrorCode_t mlsNetworkHwWrite(UInt8 *writeBuf, UInt16 writeBufLen);

/**
 * @brief This function is to read data from network hardware
 * @param[out] readBuf is pointer to reading buffer
 * @param[out] readBufLen is pointer to reading buffer length
 * @return error code
 */
mlsErrorCode_t mlsNetworkHwRead(UInt8 *readBuf, UInt16 *readBufLen);

mlsErrorCode_t mlsNetworkHwPowerOn();

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_STYL_MLSNETWORK_INC_MLSNETWORKHW_H_ */

/**@}*/
