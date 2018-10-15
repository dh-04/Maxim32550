/*
 * mlsLCD.c
 *
 *  Created on: 28 Sep 2017
 *      Author: Dawn
 */
/*
 * lcd.c
 *
 *  Created on: Sep 22, 2017
 *      Author: 
 */
#include "../mlsLCD/inc/mlsLCDDriver.h"
//#include "../mlsQRCode/inc/mlsQRBuffer.h"
#include "../mlsLCD/inc/mlsLCDFont.h"
//#include "../mlsRfApi/inc/mlsRfApi.h"

#include <mml_gpio.h>
//#include <mml_gpio_regs.h>
#include <mml_tmr.h>
#include <mml_gcr.h>

#include <stddef.h>
#include <string.h>

#include "Type.h"

#define LCD_BACK_LIGHT_TIMER	MML_TMR_DEV1
#define LCD_BACK_LIGHT_IO_PIN	(1 << 1)
#define LCD_LIGHT_PERCENT		50

#define LCD_A0_PIN			30
#define LCD_A0_PORT			MML_GPIO_DEV0
#define LCD_CS_PIN			28
#define LCD_CS_PORT			MML_GPIO_DEV0
#define LCD_SPI_DEVID		MML_SPI_DEV1
#define LCD_COMMAND_GRAM	0x2C
#define LCD_COMMAND_MAC		0x36
#define LCD_CHAR_SPACE		5
#define X_OFFSET			55
#define Y_OFFSET			10

#define LCD_CLR_A0() 	(mml_gpio_write_bit_pattern(LCD_A0_PORT, LCD_A0_PIN, 1, 0))
#define LCD_SET_A0()     (mml_gpio_write_bit_pattern(LCD_A0_PORT, LCD_A0_PIN, 1, 1))

extern volatile mml_dma_regs_t* dmaRegs;// = (volatile mml_dma_regs_t*)(MML_DMA_IOBASE);
extern mlsMutexHandle_t gLcdMutex;
mlsMutexHandle_t gMaximDMAMutex;
typedef enum {
	LCDLandscape,
	LCDPortrait
} mlsLCD_Orientation;
typedef struct {
	UInt16 width;
	UInt16 height;
	UInt8 orientation; // 1 = portrait; 0 = landscape
} mlsLCDOptions_t;

mlsLCDOptions_t gLCD_opt;

UInt16 gLCDx;
UInt16 gLCDy;
UInt8 gLcddata[256];

mlsMutexHandle_t gLcdMutex;
static mlsLcdStringDataBlock_t gLcdDataBlock =
{
	.totalRow = 0,
};

static const UInt16 gLineMask[] =
{
	(UInt16) LINE1,
	(UInt16) LINE2,
	(UInt16) LINE3,
	(UInt16) LINE4,
};

/**
 * @fn mlsLCDSetCursorPosition
 * @brief This function set the coordinate to write data to LCD
 */
static mlsErrorCode_t mlsLCDSetCursorPosition(UInt16 x1, UInt16 y1, UInt16 x2, UInt16 y2);
/**
 * @fn mlsLCDSpiInit
 * @brief This function initialize SPI channel for LCD
 */
mlsErrorCode_t mlsLCDSpiInit(void);
/**
 * @fn mlsLCDDrawPixel
 * @brief This function draw a pixel on LCD
 */
static mlsErrorCode_t mlsLCDDrawPixel(UInt16 x, UInt16 y, UInt16 color);
/**
 * @fn mlsLCDWriteCommandByte
 * @brief This function send command byte
 */
static mlsErrorCode_t mlsLCDWriteCommandByte(UInt8 command);
/**
 * @fn mlsLCDWriteDataByte
 * @brief This function send data byte
 */
static mlsErrorCode_t mlsLCDWriteDataByte(UInt8 data);
/**
 * @fn mlsLCDWriteDataBuffer
 * @brief This function write many bytes of data
 */
static mlsErrorCode_t mlsLCDWriteDataBuffer(UInt8 *buffer, UInt16 length);
/**
 * @fn mlsLCDFill
 * @brief This function fill a rectangle by a color
 */
static mlsErrorCode_t mlsLCDFill(UInt16 x1, UInt16 y1,UInt16 x2, UInt16 y2,UInt16 color);
/**
 * @fn mlsLCDSetPixelBuffer
 * @brief This function set a pixel data to be written in databuffer
 */
