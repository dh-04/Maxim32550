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
#include "mlsLCDDriver.h"
#include "mlsLCDFont.h"
#include "mlsCalibri12pts.h"

#include <MAX325xx.h>

#define mlsOsalMutexLock(x, y)
#define mlsOsalMutexUnlock(x)

#define LCD_A0_PIN			30
#define LCD_A0_PORT			MML_GPIO_DEV0
#define LCD_CS_PIN			28
#define LCD_CS_PORT			MML_GPIO_DEV0
#define LCD_SPI_DEVID		MML_SPI_DEV1
#define LCD_COMMAND_GRAM	0x2C
#define LCD_COMMAND_MAC		0x36
#define LCD_CHAR_SPACE		5
#define X_OFFSET			10
#define Y_OFFSET			16

#define NORFLASH_SECTOR_SIZE 0x00010000

#define MLS_MACRO_MIN(x,  y) ((x) > (y)? (y) : (x))

#define LCD_CLR_A0() 	(mml_gpio_write_bit_pattern(LCD_A0_PORT, LCD_A0_PIN, 1, 0))
#define LCD_SET_A0()     (mml_gpio_write_bit_pattern(LCD_A0_PORT, LCD_A0_PIN, 1, 1))


#define LCD_CLR_CS() 	(mml_gpio_write_bit_pattern(LCD_CS_PORT, LCD_CS_PIN, 1, 0))
#define LCD_SET_CS()     (mml_gpio_write_bit_pattern(LCD_CS_PORT, LCD_CS_PIN, 1, 1))
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
volatile int gBackGroundDefault = 1;

UInt16 gLCDx;
UInt16 gLCDy;
UInt8 gLcddata[100];
UInt8 gLCDBuffer[4800];



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
	(UInt16) LINE5,
	(UInt16) LINE6,
};

/**
 * @fn mlsLCDSetCursorPosition
 * @brief This function set the coordinate to write data to LCD
 */
static mlsErrorCode_t mlsLCDSetCursorPosition(UInt16 x1, UInt16 y1, UInt16 x2, UInt16 y2);
/**
 * @fn mlsLCDInitial_ST7789
 * @brief This function send command byte to initialize LCD
 */
static mlsErrorCode_t mlsLCDInitial_ST7789V(void);
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
static void mlsLCDPutc(UInt16 x, UInt16 y, UInt32 utf8, mlsLcdFontInfo_t *font, UInt16 foreground, UInt16 background);



void mlsDelay(int microSecond);

