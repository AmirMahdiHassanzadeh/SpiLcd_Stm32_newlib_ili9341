#ifndef __SPI_ILI9341_H
#define __SPI_ILI9341_H
//-------------------------------------------------------------------
#include "stm32f1xx_hal.h"
#include <cstdlib>
#include "fonts.h"
#include "main.h"
//-------------------------------------------------------------------
#define RESET_ACTIVE() HAL_GPIO_WritePin(SPI2_RST_GPIO_Port,SPI2_RST_Pin,GPIO_PIN_RESET)
#define RESET_IDLE() 	 HAL_GPIO_WritePin(SPI2_RST_GPIO_Port,SPI2_RST_Pin,GPIO_PIN_SET)
#define CS_ACTIVE() 	 HAL_GPIO_WritePin(SPI2_CS_GPIO_Port,SPI2_CS_Pin,GPIO_PIN_RESET)
#define CS_IDLE() 		 HAL_GPIO_WritePin(SPI2_CS_GPIO_Port,SPI2_CS_Pin,GPIO_PIN_SET)
#define DC_COMMAND() 	 HAL_GPIO_WritePin(SPI2_DC_GPIO_Port,SPI2_DC_Pin,GPIO_PIN_RESET)
#define DC_DATA() 		 HAL_GPIO_WritePin(SPI2_DC_GPIO_Port,SPI2_DC_Pin,GPIO_PIN_SET)
//-------------------------------------------------------------------
#define SPI_HANDLE  hspi2


extern SPI_HandleTypeDef    SPI_HANDLE ;
//-------------------------------------------------------------------

#define ili9341_MADCTL_MY  0x80
#define ili9341_MADCTL_MX  0x40
#define ili9341_MADCTL_MV  0x20
#define ili9341_MADCTL_ML  0x10
#define ili9341_MADCTL_RGB 0x00
#define ili9341_MADCTL_BGR 0x08
#define ili9341_MADCTL_MH  0x04
#define ili9341_ROTATION (ili9341_MADCTL_MX | ili9341_MADCTL_BGR)
#define	ili9341_BLACK   	  0x0000
#define	ili9341_BLUE       0x001F
#define	ili9341_RED        0xF800
#define	ili9341_GREEN      0x07E0
#define ili9341_CYAN       0x07FF
#define ili9341_MAGENTA    0xF81F
#define ili9341_YELLOW     0xFFE0
#define ili9341_WHITE      0xFFFF



//-------------------------------------------------------------------
#define swap(a,b) {int16_t t=a;a=b;b=t;}
//-------------------------------------------------------------------
void ili9341_init(uint16_t w_size, uint16_t h_size);


void ili9341_SendCommand(uint8_t cmd);
void ili9341_SendData(uint8_t dt);

void ili9341_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);


void ili9341_FillScreen(uint16_t color);


uint16_t ili9341_RandColor(void);


void ili9341_SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);


void ili9341_DrawPixel(int x, int y, uint16_t color);

void ili9341_DrawLine(uint16_t color, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2);


void ili9341_DrawRect(uint16_t color, uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2);


void ili9341_DrawCircle(uint16_t x0, uint16_t y0, int r, uint16_t color);

void ili9341_SetTextColor(uint16_t color);

void ili9341_SetBackColor(uint16_t color);

void ili9341_SetFont(sFONT *pFonts);

void ili9341_DrawChar_with_backColor(uint16_t x, uint16_t y, uint8_t c);

void ili9341_DrawChar_with_backglass(uint16_t x, uint16_t y, uint8_t c);

void ili9341_DrawChar_withOut_backColor(uint16_t x, uint16_t y, uint8_t c);

void ili9341_String_with_backColor(uint16_t x,uint16_t y, uint8_t *str);

void ili9341_String_with_backglass(uint16_t x,uint16_t y, uint8_t *str);

void ili9341_String_withOut_backColor(uint16_t x,uint16_t y, uint8_t *str);

void ili9341_SetRotation(uint8_t r);

void ili9341_show_custom_picture (uint16_t width, uint16_t height, uint16_t x, uint16_t y, uint8_t *pole);

void ili9341_show_picture(uint16_t ili_HEIGHT,uint16_t ili_WIDTH,uint8_t *buff);// sends data simply one by one from array
//-------------------------------------------------------------------
#endif /* __SPI_ILI9341_H */


