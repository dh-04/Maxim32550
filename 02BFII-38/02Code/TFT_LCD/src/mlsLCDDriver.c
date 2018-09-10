#include "mlsLCDDriver.h"
#include "mlsLCDFont.h"

#define LCD_CS_PIN			25 //CS:11 MISO_SPI1:P0.25
#define LCD_RS_PIN			29 //RS:13 SPI1_TFT_CS_N:P0.29
#define LCD_WR_PIN			27 //WR:14 SPI1_CLOCK:P0.27

#define LCD_D7_PIN			26 //D7:16 SPI1_MOSI:P0.26
#define LCD_D6_PIN			28 //D6:17 SPI1_FL_CS_N :P0.28
#define LCD_D5_PIN			18 //D5:18 SPI0_SCLK :P0.18
#define LCD_D4_PIN			17 //D4:19 SPI0_MOSI: P0.17
#define LCD_D3_PIN			16 //D3:20 SPI0_MISO: P.16
#define LCD_D2_PIN			19 //D2:21 SPI0_NFC_CS_N: P.19
#define LCD_D1_PIN			12 //D1:22 UART1_RX:P0.12
#define LCD_D0_PIN			13 //D0:23 UART1_TX:P0.13

#define LCD_PARALLEL_PORT	MML_GPIO_DEV0

#define LCD_COMMAND_GRAM	0x2C
#define LCD_COMMAND_MAC		0x36
#define LCD_CHAR_SPACE		5
#define X_OFFSET			10
#define Y_OFFSET			16
#define LCD_SPI_DEVID 		MML_SPI_DEV1
#define NORFLASH_SECTOR_SIZE 0x00010000


#define LCD_WR_LO() 	(mml_gpio_pin_output(LCD_PARALLEL_PORT, LCD_WR_PIN, MML_GPIO_OUT_LOGIC_ZERO))
#define LCD_WR_HI()     (mml_gpio_pin_output(LCD_PARALLEL_PORT, LCD_WR_PIN, MML_GPIO_OUT_LOGIC_ONE))

#define LCD_RS_CMD() 	(mml_gpio_pin_output(LCD_PARALLEL_PORT, LCD_RS_PIN, MML_GPIO_OUT_LOGIC_ZERO))
#define LCD_RS_DATA()   (mml_gpio_pin_output(LCD_PARALLEL_PORT, LCD_RS_PIN, MML_GPIO_OUT_LOGIC_ONE))

#define LCD_CS_SEL() 	(mml_gpio_pin_output(LCD_PARALLEL_PORT, LCD_CS_PIN, MML_GPIO_OUT_LOGIC_ZERO))
#define LCD_CS_CLR()    (mml_gpio_pin_output(LCD_PARALLEL_PORT, LCD_CS_PIN, MML_GPIO_OUT_LOGIC_ONE))

typedef enum
{
	Portrait_1,  /*!< Portrait orientation mode 1 */
	Portrait_2,  /*!< Portrait orientation mode 2 */
	Landscape_1, /*!< Landscape orientation mode 1 */
	Landscape_2  /*!< Landscape orientation mode 2 */
} mlsLCD_Orientation_t;

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
UInt8 gLCDBuffer[4800];

//void __attribute__ ((noinline))  __attribute__((optimize("-O0")))
void mlsOsalDelayMs(unsigned int ms)
{
     unsigned int i = ms*5000;
     unsigned int systemfreq = 0;
     mml_get_system_frequency(&systemfreq);

     i /= (MML_SYSTEM_FREQUENCY_108_MHZ/systemfreq);

     for (; i != 0 ; i--);

}
/**
 * @fn mlsParallelTransfer
 * @brief This function set the coordinate to write data to LCD
 */
void mlsParallelTransfer(UInt8 buffer);
/**
 * @fn mlsParallelTransferBuffer
 * @brief This function set the coordinate to write data to LCD
 */
void mlsParallelTransferBuffer(UInt8* buffer, UInt16 length);
/**
 * @fn mlsLCDSetCursorPosition
 * @brief This function set the coordinate to write data to LCD
 */
static mlsErrorCode_t mlsLCDSetCursorPosition(UInt16 x1, UInt16 y1, UInt16 x2, UInt16 y2);
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
static mlsErrorCode_t mlsLCDWriteDataBuffer(UInt8* buffer, UInt16 length);
/**
 * @fn mlsLCDWriteDataByte
 * @brief This function send data byte
 */

mlsErrorCode_t mlsLCDRotate(mlsLCD_Orientation_t orientation);
/**
 * @fn mlsLCDInitial_ST7789
 * @brief This function send command byte to initialize LCD
 */
