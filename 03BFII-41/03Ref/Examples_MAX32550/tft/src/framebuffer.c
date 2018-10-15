/*
 * framebuffer.c --
 *
 * ----------------------------------------------------------------------------
 * Copyright (c) 2014, Maxim Integrated
 * All rights reserved.
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

#include <string.h>
#include "MAX325xx.h"
#include "framebuffer.h"
#include "NHD-3.5-320240MF.h"

display_t display;

#define PALETTE_OFFSET(x) 	concat(images_start_addr + images_header.offset2info_palatte  + 1 /* nb_palette */ + (x)*sizeof(unsigned int), 4)
#define FONT_OFFSET(x) 		concat(images_start_addr + images_header.offset2info_font 	  + 1 /* nb_font    */ + (x)*sizeof(unsigned int), 4)
#define BITMAP_OFFSET(x) 	concat(images_start_addr + images_header.offset2info_bitmap   + 1 /* nb_bitmap  */ + (x)*sizeof(unsigned int), 4)

#pragma pack (1)

typedef struct
{
	unsigned int  w;
	unsigned int  h;
	unsigned char id_palatte;
	unsigned char rle;
	unsigned int  data_size;
} bitmap_info_t;

typedef struct
{
	unsigned short x;
	unsigned char  w;
} font_char_t;

typedef struct
{
	//unsigned char size;
	unsigned char nb_char;
	unsigned char bitmap_id;
	//font_char_t	 *char_info; // X nb_char
} font_info_t;

typedef struct {
	//unsigned short 	type;                        /* Magic identifier               */
	//unsigned short 	reserved1, reserved2;
	//unsigned short 	bits;                        /* Bits per pixel                 */
	//
	unsigned int	offset2info_palatte;
 	unsigned int	offset2info_font;
 	unsigned int	offset2info_bitmap;
 	//
	unsigned int 	nb_palette;
	unsigned int 	nb_font;
	unsigned int 	nb_bitmap;
} Header_images_t;

#pragma pack ()

extern unsigned int 	_bin_start_;
static unsigned char 	*images_start_addr = NULL;
static Header_images_t 	images_header;

/******************************************************************************/
static int concat(unsigned char* var, int size)
{
	int result = 0;

	for(int i = 1; i <= size; i++)
		result |= var[size - i] << (8 * (size - i));

	return result;
}

static void RLE_decode( unsigned char const *in, unsigned int length,
      					unsigned char *out, int width, int height, int x0, int y0)
{
	(void) width;
	(void) y0;

	unsigned char cmd, data, x, y;
	unsigned int  i, j, inpos, outpos;

	if( length < 1 ) {
	  return;
	}

	/* Get marker symbol from input stream */
	inpos = 0;

  	/* Main decompression loop */
 	outpos = x0 * (height - 1);

	do {
		cmd = in[ inpos++ ];
		data = in[ inpos++ ];
		if (cmd == 0x00) {
			switch (data) {
			case 0:
				j = outpos%x0;
				if (j) {
					for (i=j; (int)i<x0; i++)
						outpos++;
				}
				outpos -= 2*x0;
				break;
			case 1: //end of image
			  	return;
				break;
			case 2:
				x = in[ inpos++ ];
				y = in[ inpos++ ];
				for (i=0; i<x; i++)
					outpos++;

				for (i=0; i<y; i++){
					for (j=0; (int)j<x0; j++)
					  outpos++;
				}
				break;
			default:
				for (i=0; i<data; i++)
				  out[outpos++] = in[ inpos++ ];
				if (data%2)
				  inpos ++;
				break;
			}
		} else {
		  for (i=0;i<cmd;i++)
			out[outpos++] = data;
		}
	} while( inpos < length );
}

static void set_palette(unsigned int id)
{
	int i;
	unsigned char *palette;


	if (id > images_header.nb_palette) {
		return;
	}

	/* set palette only if it was changed */
	palette = (unsigned char *)(images_start_addr + PALETTE_OFFSET(id));
	/* set palette only if it was changed */
	for( i = 0; i < 16; i++) { //only test the first 16
		if( CLCD->PALETTE_RAM[i] != (unsigned int) (*(palette+2)) + (*(palette+1) << 8) + ((*palette) << 16)  )
			goto setup_palette;
		palette += 4;
	}

	return;

	setup_palette:
	/** Setup Palette */
	for(i = 0; i < 256; i++) {
		CLCD->PALETTE_RAM[i] = (unsigned int) (*(palette+2)) + (*(palette+1) << 8) + ((*palette) << 16) ;
		palette += 4;
	}

	//for( ; i < 256; i++)
	//	CLCD->PALETTE_RAM[i] = 0x00FFFFFF;
}

