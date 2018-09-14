
#ifndef __MLSLCDDRIVER_H__
#define __MLSLCDDRIVER_H__
/* C++ detection */
#ifdef __cplusplus
extern "C"
{
#endif


#include "mlsLCDFont.h"
#include "Type.h"
#define     LCD_GREEN    0x07E0
#define     LCD_BLUE     0x001F
#define     LCD_RED      0xF800
#define		LCD_WHITE	 0xFFFF
#define 	LCD_BLACK	 0x0000


#define		LCD_GREEN_TITLE_BAR		0x79EF
#define		LCD_BACKGROUND_COLOR	0xdb7b
#define		LCD_AMOUNT_COLOR		0x3192

#define 	LCD_WIDTH		240
#define 	LCD_HEIGHT		320
#define		LCD_TOP_IMG_COLOR	0x349f

#define	LINE1	(0x0001 << 0)
#define	LINE2	(0x0001 << 1)
#define	LINE3	(0x0001 << 2)
#define LINE4	(0x0001 << 3)
#define LINE5	(0x0001 << 4)
#define LINE6	(0x0001 << 5)

#define LCD_NORFLASH_CMD					0xAB
#define IMAGE_SET							0x00
#define IMAGE_DISPLAY						0x02
#define MAX_CHARACTER_PER_LINE				24
#define MAX_LCD_LINE						6

extern UInt8 gLCDBuffer[4800];

typedef struct
{
	UInt16 lineMask;
	UInt8  string[MAX_CHARACTER_PER_LINE+1];
}mlsLcdLineString_t;

/** \struct mlsLcdRow_t
 *
 */
typedef struct
{
	UInt8 dataLen;
	UInt8 data[128];
}mlsLcdRow_t;

/** \struct mlsLcdStringData_t
 *
 */
typedef struct
{
	UInt8 totalRow;
	mlsLcdRow_t row[MAX_LCD_LINE];
}mlsLcdStringDataBlock_t;

typedef enum
{
	Portrait_1,  /*!< Portrait orientation mode 1 */
	Portrait_2,  /*!< Portrait orientation mode 2 */
	Landscape_1, /*!< Landscape orientation mode 1 */
	Landscape_2  /*!< Landscape orientation mode 2 */
} mlsLCD_Orientation_t;

typedef enum
{
	LEFT,
	RIGHT,
	CENTER,
}mlsLcdAlign_t;

/*
 *@fn mlsLCDInit
 *@brief This function initialize SPI channel and send command byte to lcd
 */
mlsErrorCode_t mlsLCDInit(void);

/*
 *@fn mlsLCDDrawBuffer
 *@brief This function send many bytes of data to draw on lcd
 *@param x1
 *@param y1
 *@param x2
 *@param y2
 */
mlsErrorCode_t mlsLCDDrawBuffer(UInt16 x1, UInt16 y1,UInt16 x2, UInt16 y2);

/*
 * @fn mlsLCDDrawScreen
 * @brief This function draw whole screen with one color
 * @param color
 */
mlsErrorCode_t mlsLCDDrawScreen(UInt16 color);

/*
 *
 */
void mlsLCDPuts(UInt16 x, UInt16 y, char *str, mlsLcdFontInfo_t *font, UInt16 foreground, UInt16 background);

/*
 * @fn mlsLCDDrawImage
 * @brief This function draw an image from buffer to screen
 * @param x
 * @param y
 * @param width
 * @param height
 * @param pointer to image
 */
mlsErrorCode_t mlsLCDDrawImage(UInt16 x,UInt16 y,UInt16 width, UInt16 height, UInt16 *image);
/**
 * @fn mlsLCDRotate
 * @brief This function rotate the screen to portrait or landscape
 * @param orientation
 */
mlsErrorCode_t mlsLCDRotate(mlsLCD_Orientation_t orientation);
/**
 * @fn mlsLCDDisplayRawQRCode
 * @brief This function draw QR Code image on the screen
 * @param szSourceSring 	string that is encoded
 */

/**\fn mlsLCDTestMain()
 *  @brief: This function is used to test LCD module.
 */
mlsErrorCode_t mlsLCDTestMain();


/** \fn mlsLCDNofflahsDrawImage
 * @brief This function read data from Norflash and then display that on LCD
 *
 * @param[in]: x the coordinate on the horizontal axis
 * @param[in]: y the coordinate on the vertical axis
 * @param[in]: width is the width of the image
 * @param[in]: height is the height of the image
 * @parma[in]: address is the address that the image is saved on the NorFlash
 */
mlsErrorCode_t mlsLCDNorflashDrawImage( UInt16 x,
										UInt16 y,
										UInt16 width,
										UInt16 height,
										UInt32 address);
/**\@fn mlsLCDUpdateImageInfo
 * @brief: This function read information of the list of image on the Norflash and save to gImage
 */
mlsErrorCode_t mlsLCDUpdateImageInfo();

mlsErrorCode_t mlsLCDNorflashSettingImageInfo(	UInt8* pbInBuf,
												UInt16 wInBufLen,
												UInt8* pbOutBuf,
												UInt16* pwOutBufLen );

mlsErrorCode_t mlsLCDDisplayImage(	UInt8* pbInBuf,
									UInt16 wInBufLen,
									UInt8* pbOutBuf,
									UInt16* pwOutBufLen );

mlsErrorCode_t mlsLCDNorflashDraw(UInt8 num);

mlsErrorCode_t mlsLCDNorflashDrawXY(UInt8 num, UInt16 x, UInt16 y);

mlsErrorCode_t mlsLCDNorflashCheckInit();

mlsErrorCode_t mlsLCDDisplayCustomQRCode(Int8 * szSourceSring);

mlsErrorCode_t mlsLCDDisplayCustomQRCodeX(Int8 * szSourceSring, UInt16 inX);

mlsErrorCode_t mlsLcd_LoadString(UInt8 lineNbr, char *pString);

mlsErrorCode_t mlsLcd_DisplayString(UInt16 lineMask);

void __attribute__ ((noinline))  __attribute__((optimize("-O0")))
		mlsOsalDelayMs(unsigned int ms);

#ifdef __cplusplus
}
#endif
#endif /* SOURCE_STYL_MLSLCD_INC_MLSLCD_H_ */
