
#ifndef __MLSLCDDRIVER_H__
#define __MLSLCDDRIVER_H__
/* C++ detection */
#ifdef __cplusplus
extern "C"
{
#endif


#include "mlsLCDFont.h"
#include "mlsOsal.h"
#include "qrcodegen.h"

#define     LCD_GREEN    ~0x07E0
#define     LCD_BLUE     ~0x001F
#define     LCD_RED      ~0xF800
#define		LCD_WHITE	 ~0xFFFF
#define 	LCD_BLACK	 ~0x0000
#define		LCD_MAGENTA  ~0xF81F
#define		LCD_YELLOW	 ~0xFFE0
#define		LCD_CYAN	 ~0x07FF
#define		LCD_GREEN_TITLE_BAR		~0x79EF
#define		LCD_BACKGROUND_COLOR	~0xDEDB//0xdb7b
#define		LCD_AMOUNT_COLOR		0x42F1//0x4311//0x3192
#define		LCD_RESULT_COLOR		0x3192

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

//#define LCD_BACK_LIGHT_USE_PWM	0


typedef struct
{
	UInt16 lineMask;
	UInt8  string[30];
}mlsLcdLineString_t;

/** \struct mlsLcdRow_t
 *
 */
typedef struct
{
	UInt8 dataLen;
	UInt8 data[30];
}mlsLcdRow_t;

/** \struct mlsLcdStringData_t
 *
 */
typedef struct
{
	UInt8 totalRow;
	mlsLcdRow_t row[4];
}mlsLcdStringDataBlock_t;

typedef enum {
	Portrait_1,  /*!< Portrait orientation mode 1 */
	Portrait_2,  /*!< Portrait orientation mode 2 */
	Landscape_1, /*!< Landscape orientation mode 1 */
	Landscape_2  /*!< Landscape orientation mode 2 */
} mlsLCD_Orientation_t;

typedef enum{
	LEFT,
	RIGHT,
	CENTER,
}mlsLcdAlign_t;

/**\fn mlsOsalDelayMs()
 *  @brief: This function is used to test LCD module.
 */
void __attribute__ ((noinline))  __attribute__((optimize("-O0")))
		mlsOsalDelayMs(unsigned int ms);

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
void mlsLCDPuts(UInt16 x, UInt16 y,
		char *str, mlsLcdFontInfo_t *font,
		UInt16 foreground, UInt16 background, UInt16 space);
/**
 * @fn mlsLCDRotate
 * @brief This function rotate the screen to portrait or landscape
 * @param orientation
 */
mlsErrorCode_t mlsLCDRotate(mlsLCD_Orientation_t orientation);
/**\fn mlsLCDTestMain()
 *  @brief: This function is used to test LCD module.
 */
mlsErrorCode_t mlsLCDTestMain();
/**
 * @fn mlsLCDDrawSTYLLogo
 * @brief This function draw STYL logo
 * @param x
 * @param y
 */
mlsErrorCode_t mlsLCDDrawSTYLLogo(UInt16 x,UInt16 y);
/** \fn mlsLCDDrawFilledCircle
 * @brief: This function is used to draw a filled circle at x0, y0
 * @param[in]: x0
 * @param[in]: y0
 * @param[in]: r: radius
 * @param[in]: color: color of circle
 */
mlsErrorCode_t mlsLCDDrawFilledCircle(UInt16 x0, UInt16 y0, UInt16 r, UInt32 color);
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
/** \fn mlsLcd_LoadString
 * @brief: This function is used to load data to LCD line number.
 * It is stored in a global buffer before write to LCD
 * @param[in]: lineNbr is line number
 * @param[in]: pString is pointer to string buffer
 */
mlsErrorCode_t mlsLcd_LoadString(UInt8 lineNbr, char *pString);
/** \fn mlsLcd_DisplayString
 * @brief: This function is used to display data to LCD line number.
 * It is stored in a global buffer before write to LCD
 * @param[in]: lineMask is the bit mask
 */
mlsErrorCode_t mlsLcd_DisplayString(UInt16 lineMask);
/**
 * @fn mlsLCDDisplayRawQRCode
 * @brief This function draw QR Code image on the screen
 * @param szSourceSring 	string that is encoded
 */
mlsErrorCode_t mlsLCDDisplayRawQRCode(Int8 * szSourceSring,mlsLcdAlign_t align);
/**
 * @fn mlsLCDDisplayCustomQRCode
 * @brief This function draw QR Code image on the screen
 * @param szSourceSring 	string that is encoded
 */
mlsErrorCode_t mlsLCDDisplayCustomQRCode(Int8 * szSourceSring, uint8_t* QRCodeBufeer, uint8_t* tempBuffer);
/**
 * $fn mlsLCDDisplayQRText
 * @brief This function draw QR Code on the left of the screen and the text on the right of the screen
 * @param szSourceSring 	string that is encoded
 * @param line1 : Text at line 1
 * @param line2 : Text at line 2
 * @param line3 : Text at line 3
 * @param line4 : Text at line 4
 */
mlsErrorCode_t mlsLCDDisplayQRText(Int8 * SourceSring,
									char *line1,
									char *line2,
									char *line3,
									char *line4);

#ifdef __cplusplus
}
#endif
#endif /* SOURCE_STYL_MLSLCD_INC_MLSLCD_H_ */