static mlsErrorCode_t mlsLCDSetPixelBuffer(UInt16 index, UInt16 color);
/**
 * @fn mlsLCDPutc
 * @brief This function write a character from font
 */
static void mlsLCDPutc(UInt16 x, UInt16 y,
		char c, mlsLcdFontInfo_t *font,
		UInt16 foreground, UInt16 background, UInt16 space);

void mlsDelay(int microSecond);

#if(LCD_BACK_LIGHT_USE_PWM)
mlsErrorCode_t mlsLCDPWMInit(void)
{
	volatile mml_gcr_regs_t *mml_gcr = (mml_gcr_regs_t*)MML_GCR_IOBASE;
	volatile mml_gpio_regs_t *GPIO1 = (mml_gpio_regs_t*)MML_GPIO1_IOBASE;
	volatile mml_tmr_regs_t *mml_tmr = (volatile mml_tmr_regs_t*)MML_TMR1_IOBASE;

	mml_gcr_reset(0x2 << MML_GCR_RSTR_TIMERS_OFST);
	mml_tmr_interrupt_disable(LCD_BACK_LIGHT_TIMER);
	mml_tmr_disable(LCD_BACK_LIGHT_TIMER);

	GPIO1->out_clr = LCD_BACK_LIGHT_IO_PIN;  // assert reset
	GPIO1->out_en |= LCD_BACK_LIGHT_IO_PIN;

	/* set LCD back light pin as alternate function (PWM output)*/
	GPIO1->en &=  ~LCD_BACK_LIGHT_IO_PIN;
	GPIO1->en1 &=  ~LCD_BACK_LIGHT_IO_PIN;

	/* enable the clock */
	mml_gcr->perckcn &= ~( 1 << MML_PERCKCN_DEV_T1 );
	mml_tmr->control =  (MML_TMR_MODE_PWM | (MML_TMR_PRES_DIV_16 << MML_TMR_CN_PRES_OFST) | MML_TMR_CN_TPOL_MASK);
	return MLS_SUCCESS;
}

mlsErrorCode_t mlsLCDSetBacklight(int percent)
{
	volatile mml_tmr_regs_t *mml_tmr = (volatile mml_tmr_regs_t*)MML_TMR1_IOBASE;

	//enable timer
	mml_tmr->control &= ~MML_TMR_CN_TEN_MASK;
	mml_tmr->count = 1;
	mml_tmr->pwm = percent*10;
	mml_tmr->compare = 1000;

	mml_tmr->control |= MML_TMR_CN_TEN_MASK;

	return MLS_SUCCESS;
}
#endif /*LCD_BACK_LIGHT_USE_PWM*/

mlsErrorCode_t mlsLCDInit(void)
{
	mlsErrorCode_t errCode = MLS_SUCCESS;

#if(!LCD_BACK_LIGHT_USE_PWM)
	mlsErrorCode_t ret = MLS_SUCCESS;
	mml_gpio_config_t 	config;

	config.gpio_direction 		= MML_GPIO_DIR_OUT;
	config.gpio_function 		= MML_GPIO_NORMAL_FUNCTION;
	config.gpio_intr_mode 		= 0;
	config.gpio_intr_polarity 	= 0;
	config.gpio_pad_config 		= MML_GPIO_PAD_NORMAL;

	ret |= mml_gpio_init(MML_GPIO_DEV1, 1, 1, config);
	mml_gpio_write_bit_pattern(MML_GPIO_DEV1, 1, 1, 0x01) ;
#else
	mlsLCDPWMInit();
	mlsLCDSetBacklight(LCD_LIGHT_PERCENT);

#endif /*LCD_BACK_LIGHT_USE_PWM*/

	mlsOsalMutexCreate(&gLcdMutex, "LCD MUTEX");
	mlsLCDSpiInit();
	mlsLCDInitial_ST7789V();
	mlsLCDDrawScreen(LCD_BACKGROUND_COLOR/*LCD_WHITE*/);
	return errCode;
}

