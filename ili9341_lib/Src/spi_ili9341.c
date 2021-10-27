#include "spi_ili9341.h"

#include <stdio.h>
#include <stdlib.h>


#include "spi_ili9341.h"
#include "font24.c"
#include "font20.c"
#include "font16.c"
#include "font12.c"
//#include "font8.c"
//-------------------------------------------------------------------

//extern RNG_HandleTypeDef hrng;
//-------------------------------------------------------------------
uint16_t ili9341_WIDTH =240;
uint16_t ili9341_HEIGHT =320;
//-------------------------------------------------------------------
typedef struct
{
	uint16_t TextColor;
	uint16_t BackColor;
	sFONT *pFont;
}LCD_DrawPropTypeDef;
LCD_DrawPropTypeDef lcdprop;
//-------------------------------------------------------------------
void ili9341_SendCommand(uint8_t cmd)
{
  DC_COMMAND();
  HAL_SPI_Transmit_IT (&SPI_HANDLE, &cmd, 1);
}
//-------------------------------------------------------------------
void ili9341_SendData(uint8_t dt)
{
	DC_DATA();
	HAL_SPI_Transmit_IT (&SPI_HANDLE, &dt, 1);
}
//-------------------------------------------------------------------
static void ili9341_WriteData(uint8_t* buff, size_t buff_size) {
	DC_DATA();
	while(buff_size > 0) {
		uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
		HAL_SPI_Transmit_IT(&SPI_HANDLE, buff, chunk_size);
		buff += chunk_size;
		buff_size -= chunk_size;
	}
}
//-------------------------------------------------------------------
void ili9341_reset(void)
{
	RESET_ACTIVE();
	HAL_Delay(5);
	RESET_IDLE();
}
//-------------------------------------------------------------------
 void ili9341_SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  // column address set
  ili9341_SendCommand(0x2A); // CASET
  {
    uint8_t data[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
    ili9341_WriteData(data, sizeof(data));
  }
 
  // row address set
  ili9341_SendCommand(0x2B); // RASET
  {
    uint8_t data[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
    ili9341_WriteData(data, sizeof(data));
  }
 
  // write to RAM
  ili9341_SendCommand(0x2C); // RAMWR
}//-------------------------------------------------------------------



void ili9341_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  if((x1 >= ili9341_WIDTH) || (y1 >= ili9341_HEIGHT) || (x2 >= ili9341_WIDTH) || (y2 >= ili9341_HEIGHT)) return;
	if(x1>x2) swap(x1,x2);
	if(y1>y2) swap(y1,y2);
  ili9341_SetAddrWindow(x1, y1, x2, y2);
  uint8_t data[] = { color >> 8, color & 0xFF };
  DC_DATA();
  for(uint32_t i = 0; i < (x2-x1+1)*(y2-y1+1); i++)
  {
      HAL_SPI_Transmit_IT(&SPI_HANDLE, data, 2);
  }
}
//-------------------------------------------------------------------
void ili9341_FillScreen(uint16_t color)
{
  ili9341_FillRect(0, 0, ili9341_WIDTH-1, ili9341_HEIGHT-1, color);
}
//-------------------------------------------------------------------
void ili9341_DrawPixel(int x, int y, uint16_t color)
{
	if((x<0)||(y<0)||(x>=ili9341_WIDTH)||(y>=ili9341_HEIGHT)) return;
	ili9341_SetAddrWindow(x,y,x,y);
	ili9341_SendCommand(0x2C);
	ili9341_SendData(color>>8);
	ili9341_SendData(color & 0xFF);
}
//------------------------------------------------------------------
void ili9341_DrawLine(uint16_t color, uint16_t x1, uint16_t y1,
											uint16_t x2, uint16_t y2)
{
  int steep = abs(y2-y1)>abs(x2-x1);
  if(steep)
  {
    swap(x1,y1);
    swap(x2,y2);
  }
  if(x1>x2)
  {
    swap(x1,x2);
    swap(y1,y2);
  }
  int dx,dy;
  dx=x2-x1;
  dy=abs(y2-y1);
  int err=dx/2;
  int ystep;
  if(y1<y2) ystep=1;
  else ystep=-1;
  for(;x1<=x2;x1++)
  {
    if(steep) ili9341_DrawPixel(y1,x1,color);
    else ili9341_DrawPixel(x1,y1,color);
    err-=dy;
    if(err<0)
    {
      y1 += ystep;
      err+=dx;
    }
  }
}
//-------------------------------------------------------------------
void ili9341_DrawRect(uint16_t color, uint16_t x1, uint16_t y1,
											uint16_t x2, uint16_t y2)
{
	ili9341_DrawLine(color,x1,y1,x2,y1);
	ili9341_DrawLine(color,x2,y1,x2,y2);
	ili9341_DrawLine(color,x1,y1,x1,y2);
	ili9341_DrawLine(color,x1,y2,x2,y2);
}
//-------------------------------------------------------------------
void ili9341_DrawCircle(uint16_t x0, uint16_t y0, int r, uint16_t color)
{
	int f = 1-r;
	int ddF_x=1;
	int ddF_y=-2*r;
	int x = 0;
	int y = r;
	ili9341_DrawPixel(x0,y0+r,color);
	ili9341_DrawPixel(x0,y0-r,color);
	ili9341_DrawPixel(x0+r,y0,color);
	ili9341_DrawPixel(x0-r,y0,color);
	while (x<y)
	{
		if (f>=0)
		{
			y--;
			ddF_y+=2;
			f+=ddF_y;
		}
		x++;
		ddF_x+=2;
		f+=ddF_x;
		ili9341_DrawPixel(x0+x,y0+y,color);
		ili9341_DrawPixel(x0-x,y0+y,color);
		ili9341_DrawPixel(x0+x,y0-y,color);
		ili9341_DrawPixel(x0-x,y0-y,color);
		ili9341_DrawPixel(x0+y,y0+x,color);
		ili9341_DrawPixel(x0-y,y0+x,color);
		ili9341_DrawPixel(x0+y,y0-x,color);
		ili9341_DrawPixel(x0-y,y0-x,color);
	}
}
//-------------------------------------------------------------------
uint16_t ili9341_RandColor(void)
{
	return rand()&0x0000FFFF;
}
//-------------------------------------------------------------------
void ili9341_SetTextColor(uint16_t color)
{
  lcdprop.TextColor=color;
}
//------------------------------------
void ili9341_SetBackColor(uint16_t color)
{
  lcdprop.BackColor=color;
}
//------------------------------------
void ili9341_SetFont(sFONT *pFonts)
{
  lcdprop.pFont=pFonts;
}
//-------------------------------------------------------------------
void ili9341_DrawChar_with_backColor(uint16_t x, uint16_t y, uint8_t c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t offset;
  uint8_t *c_t;
  uint8_t *pchar;
  uint32_t line=0;
  height = lcdprop.pFont->Height;
  width  = lcdprop.pFont->Width;
  offset = 8 *((width + 7)/8) -  width ;
  c_t = (uint8_t*) &(lcdprop.pFont->table[(c-' ') * lcdprop.pFont->Height * ((lcdprop.pFont->Width + 7) / 8)]);
  for(i = 0; i < height; i++)
  {
    pchar = ((uint8_t *)c_t + (width + 7)/8 * i);
    switch(((width + 7)/8))
    {
      case 1:
          line =  pchar[0];      
          break;
      case 2:
          line =  (pchar[0]<< 8) | pchar[1];
          break;
      case 3:
      default:
        line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];      
        break;
    }
    for (j = 0; j < width; j++)
    {
      if(line & (1 << (width- j + offset- 1))) 
      {
        ili9341_DrawPixel((x + j), y, lcdprop.TextColor);
      }
      else
      {
				ili9341_DrawPixel((x + j), y, lcdprop.BackColor);	
				
      
      } 
    }
    y++;      
  }
}


