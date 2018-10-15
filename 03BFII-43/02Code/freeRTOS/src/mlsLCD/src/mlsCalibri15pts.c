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
 * @defgroup mlsCalibri15pts.h	mlsCalibri15pts.h
 * @brief mlsCalibri15pts.h
 *
 * Add more details about module
 * @{
 */

/**
 * @file	mlsCalibri15pts.h
 *
 * @date 	19 Mar 2018
 * @author	sirius
 */

#ifndef SOURCE_STYL_MLSLCD_INC_MLSCALIBRI15PTS_H_
#define SOURCE_STYL_MLSLCD_INC_MLSCALIBRI15PTS_H_


#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include "mlsLCDFont.h"
/********** Constant  and compile switch definition section *******************/

/********** Type definition section *******************************************/

/********** Macro definition section*******************************************/

/********** Function declaration section **************************************/
//
//  Font data for Calibri 15pt
//

// Character bitmaps for Calibri 15pt
const UInt8 calibri_15ptBitmaps[] =
{
	// @0 '!' (3 pixels wide)
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @19 '"' (5 pixels wide)
	0x00, //
	0xD8, // ## ##
	0xD8, // ## ##
	0xD8, // ## ##
	0xD8, // ## ##
	0xD8, // ## ##
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @38 '#' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x33, 0x00, //   ##  ##
	0x33, 0x00, //   ##  ##
	0x33, 0x00, //   ##  ##
	0xFF, 0x80, // #########
	0xFF, 0x80, // #########
	0x33, 0x00, //   ##  ##
	0x33, 0x00, //   ##  ##
	0x66, 0x00, //  ##  ##
	0xFF, 0x80, // #########
	0xFF, 0x80, // #########
	0x66, 0x00, //  ##  ##
	0x66, 0x00, //  ##  ##
	0x66, 0x00, //  ##  ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @76 '$' (8 pixels wide)
	0x18, //    ##
	0x18, //    ##
	0x7C, //  #####
	0x7E, //  ######
	0xE2, // ###   #
	0xE0, // ###
	0xF0, // ####
	0xFC, // ######
	0x7E, //  ######
	0x1F, //    #####
	0x07, //      ###
	0x07, //      ###
	0x87, // #    ###
	0xFE, // #######
	0x7C, //  #####
	0x30, //   ##
	0x30, //   ##
	0x00, //
	0x00, //

	// @95 '%' (14 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x78, 0x18, //  ####      ##
	0xFC, 0x30, // ######    ##
	0xCC, 0x70, // ##  ##   ###
	0xCC, 0x60, // ##  ##   ##
	0xCC, 0xC0, // ##  ##  ##
	0xFD, 0x80, // ###### ##
	0x7B, 0xF8, //  #### #######
	0x03, 0xFC, //       ########
	0x06, 0xCC, //      ## ##  ##
	0x0C, 0xCC, //     ##  ##  ##
	0x1C, 0xCC, //    ###  ##  ##
	0x18, 0xFC, //    ##   ######
	0x30, 0x78, //   ##     ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @133 '&' (13 pixels wide)
	0x00, 0x00, //
	0x1F, 0x00, //    #####
	0x3F, 0xC0, //   ########
	0x71, 0xC0, //  ###   ###
	0x71, 0xC0, //  ###   ###
	0x73, 0xC0, //  ###  ####
	0x3F, 0x80, //   #######
	0x3E, 0x00, //   #####
	0x7E, 0x30, //  ######   ##
	0xEF, 0x30, // ### ####  ##
	0xE7, 0xF0, // ###  #######
	0xE3, 0xE0, // ###   #####
	0xF1, 0xF0, // ####   #####
	0x7F, 0xF8, //  ############
	0x3F, 0x18, //   ######   ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @171 ''' (2 pixels wide)
	0x00, //
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @190 '(' (5 pixels wide)
	0x00, //
	0x18, //    ##
	0x38, //   ###
	0x30, //   ##
	0x70, //  ###
	0x70, //  ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x70, //  ###
	0x70, //  ###
	0x30, //   ##
	0x38, //   ###
	0x18, //    ##

	// @209 ')' (5 pixels wide)
	0x00, //
	0xC0, // ##
	0xE0, // ###
	0x60, //  ##
	0x70, //  ###
	0x70, //  ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x70, //  ###
	0x70, //  ###
	0x60, //  ##
	0xE0, // ###
	0xC0, // ##

	// @228 '*' (8 pixels wide)
	0x00, //
	0x18, //    ##
	0x18, //    ##
	0xFF, // ########
	0x7E, //  ######
	0x7E, //  ######
	0xFF, // ########
	0x18, //    ##
	0x18, //    ##
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @247 '+' (8 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x18, //    ##
	0x18, //    ##
	0x18, //    ##
	0x18, //    ##
	0xFF, // ########
	0xFF, // ########
	0x18, //    ##
	0x18, //    ##
	0x18, //    ##
	0x18, //    ##
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @266 ',' (4 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x70, //  ###
	0x70, //  ###
	0x70, //  ###
	0x60, //  ##
	0xE0, // ###
	0xC0, // ##
	0x00, //

	// @285 '-' (5 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0xF8, // #####
	0xF8, // #####
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @304 '.' (3 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @323 '/' (9 pixels wide)
	0x03, 0x80, //       ###
	0x03, 0x80, //       ###
	0x07, 0x00, //      ###
	0x07, 0x00, //      ###
	0x07, 0x00, //      ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x1E, 0x00, //    ####
	0x1C, 0x00, //    ###
	0x1C, 0x00, //    ###
	0x3C, 0x00, //   ####
	0x38, 0x00, //   ###
	0x38, 0x00, //   ###
	0x70, 0x00, //  ###
	0x70, 0x00, //  ###
	0x70, 0x00, //  ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0x00, 0x00, //

	// @361 '0' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x3E, 0x00, //   #####
	0x7F, 0x00, //  #######
	0x77, 0x00, //  ### ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0x77, 0x00, //  ### ###
	0x7F, 0x00, //  #######
	0x3E, 0x00, //   #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @399 '1' (8 pixels wide)
	0x00, //
	0x00, //
	0x1C, //    ###
	0xFC, // ######
	0xDC, // ## ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0xFF, // ########
	0xFF, // ########
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @418 '2' (8 pixels wide)
	0x00, //
	0x00, //
	0x7C, //  #####
	0xFE, // #######
	0x87, // #    ###
	0x07, //      ###
	0x07, //      ###
	0x07, //      ###
	0x0E, //     ###
	0x1C, //    ###
	0x38, //   ###
	0x70, //  ###
	0xE0, // ###
	0xFF, // ########
	0xFF, // ########
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @437 '3' (8 pixels wide)
	0x00, //
	0x00, //
	0x7C, //  #####
	0xFE, // #######
	0x8E, // #   ###
	0x0E, //     ###
	0x0E, //     ###
	0x7C, //  #####
	0x7E, //  ######
	0x0F, //     ####
	0x07, //      ###
	0x07, //      ###
	0x8F, // #   ####
	0xFE, // #######
	0x7C, //  #####
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @456 '4' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x0F, 0x00, //     ####
	0x1F, 0x00, //    #####
	0x1F, 0x00, //    #####
	0x37, 0x00, //   ## ###
	0x37, 0x00, //   ## ###
	0x67, 0x00, //  ##  ###
	0x67, 0x00, //  ##  ###
	0xC7, 0x00, // ##   ###
	0xFF, 0x80, // #########
	0xFF, 0x80, // #########
	0x07, 0x00, //      ###
	0x07, 0x00, //      ###
	0x07, 0x00, //      ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @494 '5' (8 pixels wide)
	0x00, //
	0x00, //
	0xFE, // #######
	0xFE, // #######
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xFC, // ######
	0xFE, // #######
	0x0F, //     ####
	0x07, //      ###
	0x07, //      ###
	0x8F, // #   ####
	0xFE, // #######
	0x7C, //  #####
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @513 '6' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x00, //    #####
	0x3F, 0x00, //   ######
	0x70, 0x00, //  ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xFF, 0x00, // ########
	0xFF, 0x80, // #########
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xF3, 0x80, // ####  ###
	0x7F, 0x00, //  #######
	0x3E, 0x00, //   #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @551 '7' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xFF, 0x80, // #########
	0xFF, 0x80, // #########
	0x03, 0x80, //       ###
	0x07, 0x00, //      ###
	0x07, 0x00, //      ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x1C, 0x00, //    ###
	0x1C, 0x00, //    ###
	0x38, 0x00, //   ###
	0x38, 0x00, //   ###
	0x70, 0x00, //  ###
	0x70, 0x00, //  ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @589 '8' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x3E, 0x00, //   #####
	0x7F, 0x80, //  ########
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xF3, 0x80, // ####  ###
	0x7F, 0x00, //  #######
	0x3E, 0x00, //   #####
	0x77, 0x00, //  ### ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xFF, 0x00, // ########
	0x3E, 0x00, //   #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @627 '9' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x3E, 0x00, //   #####
	0x7F, 0x00, //  #######
	0xE7, 0x80, // ###  ####
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xFF, 0x80, // #########
	0x7F, 0x80, //  ########
	0x03, 0x80, //       ###
	0x03, 0x00, //       ##
	0x07, 0x00, //      ###
	0xFE, 0x00, // #######
	0xFC, 0x00, // ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @665 ':' (3 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0x00, //
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @684 ';' (4 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x70, //  ###
	0x70, //  ###
	0x70, //  ###
	0x00, //
	0x00, //
	0x00, //
	0x70, //  ###
	0x70, //  ###
	0x70, //  ###
	0x60, //  ##
	0xE0, // ###
	0xC0, // ##
	0x00, //

	// @703 '<' (8 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x01, //        #
	0x07, //      ###
	0x1E, //    ####
	0x78, //  ####
	0xE0, // ###
	0xE0, // ###
	0x78, //  ####
	0x1E, //    ####
	0x07, //      ###
	0x01, //        #
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @722 '=' (8 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0xFF, // ########
	0xFF, // ########
	0x00, //
	0x00, //
	0xFF, // ########
	0xFF, // ########
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @741 '>' (8 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x80, // #
	0xE0, // ###
	0x78, //  ####
	0x1E, //    ####
	0x07, //      ###
	0x07, //      ###
	0x1E, //    ####
	0x78, //  ####
	0xE0, // ###
	0x80, // #
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @760 '?' (7 pixels wide)
	0x00, //
	0x78, //  ####
	0xFC, // ######
	0x8E, // #   ###
	0x0E, //     ###
	0x0E, //     ###
	0x0E, //     ###
	0x3E, //   #####
	0x3C, //   ####
	0x38, //   ###
	0x38, //   ###
	0x00, //
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @779 '@' (16 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x03, 0xF8, //       #######
	0x0F, 0xFC, //     ##########
	0x3C, 0x0E, //   ####      ###
	0x33, 0xB7, //   ##  ### ## ###
	0x67, 0xF3, //  ##  #######  ##
	0x66, 0x73, //  ##  ##  ###  ##
	0xCC, 0x63, // ##  ##   ##   ##
	0xCC, 0x63, // ##  ##   ##   ##
	0xCC, 0xE6, // ##  ##  ###  ##
	0xCF, 0xFE, // ##  ###########
	0xC7, 0x3C, // ##   ###  ####
	0xE0, 0x00, // ###
	0x70, 0x20, //  ###      #
	0x3F, 0xE0, //   #########
	0x1F, 0xC0, //    #######
	0x00, 0x00, //
	0x00, 0x00, //

	// @817 'A' (13 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x07, 0x00, //      ###
	0x07, 0x80, //      ####
	0x0D, 0x80, //     ## ##
	0x0D, 0x80, //     ## ##
	0x1D, 0xC0, //    ### ###
	0x18, 0xC0, //    ##   ##
	0x18, 0xE0, //    ##   ###
	0x38, 0xE0, //   ###   ###
	0x3F, 0xE0, //   #########
	0x3F, 0xF0, //   ##########
	0x70, 0x70, //  ###     ###
	0x60, 0x30, //  ##       ##
	0xE0, 0x38, // ###       ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @855 'B' (10 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xFE, 0x00, // #######
	0xFF, 0x80, // #########
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xFE, 0x00, // #######
	0xFF, 0x80, // #########
	0xE3, 0xC0, // ###   ####
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE3, 0xC0, // ###   ####
	0xFF, 0x80, // #########
	0xFF, 0x00, // ########
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @893 'C' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x00, //    #####
	0x3F, 0x80, //   #######
	0x70, 0x80, //  ###    #
	0x70, 0x00, //  ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xF0, 0x00, // ####
	0x70, 0x80, //  ###    #
	0x3F, 0x80, //   #######
	0x1F, 0x00, //    #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @931 'D' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xFE, 0x00, // #######
	0xFF, 0x80, // #########
	0xE3, 0xC0, // ###   ####
	0xE1, 0xE0, // ###    ####
	0xE0, 0xE0, // ###     ###
	0xE0, 0xE0, // ###     ###
	0xE0, 0xE0, // ###     ###
	0xE0, 0xE0, // ###     ###
	0xE0, 0xE0, // ###     ###
	0xE1, 0xC0, // ###    ###
	0xE3, 0xC0, // ###   ####
	0xFF, 0x80, // #########
	0xFE, 0x00, // #######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @969 'E' (8 pixels wide)
	0x00, //
	0x00, //
	0xFF, // ########
	0xFF, // ########
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xFE, // #######
	0xFE, // #######
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xFF, // ########
	0xFF, // ########
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @988 'F' (7 pixels wide)
	0x00, //
	0x00, //
	0xFE, // #######
	0xFE, // #######
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xFE, // #######
	0xFE, // #######
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @1007 'G' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x0F, 0xC0, //     ######
	0x3F, 0xE0, //   #########
	0x78, 0x20, //  ####     #
	0x70, 0x00, //  ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE3, 0xE0, // ###   #####
	0xE3, 0xE0, // ###   #####
	0xE0, 0xE0, // ###     ###
	0x70, 0xE0, //  ###    ###
	0x78, 0xE0, //  ####   ###
	0x3F, 0xE0, //   #########
	0x0F, 0xC0, //     ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1045 'H' (10 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xFF, 0xC0, // ##########
	0xFF, 0xC0, // ##########
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1083 'I' (3 pixels wide)
	0x00, //
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @1102 'J' (6 pixels wide)
	0x00, //
	0x00, //
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0x1C, //    ###
	0xFC, // ######
	0xF8, // #####
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @1121 'K' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xE3, 0x80, // ###   ###
	0xE7, 0x80, // ###  ####
	0xE7, 0x00, // ###  ###
	0xEE, 0x00, // ### ###
	0xEE, 0x00, // ### ###
	0xFC, 0x00, // ######
	0xFC, 0x00, // ######
	0xEE, 0x00, // ### ###
	0xEE, 0x00, // ### ###
	0xEF, 0x00, // ### ####
	0xE7, 0x00, // ###  ###
	0xE7, 0x80, // ###  ####
	0xE3, 0x80, // ###   ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1159 'L' (7 pixels wide)
	0x00, //
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xFE, // #######
	0xFE, // #######
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @1178 'M' (15 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xF8, 0x3E, // #####     #####
	0xF8, 0x3E, // #####     #####
	0xFC, 0x3E, // ######    #####
	0xFC, 0x7E, // ######   ######
	0xEC, 0x6E, // ### ##   ## ###
	0xEC, 0x6E, // ### ##   ## ###
	0xEE, 0xEE, // ### ### ### ###
	0xE6, 0xCE, // ###  ## ##  ###
	0xE6, 0xCE, // ###  ## ##  ###
	0xE7, 0xCE, // ###  #####  ###
	0xE3, 0x8E, // ###   ###   ###
	0xE3, 0x8E, // ###   ###   ###
	0xE3, 0x8E, // ###   ###   ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1216 'N' (10 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xE0, 0xC0, // ###     ##
	0xF0, 0xC0, // ####    ##
	0xF0, 0xC0, // ####    ##
	0xD8, 0xC0, // ## ##   ##
	0xD8, 0xC0, // ## ##   ##
	0xCC, 0xC0, // ##  ##  ##
	0xCC, 0xC0, // ##  ##  ##
	0xC6, 0xC0, // ##   ## ##
	0xC6, 0xC0, // ##   ## ##
	0xC2, 0xC0, // ##    # ##
	0xC3, 0xC0, // ##    ####
	0xC3, 0xC0, // ##    ####
	0xC1, 0xC0, // ##     ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1254 'O' (12 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x80, //    ######
	0x3F, 0xC0, //   ########
	0x70, 0xE0, //  ###    ###
	0x70, 0x70, //  ###     ###
	0xE0, 0x70, // ###      ###
	0xE0, 0x70, // ###      ###
	0xE0, 0x70, // ###      ###
	0xE0, 0x70, // ###      ###
	0xE0, 0x70, // ###      ###
	0xE0, 0xE0, // ###     ###
	0x70, 0xE0, //  ###    ###
	0x3F, 0xC0, //   ########
	0x1F, 0x80, //    ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1292 'P' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xFE, 0x00, // #######
	0xFF, 0x00, // ########
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE7, 0x80, // ###  ####
	0xFF, 0x00, // ########
	0xFE, 0x00, // #######
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1330 'Q' (13 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x80, //    ######
	0x3F, 0xC0, //   ########
	0x70, 0xE0, //  ###    ###
	0x70, 0x70, //  ###     ###
	0xE0, 0x70, // ###      ###
	0xE0, 0x70, // ###      ###
	0xE0, 0x70, // ###      ###
	0xE0, 0x70, // ###      ###
	0xE0, 0x70, // ###      ###
	0xE0, 0xE0, // ###     ###
	0x70, 0xE0, //  ###    ###
	0x7F, 0xC0, //  #########
	0x1F, 0xF0, //    #########
	0x00, 0x78, //          ####
	0x00, 0x18, //            ##
	0x00, 0x00, //
	0x00, 0x00, //

	// @1368 'R' (10 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xFE, 0x00, // #######
	0xFF, 0x00, // ########
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE7, 0x80, // ###  ####
	0xFF, 0x00, // ########
	0xFE, 0x00, // #######
	0xE7, 0x00, // ###  ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1406 'S' (8 pixels wide)
	0x00, //
	0x00, //
	0x3C, //   ####
	0x7E, //  ######
	0xE2, // ###   #
	0xE0, // ###
	0xF0, // ####
	0xFC, // ######
	0x7E, //  ######
	0x1F, //    #####
	0x07, //      ###
	0x07, //      ###
	0x87, // #    ###
	0xFE, // #######
	0x7C, //  #####
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @1425 'T' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xFF, 0xE0, // ###########
	0xFF, 0xE0, // ###########
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1463 'U' (10 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xE1, 0xC0, // ###    ###
	0xF3, 0xC0, // ####  ####
	0x7F, 0x80, //  ########
	0x3F, 0x00, //   ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1501 'V' (12 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xF0, 0x70, // ####     ###
	0x70, 0xE0, //  ###    ###
	0x70, 0xE0, //  ###    ###
	0x70, 0xE0, //  ###    ###
	0x38, 0xC0, //   ###   ##
	0x39, 0xC0, //   ###  ###
	0x39, 0xC0, //   ###  ###
	0x1D, 0xC0, //    ### ###
	0x1D, 0x80, //    ### ##
	0x1D, 0x80, //    ### ##
	0x0F, 0x80, //     #####
	0x0F, 0x00, //     ####
	0x0F, 0x00, //     ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1539 'W' (18 pixels wide)
	0x00, 0x00, 0x00, //
	0x00, 0x00, 0x00, //
	0xE1, 0xC1, 0xC0, // ###    ###     ###
	0x71, 0xC3, 0x80, //  ###   ###    ###
	0x71, 0xE3, 0x80, //  ###   ####   ###
	0x71, 0xE3, 0x80, //  ###   ####   ###
	0x71, 0x63, 0x80, //  ###   # ##   ###
	0x3B, 0x63, 0x00, //   ### ## ##   ##
	0x3B, 0x77, 0x00, //   ### ## ### ###
	0x3B, 0x77, 0x00, //   ### ## ### ###
	0x3B, 0x36, 0x00, //   ### ##  ## ##
	0x1A, 0x36, 0x00, //    ## #   ## ##
	0x1E, 0x3E, 0x00, //    ####   #####
	0x1E, 0x3E, 0x00, //    ####   #####
	0x0E, 0x1C, 0x00, //     ###    ###
	0x00, 0x00, 0x00, //
	0x00, 0x00, 0x00, //
	0x00, 0x00, 0x00, //
	0x00, 0x00, 0x00, //

	// @1596 'X' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x71, 0xC0, //  ###   ###
	0x71, 0xC0, //  ###   ###
	0x3B, 0x80, //   ### ###
	0x3B, 0x80, //   ### ###
	0x1B, 0x00, //    ## ##
	0x1F, 0x00, //    #####
	0x0E, 0x00, //     ###
	0x1F, 0x00, //    #####
	0x3B, 0x80, //   ### ###
	0x3B, 0x80, //   ### ###
	0x71, 0xC0, //  ###   ###
	0x71, 0xC0, //  ###   ###
	0xF1, 0xE0, // ####   ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1634 'Y' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0x77, 0x00, //  ### ###
	0x77, 0x00, //  ### ###
	0x77, 0x00, //  ### ###
	0x3E, 0x00, //   #####
	0x3E, 0x00, //   #####
	0x1C, 0x00, //    ###
	0x1C, 0x00, //    ###
	0x1C, 0x00, //    ###
	0x1C, 0x00, //    ###
	0x1C, 0x00, //    ###
	0x1C, 0x00, //    ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1672 'Z' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xFF, 0x80, // #########
	0xFF, 0x80, // #########
	0x03, 0x80, //       ###
	0x07, 0x00, //      ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x1C, 0x00, //    ###
	0x38, 0x00, //   ###
	0x38, 0x00, //   ###
	0x70, 0x00, //  ###
	0xE0, 0x00, // ###
	0xFF, 0x80, // #########
	0xFF, 0x80, // #########
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1710 '[' (4 pixels wide)
	0x00, //
	0xF0, // ####
	0xF0, // ####
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xF0, // ####
	0xF0, // ####
	0x00, //

	// @1729 '\' (9 pixels wide)
	0x00, 0x00, //
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0x70, 0x00, //  ###
	0x70, 0x00, //  ###
	0x78, 0x00, //  ####
	0x38, 0x00, //   ###
	0x38, 0x00, //   ###
	0x1C, 0x00, //    ###
	0x1C, 0x00, //    ###
	0x1C, 0x00, //    ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0F, 0x00, //     ####
	0x07, 0x00, //      ###
	0x07, 0x00, //      ###
	0x03, 0x80, //       ###
	0x03, 0x80, //       ###
	0x00, 0x00, //

	// @1767 ']' (4 pixels wide)
	0x00, //
	0xF0, // ####
	0xF0, // ####
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0xF0, // ####
	0xF0, // ####
	0x00, //

	// @1786 '^' (10 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1C, 0x00, //    ###
	0x1E, 0x00, //    ####
	0x36, 0x00, //   ## ##
	0x37, 0x00, //   ## ###
	0x37, 0x00, //   ## ###
	0x73, 0x80, //  ###  ###
	0x73, 0x80, //  ###  ###
	0xF3, 0xC0, // ####  ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1824 '_' (10 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0xFF, 0xC0, // ##########
	0xFF, 0xC0, // ##########

	// @1862 '`' (4 pixels wide)
	0x00, //
	0x00, //
	0xE0, // ###
	0x60, //  ##
	0x30, //   ##
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @1881 'a' (8 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x7E, //  ######
	0xFF, // ########
	0x87, // #    ###
	0x7F, //  #######
	0xFF, // ########
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xFF, // ########
	0x7F, //  #######
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @1900 'b' (9 pixels wide)
	0x00, 0x00, //
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xEE, 0x00, // ### ###
	0xFF, 0x00, // ########
	0xF3, 0x80, // ####  ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xF3, 0x80, // ####  ###
	0xFF, 0x00, // ########
	0xEE, 0x00, // ### ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1938 'c' (7 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x3C, //   ####
	0x7E, //  ######
	0xF2, // ####  #
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xF2, // ####  #
	0x7E, //  ######
	0x3C, //   ####
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @1957 'd' (9 pixels wide)
	0x00, 0x00, //
	0x03, 0x80, //       ###
	0x03, 0x80, //       ###
	0x03, 0x80, //       ###
	0x03, 0x80, //       ###
	0x03, 0x80, //       ###
	0x3B, 0x80, //   ### ###
	0x7F, 0x80, //  ########
	0xE7, 0x80, // ###  ####
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE7, 0x80, // ###  ####
	0x7F, 0x80, //  ########
	0x3B, 0x80, //   ### ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1995 'e' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x3E, 0x00, //   #####
	0x7F, 0x00, //  #######
	0xE3, 0x80, // ###   ###
	0xFF, 0x80, // #########
	0xFF, 0x80, // #########
	0xE0, 0x00, // ###
	0xF0, 0x80, // ####    #
	0x7F, 0x80, //  ########
	0x3F, 0x00, //   ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2033 'f' (6 pixels wide)
	0x00, //
	0x3C, //   ####
	0x7C, //  #####
	0x70, //  ###
	0x70, //  ###
	0x70, //  ###
	0xFC, // ######
	0xFC, // ######
	0x70, //  ###
	0x70, //  ###
	0x70, //  ###
	0x70, //  ###
	0x70, //  ###
	0x70, //  ###
	0x70, //  ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2052 'g' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x3F, 0x80, //   #######
	0x7F, 0x80, //  ########
	0x63, 0x00, //  ##   ##
	0x63, 0x00, //  ##   ##
	0x7F, 0x00, //  #######
	0x7E, 0x00, //  ######
	0x60, 0x00, //  ##
	0x7F, 0x00, //  #######
	0x3F, 0x80, //   #######
	0x63, 0x80, //  ##   ###
	0xE3, 0x80, // ###   ###
	0xFF, 0x00, // ########
	0x3E, 0x00, //   #####

	// @2090 'h' (8 pixels wide)
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xEE, // ### ###
	0xFF, // ########
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2109 'i' (3 pixels wide)
	0x00, //
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2128 'j' (5 pixels wide)
	0x00, //
	0x00, //
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x00, //
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0xF8, // #####
	0xF0, // ####

	// @2147 'k' (8 pixels wide)
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE7, // ###  ###
	0xEE, // ### ###
	0xEE, // ### ###
	0xFC, // ######
	0xFC, // ######
	0xEE, // ### ###
	0xEE, // ### ###
	0xEF, // ### ####
	0xE7, // ###  ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2166 'l' (3 pixels wide)
	0x00, //
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2185 'm' (14 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0xEF, 0x38, // ### ####  ###
	0xFF, 0xFC, // ##############
	0xF3, 0x9C, // ####  ###  ###
	0xE3, 0x9C, // ###   ###  ###
	0xE3, 0x9C, // ###   ###  ###
	0xE3, 0x9C, // ###   ###  ###
	0xE3, 0x9C, // ###   ###  ###
	0xE3, 0x9C, // ###   ###  ###
	0xE3, 0x9C, // ###   ###  ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2223 'n' (8 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0xEE, // ### ###
	0xFF, // ########
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2242 'o' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x3E, 0x00, //   #####
	0x7F, 0x00, //  #######
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0x7F, 0x00, //  #######
	0x3E, 0x00, //   #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2280 'p' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0xEE, 0x00, // ### ###
	0xFF, 0x00, // ########
	0xF3, 0x80, // ####  ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xF7, 0x80, // #### ####
	0xFF, 0x00, // ########
	0xEE, 0x00, // ### ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###
	0xE0, 0x00, // ###

	// @2318 'q' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x3B, 0x80, //   ### ###
	0x7F, 0x80, //  ########
	0xE7, 0x80, // ###  ####
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE3, 0x80, // ###   ###
	0xE7, 0x80, // ###  ####
	0x7F, 0x80, //  ########
	0x3B, 0x80, //   ### ###
	0x03, 0x80, //       ###
	0x03, 0x80, //       ###
	0x03, 0x80, //       ###
	0x03, 0x80, //       ###

	// @2356 'r' (6 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0xFC, // ######
	0xFC, // ######
	0xF0, // ####
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0xE0, // ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2375 's' (7 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x78, //  ####
	0xFC, // ######
	0xC4, // ##   #
	0xF0, // ####
	0x7C, //  #####
	0x1E, //    ####
	0x8E, // #   ###
	0xFE, // #######
	0x7C, //  #####
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2394 't' (7 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x38, //   ###
	0x38, //   ###
	0xFE, // #######
	0xFE, // #######
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x38, //   ###
	0x3E, //   #####
	0x1E, //    ####
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2413 'u' (8 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xFF, // ########
	0x77, //  ### ###
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2432 'v' (8 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0xE7, // ###  ###
	0xE7, // ###  ###
	0xE7, // ###  ###
	0x67, //  ##  ###
	0x76, //  ### ##
	0x76, //  ### ##
	0x36, //   ## ##
	0x3C, //   ####
	0x3C, //   ####
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2451 'w' (15 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0xF3, 0x8E, // ####  ###   ###
	0x73, 0x8C, //  ###  ###   ##
	0x73, 0x8C, //  ###  ###   ##
	0x76, 0xDC, //  ### ## ## ###
	0x36, 0xD8, //   ## ## ## ##
	0x36, 0xD8, //   ## ## ## ##
	0x36, 0xD8, //   ## ## ## ##
	0x3C, 0x70, //   ####   ###
	0x1C, 0x70, //    ###   ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2489 'x' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x73, 0x00, //  ###  ##
	0x77, 0x00, //  ### ###
	0x36, 0x00, //   ## ##
	0x3E, 0x00, //   #####
	0x1C, 0x00, //    ###
	0x3E, 0x00, //   #####
	0x36, 0x00, //   ## ##
	0x77, 0x00, //  ### ###
	0xF7, 0x80, // #### ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2527 'y' (10 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0xE3, 0xC0, // ###   ####
	0x73, 0x80, //  ###  ###
	0x73, 0x80, //  ###  ###
	0x3B, 0x80, //   ### ###
	0x3B, 0x00, //   ### ##
	0x3B, 0x00, //   ### ##
	0x1F, 0x00, //    #####
	0x1E, 0x00, //    ####
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0C, 0x00, //     ##
	0x1C, 0x00, //    ###
	0x1C, 0x00, //    ###

	// @2565 'z' (7 pixels wide)
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0xFE, // #######
	0xFE, // #######
	0x0E, //     ###
	0x1C, //    ###
	0x38, //   ###
	0x70, //  ###
	0x60, //  ##
	0xFE, // #######
	0xFE, // #######
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	// @2584 '{' (6 pixels wide)
	0x00, //
	0x1C, //    ###
	0x3C, //   ####
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x70, //  ###
	0xE0, // ###
	0xE0, // ###
	0x70, //  ###
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x3C, //   ####
	0x1C, //    ###
	0x00, //

	// @2603 '|' (2 pixels wide)
	0x00, //
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##

	// @2622 '}' (6 pixels wide)
	0x00, //
	0xE0, // ###
	0xF0, // ####
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x1C, //    ###
	0x1C, //    ###
	0x38, //   ###
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0x30, //   ##
	0xF0, // ####
	0xE0, // ###
	0x00, //

	// @2641 '~' (9 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x71, 0x80, //  ###   ##
	0xF9, 0x80, // #####  ##
	0xCF, 0x80, // ##  #####
	0xC7, 0x00, // ##   ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
};

// Character descriptors for Calibri 15pt
// { [Char width in bits], [Offset into calibri_15ptCharBitmaps in bytes] }
const UInt16 calibri_15ptDescriptors[] =
{
		3, 0, 		// !
		5, 19, 		// "
		9, 38, 		// #
		8, 76, 		// $
		14, 95, 		// %
		13, 133, 		// &
		2, 171, 		// '
		5, 190, 		// (
		5, 209, 		// )
		8, 228, 		// *
		8, 247, 		// +
		4, 266, 		// ,
		5, 285, 		// -
		3, 304, 		// .
		9, 323, 		// /
		9, 361, 		// 0
		8, 399, 		// 1
		8, 418, 		// 2
		8, 437, 		// 3
		9, 456, 		// 4
		8, 494, 		// 5
		9, 513, 		// 6
		9, 551, 		// 7
		9, 589, 		// 8
		9, 627, 		// 9
		3, 665, 		// :
		4, 684, 		// ;
		8, 703, 		// <
		8, 722, 		// =
		8, 741, 		// >
		7, 760, 		// ?
		16, 779, 		// @
		13, 817, 		// A
		10, 855, 		// B
		9, 893, 		// C
		11, 931, 		// D
		8, 969, 		// E
		7, 988, 		// F
		11, 1007, 		// G
		10, 1045, 		// H
		3, 1083, 		// I
		6, 1102, 		// J
		9, 1121, 		// K
		7, 1159, 		// L
		15, 1178, 		// M
		10, 1216, 		// N
		12, 1254, 		// O
		9, 1292, 		// P
		13, 1330, 		// Q
		10, 1368, 		// R
		8, 1406, 		// S
		11, 1425, 		// T
		10, 1463, 		// U
		12, 1501, 		// V
		18, 1539, 		// W
		11, 1596, 		// X
		9, 1634, 		// Y
		9, 1672, 		// Z
		4, 1710, 		// [
		9, 1729, 		//
		4, 1767, 		// ]
		10, 1786, 		// ^
		10, 1824, 		// _
		4, 1862, 		// `
		8, 1881, 		// a
		9, 1900, 		// b
		7, 1938, 		// c
		9, 1957, 		// d
		9, 1995, 		// e
		6, 2033, 		// f
		9, 2052, 		// g
		8, 2090, 		// h
		3, 2109, 		// i
		5, 2128, 		// j
		8, 2147, 		// k
		3, 2166, 		// l
		14, 2185, 		// m
		8, 2223, 		// n
		9, 2242, 		// o
		9, 2280, 		// p
		9, 2318, 		// q
		6, 2356, 		// r
		7, 2375, 		// s
		7, 2394, 		// t
		8, 2413, 		// u
		8, 2432, 		// v
		15, 2451, 		// w
		9, 2489, 		// x
		10, 2527, 		// y
		7, 2565, 		// z
		6, 2584, 		//
		2, 2603, 		// |
		6, 2622, 		//
		9, 2641, 		// ~
};

// Font information for Calibri 15pt
const mlsLcdFontInfo_t calibri_15ptFontInfo =
{
	19, //  Character height
	'!', //  Start character
	'~', //  End character
	25,
	31,
	calibri_15ptDescriptors, //  Character descriptor array
	calibri_15ptBitmaps, //  Character bitmap array
};

// Font data for Calibri 15pt
extern const UInt8 calibri_15ptBitmaps[];
extern const mlsLcdFontInfo_t calibri_15ptFontInfo;
extern const UInt16 calibri_15ptDescriptors[];

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_STYL_MLSLCD_INC_MLSCALIBRI15PTS_H_ */

/**@}*/