static mlsErrorCode_t mlsLCDInitial_ST7789V(void);
/**
 * @fn mlsLCDSpiInit
 * @brief This function initialize SPI channel for LCD
 */
mlsErrorCode_t mlsLCDParallelInit(void);
/**
 * @fn mlsLCDInit
 * @brief This function initialize SPI channel for LCD
 */
mlsErrorCode_t mlsLCDInit(void);
/**
 * @fn mlsLCDInit
 * @brief This function initialize SPI channel for LCD
 */
mlsErrorCode_t mlsLCDDrawScreen(UInt16 color);

mlsErrorCode_t mlsLCDInit(void)
{
	mlsErrorCode_t errCode = MLS_SUCCESS;

	mlsLCDParallelInit();
	mlsLCDInitial_ST7789V();
//	mlsLCDDrawScreen(LCD_BLUE);
	return errCode;
}
mlsErrorCode_t mlsLCDParallelInit(void)
{
	mlsErrorCode_t errCode = MLS_SUCCESS;

	/* Initialize GPIO */
	mml_gpio_config_t 	gpio_conf;

	/** Initialize GPIO port */
	gpio_conf.gpio_direction = MML_GPIO_DIR_OUT;
	gpio_conf.gpio_function = MML_GPIO_NORMAL_FUNCTION;
	gpio_conf.gpio_pad_config = MML_GPIO_PAD_NORMAL;

	mml_gpio_init(MML_GPIO_DEV0, 12, 2, gpio_conf);
	mml_gpio_init(MML_GPIO_DEV0, 16, 4, gpio_conf);
	mml_gpio_init(MML_GPIO_DEV0, 25, 5, gpio_conf);

	LCD_RS_CMD();
	LCD_WR_LO();
	LCD_CS_CLR();
	mlsParallelTransfer(0x00);

	return errCode;
}

void mlsParallelTransfer(UInt8 buffer)
{
	mml_gpio_pin_output(LCD_PARALLEL_PORT,LCD_D7_PIN, (buffer >> 7) & 0x01);
	mml_gpio_pin_output(LCD_PARALLEL_PORT,LCD_D6_PIN, (buffer >> 6) & 0x01);
	mml_gpio_pin_output(LCD_PARALLEL_PORT,LCD_D5_PIN, (buffer >> 5) & 0x01);
	mml_gpio_pin_output(LCD_PARALLEL_PORT,LCD_D4_PIN, (buffer >> 4) & 0x01);
	mml_gpio_pin_output(LCD_PARALLEL_PORT,LCD_D3_PIN, (buffer >> 3) & 0x01);
	mml_gpio_pin_output(LCD_PARALLEL_PORT,LCD_D2_PIN, (buffer >> 2) & 0x01);
	mml_gpio_pin_output(LCD_PARALLEL_PORT,LCD_D1_PIN, (buffer >> 1) & 0x01);
	mml_gpio_pin_output(LCD_PARALLEL_PORT,LCD_D0_PIN, (buffer >> 0) & 0x01);
}

void mlsParallelTransferBuffer(UInt8* buffer, UInt16 length)
{
	unsigned int	size = length;
	unsigned char 	*ptr = (unsigned char *)buffer;

	while(size)
	{
		mlsLCDWriteDataByte(*ptr);
		size--;
		ptr++;
	}
}
void CHECK(UInt8 data)
{
	mlsLCDWriteCommandByte(data);
}

static mlsErrorCode_t mlsLCDWriteCommandByte(UInt8 command) // tested
{
	LCD_CS_SEL();
	LCD_WR_LO();
	LCD_RS_CMD();
	mlsParallelTransfer(command);
	LCD_WR_HI();
	LCD_CS_CLR();
	return MLS_SUCCESS;
}

static mlsErrorCode_t mlsLCDWriteDataByte(UInt8 data)  //tested
{
	LCD_CS_SEL();
	LCD_WR_LO();
	LCD_RS_DATA();
	mlsParallelTransfer(data);
	LCD_WR_HI();
	LCD_CS_CLR();
	return MLS_SUCCESS;
}
static mlsErrorCode_t mlsLCDWriteDataBuffer(UInt8* buffer, UInt16 length)
{
	mlsParallelTransferBuffer(buffer,length);
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

mlsErrorCode_t mlsLCDRotate(mlsLCD_Orientation_t orientation)
{
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

mlsErrorCode_t mlsLCDDrawScreen(UInt16 color)
{
    UInt32 i;
//    mlsOsalMutexLock(&gLcdMutex, MLS_OSAL_MAX_DELAY);
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

//    mlsOsalMutexUnlock(&gLcdMutex);
    return MLS_SUCCESS;
}
