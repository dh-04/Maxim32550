/*
 * tft.c --
 *
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012, Maxim Integrated
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

#include "MAX325xx.h"
#include "NHD-3.5-320240MF.h"

#define LCD_S_DATA	27
#define LCD_S_CLOCK	0
#define LCD_CS		29

/** LQ043T1DG01 */
tft_timings_t panel =
{
	.width       = 320,
	.height      = 240,
	.frequency   = 6400000,

	.vfrontporch = 13,
	.vbackporch  = 10,
	.vsyncwidth  = 3,
	.hfrontporch = 70,
	.hbackporch  = 18,
	.hsyncwidth  = 52,
};

/******************************************************************************/
/**
 * Displays the Maxim LOGO on the TFT panel
 */
void tft_config(void)
{
	mml_gpio_config_t 			config_bank1;
	int 					result;

	/** Disable the panel */
	CLCD->CTRL = 0;
	CLCD->CLK = 0;

	/** Enable TFT IOs functions */
	config_bank1.gpio_direction = MML_GPIO_DIR_OUT;
	config_bank1.gpio_function = MML_GPIO_TERTIARY_ALT_FUNCTION;
	config_bank1.gpio_pad_config = MML_GPIO_PAD_NORMAL;
	config_bank1.gpio_intr_mode = 0;
	config_bank1.gpio_intr_polarity = 0;
	result = mml_gpio_init(MML_GPIO_DEV1,	2,	27, config_bank1);
	if ( result ) {
		/** Oops, I did it again ... */
		goto main_out;
	}

	//mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_PAD_PULLUP, 0, 32);
	/** CLCD_CLK */
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 2, 1);
	/** CLCD_HSYNC */
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 3, 1);
	/** CLCD_VSYNC */
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 4, 1);
	/** CLCD_VDEN */
	//mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 5, 1);
	/** CLCD_GREEN0 */
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 6, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 7, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 8, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 9, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 10, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 11, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 12, 1);
	/** CLCD_GREEN7 */
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 13, 1);

	/** CLCD_BLUE0 */
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 14, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 15, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 16, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 17, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 18, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 19, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 20, 1);
	/** CLCD_BLUE7 */
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 21, 1);

	/** CLCD_RED0 */
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 22, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 23, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 24, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 25, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 26, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 27, 1);
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 28, 1);
	/** CLCD_RED7 */
	mml_gpio_configure_function(MML_GPIO_DEV1, MML_GPIO_TERTIARY_ALT_FUNCTION, 29, 1);

	/** Configure the panel */
	CLCD->CLK = ( (CLCD_CLK_EDGE_negEdge << CLCD_CLK_EDGE_Pos) | ( CLCD_CLK_HPOL_activeHi << CLCD_CLK_HPOL_Pos)
						| (CLCD_CLK_VPOL_activeHi << CLCD_CLK_VPOL_Pos) | (CLCD_CLK_DPOL_activeHi << CLCD_CLK_DPOL_Pos)
						| ((SYSTEM_FREQ / panel.frequency) << CLCD_CLK_CLKDIV_Pos));

	CLCD->VTIM_0 = (panel.vbackporch << CLCD_VTIM0_VBACKPORCH_SHIFT) | (panel.height - 1);
	CLCD->VTIM_1 = (panel.vfrontporch << CLCD_VTIM1_VFRONTPORCH_SHIFT) | ((panel.vsyncwidth - 1) << CLCD_VTIM1_VSYNCWIDTH_SHIFT);

	CLCD->HTIM = ((panel.hfrontporch - 1) << CLCD_HTIM_HFRONTPORCH_SHIFT) |
					   ((panel.hbackporch - 1) << CLCD_HTIM_HBACKPORCH_SHIFT) |
					   ((panel.hsyncwidth - 1) << CLCD_HTIM_HSYNCWIDTH_SHIFT) |
					   (((panel.width >> 4) - 1) << CLCD_HTIM_HLINES_SHIFT);

	CLCD->FRBUF_0 = IRAM_BUFFER_BASE;

	CLCD->INT_EN = 0;

	/** Enable panel */
	CLCD->CTRL = 	(CLCD_CTRL_BPP8 << CLCD_CTRL_BPP_SHIFT) |
				(TFT << CLCD_CTRL_DISPTYPE_SHIFT) |
				(CLCD_CTRL_PEN_enable << CLCD_CTRL_PEN_Pos);