void ili9341_DrawChar_with_backglass(uint16_t x, uint16_t y, uint8_t c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t offset;
  uint8_t *c_t;
  uint8_t *pchar;
  uint32_t line=0;
  height = lcdprop.pFont->Height;
  width  = lcdprop.pFont->Width;
  offset = 8 *((width + 7)/8) -  width ;
  c_t = (uint8_t*) &(lcdprop.pFont->table[(c-' ') * lcdprop.pFont->Height * ((lcdprop.pFont->Width + 7) / 8)]);
  for(i = 0; i < height; i++)
  {
    pchar = ((uint8_t *)c_t + (width + 7)/8 * i);
    switch(((width + 7)/8))
    {
      case 1:
          line =  pchar[0];      
          break;
      case 2:
          line =  (pchar[0]<< 8) | pchar[1];
          break;
      case 3:
      default:
        line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];      
        break;
    }
    for (j = 0; j < width; j++)
    {
      if(line & (1 << (width- j + offset- 1))) 
      {
        ili9341_DrawPixel((x + j), y, lcdprop.TextColor);
      }
      else
      {
				static char s = 0 ; 
				switch (s){
					case 0:
						  ili9341_DrawPixel((x + j), y, lcdprop.BackColor);
					s=1;
					break;
					case 1:
						
					s=0;
					break;
				}
      
      } 
    }
    y++;      
  }
}

extern const uint8_t windows11_img[];


