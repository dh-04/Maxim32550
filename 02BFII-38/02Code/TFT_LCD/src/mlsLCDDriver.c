#include "mlsLCDDriver.h"
#include "mlsLCDFont.h"

#include <MAX325xx.h>

#define LCD_A0_PIN			30
#define LCD_A0_PORT			MML_GPIO_DEV0

#define LCD_CS_PIN			28
#define LCD_CS_PORT			MML_GPIO_DEV0

#define LCD_RTS_PIN			31
#define LCD_RTS_PORT		MML_GPIO_DEV0

#define LCD_SPI_DEVID		MML_SPI_DEV1

#define LCD_COMMAND_GRAM	0x2C
#define LCD_COMMAND_MAC		0x36
#define LCD_CHAR_SPACE		5
#define X_OFFSET			10
#define Y_OFFSET			16


#define LCD_A0_CMD() 	(mml_gpio_pin_output(LCD_A0_PORT, LCD_A0_PIN, MML_GPIO_OUT_LOGIC_ZERO))
#define LCD_A0_DATA()   (mml_gpio_pin_output(LCD_A0_PORT, LCD_A0_PIN, MML_GPIO_OUT_LOGIC_ONE))

#define LCD_CS_SEL() 	(mml_gpio_pin_output(LCD_CS_PORT, LCD_CS_PIN, MML_GPIO_OUT_LOGIC_ZERO))
#define LCD_CS_CLR()    (mml_gpio_pin_output(LCD_CS_PORT, LCD_CS_PIN, MML_GPIO_OUT_LOGIC_ONE))

#define LCD_RTS_LO() 	(mml_gpio_pin_output(LCD_RTS_PORT, LCD_RTS_PIN, MML_GPIO_OUT_LOGIC_ZERO))
#define LCD_RTS_HI()    (mml_gpio_pin_output(LCD_RTS_PORT, LCD_RTS_PIN, MML_GPIO_OUT_LOGIC_ONE))

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

void __attribute__ ((noinline))  __attribute__((optimize("-O0")))  mlsOsalDelayMs(unsigned int ms)
{
    unsigned int i = ms*5000;
    unsigned int systemfreq = 0;
    mml_get_system_frequency(&systemfreq);

    i /= (MML_SYSTEM_FREQUENCY_108_MHZ/systemfreq);

    for (; i != 0 ; i--)
         __asm volatile ("nop\n");
}

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
	mlsLCDDrawScreen(LCD_RED);
	return errCode;
}
mlsErrorCode_t mlsLCDParallelInit(void)
{
	mlsErrorCode_t errCode = MLS_SUCCESS;

	mml_spi_params_t	spi_conf;

	/** Fill parameters */
	spi_conf.baudrate 		= 12000000;
	spi_conf.word_size 		= 8;
	spi_conf.mode 			= SPIn_CNTL_MMEN_master;
	spi_conf.wor 			= SPIn_CNTL_WOR_disable;
	spi_conf.clk_pol 		= SPIn_CNTL_CLKPOL_idleLo;
	spi_conf.phase 			= SPIn_CNTL_PHASE_inactiveEdge;
	spi_conf.brg_irq 		= SPIn_CNTL_BIRQ_disable;
	spi_conf.ssv 			= SPIn_MOD_SSV_hi;
	spi_conf.ssio 			= SPIn_MOD_SSIO_output;
	spi_conf.tlj 			= SPIn_MOD_TX_LJ_disable;
	spi_conf.dma_tx.active 	= SPIn_DMA_REG_DMA_EN_disable;
	spi_conf.dma_rx.active 	= SPIn_DMA_REG_DMA_EN_disable;

	/** Set CS0 */
	//spi_conf.ssio = 0x1;

	/** Call initialization function from driver */
	if(mml_spi_init(LCD_SPI_DEVID, (mml_spi_params_t*)&spi_conf) == NO_ERROR)
	{
		/** Now enable SPI interface */
		M_MML_SPI_ENABLE(LCD_SPI_DEVID);
	}

	/*Channel Source Address*/
	DMAC->DMA_CH[MML_DMA_CH0].DMAn_SRC = 0;

	/*Enable Destination Increment*/
	DMAC->DMA_CH[MML_DMA_CH0].DMAn_CFG |= DMAn_CFG_DSTINC_Msk;

	/*Channel Destination Address*/
	DMAC->DMA_CH[MML_DMA_CH0].DMAn_DST = 0;

	/*Enable Source Increment*/
	DMAC->DMA_CH[MML_DMA_CH0].DMAn_CFG |= DMAn_CFG_SRCINC_Msk;

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
	mml_gpio_init(LCD_RTS_PORT, LCD_RTS_PIN, 1, gpio_conf);

	LCD_CS_CLR();
	LCD_A0_CMD();
	LCD_RTS_HI();

	return errCode;
}

static mlsErrorCode_t mlsLCDWriteCommandByte(UInt8 command)
{
	LCD_A0_CMD();
	LCD_CS_SEL();
	mml_spi_dma_rw(LCD_SPI_DEVID, MML_DMA_CH0, MML_DMA_CH1, &command, NULL, 1);
	LCD_CS_CLR();
	return MLS_SUCCESS;
}

static mlsErrorCode_t mlsLCDWriteDataByte(UInt8 data)
{
	LCD_A0_DATA();
	LCD_CS_SEL();
	mml_spi_dma_rw(LCD_SPI_DEVID, MML_DMA_CH0, MML_DMA_CH1, &data, NULL, 1);
	LCD_CS_CLR();
	return MLS_SUCCESS;
}

static mlsErrorCode_t mlsLCDWriteDataBuffer(UInt8* buffer, UInt16 length)
{
	LCD_A0_DATA();
	LCD_CS_SEL();
	mml_spi_dma_rw(LCD_SPI_DEVID, MML_DMA_CH0, MML_DMA_CH1, buffer, NULL, length);
	LCD_CS_CLR();
	return MLS_SUCCESS;
}

static mlsErrorCode_t mlsLCDInitial_ST7789V(void)
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

    return MLS_SUCCESS;
}
