
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
 * @fn mlsLCDDrawScreen
 * @brief This function draw whole screen with one color
 * @param color
 */
mlsErrorCode_t mlsLCDDrawScreen(UInt16 color);

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

void __attribute__ ((noinline))  __attribute__((optimize("-O0")))
		mlsOsalDelayMs(unsigned int ms);

#ifdef __cplusplus
}
#endif
#endif /* SOURCE_STYL_MLSLCD_INC_MLSLCD_H_ */