main_out:
	/** We're done */
	return;
}

void tft_poweron_only() {
	CLCD->CTRL |= (CLCD_CTRL_PEN_enable << CLCD_CTRL_LCDEN_Pos );
}

void tft_poweroff_only() {
	CLCD->CTRL &= ~(CLCD_CTRL_PEN_enable << CLCD_CTRL_LCDEN_Pos );
}

void set_mode(int mode)
{
	//disable LCD
	CLCD->CTRL &= ~(CLCD_CTRL_PEN_enable << CLCD_CTRL_LCDEN_Pos );
	switch (mode) {
	case 8:
	    CLCD->CTRL &= ~(7 << CLCD_CTRL_BPP_SHIFT); //clear all bit of BPP info
	    CLCD->CTRL |= (CLCD_CTRL_BPP8 << CLCD_CTRL_BPP_SHIFT); //clear all bit of BPP info
	    break;
	case 4:
	    CLCD->CTRL &= ~(7 << CLCD_CTRL_BPP_SHIFT); //clear all bit of BPP info
	    CLCD->CTRL |= (CLCD_CTRL_BPP4 << CLCD_CTRL_BPP_SHIFT); //clear all bit of BPP info
	    break;
	}
	//enable LCD
	CLCD->CTRL |= 	(CLCD_CTRL_PEN_enable << CLCD_CTRL_LCDEN_Pos );
}

/******************************************************************************/
/*  Maximum clock speed is 3.125MHz
    The core is running at 108MHz
    With the function call overhead, 1 nop is sufficient.
    TODO - FIXME: This is tuned by hand with cache disabled
*/
static void __attribute__ ((noinline)) halfClockDelay(void)
{
    __asm volatile ("nop\n");
}


//******************************************************************************
static unsigned char clcdSerial(unsigned char addr, unsigned char write, unsigned char outdata)
{
    unsigned int i;
    unsigned char indata = 0;

    // Assert chip enable
    GPIO0->OUT_CLR = (1 << LCD_CS);
    halfClockDelay();

    // Clock out 6-bit address
    if (addr & 0x20) {
        GPIO0->OUT_SET = (1 << LCD_S_DATA);
    } else {
        GPIO0->OUT_CLR = (1 << LCD_S_DATA);
    }
    GPIO0->OUT_EN |= (1 << LCD_S_DATA);

    for (i = 0; i < 6; i++) {
        // Output data on falling edge
        GPIO1->OUT_CLR = (1 << LCD_S_CLOCK);
        if (addr & 0x20)
            GPIO0->OUT_SET = (1 << LCD_S_DATA);
        else
            GPIO0->OUT_CLR = (1 << LCD_S_DATA);
        halfClockDelay();

        GPIO1->OUT_SET = (1 << LCD_S_CLOCK);
        halfClockDelay();
        addr <<= 1;
    }

    // Clock out 1-bit read/write
    GPIO1->OUT_CLR = (1 << LCD_S_CLOCK);
    if (write)
        GPIO0->OUT_SET = (1 << LCD_S_DATA);
    else
        GPIO0->OUT_CLR = (1 << LCD_S_DATA);
    halfClockDelay();

    GPIO1->OUT_SET = (1 << LCD_S_CLOCK);
    halfClockDelay();


    // Clock 1-bit Hi-Z
    GPIO1->OUT_CLR = (1 << LCD_S_CLOCK);
    if (!write) {
        // Disable output driver
        GPIO0->OUT_EN &= ~(1 << LCD_S_DATA);
    }
    halfClockDelay();

    GPIO1->OUT_SET = (1 << LCD_S_CLOCK);
    halfClockDelay();

    if (write) {
        // Clock out the write data
        for (i = 0; i < 8; i++) {
            // Output data on falling edge
            GPIO1->OUT_CLR = (1 << LCD_S_CLOCK);
            if (outdata & 0x80)
                GPIO0->OUT_SET = (1 << LCD_S_DATA);
            else
                GPIO0->OUT_CLR = (1 << LCD_S_DATA);
            halfClockDelay();

            GPIO1->OUT_SET = (1 << LCD_S_CLOCK);
            halfClockDelay();
            outdata <<= 1;
        }
    } else {
        // Clock in the read data
        for (i = 0; i < 8; i++) {
            GPIO1->OUT_CLR = (1 << LCD_S_CLOCK);
            halfClockDelay();

            // Latch data at the rising edge of SPCK
            GPIO1->OUT_SET = (1 << LCD_S_CLOCK);
            indata <<= 1;
            indata |= !!(GPIO0->IN & (1 << LCD_S_DATA));
            halfClockDelay();
        }
    }

    halfClockDelay();
    GPIO0->OUT_SET = (1 << LCD_CS);
    GPIO0->OUT_EN &= ~(1 << LCD_S_DATA);

    // Observe chip select distinguish time
    halfClockDelay();
    halfClockDelay();

    return indata;
}