void ili9341_DrawChar_withOut_backColor(uint16_t x, uint16_t y, uint8_t c)
{
	uint8_t bx[x];
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t offset;
  uint8_t *c_t;
  uint8_t *pchar;
  uint32_t line=0;
  height = lcdprop.pFont->Height;
  width  = lcdprop.pFont->Width;
  offset = 8 *((width + 7)/8) -  width ;
  c_t = (uint8_t*) &(lcdprop.pFont->table[(c-' ') * lcdprop.pFont->Height * ((lcdprop.pFont->Width + 7) / 8)]);
  	
	for(i = 0; i < height; i++)
  {
    pchar = ((uint8_t *)c_t + (width + 7)/8 * i);
    switch(((width + 7)/8))
    {
      case 1:
          line =  pchar[0];      
          break;
      case 2:
          line =  (pchar[0]<< 8) | pchar[1];
          break;
      case 3:
      default:
        line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];      
        break;
    }
    for (j = 0; j < width; j++)
    {
      if(line & (1 << (width- j + offset- 1))) 
      {
        ili9341_DrawPixel((x + j), y, lcdprop.TextColor);
      }
      else
      {
				
			
      } 
    }
    y++;      
  }
}
//-------------------------------------------------------------------
void ili9341_String_with_backColor(uint16_t x,uint16_t y, uint8_t *str)
{
  while(*str)
  {
    ili9341_DrawChar_with_backColor(x,y,str[0]);
    x+=lcdprop.pFont->Width;
    (void)*str++;
  }
}


void ili9341_String_with_backglass(uint16_t x,uint16_t y, uint8_t *str)
{
  while(*str)
  {
    ili9341_DrawChar_with_backglass(x,y,str[0]);
    x+=lcdprop.pFont->Width;
    (void)*str++;
  }
}