static void write_sub_bitmap(int x0, int y0, int img_w, int img_h, const unsigned char *img_data, int sub_x, int sub_w)
{
	unsigned char 	*dest;
	int 			img_w_rounded = ((8 * img_w + 31) / 32) * 4;
	int 			i;

	if (x0 >= display.width) {
		return;
	}

	if ( (x0 + sub_w) > display.width)
		sub_w = display.width - x0;
	if ( (y0 + img_h) > display.height)
		img_h = display.height - y0;

	dest  = (unsigned char *)(IRAM_BUFFER_BASE + y0*display.width + x0);
	for (i = img_h-1; i > 0; i--) {
		memcpy(dest, &img_data[i*img_w_rounded + sub_x], sub_w);
		dest += display.width;
	}
}

/******************************************************************************/
void set_display(int m,int w,int h)
{
	if (images_start_addr == NULL) {
		images_start_addr = (unsigned char *)&_bin_start_;
	}

	memset(&images_header, 0, sizeof(Header_images_t));
	memcpy(&images_header, images_start_addr, sizeof(Header_images_t));

    set_mode(m);
    display.mode = m;
    display.width = w;
    display.height = h;
}

void set_background(unsigned char index_color)
{
    switch (display.mode) {
		case 8:
			memset((unsigned char *)(IRAM_BUFFER_BASE), index_color, display.width * display.height);
			break;
		case 4:
			memset((unsigned char *)(IRAM_BUFFER_BASE), ((index_color<<4) + index_color), display.width * display.height);
			break;
    }
}

void write_bitmap(int x0, int y0, int id)
{
	unsigned char 		*dest;
	unsigned int  		bitmap_offset;
	bitmap_info_t 		bitmap_info;
	unsigned char const *pixel;

	if((unsigned int)id >= images_header.nb_bitmap)
		return;

	bitmap_offset = BITMAP_OFFSET(id);
	memcpy(&bitmap_info, (images_start_addr + bitmap_offset), sizeof(bitmap_info));
	pixel = (unsigned char *)((images_start_addr + bitmap_offset + sizeof(bitmap_info_t)));

	set_palette(bitmap_info.id_palatte);
	if (bitmap_info.rle == 1) {
		int x0c    = x0;
		int xd     = bitmap_info.w;
		int yd     = bitmap_info.h;
		int size   = bitmap_info.data_size;

		if (display.mode == 4) {
		    xd >>= 1;
		    x0c >>= 1;
		}

		if ( (x0c + xd) > display.width)
			xd = display.width - x0c;
		if ( (y0 + yd) > display.height)
			yd = display.height - y0;

		dest  = (unsigned char *)(IRAM_BUFFER_BASE + y0*display.width + x0c);
		RLE_decode(pixel, size, dest, xd, yd, display.width, display.height);
	} else {
		/* It is standard bitmap */
		write_sub_bitmap(x0, y0, bitmap_info.w, bitmap_info.h, pixel, x0, bitmap_info.w);
	}
}

void print_font(int x0, int y0, int id, text_t *str, area_t *area)
{
	int 			i;
	int				x;
	bitmap_info_t 	bitmap_info;
	font_info_t 	font_info;
	unsigned int  	offset;
	font_char_t 	*chr_pos;
	unsigned char 	chId;
	unsigned char 	*pixel;

	if((unsigned int)id >= images_header.nb_font)
		return;

	offset = FONT_OFFSET(id);
	memcpy(&font_info, (unsigned char *)(images_start_addr + offset), sizeof(font_info_t));
	chr_pos = (font_char_t *)(images_start_addr + offset + sizeof(font_info_t));

	offset = BITMAP_OFFSET(font_info.bitmap_id);
	memcpy(&bitmap_info, (images_start_addr + offset), sizeof(bitmap_info_t));
	pixel    = (unsigned char *)((images_start_addr + offset + sizeof(bitmap_info_t)));

	x = x0;
	if (area) {
	    area->x = x0;
	    area->y = y0;
	    area->h = bitmap_info.h;
	}
	for (i=0; i<str->len; i++) {
		if ((str->data[i] < '!') || (str->data[i] > '~'))
		    x+= 8; //font.space; // TODO add space in font bitmap file
		else {
			chId = str->data[i]-'!';
			write_sub_bitmap(x, y0, bitmap_info.w, bitmap_info.h, pixel, chr_pos[chId].x, chr_pos[chId].w);
		    x += chr_pos[chId].w + 1;// font.intr_chr;
		}
	}
	if (area)
	    area->w = x - area->x;
}

void clear_area( area_t *area, int color)
{
    unsigned char *dest;
    int i, h, w;

    w = area->w;
    h = area->h;
    if ( (area->x + w) > display.width)
	    w = display.width - area->x;
    if ( (area->y + h) > display.height)
	    h = display.height - area->y;

    dest  = (unsigned char *)(IRAM_BUFFER_BASE + area->y*display.width + area->x);
    for (i = 0; i < h; i++, dest+=display.width) {
	    memset(dest, color, w);
    }
}

/* EOF */