int tft_extra_config(void)
{
    unsigned char regval;
    unsigned int i;

    // LCD reset
    GPIO0->OUT_CLR   =  (1 << 28);  // assert reset
    GPIO0->OUT_EN   |=  (1 << 28);
    GPIO0->PAD_CFG1 &= ~(1 << 28);
    GPIO0->PAD_CFG2 &= ~(1 << 28);
    GPIO0->EN       |=  (1 << 28);

    // LCD serial interface
    GPIO0->OUT_SET   =  (1 << LCD_CS);
    GPIO0->OUT_EN   |=  (1 << LCD_CS);
    GPIO0->PAD_CFG1 &= ~(1 << LCD_CS);
    GPIO0->PAD_CFG2 &= ~(1 << LCD_CS);
    GPIO0->EN       |=  (1 << LCD_CS);

    GPIO1->OUT_SET   =  (1 << LCD_S_CLOCK);
    GPIO1->OUT_EN   |=  (1 << LCD_S_CLOCK);
    GPIO1->PAD_CFG1 &= ~(1 << LCD_S_CLOCK);
    GPIO1->PAD_CFG2 &= ~(1 << LCD_S_CLOCK);
    GPIO1->EN       |=  (1 << LCD_S_CLOCK);
    GPIO1->EN1      &= ~(1 << LCD_S_CLOCK);

    GPIO0->PAD_CFG1 &= ~(1 << LCD_S_DATA);
    GPIO0->PAD_CFG2 &= ~(1 << LCD_S_DATA);
    GPIO0->EN       |=  (1 << LCD_S_DATA);

    // Hold LCD reset for min 40 us then release
    // TODO - FIXME: This is tuned by hand with cache disabled
    for (i = 0; i < 100; i++) {
        halfClockDelay();
    }
    GPIO0->OUT_SET = (1 << 28);
    halfClockDelay();

    // Read the System Control Register to confirm communication
    regval = clcdSerial(0x00, 0, 0x00);
    if ( (regval == 0xFF) || (regval == 0x00) ) {
        return COMMON_ERR_UNKNOWN;
    }

    /** Configure TFT for no flicking -- From Dallas Team  */
    // Adjust VCOM per datasheet
    clcdSerial(0x0E, 1, 0x6B);
    // Adjust VGL per datasheet
    clcdSerial(0x0F, 1, 0x24);

    return NO_ERROR;
}

/* EOF */
