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
 *     * Neither the name of the Maxim Integrated nor the
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

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "bitmap.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _area
{
	int		x;
	int		y;
	int		w;
	int		h;
}area_t;

typedef struct _text
{
	char	*data;
	int 	len;
}text_t;

typedef struct _display
{
    int mode;
    int width;
    int height;
}display_t;

void set_display(int m,int w,int h);
void set_background(unsigned char index_color);
void print_font(int x0, int y0, int font_id, text_t *str, area_t *area);
void clear_area( area_t *area, int color);
void write_bitmap(int x0, int y0, int id);

#ifdef __cplusplus
}
#endif

#endif /* FRAMEBUFFER_H_ */