mlsErrorCode_t mlsLCDSpiInit(void)
{
	mlsErrorCode_t errCode = MLS_SUCCESS;

	mml_spi_params_t	spi_conf;

	/** Fill parameters */
	spi_conf.baudrate 		= 12000000;
	spi_conf.word_size 		= 8;
	spi_conf.mode 			= MML_SPI_MODE_MASTER;
	spi_conf.wor 			= MML_SPI_WOR_NOT_OPEN_DRAIN;
	spi_conf.clk_pol 		= MML_SPI_SCLK_LOW;
	spi_conf.phase 			= MML_SPI_PHASE_LOW;
	spi_conf.brg_irq 		= MML_SPI_BRG_IRQ_DISABLE;
	spi_conf.ssv 			= MML_SPI_SSV_HIGH;
	spi_conf.ssio 			= MML_SPI_SSIO_OUTPUT;
	spi_conf.tlj 			= MML_SPI_TLJ_DIRECT;
	spi_conf.dma_tx.active 	= MML_SPI_DMA_DISABLE;
	spi_conf.dma_rx.active 	= MML_SPI_DMA_DISABLE;

	/** Set CS0 */
	spi_conf.ssio = 0x1;

	/** Call initialization function from driver */
	if(mml_spi_init(LCD_SPI_DEVID, (mml_spi_params_t*)&spi_conf) == NO_ERROR)
	{
		/** Now enable SPI interface */
		M_MML_SPI_ENABLE(LCD_SPI_DEVID);
	}
	/*Channel Source Address*/
	dmaRegs->ch[MLS_SPI_DMA_CHANNEL_RX].src = 0;

	/*Enable Destination Increment*/
	dmaRegs->ch[MLS_SPI_DMA_CHANNEL_RX].cfg |= MML_DMA_CFG_DSTINC_MASK;

	/*Channel Destination Address*/
	dmaRegs->ch[MLS_SPI_DMA_CHANNEL_TX].dest = 0;

	/*Enable Source Increment*/
	dmaRegs->ch[MLS_SPI_DMA_CHANNEL_TX].cfg |= MML_DMA_CFG_SRCINC_MASK;


	/* Initialize GPIO */
	mml_gpio_config_t 	gpio_conf;

	/** Initialize GPIO port */
	gpio_conf.gpio_direction = MML_GPIO_DIR_OUT;
	gpio_conf.gpio_function = MML_GPIO_NORMAL_FUNCTION;
	gpio_conf.gpio_intr_mode = 0;
	gpio_conf.gpio_intr_polarity = 0;
	gpio_conf.gpio_pad_config = MML_GPIO_PAD_NORMAL;

	mml_gpio_init(LCD_A0_PORT, LCD_A0_PIN, 1, gpio_conf);
	mml_gpio_init(LCD_CS_PORT, LCD_CS_PIN, 1, gpio_conf);
	return errCode;
}
static mlsErrorCode_t mlsLCDWriteCommandByte(UInt8 command)
{
	LCD_CLR_A0();
	phbalReg_Stub_LCD_Dma(LCD_SPI_DEVID,&command,Null,1);
	return MLS_SUCCESS;
}
static mlsErrorCode_t mlsLCDWriteDataByte(UInt8 data)
{
	LCD_SET_A0();
	phbalReg_Stub_LCD_Dma(LCD_SPI_DEVID,&data,Null,1);
	return MLS_SUCCESS;
}
static mlsErrorCode_t mlsLCDWriteDataBuffer(UInt8 *buffer, UInt16 length)
{
	LCD_SET_A0();
	phbalReg_Stub_LCD_Dma(LCD_SPI_DEVID,buffer,Null,length);
	return MLS_SUCCESS;
}
static mlsErrorCode_t mlsLCDFill(UInt16 x1, UInt16 y1,UInt16 x2, UInt16 y2,UInt16 color)
{
	Int16 count;
	UInt32 i;

	count = (Int16)(x2 - x1 + 1)*(y2 - y1 + 1)*2;

    for(i = 0; i < count; i++)
    {
        gLCDBuffer[2*i] =   (uint8_t) (color >> 8);
        gLCDBuffer[2*i+1] = (uint8_t) color & 0xFF;
    }
    mlsLCDSetCursorPosition(x1,y1,x2,y2);
    mlsLCDWriteCommandByte(LCD_COMMAND_GRAM);

    while(count > 0)
    {
    	mlsLCDWriteDataBuffer(gLCDBuffer,count < 4800 ? count : 4800);
    	mlsDelay(2000);
    	count = count - 4800;
    }
    return MLS_SUCCESS;
}
static mlsErrorCode_t mlsLCDSetCursorPosition(UInt16 x1, UInt16 y1, UInt16 x2, UInt16 y2)
{
    mlsLCDWriteCommandByte(0x2a);
    mlsLCDWriteDataByte(x1 >> 8);
    mlsLCDWriteDataByte(x1 & 0xFF);
    mlsLCDWriteDataByte(x2 >> 8);
    mlsLCDWriteDataByte(x2 & 0xFF);

    mlsLCDWriteCommandByte(0x2b);
    mlsLCDWriteDataByte(y1 >> 8);
    mlsLCDWriteDataByte(y1 & 0xFF);
    mlsLCDWriteDataByte(y2 >> 8);
    mlsLCDWriteDataByte(y2 & 0xFF);
    return MLS_SUCCESS;
}
static mlsErrorCode_t mlsLCDDrawPixel(UInt16 x, UInt16 y, UInt16 color)
{
    mlsLCDSetCursorPosition(x, y, x, y);
    mlsLCDWriteCommandByte(LCD_COMMAND_GRAM);
    mlsLCDWriteDataByte(color >> 8);
    mlsLCDWriteDataByte(color & 0xFF);
    return MLS_SUCCESS;
}
UInt8 mlsLcdGetPixel(UInt16 x, UInt16 y, UInt8 bytewidth, UInt8 *data)
{
    UInt16 t;
    UInt8 x_offset;
    t = x/8;
    x_offset = x % 8;
    if (((data[y * bytewidth + t]) << x_offset)&0x80)
    {
        return 1;
    }
    else
        return 0;
}
static mlsErrorCode_t mlsLCDSetPixelBuffer(UInt16 index, UInt16 color)
{
	gLCDBuffer[2*index] = color >> 8;
	gLCDBuffer[2*index + 1] = color & 0xFF;
	return MLS_SUCCESS;
}
mlsErrorCode_t mlsLCDInitial_ST7789V(void)
{
	//---------------------------------------------------------------------------------------------------//
	mlsLCDWriteCommandByte(0x11);
	mlsOsalDelayMs(120);                //Delay 120ms
	//--------------------------------------Display Setting------------------------------------------//
	mlsLCDRotate(Landscape_1);
	mlsLCDWriteCommandByte( 0x3a);
	mlsLCDWriteDataByte(0x05);
	//--------------------------------ST7789V Frame rate setting----------------------------------//
	mlsLCDWriteCommandByte(0xb2);
	mlsLCDWriteDataByte(0x0c);
	mlsLCDWriteDataByte(0x0c);
	mlsLCDWriteDataByte(0x00);
	mlsLCDWriteDataByte(0x33);
	mlsLCDWriteDataByte(0x33);
	mlsLCDWriteCommandByte( 0xb7);
	mlsLCDWriteDataByte(0x35);
	//---------------------------------ST7789V Power setting--------------------------------------//
	mlsLCDWriteCommandByte( 0xbb);
	mlsLCDWriteDataByte(0x2b);
	mlsLCDWriteCommandByte( 0xc0);
	mlsLCDWriteDataByte(0x2c);
	mlsLCDWriteCommandByte( 0xc2);
	mlsLCDWriteDataByte(0x01);
	mlsLCDWriteCommandByte( 0xc3);
	mlsLCDWriteDataByte(0x11);
	mlsLCDWriteCommandByte( 0xc4);
	mlsLCDWriteDataByte(0x20);
	mlsLCDWriteCommandByte( 0xc6);
	mlsLCDWriteDataByte(0x0f);
	mlsLCDWriteCommandByte( 0xd0);
	mlsLCDWriteDataByte(0xa4);
	mlsLCDWriteDataByte(0xa1);
	//--------------------------------ST7789V gamma setting---------------------------------------//
	mlsLCDWriteCommandByte( 0xe0);
	mlsLCDWriteDataByte(0xd0);
	mlsLCDWriteDataByte(0x00);
	mlsLCDWriteDataByte(0x05);
	mlsLCDWriteDataByte(0x0e);
	mlsLCDWriteDataByte(0x15);
	mlsLCDWriteDataByte(0x0d);
	mlsLCDWriteDataByte(0x37);
	mlsLCDWriteDataByte(0x43);
	mlsLCDWriteDataByte(0x47);
	mlsLCDWriteDataByte(0x09);
	mlsLCDWriteDataByte(0x15);
	mlsLCDWriteDataByte(0x12);
	mlsLCDWriteDataByte(0x16);
	mlsLCDWriteDataByte(0x19);
	mlsLCDWriteCommandByte( 0xe1);
	mlsLCDWriteDataByte(0xd0);
	mlsLCDWriteDataByte(0x00);
	mlsLCDWriteDataByte(0x05);
	mlsLCDWriteDataByte(0x0d);
	mlsLCDWriteDataByte(0x0c);
	mlsLCDWriteDataByte(0x06);
	mlsLCDWriteDataByte(0x2d);
	mlsLCDWriteDataByte(0x44);
	mlsLCDWriteDataByte(0x40);
	mlsLCDWriteDataByte(0x0e);
	mlsLCDWriteDataByte(0x1c);
	mlsLCDWriteDataByte(0x18);
	mlsLCDWriteDataByte(0x16);
	mlsLCDWriteDataByte(0x19);

	mlsLCDWriteCommandByte(0x20);  // IPS Inversion off
	mlsOsalDelayMs(100);

	mlsLCDWriteCommandByte(0x29);  //Display on
	mlsOsalDelayMs(100);

    return MLS_SUCCESS;
}
void mlsDelay(int microSecond)
{
	int i = microSecond*5;
	for(;i > 0; i--);
}
mlsErrorCode_t mlsLCDDrawScreen(UInt16 color)
{
    UInt32 i;
    mlsOsalMutexLock(&gLcdMutex, MLS_OSAL_MAX_DELAY);
    for(i = 0; i < 2400; i++)
    {
        gLCDBuffer[2*i] =   (uint8_t) (color >> 8);
        gLCDBuffer[2*i+1] = (uint8_t) color & 0xFF;
    }
    mlsLCDSetCursorPosition(0,0,gLCD_opt.width -1,gLCD_opt.height - 1);
    mlsLCDWriteCommandByte(LCD_COMMAND_GRAM);
//    mlsOsalDelayMs(1);
    for(i = 0; i <32; i++)
    {
    	mlsLCDWriteDataBuffer(gLCDBuffer,4800);
//    	mlsDelay(2000);
    }
    mlsOsalMutexUnlock(&gLcdMutex);
    return MLS_SUCCESS;
}
mlsErrorCode_t mlsLCDDrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	/* Code by dewoller: https://github.com/dewoller */

	int16_t dx, dy, sx, sy, err, e2;
	uint16_t tmp;

	/* Check for overflow */
	if (x0 >= gLCD_opt.width) {
		x0 = gLCD_opt.width - 1;
	}
	if (x1 >= gLCD_opt.width) {
		x1 = gLCD_opt.width - 1;
	}
	if (y0 >= gLCD_opt.height) {
		y0 = gLCD_opt.height - 1;
	}
	if (y1 >= gLCD_opt.height) {
		y1 = gLCD_opt.height - 1;
	}

	/* Check correction */
	if (x0 > x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y0 > y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	dx = x1 - x0;
	dy = y1 - y0;

	/* Vertical or horizontal line */
	if (dx == 0 || dy == 0) {
		mlsLCDFill(x0, y0, x1, y1, color);
		return MLS_SUCCESS;
	}

	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	while (1) {
		mlsLCDDrawPixel(x0, y0, color);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
	return MLS_SUCCESS;
}
mlsErrorCode_t mlsLCDDrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{
	mlsLCDDrawLine(x0, y0, x1, y0, color); //Top
	mlsLCDDrawLine(x0, y0, x0, y1, color);	//Left
	mlsLCDDrawLine(x1, y0, x1, y1, color);	//Right
	mlsLCDDrawLine(x0, y1, x1, y1, color);	//Bottom
	return MLS_SUCCESS;
}
mlsErrorCode_t mlsLCDDrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	uint16_t tmp;

	/* Check correction */
	if (x0 > x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y0 > y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	/* Fill rectangle */
	mlsLCDFill(x0, y0, x1, y1, color);
	return MLS_SUCCESS;
}
mlsErrorCode_t mlsLCDDrawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    mlsLCDDrawPixel(x0, y0 + r, color);
    mlsLCDDrawPixel(x0, y0 - r, color);
    mlsLCDDrawPixel(x0 + r, y0, color);
    mlsLCDDrawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        mlsLCDDrawPixel(x0 + x, y0 + y, color);
        mlsLCDDrawPixel(x0 - x, y0 + y, color);
        mlsLCDDrawPixel(x0 + x, y0 - y, color);
        mlsLCDDrawPixel(x0 - x, y0 - y, color);

        mlsLCDDrawPixel(x0 + y, y0 + x, color);
        mlsLCDDrawPixel(x0 - y, y0 + x, color);
        mlsLCDDrawPixel(x0 + y, y0 - x, color);
        mlsLCDDrawPixel(x0 - y, y0 - x, color);
    }
    return MLS_SUCCESS;
}
mlsErrorCode_t mlsLCDDrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	mlsLCDDrawPixel(x0, y0 + r, color);
	mlsLCDDrawPixel(x0, y0 - r, color);
	mlsLCDDrawPixel(x0 + r, y0, color);
	mlsLCDDrawPixel(x0 - r, y0, color);
	mlsLCDDrawLine(x0 - r, y0, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        mlsLCDDrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        mlsLCDDrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

        mlsLCDDrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
        mlsLCDDrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }
    return MLS_SUCCESS;
}
mlsErrorCode_t mlsLCDDrawBuffer(UInt16 x1, UInt16 y1,UInt16 x2, UInt16 y2)
{
	UInt16 length;

	UInt16 tmp;

	/* Check correction */
	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	length = (x2 - x1 +1)*(y2 - y1 +1)*2;
	mlsLCDSetCursorPosition(x1,y1,x2,y2);
	mlsLCDWriteCommandByte(LCD_COMMAND_GRAM);
	mlsLCDWriteDataBuffer(gLCDBuffer,length);
	return MLS_SUCCESS;
}
mlsErrorCode_t mlsLCDRotate(mlsLCD_Orientation_t orientation) {
	mlsLCDWriteCommandByte(LCD_COMMAND_MAC);
	if (orientation == Portrait_1) {
		mlsLCDWriteDataByte(0x00);
	} else if (orientation ==  Portrait_2) {
		mlsLCDWriteDataByte(0xC0);
	} else if (orientation == Landscape_1) {
		mlsLCDWriteDataByte(0xA0);
	} else if (orientation == Landscape_2) {
		mlsLCDWriteDataByte(0x60);
	}

	if (orientation == Portrait_1 || orientation == Portrait_2) {
	    gLCD_opt.width = LCD_WIDTH;
	    gLCD_opt.height = LCD_HEIGHT;
		gLCD_opt.orientation = LCDPortrait;
	} else {
	    gLCD_opt.width = LCD_HEIGHT;
	    gLCD_opt.height = LCD_WIDTH;
	    gLCD_opt.orientation = LCDLandscape;
	}
	return MLS_SUCCESS;
}
static void mlsLCDPutc(UInt16 x, UInt16 y,
			char c, mlsLcdFontInfo_t *font,
			UInt16 foreground, UInt16 background, UInt16 space)
{
    UInt16 i ,t;
    UInt16 bitwidth;
    UInt16 ynew;
    UInt16 xnew;
    UInt8 bytewidth;
    gLCDx = x;
    gLCDy = y;
    if (((c >= font->firstchar) && (c <= font->lastchar))||((c >= font->firstsymbol) && (c <= font->lastsymbol)))
    {
    	UInt16 idx;
    	if ((c >= font->firstchar) && (c <= font->lastchar))
    		idx = c - font->firstchar;
    	else
    		idx = c - font->firstsymbol + font->lastchar - font->firstchar + 1;
        bitwidth = (UInt16) (font->fontIndex[idx*2]);
        bytewidth = (UInt8) (bitwidth%8 == 0 ? bitwidth/8 : bitwidth/8 + 1);
        memcpy(gLcddata,font->fontdata + font->fontIndex[idx*2 + 1],(size_t) (bytewidth*font->fontHeight));

        if ((gLCDx + bitwidth) > gLCD_opt.width) {
                /* If at the end of a line of display, go to new line and set x to 0 position */
            gLCDy = (UInt16) (gLCDy + font->fontHeight);
            gLCDx = 0;
            }
            /* Draw font data */
            for (i = 0; i < font->fontHeight; i++)
            {
                for(t = 0; t < bitwidth; t++)
                {
                    UInt16 index = (UInt16) (t + i*(bitwidth + space));
                    if (mlsLcdGetPixel(t,i,bytewidth,gLcddata))
                    {
                        mlsLCDSetPixelBuffer(index,foreground);
                    }
                    else
                        mlsLCDSetPixelBuffer(index,background);
                }
                for(t = bitwidth; t < (bitwidth + space);t++)
                {
                	UInt16 index = (UInt16) (t + i*(bitwidth + space));
                	mlsLCDSetPixelBuffer(index,background);
                }
            }
    }
    else
    {
        bitwidth = font->fontHeight/3;
        for(i = 0; i < (bitwidth + space)*font->fontHeight; i++)
            mlsLCDSetPixelBuffer(i,background);
    }
    xnew = (UInt16) (gLCDx + bitwidth + space - 1);
    ynew = (UInt16) (gLCDy + font->fontHeight - 1);
    mlsLCDDrawBuffer(gLCDx,gLCDy,xnew ,ynew);
        /* Set new pointer */
    gLCDx = (UInt16) (gLCDx + bitwidth + space);
}
void mlsLCDPuts(UInt16 x, UInt16 y,
		char *str, mlsLcdFontInfo_t *font,
		UInt16 foreground, UInt16 background, UInt16 space)
{
	mlsOsalMutexLock(&gLcdMutex, MLS_OSAL_MAX_DELAY);

	UInt16 startX = x;

	/* Set X and Y coordinates */
	gLCDx = x;
	gLCDy = y;

	while (*str) {
		/* New line */
		if (*str == '\n') {
			gLCDy += font->fontHeight + 1;
			/* if after \n is also \r, than go to the left of the screen */
			if (*(str + 1) == '\r') {
				gLCDx = 0;
				str++;
			} else {
				gLCDx = startX;
			}
			str++;
			continue;
		} else if (*str == '\r') {
			str++;
			continue;
		}

		/* Put character to LCD */
		mlsLCDPutc(gLCDx, gLCDy, *str++, font, foreground, background, space);
	}

	mlsOsalMutexUnlock(&gLcdMutex);
}
mlsErrorCode_t mlsLCDDrawImage(UInt16 x,UInt16 y,UInt16 width, UInt16 height, UInt16 *image)
{
	mlsOsalMutexLock(&gLcdMutex, MLS_OSAL_MAX_DELAY);

	UInt16 index, t, m;
	mlsLCDSetCursorPosition(x,y,x+width -1,y+height -1);
	mlsLCDWriteCommandByte(LCD_COMMAND_GRAM);
	index = (width*height)/2400;
	for (t = 0; t < index; t ++)
	{
		for(m = 0; m < 2400; m++)
		{
			mlsLCDSetPixelBuffer(m,image[t*2400 + m]);
		}
		mlsLCDWriteDataBuffer(gLCDBuffer,4800);
	}
	if ((width*height%2400)!=0)
	{
		for(m = 0; m < width*height - 2400*index;m++)
		{
			mlsLCDSetPixelBuffer(m,image[index*2400 + m]);
		}
		mlsLCDWriteDataBuffer(gLCDBuffer,(m+1)*2);
	}

	mlsOsalMutexUnlock(&gLcdMutex);

	return MLS_SUCCESS;
}

