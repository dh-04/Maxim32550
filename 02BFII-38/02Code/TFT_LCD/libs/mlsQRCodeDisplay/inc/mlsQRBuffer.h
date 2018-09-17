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
 * @defgroup <groupIdentifier>	<Group Name> //Optional
 * @ingroup	superGroupIdentifier             //Optional
 * @brief <Brief description>
 *
 * <Long description>
 * @{
 */

/**
 * @file <fileName>.h
 * @brief <Brief description>
 *
 * Long description.
 * @date <The date when the file was initial crated>
 * @author	mlsvnxxx
 */

#ifndef __MLSQRBUFFER_H__
#define __MLSQRBUFFER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include "mlsInclude.h"
#include <stdint.h>

/********** Constant  and compile switch definition section *******************/
#define QRCODE_TEXT                 "http://developers.jollypad.com/fb/index.php?dmmy=1&fb_sig_in_iframe=1&fb_sig_iframe_key=8e296a067a37563370ded05f5a3bf3ec&fb_sig_locale=bg_BG&fb_sig_inuser_about_me,user_status,friends_birthday,friends_religion_politics,friends_relationships"       // Text to encode into QRCode
#define QRCODE_TEXT_2				"http://d"
#define WIDTH						240
#define HEIGHT                      320
#define LCD_SIZE 					WIDTH * HEIGHT
#define QRCODE_DELAY_MLS			15000

#define QRCODE_CMD					0xAA
#define QRCODE_CTRL_RAW_STRING		0x00
#define QRCODE_CTRL_STOP_DISPLAY	0x02
#define SCALE						3
/********** Type definition section *******************************************/
typedef enum mlsByeOrien_st
{
    VERTICAL,
    HORIZONTAL,
} mlsByteOrien_t;
typedef struct mlsRawData_st
{
	int8_t	*data;
	uint16_t length;
}mlsRawData_t;
typedef struct mlsQRRecord_st
{
	int8_t rec_type[3];
	int8_t rec_len[3];
	int8_t rec_data[99];
}mlsQRRecord_t;
typedef struct mlsStaticData_st
{
	int8_t			Header[9];
	int8_t			Version[2];
	int8_t			Nr[3];
	int8_t			QR_Issuer[11];
	int8_t			QR_ExpDate[9];
	mlsQRRecord_t	Recor[10];
	int8_t			Signature[65];
	mlsRawData_t	rawData;
}mlsStaticData_t;
typedef struct mlsDynamicData_st
{
	int8_t			Header[9];
	int8_t			Version[2];
	int8_t			TID[9];
	int8_t			Merch_ID[16];
	int8_t			Stan[7];
	int8_t			Time_Stamp[9];
	int8_t			OP[2];
	int8_t			Channel[2];
	int8_t			AMT[9];
	int8_t			PaymentAcceptedBitmap[5];
	int8_t			Currency[2];
	int8_t			OTRS[9];
	int8_t			Merchant_Name[18];
	int8_t			Checksum[5];
	mlsRawData_t	rawData;
}mlsDynamicData_t;

extern uint32_t	gQRCodeTimeoutMls;
extern uint32_t	gQRCodeTimeStart;
/********** Macro definition section*******************************************/

/********** Function declaration section **************************************/

/** \fn mlsGenerateQRCode
 * @brief: This function is used generate QR Buffer on LCD
 * @param[in]: x2, y2 are the coordinates of QRImage on LCD
 */
mlsErrorCode_t mlsGenerateQRCode(Int32 *pwidth,UInt8 *LcdBuffer, Int8 * szSourceSring);

/** \fn mlsInitBuffer
 * @brief: This function is used to initialize the LCDBuffer
 * @param[in]:
 */
Void mlsInitBuffer(int8_t * Buffer);

/**
 * @fn void mlsQRCgenerator_test
 * @brief    Test to display the QR code on LCD
 * @param[in]
 */
mlsErrorCode_t mlsTestQRCgenerator();

/**
 * @fn void mlsDisplayBuffer
 * @brief    Display buffer on LCD
 * @param[in]
 */
void mlsDisplayBuffer(const UInt8 *buffer, UInt32 bufferSize);
/**
 * @function: mlsDisplayRawQRCode
 */
mlsErrorCode_t mlsDisplayRawQRCode(int8_t * szSourceSring);
/**
 * @function: mlsDisplayStaticQRCode
 */
mlsErrorCode_t mlsDisplayStaticQRCode();
/**
 * @function: mlsDisplayDynamicQRCode
 */
mlsErrorCode_t mlsDisplayDynamicQRCode();
mlsErrorCode_t mlsQRCodeRawHanlder(UInt8* pbInBuf, UInt16 wInBufLen, UInt8* pbOutBuf, UInt16* pwOutBufLen);
mlsErrorCode_t mlsQRCodeStopDisplay(UInt8* pbInBuf, UInt16 wInBufLen, UInt8* pbOutBuf, UInt16* pwOutBufLen);
#ifdef __cplusplus
}
#endif

#endif /*MLSQRBUFFER_H_ */