void ili9341_String_withOut_backColor(uint16_t x,uint16_t y, uint8_t *str)
{
	
  while(*str)
  {
	
    ili9341_DrawChar_withOut_backColor(x,y,str[0]);
    x+=lcdprop.pFont->Width;
    (void)*str++;
  }
	
}
//-------------------------------------------------------------------
void ili9341_SetRotation(uint8_t r)
{
  ili9341_SendCommand(0x36);
  switch(r)
  {
    case 0:
      ili9341_SendData(0x48);
      ili9341_WIDTH = 240;
      ili9341_HEIGHT = 320;
      break;
    case 1:
      ili9341_SendData(0x28);
      ili9341_WIDTH = 	320;
      ili9341_HEIGHT = 240;
      break;
    case 2:
      ili9341_SendData(0x88);
      ili9341_WIDTH = 240;
      ili9341_HEIGHT = 320;
      break;
    case 3:
      ili9341_SendData(0xE8);
      ili9341_WIDTH = 320;
      ili9341_HEIGHT = 240;
      break;
  }
}
//-------------------------------------------------------------------
void ili9341_FontsIni(void)
{
  Font8.Height = 8;
  Font8.Width = 5;
  Font12.Height = 12;
  Font12.Width = 7;
  Font16.Height = 16;
  Font16.Width = 11;
  Font20.Height = 20;
  Font20.Width = 14;
  Font24.Height = 24;
  Font24.Width = 17;
  lcdprop.BackColor=ili9341_BLACK;
  lcdprop.TextColor=ili9341_GREEN;
  lcdprop.pFont=&Font16;
}
//-------------------------------------------------------------------
void ili9341_init(uint16_t w_size, uint16_t h_size)
{
  uint8_t data[15];
  CS_ACTIVE();
  ili9341_reset();
  //Software Reset
  ili9341_SendCommand(0x01);
  HAL_Delay(1000);
  //Power Control A  
  data[0] = 0x39;
  data[1] = 0x2C;
  data[2] = 0x00;
  data[3] = 0x34;
  data[4] = 0x02;
  ili9341_SendCommand(0xCB);
  ili9341_WriteData(data, 5);
  //Power Control B
  data[0] = 0x00;
  data[1] = 0xC1;
  data[2] = 0x30;
  ili9341_SendCommand(0xCF);
  ili9341_WriteData(data, 3);
  //Driver timing control A
  data[0] = 0x85;
  data[1] = 0x00;
  data[2] = 0x78;
  ili9341_SendCommand(0xE8);
  ili9341_WriteData(data, 3);
  //Driver timing control B
  data[0] = 0x00;
  data[1] = 0x00;
  ili9341_SendCommand(0xEA);
  ili9341_WriteData(data, 2);
  //Power on Sequence control
  data[0] = 0x64;
  data[1] = 0x03;
  data[2] = 0x12;
  data[3] = 0x81;
  ili9341_SendCommand(0xED);
  ili9341_WriteData(data, 4);
  //Pump ratio control
  data[0] = 0x20;
  ili9341_SendCommand(0xF7);
  ili9341_WriteData(data, 1);
  //Power Control,VRH[5:0]
  data[0] = 0x10;
  ili9341_SendCommand(0xC0);
  ili9341_WriteData(data, 1);
  //Power Control,SAP[2:0];BT[3:0]
  data[0] = 0x10;
  ili9341_SendCommand(0xC1);
  ili9341_WriteData(data, 1);
  //VCOM Control 1
  data[0] = 0x3E;
  data[1] = 0x28;
  ili9341_SendCommand(0xC5);
  ili9341_WriteData(data, 2);
  //VCOM Control 2
  data[0] = 0x86;
  ili9341_SendCommand(0xC7);
  ili9341_WriteData(data, 1);
  //Memory Acsess Control
  data[0] = 0x48;
  ili9341_SendCommand(0x36);
  ili9341_WriteData(data, 1);
  //Pixel Format Set
  data[0] = 0x55;//16bit
  ili9341_SendCommand(0x3A);
  ili9341_WriteData(data, 1);
  //Frame Rratio Control, Standard RGB Color
  data[0] = 0x00;
  data[1] = 0x18;
  ili9341_SendCommand(0xB1);
  ili9341_WriteData(data, 2);
  //Display Function Control
  data[0] = 0x08;
  data[1] = 0x82;
  data[2] = 0x27;//320 строк
  ili9341_SendCommand(0xB6);
  ili9341_WriteData(data, 3);
  //Enable 3G (пока не знаю что это за режим)
  data[0] = 0x00;//не включаем
  ili9341_SendCommand(0xF2);
  ili9341_WriteData(data, 1);
  //Gamma set
  data[0] = 0x01;//Gamma Curve (G2.2) (Кривая цветовой гаммы)
  ili9341_SendCommand(0x26);
  ili9341_WriteData(data, 1);
  //Positive Gamma  Correction
  data[0] = 0x0F;
  data[1] = 0x31;
  data[2] = 0x2B;
  data[3] = 0x0C;
  data[4] = 0x0E;
  data[5] = 0x08;
  data[6] = 0x4E;
  data[7] = 0xF1;
  data[8] = 0x37;
  data[9] = 0x07;
  data[10] = 0x10;
  data[11] = 0x03;
  data[12] = 0x0E;
  data[13] = 0x09;
  data[14] = 0x00;
  ili9341_SendCommand(0xE0);
  ili9341_WriteData(data, 15);
  //Negative Gamma  Correction
  data[0] = 0x00;
  data[1] = 0x0E;
  data[2] = 0x14;
  data[3] = 0x03;
  data[4] = 0x11;
  data[5] = 0x07;
  data[6] = 0x31;
  data[7] = 0xC1;
  data[8] = 0x48;
  data[9] = 0x08;
  data[10] = 0x0F;
  data[11] = 0x0C;
  data[12] = 0x31;
  data[13] = 0x36;
  data[14] = 0x0F;
  ili9341_SendCommand(0xE1);
  ili9341_WriteData(data, 15);
  ili9341_SendCommand(0x11);//Выйдем из спящего режима
  HAL_Delay(120);
  //Display ON
  data[0] = ili9341_ROTATION;
  ili9341_SendCommand(0x29);
  ili9341_WriteData(data, 1);
  ili9341_WIDTH = w_size;
  ili9341_HEIGHT = h_size;
  ili9341_FontsIni();
}
//-------------------------------------------------------------------



void ili9341_show_custom_picture (uint16_t width, uint16_t height, uint16_t x, uint16_t y, uint8_t *pole)
{

	uint16_t i;

	for(i=0;i<width;i++)
	{
		ili9341_SetAddrWindow(x,ili9341_HEIGHT-(y+i),ili9341_WIDTH,ili9341_HEIGHT);
		DC_DATA() ;
		HAL_SPI_Transmit_IT(&SPI_HANDLE, &pole[(height*width*2)-(i*height*2+(height*2))], 2*height);
	}
}

void ili9341_show_picture(uint16_t ili_HEIGHT,uint16_t ili_WIDTH,uint8_t *buff)// sends data simply one by one from array
{	
		ili9341_SetAddrWindow(0,0,ili_WIDTH,ili_HEIGHT);
		DC_DATA() ;
		HAL_SPI_Transmit_IT(&SPI_HANDLE, (uint8_t *)&buff[0], 38400);
		HAL_SPI_Transmit_IT(&SPI_HANDLE, (uint8_t *)&buff[38400], 38400);
		HAL_SPI_Transmit_IT(&SPI_HANDLE, (uint8_t *)&buff[76800], 38400);
		HAL_SPI_Transmit_IT(&SPI_HANDLE, (uint8_t *)&buff[115200], 38400);
}


