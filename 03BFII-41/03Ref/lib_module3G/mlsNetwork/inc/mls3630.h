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
 * @defgroup mls3630.h	mls3630.h
 * @brief mls3630.h
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mls3630.h
 *
 * @date 	8 Feb 2018
 * @author	stylvn008
 */

#ifndef SOURCE_STYL_MLSNETWORK_INC_MLS3630_H_
#define SOURCE_STYL_MLSNETWORK_INC_MLS3630_H_


#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include "mlsInclude.h"
/********** Constant  and compile switch definition section *******************/
#define AT_DATA_CR		'\r'
#define AT_DATA_LF		'\n'
#define AT_DATA_SEND	'>'
#define AT_CR_LF		"\r\n"
#define AT_RESPONSE_PER_BYTE_TIMEOUT_MS	300

#define CHECK_3630_READY				"AT"
#define CHECK_3630_READY_CR_LF			"AT\r\n"
#define ENABLE_3630_COMMAND_ECHO		"ATE1\r\n"
#define DISABLE_3630_COMMAND_ECHO		"ATE0\r\n"
#define QUERY_3630_NETWORK_STATE		"AT+ZIPCALL?\r\n"
#define ACTIVE_3630_NETWORK				"AT+ZIPCALL=1\r\n"
#define CHECK_3630_SIM_STATUS			"AT+CPIN?\r\n"
#define NETWORK_CREG_REGISTRATION		"AT+CREG?\r\n"
#define NETWORK_CEREG_REGISTRATION		"AT+CEREG?\r\n"
#define CHECK_3630_SIGNAL_QUALITY		"AT+CSQ\r\n"
#define GET_3630_SIM_ID					"AT+ZGETICCID\r\n"

#define TIMEOUT_BASE_MS                 0
#define CHECK_3630_READY_TIMEOUT_MS		300
#define AT_COMMAND_TIMEOUT_MS			2000
#define ESTABLISH_TCP_TIMEOUT_MS		TIMEOUT_BASE_MS + 30000
#define SOCKET_TIMEOUT_MS				TIMEOUT_BASE_MS + 20000
#define SOCKET_CLOSE_TIMEOUT_MS         TIMEOUT_BASE_MS + 200

#define RESP_OK							"OK"
#define RESP_ERROR						"ERROR"
#define RESP_CME_ERROR					"+CME ERROR:"
#define RESP_READY						"+ZREADY"
#define RESP_RECV						"+ZIPRECV:"
#define RESP_3630_NETWORK_DOWN          "+ZIPCALL: 0,0.0.0.0"
#define RESP_3630_CLOSED                "+ZIPSTAT: %d,0"
#define RESP_SENDRAW					"+ZIPSENDRAW: %d,%d"
#define RESP_3630_CPIN_READY			"+CPIN: READY"
#define RESP_CREG_REGISTERED			"+CREG: 0,"
#define RESP_CEREG						"+CEREG: 0,"
#define RESP_3630_CSQ					"+CSQ: "
#define RESP_3630_GET_SIM_ID			"+ZGETICCID: "
#define RSSI_VALUE_NO_NETWORK			(99)
#define SOCKET_MODE						TCP_CLI
#define MAX_3630_SOCKET                 5

/********** Type definition section *******************************************/
typedef enum
{
    TCP_CLI = 0,
    TCP_SERVER,
    UDP
}SocketModeOperate_t;
/********** Macro definition section*******************************************/

/********** Function declaration section **************************************/

/**
 * @brief This function is to initialize network module
 * @return error code.
 */
mlsErrorCode_t mls3630Init();

/**
 * @brief This function is to check device ready or not
 * @return error code
 */
mlsErrorCode_t mls3630CheckReady();

/**
 * @brief This function is to configure network APN
 * @param[in, out] pParam is pointer to parameter input
 * @return error code
 */
mlsErrorCode_t mls3630SetAPN(const char *pAPN);

/**
 * @brief This function is to connect device to 3G/4G network
 * @param[in, out] pParam is pointer to parameter input
 * @return error code
 */
mlsErrorCode_t mls3630Act();

/**
 * @brief This function is to create a socket connection for application
 * @param[in] ip is pointer to IP address
 * @param[in] port is pointer to port
 * @return error code or Socket ID if success
 */
Int32 mls3630CreateSocket(Char *ip, Char *port);

/**
 * @brief This function is to write data to target socket
 * @param[in] sockID is socket ID
 * @param[in] buffer is pointer to buffer input
 * @param[in] bufferLen is buffer input length
 * @return error code
 */
mlsErrorCode_t mls3630WriteSocket(Int32 sockID, Char *buffer, UInt16 bufferLen);

/**
 * @brief This function is to read data from target socket
 * @param[in] sockID is socket ID
 * @param[out] buffer is pointer to buffer output
 * @param[out] bufferLen is pointer to buffer output length
 * @return error code
 */
mlsErrorCode_t mls3630ReadSocket(Int32 sockID, Char *buffer, UInt16 *bufferLen);

/**
 * @brief This function is to close the socket which open before
 * @param[in] sockID is socket ID
 * @return error code
 */
mlsErrorCode_t mls3630CloseSocket(Int32 sockID);

/**
 * @brief This function is to get RSSI at current
 * @param[out] rssi is pointer to RSSI
 * @return error code
 */
mlsErrorCode_t mls3630GetRSSI(UInt32 *rssi);

mlsErrorCode_t mls3630GetSIMID(UInt8 *id, UInt8 *lenID);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_STYL_MLSNETWORK_INC_MLS3630_H_ */

/**@}*/