mlsErrorCode_t mlsLCDInit(void)
{
	mlsErrorCode_t errCode = MLS_SUCCESS;

	mml_gpio_config_t 	config;

	config.gpio_direction 		= MML_GPIO_DIR_OUT;
	config.gpio_function 		= MML_GPIO_NORMAL_FUNCTION;
	config.gpio_intr_mode 		= 0;
	config.gpio_intr_polarity 	= 0;
	config.gpio_pad_config 		= MML_GPIO_PAD_NORMAL;

	errCode |= mml_gpio_init(MML_GPIO_DEV1, 1, 1, config);
	mml_gpio_write_bit_pattern(MML_GPIO_DEV1, 1, 1, 0x01) ;


	mlsLCDSpiInit();
	mlsLCDInitial_ST7789V();
	mlsLCDDrawScreen(LCD_WHITE);

	return errCode;
}
mlsErrorCode_t mlsLCDSpiInit(void)
{
	mlsErrorCode_t errCode = MLS_SUCCESS;

	mml_spi_params_t	spi_conf;

	/** Fill parameters */
	spi_conf.baudrate 		= 12000000;
	spi_conf.word_size 		= 8;
	spi_conf.mode 			= SPIn_CNTL_MMEN_master;
	spi_conf.wor 			= SPIn_CNTL_WOR_disable;
	spi_conf.clk_pol 		= SPIn_CNTL_CLKPOL_idleLo;
	spi_conf.phase 			= SPIn_CNTL_PHASE_activeEdge;
	spi_conf.brg_irq 		= SPIn_CNTL_BIRQ_disable;
	spi_conf.ssv 			= SPIn_MOD_SSV_hi;
	spi_conf.ssio 			= SPIn_MOD_SSIO_output;
	spi_conf.tlj 			= SPIn_MOD_TX_LJ_disable;
	spi_conf.dma_tx.active 	= SPIn_DMA_REG_DMA_EN_disable;
	spi_conf.dma_rx.active 	= SPIn_DMA_REG_DMA_EN_disable;

	/** Set CS0 */
	spi_conf.ssio = 0x1;

	/** Call initialization function from driver */
	if(mml_spi_init(LCD_SPI_DEVID, (mml_spi_params_t*)&spi_conf) == NO_ERROR)
	{
		/** Now enable SPI interface */
		M_MML_SPI_ENABLE(LCD_SPI_DEVID);
	}

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
	LCD_CLR_CS();
	mml_spi_read_write(LCD_SPI_DEVID,&command,NULL,1);
	LCD_SET_CS() ;
	return MLS_SUCCESS;
}
static mlsErrorCode_t mlsLCDWriteDataByte(UInt8 data)
{
	LCD_SET_A0();
	LCD_CLR_CS();
	mml_spi_read_write(LCD_SPI_DEVID,&data,NULL,1);
	LCD_SET_CS() ;
	return MLS_SUCCESS;
}
static mlsErrorCode_t mlsLCDWriteDataBuffer(UInt8 *buffer, UInt16 length)
{
	LCD_SET_A0();
	LCD_CLR_CS();
	mml_spi_read_write(LCD_SPI_DEVID,buffer,NULL,length);
	LCD_SET_CS() ;
	return MLS_SUCCESS;
}
static mlsErrorCode_t mlsLCDFill(UInt16 x1, UInt16 y1,UInt16 x2, UInt16 y2,UInt16 color)
{
	Int16 count;
	Int16 i;

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
static mlsErrorCode_t mlsLCDInitial_ST7789V(void)
{
    mlsLCDWriteCommandByte(0x11);      //Exit sleep
    mlsOsalDelayMs(120);

    mlsLCDWriteCommandByte(0x3A);      //interface pixel format
    mlsLCDWriteDataByte(0x55);         // 65K of RGB interface and 16 bit/pixel

    mlsLCDRotate(Landscape_1);

    mlsLCDWriteCommandByte(0xB2);      //Porch setting
    mlsLCDWriteDataByte(0x00);//00
    mlsLCDWriteDataByte(0x00);//00
    mlsLCDWriteDataByte(0x00);
    mlsLCDWriteDataByte(0x33);
    mlsLCDWriteDataByte(0x33);

    mlsLCDWriteCommandByte(0xb7);      //Gate control
    mlsLCDWriteDataByte(0x35);

    mlsLCDWriteCommandByte(0xb8);
    mlsLCDWriteDataByte(0x2f);
    mlsLCDWriteDataByte(0x2b);
    mlsLCDWriteDataByte(0x2f);

    mlsLCDWriteCommandByte(0xbb);      //Vcom setiing
    mlsLCDWriteDataByte(0x24);

    mlsLCDWriteCommandByte(0xc0);      //LCM control
    mlsLCDWriteDataByte(0x2c);

    mlsLCDWriteCommandByte(0xc3);      //VHR set
    mlsLCDWriteDataByte(0x10);

    mlsLCDWriteCommandByte(0xc4);      //VDV set
    mlsLCDWriteDataByte(0x20);

    mlsLCDWriteCommandByte(0xC6);      //Frame rate control in Normal mode
    mlsLCDWriteDataByte(0x11);

    mlsLCDWriteCommandByte(0xd0);      //Power control
    mlsLCDWriteDataByte(0xa4);
    mlsLCDWriteDataByte(0xa1);

    mlsLCDWriteCommandByte(0xe8);      //Power control 2
    mlsLCDWriteDataByte(0x19);

    mlsLCDWriteCommandByte(0xe9);      //Equalize time control
    mlsLCDWriteDataByte(0x0d);
    mlsLCDWriteDataByte(0x12);
    mlsLCDWriteDataByte(0x00);

    mlsLCDWriteCommandByte(0xE0);      //Gamma
    mlsLCDWriteDataByte(0xd0);
    mlsLCDWriteDataByte(0x00);
    mlsLCDWriteDataByte(0x00);
    mlsLCDWriteDataByte(0x08);
    mlsLCDWriteDataByte(0x11);
    mlsLCDWriteDataByte(0x1a);
    mlsLCDWriteDataByte(0x2b);
    mlsLCDWriteDataByte(0x33);
    mlsLCDWriteDataByte(0x42);
    mlsLCDWriteDataByte(0x26);
    mlsLCDWriteDataByte(0x12);
    mlsLCDWriteDataByte(0x21);
    mlsLCDWriteDataByte(0x2f);
    mlsLCDWriteDataByte(0x11);

    mlsLCDWriteCommandByte(0xE1);      //Gamma
    mlsLCDWriteDataByte(0xd0);
    mlsLCDWriteDataByte(0x02);
    mlsLCDWriteDataByte(0x09);
    mlsLCDWriteDataByte(0x0d);
    mlsLCDWriteDataByte(0x0d);
    mlsLCDWriteDataByte(0x27);
    mlsLCDWriteDataByte(0x2b);
    mlsLCDWriteDataByte(0x33);
    mlsLCDWriteDataByte(0x42);
    mlsLCDWriteDataByte(0x17);
    mlsLCDWriteDataByte(0x12);
    mlsLCDWriteDataByte(0x11);
    mlsLCDWriteDataByte(0x2f);
    mlsLCDWriteDataByte(0x31);

    mlsLCDWriteCommandByte(0x2a);      //Column Address set
    mlsLCDWriteDataByte(0x00);
    mlsLCDWriteDataByte(0x00);
    mlsLCDWriteDataByte(0x00);
    mlsLCDWriteDataByte(0xEF);

    mlsLCDWriteCommandByte(0x2b);      //Row address set
    mlsLCDWriteDataByte(0x00);
    mlsLCDWriteDataByte(0x00);
    mlsLCDWriteDataByte(0x01);
    mlsLCDWriteDataByte(0x3F);

    mlsLCDWriteCommandByte(0x21);  // IPS Inversion on
    mlsOsalDelayMs(100);

    mlsLCDWriteCommandByte(0x29);  //Display on
    mlsOsalDelayMs(100);
    mlsLCDWriteCommandByte(LCD_COMMAND_GRAM);
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

    for(i = 0; i <32; i++)
    {
    	mlsLCDWriteDataBuffer(gLCDBuffer,4800);
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

static Bool mlsGetFontIndex(UInt32 utf8, mlsLcdFontInfo_t *font, UInt16 *index)
{
	UInt32 i = 0;

	for (i = 0; i < FONT_SIZE; i++)
	{
		if(utf8 == font->fontIndex[i].Utf8Decode)
		{
			*index = i;
			return True;
		}
	}

	return False;
}

static void mlsLCDPutc(UInt16 x, UInt16 y, UInt32 utf8, mlsLcdFontInfo_t *font, UInt16 foreground, UInt16 background)
{
    UInt16 i ,t;
    UInt16 bitwidth;
    UInt16 ynew;
    UInt16 xnew;
    UInt8 bytewidth;
    gLCDx = x;
    gLCDy = y;

    UInt16 idx;

    if(mlsGetFontIndex(utf8, font, &idx))
//    if (((c >= font->firstchar) && (c <= font->lastchar))||((c >= font->firstsymbol) && (c <= font->lastsymbol)))
    {
//    	UInt16 idx;
//    	if ((c >= font->firstchar) && (c <= font->lastchar))
//    		idx = c - font->firstchar;
//    	else
//    		idx = c - font->firstsymbol + font->lastchar - font->firstchar + 1;
        bitwidth = (UInt16) (font->fontIndex[idx].width);
        bytewidth = (UInt8) (bitwidth%8 == 0 ? bitwidth/8 : bitwidth/8 + 1);
        memcpy(gLcddata,&font->fontdata[font->fontIndex[idx].index],(size_t) (bytewidth*font->fontHeight));
    //    data = font->fontdata + font->fontIndex[(c-32)*2 + 1];

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
                    UInt16 index = (UInt16) (t + i*(bitwidth + LCD_CHAR_SPACE));
                    if (mlsLcdGetPixel(t,i,bytewidth,gLcddata))
                    {
                        mlsLCDSetPixelBuffer(index,foreground);
                    }
                    else
                        mlsLCDSetPixelBuffer(index,background);
                }
                for(t = bitwidth; t < (bitwidth + LCD_CHAR_SPACE);t++)
                {
                	UInt16 index = (UInt16) (t + i*(bitwidth + LCD_CHAR_SPACE));
                	mlsLCDSetPixelBuffer(index,background);
                }
            }
    }
    else
    {
        bitwidth = font->fontHeight/3;
        for(i = 0; i < (bitwidth + LCD_CHAR_SPACE)*font->fontHeight; i++)
            mlsLCDSetPixelBuffer(i,background);
    }
    xnew = (UInt16) (gLCDx + bitwidth + LCD_CHAR_SPACE - 1);
    ynew = (UInt16) (gLCDy + font->fontHeight - 1);
    mlsLCDDrawBuffer(gLCDx,gLCDy,xnew ,ynew);
        /* Set new pointer */
    gLCDx = (UInt16) (gLCDx + bitwidth + LCD_CHAR_SPACE);
}

void mlsLCDPuts(UInt16 x, UInt16 y, char *str, mlsLcdFontInfo_t *font, UInt16 foreground, UInt16 background)
{
	UInt16 startX = x;
	UInt32 utf8;
	int count = 0;
	int maxCount = 0;

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
		else if(*str >= 0x01 && *str <= 0x7F)
		{
			utf8 = *str;
			str++;
		}
		else
		{
			utf8 = 0;
			count = 0;
			maxCount = 0;

			if((*str & 0xF0) == 0xC0)
			{
				maxCount = 2;
			}
			else if((*str & 0xF0) == 0xE0)
			{
				maxCount = 3;
			}

			while(!(*str >= 0x01 && *str <= 0x7F) && count < maxCount)
			{
				count++;
				utf8 = utf8 << 8;
				utf8 |= (UInt32)(*str);

				str++;
			}
		}

		/* Put character to LCD */
		mlsLCDPutc(gLCDx, gLCDy, utf8, font, foreground, background);
	}
}
mlsErrorCode_t mlsLCDDrawImage(UInt16 x,UInt16 y,UInt16 width, UInt16 height, UInt16 *image)
{
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
	return MLS_SUCCESS;
}

