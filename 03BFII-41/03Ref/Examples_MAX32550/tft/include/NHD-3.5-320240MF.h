/*
 ******************************************************************************
 * Copyright (C) 2018 Maxim Integrated Products, Inc., All rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MAXIM INTEGRATED ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TFT_H_
#define _TFT_H_

#include "MAX325xx.h"

/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int __fbram_start;
#define IRAM_BUFFER_BASE				((uint32_t)&__fbram_start) // frame buffer ram
#define	SYSTEM_FREQ						54000000
#define	MML_TDC_CLKR_CLKDIV				0x00

#define	CLCD_VTIM0_VBACKPORCH_SHIFT 	16
#define CLCD_VTIM1_VSYNCWIDTH_SHIFT  	0
#define CLCD_VTIM1_VFRONTPORCH_SHIFT 	16
#define CLCD_HTIM_HSYNCWIDTH_SHIFT  	0
#define CLCD_HTIM_HSYNCWIDTH_MASK   	(0xFF << CLCD_HTIM_HSYNCWIDTH_SHIFT)
#define CLCD_HTIM_HFRONTPORCH_SHIFT 	8
#define CLCD_HTIM_HFRONTPORCH_MASK  	(0xFF << CLCD_HTIM_HFRONTPORCH_SHIFT)
#define CLCD_HTIM_HLINES_SHIFT      	16
#define CLCD_HTIM_HLINES_MASK       	(0xFF << CLCD_HTIM_HLINES_SHIFT)
#define CLCD_HTIM_HBACKPORCH_SHIFT  	24
#define CLCD_HTIM_HBACKPORCH_MASK   	(0xFF << CLCD_HTIM_HBACKPORCH_SHIFT)
#define CLCD_CTRL_VISEL_SHIFT    		1
#define CLCD_CTRL_VISEL_MASK     		(3 << CLCD_CTRL_VISEL_SHIFT)
#define CLCD_CTRL_DISPTYPE_SHIFT 		4
#define CLCD_CTRL_DISPTYPE_MASK  		(15 << CLCD_CTRL_DISPTYPE_SHIFT)
#define CLCD_CTRL_BPP_SHIFT      		8
#define CLCD_CTRL_BPP1           		0
#define CLCD_CTRL_BPP2           		1
#define CLCD_CTRL_BPP4           		2
#define CLCD_CTRL_BPP8           		3
#define CLCD_CTRL_BPP16          		4
#define CLCD_CTRL_BPP24          		5

typedef struct _tft_timings
{
	int width;
	int height;
	int frequency;
	int vfrontporch;
	int vbackporch;
	int vsyncwidth;
	int hfrontporch;
	int hbackporch;
	int hsyncwidth;
} tft_timings_t;

typedef enum display_type
{
	STN_MONO_4 = 0,
	STN_MONO_4_DUAL,
	STN_NONO_8,
	STN_MONO_8_BUAL,
	STN_COLOR_8,
	STN_COLOR_8_DUAL,
	TFT = 8,
	HRTFT,
} display_type_t;

int  tft_extra_config(void);
void tft_config(void);
void tft_poweron_only(void);
void tft_poweroff_only(void);
int  enable_backlight(void);
void set_mode(int mode);

#ifdef __cplusplus
}
#endif

#endif /* _TFT_H_ */
/******************************************************************************/
/* EOF */