mlsErrorCode_t mlsLcd_LoadString(UInt8 lineNbr, char *pString)
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;

	mlsOsalMutexLock(&gLcdMutex, MLS_OSAL_MAX_DELAY);

	gLcdDataBlock.totalRow ++;
	gLcdDataBlock.row[lineNbr - 1].dataLen = MLS_MACRO_MIN(strlen(pString), 19);
	memset(gLcdDataBlock.row[lineNbr - 1].data,32, 19);
	memcpy(gLcdDataBlock.row[lineNbr - 1].data,
			pString, MLS_MACRO_MIN(strlen(pString), 19));

	mlsOsalMutexUnlock(&gLcdMutex);

	return errorCode;
}

mlsErrorCode_t mlsLcd_DisplayString(UInt16 lineMask)
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;
	UInt8 idx;
	UInt16 mask = lineMask;
	UInt16 background = LCD_BACKGROUND_COLOR/*LCD_WHITE*/;

#if (MLS_BOARDSEL_ES1 == 0)
	mlsLcdSpiInit();
#endif

	mlsOsalMutexLock(&gLcdMutex, MLS_OSAL_MAX_DELAY);

	for (idx = 0; idx < sizeof(gLineMask)/ sizeof(gLineMask[0]); idx++)
	{
		if ((mask & gLineMask[idx]) == gLineMask[idx])
		{
			mask &= ~gLineMask[idx];

			mlsLCDPuts(X_OFFSET,Y_OFFSET + idx*(gLCD_opt.height - Y_OFFSET)/4,
					(char *)gLcdDataBlock.row[idx].data,
					&Arial_16,
					LCD_BLACK,
					background, 5);
		}
		if (mask  == 0x0000)
		{
			break;
		}
	}

	gLcdDataBlock.totalRow  = 0;
	mlsOsalMutexUnlock(&gLcdMutex);

