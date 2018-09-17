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
 * @defgroup mlsLcd.h	mlsLcd.h
 * @brief mlsLcd.h
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mlsLcd.h
 *
 * @date 	Apr 26, 2017
 * @author	stylvn008
 */


#ifndef __MLSLCDFONT_H_
#define __MLSLCDFONT_H_

#ifdef __cplusplus
extern C {
#endif

#include "string.h"
#include "Type.h"

#define MAX_NUM_IMAGE 50

typedef struct mlsLcdFontInfo
{
	UInt16 fontHeight;
	UInt8 firstchar;
	UInt8 lastchar;
	UInt8 firstsymbol;
	UInt8 lastsymbol;
	const UInt16 *fontIndex;
	const UInt8 *fontdata;
}mlsLcdFontInfo_t;

typedef struct __attribute__ ((packed))
{
//	char name[20];
	UInt16 width;
	UInt16 height;
}mlsImage_t;


typedef struct __attribute__ ((packed))
{
	UInt8	numImage;
	mlsImage_t	Img[MAX_NUM_IMAGE];
	UInt32	address[MAX_NUM_IMAGE];
}mlsLcdImageInfo_t;


extern mlsLcdFontInfo_t Arial_16;

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif /* SOURCE_STYL_MLSLCD_INC_MLSLCDFONT_H_ */
