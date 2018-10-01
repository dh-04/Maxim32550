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
 * @file <fileName>.c
 * @brief <Brief description>
 *
 * Long description.
 * @date <The date when the file was initial crated>
 * @author	mlsvnxxx
 */

/********** Include section ***************************************************/
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>

#include "../inc/mlsQRBuffer.h"
#include "../inc/qrencode.h"
//#include "mlsOsal.h"
#include "mlsLCDDriver.h"

/********** Local Constant and compile switch definition section **************/

/********** Local Type definition section *************************************/

/********** Local Macro definition section ************************************/

/********** Local (static) variable definition ********************************/

/********** Local (static) function declaration section ***********************/

/********** Local function definition section *********************************/

/********** Global function definition section ********************************/
//extern UInt32 gQPayStatus;
//extern HostResponse_t gHostResponse;
//extern UInt32 gAmount;

//mlsErrorCode_t mlsQRCodeRawHanlder(
//		UInt8* pbInBuf,
//		UInt16 wInBufLen,
//		UInt8* pbOutBuf,
//		UInt16* pwOutBufLen )
//{
//	mlsErrorCode_t status;
//	CHECK_SUCCESS_FCT(status, mlsRfApiSetResponseCode(pbInBuf, pbOutBuf, pwOutBufLen));
//
//
//	if (status == MLS_SUCCESS)
//	{
////		gTxnInstructionType = TXN_QR_CODE;
//		pbOutBuf[2] = 0x00;
//	}
//	else
//	{
//		pbOutBuf[2] = 0x01;
//	}
//
//	return status;
//}
//
//mlsErrorCode_t mlsQRCodeStopDisplay(
//		UInt8* pbInBuf,
//		UInt16 wInBufLen,
//		UInt8* pbOutBuf,
//		UInt16* pwOutBufLen )
//{
//	mlsErrorCode_t status;
//	CHECK_SUCCESS_FCT(status, mlsRfApiSetResponseCode(pbInBuf, pbOutBuf, pwOutBufLen));
//
//	if (status == MLS_SUCCESS)
//	{
////		UInt32 balance;
//		pbOutBuf[2] = 0x00;
//
////		gQPayStatus = pbInBuf[2];
////		memcpy(&balance, &pbInBuf[3], sizeof(balance));
////		memcpy(&gAmount, &pbInBuf[7], sizeof(gAmount));
////		mlsSSS_ConvertInt2BCD(balance, gHostResponse.balance, sizeof(gHostResponse.balance));
//	}
//	else
//	{
//		pbOutBuf[2] = 0x01;
//	}
//
//	return status;
//}

mlsErrorCode_t mlsGenerateQRCode(Int32 *pwidth,UInt8 *LcdBuffer, Int8 * szSourceSring)
{

	QRcode*     pQRC;
	int i;
	unsigned char * pdata;

	// Compute QRCode
	pQRC = QRcode_encodeString((const char *)szSourceSring,
								0,
								QR_ECLEVEL_M,
								QR_MODE_8,
								1);
	if (pQRC != NULL )
    {
		pdata = pQRC->data;
        *pwidth = pQRC->width;
        for(i = 0; i < pQRC->width*pQRC->width; i++)
        {
        	LcdBuffer[i] = *pdata;
        	pdata++;
        }
        QRcode_free(pQRC);
    }
    return MLS_SUCCESS;
}
void mlsTestMallocFree(void)
{
	uint32_t *ptest;
	ptest =(uint32_t *) malloc(50);
	free(ptest);
}
/**@}*/

//  -------------------------------------------------------