#if (MLS_BOARDSEL_ES1 == 0)
	mlsLcdSpiDeInit();
#endif

	return errorCode;
}
mlsErrorCode_t mlsLCDDisplayRawQRCode(Int8 * szSourceSring,mlsLcdAlign_t align)
{
	UInt8 x,y,i,t;
	UInt8 x1 = 0, x2 = 0;
	Int32 width;
	UInt8 scale;
	UInt8 QRCodeBufeer[4410];

	mlsGenerateQRCode(&width,QRCodeBufeer,szSourceSring);
	scale = LCD_HEIGHT/(2*width);
	switch(align)
	{
	case LEFT:
		x1 = gLCD_opt.width/4 - width*scale/2;
		x2 = width*scale - 1 + x1;
		break;
	case RIGHT:
		x1 = 3*gLCD_opt.width/4 - width*scale/2;
		x2 = width*scale - 1 + x1;
		break;
	case CENTER:
		x1 = (gLCD_opt.width - width*scale)/2;
		x2 = width*scale - 1 + x1;
		break;
	default:
		break;
	}
	for(y = 0; y < width; y++)
	{
		for(x = 0; x < width; x++)
		{
			if (QRCodeBufeer[y*width + x] & 1)
			{
				for(i = 0;i < scale; i++)
				{
					for(t = 0;t < scale; t++)
					{
						mlsLCDSetPixelBuffer(x*scale + i +  t*scale*width,LCD_BLACK);
					}
				}
			}
			else
			{
				for(i = 0;i < scale; i++)
				{
					for(t = 0;t < scale; t++)
					{
						mlsLCDSetPixelBuffer(x*scale + i +  t*scale*width,LCD_BACKGROUND_COLOR/*LCD_WHITE*/);
					}
				}
			}
		}
		mlsLCDDrawBuffer(x1,y*scale + (gLCD_opt.height - width*scale)/2,x2,(y + 1)*scale - 1 + (gLCD_opt.height - width*scale)/2);
	}
	return MLS_SUCCESS;
}