static int GetStringLenUnicodeVi(char* pString)
{
	int len = 0;

	while(*pString)
	{
		len++;
		if(*pString >= 0x01 && *pString <= 0x7F)
		{
			pString++;
		}
		else
		{
			if((*pString & 0xF0) == 0xC0)
			{

				pString += 2;
			}
			else if((*pString & 0xF0) == 0xE0)
			{
				pString += 3;
			}
			else
			{
				pString++;
			}
		}
	}

	return len;
}
mlsErrorCode_t mlsLcd_LoadString(UInt8 lineNbr, char *pString)
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;

	mlsOsalMutexLock(&gLcdMutex, MLS_OSAL_MAX_DELAY);

	gLcdDataBlock.totalRow ++;
	gLcdDataBlock.row[lineNbr - 1].dataLen = MLS_MACRO_MIN(GetStringLenUnicodeVi(pString), MAX_CHARACTER_PER_LINE);
	memset(gLcdDataBlock.row[lineNbr - 1].data, 0, sizeof(gLcdDataBlock.row[lineNbr - 1].data));
	memcpy(gLcdDataBlock.row[lineNbr - 1].data, pString, MLS_MACRO_MIN(strlen(pString), sizeof(gLcdDataBlock.row[lineNbr - 1].data)));

	mlsOsalMutexUnlock(&gLcdMutex);

	return errorCode;
}
mlsErrorCode_t mlsLcd_DisplayString(UInt16 lineMask)
{
	mlsErrorCode_t errorCode = MLS_SUCCESS;
	UInt8 idx;
	UInt16 mask = lineMask;
	UInt16 background = LCD_WHITE;


	mlsOsalMutexLock(&gLcdMutex, MLS_OSAL_MAX_DELAY);

	for (idx = 0; idx < sizeof(gLineMask)/ sizeof(gLineMask[0]); idx++)
	{
		if ((mask & gLineMask[idx]) == gLineMask[idx])
		{
			mask &= ~gLineMask[idx];

			mlsLCDPuts(X_OFFSET,Y_OFFSET + idx*(gLCD_opt.height - Y_OFFSET)/6,
					(char *)gLcdDataBlock.row[idx].data,
					&arialNarrow_FontInfo,
					LCD_BLACK,
					background);
		}
		if (mask  == 0x0000)
		{
			break;
		}
	}

	gLcdDataBlock.totalRow  = 0;
	mlsOsalMutexUnlock(&gLcdMutex);



	return errorCode;
}





void __attribute__ ((noinline))  __attribute__((optimize("-O0")))
		mlsOsalDelayMs(unsigned int ms)
{
    int i = ms*5000;
    unsigned int systemfreq = 0;
    mml_get_system_frequency(&systemfreq);

    i /= (MML_SYSTEM_FREQUENCY_108_MHZ/systemfreq);

    for (; i > 0 ; i--)
         __asm volatile ("nop\n");
}