mlsErrorCode_t mlsLCDDisplayCustomQRCode(Int8 * szSourceSring)
{
	UInt8 x,y,i,t;
	UInt8 x1, x2;
	Int32 width;
	UInt8 scale;
	UInt8 QRCodeBufeer[4410];
	const UInt32 MAX_RESOLUTION = 190;

	mlsGenerateQRCode(&width,QRCodeBufeer,szSourceSring);
	scale = MAX_RESOLUTION/width;

	x1 = 105 - width*scale/2;
	x2 = width*scale - 1 + x1;

	for(y = 0; y < width; y++)
	{
		for(x = 0; x < width; x++)
		{
			if (QRCodeBufeer[y*width + x] & 1)
			{
				for(i = 0;i < scale; i++)
				{
					for(t = 0;t < scale; t++)
					{
						mlsLCDSetPixelBuffer(x*scale + i +  t*scale*width,LCD_BLACK);
					}
				}
			}
			else
			{
				for(i = 0;i < scale; i++)
				{
					for(t = 0;t < scale; t++)
					{
						mlsLCDSetPixelBuffer(x*scale + i +  t*scale*width,LCD_BACKGROUND_COLOR/*LCD_WHITE*/);
					}
				}
			}
		}
		mlsLCDDrawBuffer(x1,y*scale + (gLCD_opt.height - width*scale)/2,x2,(y + 1)*scale - 1 + (gLCD_opt.height - width*scale)/2);
	}
	return MLS_SUCCESS;
}

mlsErrorCode_t mlsLCDDisplayQRText(Int8 * SourceSring,
									char *line1,
									char *line2,
									char *line3,
									char *line4)
{
	mlsErrorCode_t err = MLS_SUCCESS;
	char Line1[20] = "          ";
	char Line2[20] = "          ";
	char Line3[20] = "          ";
	char Line4[20] = "          ";

	memcpy(&Line1[10], line1, MLS_MACRO_MIN(strlen(line1), 10));
	memcpy(&Line2[10], line2, MLS_MACRO_MIN(strlen(line2), 10));
	memcpy(&Line3[10], line3, MLS_MACRO_MIN(strlen(line3), 10));
	memcpy(&Line4[10], line4, MLS_MACRO_MIN(strlen(line4), 10));

	mlsLcd_LoadString(1,Line1);
	mlsLcd_LoadString(2,Line2);
	mlsLcd_LoadString(3,Line3);
	mlsLcd_LoadString(4,Line4);

	mlsLcd_DisplayString(LINE1|LINE2|LINE3|LINE4);
	mlsLCDDisplayRawQRCode(SourceSring,LEFT);

	return err;
}
