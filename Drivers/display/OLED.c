//-----------------------------------------------------------------------------//
#include "OLED.h"
#include "gpio.h"
#include "spi.h"
#include "systick.h"
#include <string.h>
#include <stdio.h>

//-----------------------------------------------------------------------------//
#define SWAP(A, B) { A ^= B; B = A ^ B; A ^= B; }
#define ABS(A) (((A) < 0) ? ((-1) * (A)) : (A))

//-----------------------------------------------------------------------------//
static uint8_t oled_start_column = 2;		// Display shift
static uint8_t oled_buffer[1024];			// Display buffer

//-----------------------------------------------------------------------------//
void OLED_SendCommand(uint8_t command);
void OLED_SendData(uint8_t data);
void OLED_SendBuff(uint8_t * buf, uint8_t count);

//-----------------------------------------------------------------------------//
void OLED_ClearBuffer(uint8_t value)
{
	for (uint16_t i = 0; i < OLED_BUFFERSIZE; i++)
	{	oled_buffer[i] = value;	}
}

//-----------------------------------------------------------------------------//
void OLED_SendCommand(uint8_t command)
{
	LCD_CS(ON);
	LCD_DC(OFF);
	spi_write_byte (command);
	LCD_CS(OFF);
}

//-----------------------------------------------------------------------------//
void OLED_SendData(uint8_t data)
{
	LCD_CS(ON);
	//for (uint16_t count=0; count < 0xFFF; count++) {}
	//GPIO_OCTL(DISP_GPIO_Port) |= ((uint32_t)DC_Pin);
	LCD_DC(ON);
	spi_write_byte (data);
	LCD_CS(OFF);
}

//-----------------------------------------------------------------------------//
void OLED_SendBuff(uint8_t * buf, uint8_t count)
{
	LCD_CS(ON);
	for (uint8_t count; count < 0xFF; count++) {}
	LCD_DC(ON);
	for (uint8_t i = 0; i < count; i++)
	{
		spi_write_byte (*(buf + i));
	}
	LCD_CS(OFF);
}

//-----------------------------------------------------------------------------//
uint8_t OLED_init(void)
{
	LCD_RST(OFF);
	delay_1ms(3);	
  	LCD_RST(ON);
  	delay_1ms(3);	
	
	OLED_SendCommand(DISPLAY_OFF); // Display Off

	OLED_SendCommand(SET_CONTRAST_CONTROL);
	OLED_SendCommand(0xFF);

	OLED_SendCommand(NORMAL_DISPLAY); //0xA6-normal display, 0xA7-invert display
	OLED_SendCommand(DISPLAY_ALL_ON_RESUME);

	OLED_SendCommand(MEMORY_ADDR_MODE); //Set Memory Addressing Mode, 0x20   
	OLED_SendCommand(0x00); //00-Horizontal Addressing Mode
	/*OLED_SendCommand(PAGEMODE); //00-Horizontal Addressing Mode;01-Vertical Addressing Mode;10-Page Addressing Mode (RESET);11,Invalid
	OLED_SendCommand(STARTPAGEADDR);
	OLED_SendCommand(SET_LOWER_COLUMN);
	OLED_SendCommand(SET_HIGHER_COLUMN);*/

	OLED_SendCommand(SET_START_LINE | 0x00);
	OLED_SendCommand(SET_SEGMENT_REMAP | 0x00); //0x1 - left rotation, 0x0 - right rotation

	OLED_SendCommand(SET_MULTIPLEX_RATIO);
	OLED_SendCommand(0x3F); //64 (128x64)

	OLED_SendCommand(COM_SCAN_DIR_DEC); //Set COM Output Scan Direction. 0xC8: remapped mode

	OLED_SendCommand(SET_DISPLAY_OFFSET);  //-set display offset
	OLED_SendCommand(0x00);

	OLED_SendCommand(SET_COM_PINS);//set com pins hardware configuration
	OLED_SendCommand(0x12);

	OLED_SendCommand(SET_DISPLAY_CLOCK_DIV_RATIO);
	//OLED_SendCommand(0x80);
	OLED_SendCommand(SETDIVIDERATIO);

	OLED_SendCommand(CHARGE_PUMP); //set pump voltage value
	OLED_SendCommand(0x22);

	OLED_SendCommand(SET_VCOM_DESELECT); //set Vcomh Deselect Level
	OLED_SendCommand(0x20); //~ 0.77·VCC

	OLED_SendCommand(DISPLAY_ON);  // Display On	

	OLED_Clear(NONE_INVERTED);

	OLED_UpdateScreen();

	return 1;
}

//-----------------------------------------------------------------------------//
void OLED_Clear(uint8_t mode)
{
	if (mode) 
	{	OLED_ClearBuffer(0xFF); }
	else
	{	OLED_ClearBuffer(0x00); }
}

//-----------------------------------------------------------------------------//
void OLED_Invert(uint8_t inverted)
{
	if (inverted)	
	{	OLED_SendCommand(INVERT_DISPLAY); }
	else			
	{	OLED_SendCommand(NORMAL_DISPLAY);	}
}

//-----------------------------------------------------------------------------//
void OLED_SetContrast(uint8_t contrast)
{
	OLED_SendCommand(SET_CONTRAST_CONTROL);
	OLED_SendCommand(contrast);
}

//---------------------------------------------------------------------//
uint8_t OLED_GetWidthStr(const char* str)
{
	uint8_t stl = strlen(str);
	uint8_t len = 0;

	for (uint8_t i = 0; i < stl; i++, *str++)
	{	len += fontbyte(Font.numchars * (*str - Font.offset) + 4);	}

	return len;
}

//---------------------------------------------------------------------//
uint8_t OLED_GetWidthNum(int16_t num)
{
	char buf[15];
	sprintf(buf, "%d", num);
	return OLED_GetWidthStr(buf);
}

//------------------------------DRAW_SCREEN------------------------------//
void OLED_DrawNumSetMode(int16_t num, uint8_t x, uint8_t y, uint8_t mode)
{
	char buf[15];
	sprintf(buf, "%d", num);
	OLED_DrawStrSetMode (buf, x, y, mode);
}

//---------------------------------------------------------------------//
void OLED_DrawNum(int16_t num, uint8_t x, uint8_t y)
{
	char buf[15];
	sprintf(buf, "%d", num);
	OLED_DrawStr (buf, x, y);
}

//---------------------------------------------------------------------//
void OLED_DrawStrSetMode(const char *str, uint8_t x, uint8_t y, uint8_t mode)
{
	uint8_t stl = strlen(str);
	uint8_t pos = x;

	if (pos == RIGHT) 
	{	pos = OLED_WIDTH - OLED_GetWidthStr(str) - 1; }
	if (pos == CENTER) 
	{	pos = (OLED_WIDTH - OLED_GetWidthStr(str) - 1) / 2; }
	if (y == CENTER) 
	{	y = (OLED_HEIGHT - Font.height) / 2;	}

	for (uint8_t cnt = 0; cnt < stl; cnt++, *str++)
	{
		OLED_DrawCharSetMode(*str, pos, y, mode);
		pos += fontbyte(Font.numchars * (*str - Font.offset) + 4);
	}
}

//---------------------------------------------------------------------//
void OLED_DrawCharSetMode(char c, uint8_t ix, uint8_t iy, uint8_t mode)
{
	const uint8_t* symbol = &fontbyte(Font.numchars*(c-Font.offset) + 4);

	for (uint8_t x = 0; x < symbol[0]; x++)
	{
		for (uint8_t y = 0; y < Font.height; y++)
		{
			
			if (symbol[1 + y / 8 + x * (Font.numchars - 1) / Font.width] & (0x01 << (y % 8)))
			{	OLED_DrawPixelStatus(ix + x, iy + y, (mode) ? 1 : 0); }
			else
			{	OLED_DrawPixelStatus(ix + x, iy + y, (mode) ? 0 : 1); }
		}
	}
}

//---------------------------------------------------------------------//
void OLED_DrawStr(const char *str, uint8_t x, uint8_t y)
{
	uint8_t stl = strlen(str);
	uint8_t pos = x;

	if (pos == RIGHT) 
	{	pos = OLED_WIDTH - OLED_GetWidthStr(str) - 1; }
	if (pos == CENTER) 
	{	pos = (OLED_WIDTH - OLED_GetWidthStr(str) - 1) / 2; }
	if (y == CENTER) 
	{	y = (OLED_HEIGHT - Font.height) / 2;	}

	for (uint8_t cnt = 0; cnt < stl; cnt++, *str++)
	{
		OLED_DrawChar(*str, pos, y);
		pos += fontbyte(Font.numchars * (*str - Font.offset) + 4);
	}
}

//---------------------------------------------------------------------//
void OLED_DrawChar(char c, uint8_t ix, uint8_t iy)
{
	const uint8_t* symbol = &fontbyte(Font.numchars*(c-Font.offset) + 4);

	for (uint8_t x = 0; x < symbol[0]; x++)
	{
		for (uint8_t y = 0; y < Font.height; y++)
		{
			
			if (symbol[1 + y / 8 + x * (Font.numchars - 1) / Font.width] & (0x01 << (y % 8)))
			{	OLED_DrawPixelStatus(ix + x, iy + y, 1); } 
			else
			{	OLED_DrawPixelStatus(ix + x, iy + y, 0);	}
		}
	}
}

//---------------------------------------------------------------------//
void OLED_DrawBuffer(const uint8_t *progmem_buffer)
{
	uint8_t current_byte;

	for (uint8_t y_pos = 0; y_pos < OLED_HEIGHT; y_pos++)
	{
		for (uint8_t x_pos = 0; x_pos < OLED_WIDTH; x_pos++)
		{
			current_byte = progmem_buffer[y_pos * 16 + x_pos / 8];

			if (current_byte & (OLED_WIDTH >> (x_pos & 7)))	
			{	OLED_DrawPixelStatus(x_pos, y_pos, 1); }
			else											
			{	OLED_DrawPixelStatus(x_pos, y_pos, 0); }
		}
	}
}

//---------------------------------------------------------------------//
void OLED_DrawPixelStatus(uint8_t pos_x, uint8_t pos_y, uint8_t mode)
{
	if (pos_x >= OLED_WIDTH || pos_y >= OLED_HEIGHT) 
		return;
	
	if (mode)	
	{	oled_buffer[pos_x + (pos_y / 8) * OLED_WIDTH] |= (1 << (pos_y & 7)); }
	else		
	{	oled_buffer[pos_x + (pos_y / 8) * OLED_WIDTH] &= ~(1 << (pos_y & 7)); }
}

//---------------------------------------------------------------------//
void OLED_DrawPixel(uint8_t pos_x, uint8_t pos_y)
{
	if (pos_x >= OLED_WIDTH || pos_y >= OLED_HEIGHT) 
		return;
	
	oled_buffer[pos_x + (pos_y / 8) * OLED_WIDTH] |= (1 << (pos_y & 7));
}

//------------------------------------------------------------------------//
void OLED_DrawBitmap(const uint8_t *progmem_bitmap, uint8_t height, uint8_t width, uint8_t pos_x, uint8_t pos_y)
{
	uint8_t current_byte;
	uint8_t byte_width = (width + 7) / 8;

	for (uint8_t current_y = 0; current_y < height; current_y++)
	{
		for (uint8_t current_x = 0; current_x < width; current_x++)
		{
			current_byte = progmem_bitmap[current_y * byte_width + current_x / 8];

			if (current_byte & (OLED_WIDTH >> (current_x & 7)))	
			{	OLED_DrawPixelStatus(current_x + pos_x, current_y + pos_y, 1);	}
			else												
			{	OLED_DrawPixelStatus(current_x + pos_x, current_y + pos_y, 0);	}
		}
	}
}

//---------------------------------------------------------------------//
void OLED_DrawXBM(int16_t xMove, int16_t yMove, const uint8_t *xbm)
{
	int16_t width = xbm[0];
	int16_t height = xbm[1];

	int16_t widthInXbm = (width + 7) / 8;
	uint8_t data = 0;
	
	for (int16_t y = 0; y < height; y++)
	{
		for (int16_t x = 0; x < width; x++ )
		{
			if (x & 0x7)	
			{	data >>= 1;	}									// Move a bit
			else			
			{	data = xbm[(x / 8) + y * widthInXbm + 2]; 	}	// Read new data every 8 bit

			if (data & 0x01)
			{	OLED_DrawPixel(xMove + x, yMove + y);	}	// if there is a bit draw it
		}
	}
}

//---------------------------------------------------------------------//
void OLED_DrawVLine(uint8_t x, uint8_t y, uint8_t length)
{
	for (uint8_t i = 0; i < length; ++i)
	{	OLED_DrawPixel(x, i + y);	}
}

//---------------------------------------------------------------------//
void OLED_DrawHLine(uint8_t x, uint8_t y, uint8_t length, uint8_t mode)
{
	for (uint8_t i = 0; i < length; ++i)
	{	OLED_DrawPixelStatus(i + x, y, mode);	}
}

//---------------------------------------------------------------------//
void OLED_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	uint8_t length = x2 - x1 + 1;
	uint8_t height = y2 - y1;

	OLED_DrawHLine(x1, y1, length, 1);
	OLED_DrawHLine(x1, y2, length, 1);
	OLED_DrawVLine(x1, y1, height);
	OLED_DrawVLine(x2, y1, height);
}

//---------------------------------------------------------------------//
void OLED_DrawRectangleFill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode)
{
	uint8_t length = x2 - x1 + 1;
	uint8_t height = y2 - y1;

	for (int16_t x = 0; x < length; ++x)
	{
		for (int16_t y = 0; y <= height; ++y)
		{	OLED_DrawPixelStatus(x1 + x, y + y1, mode);	}
	}
}

//---------------------------------------------------------------------//
void OLED_DrawTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3) 
{
	OLED_DrawLine(x1, y1, x2, y2);
	OLED_DrawLine(x2, y2, x3, y3);
	OLED_DrawLine(x3, y3, x1, y1);
}

//---------------------------------------------------------------------//
void OLED_DrawTriangleFill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3) 
{
	int16_t delta_x = 0, delta_y = 0;
	int16_t	x = 0, y = 0, x_inc_1 = 0, x_inc_2 = 0, y_inc_1 = 0, y_inc_2 = 0;
	int16_t	den = 0, num = 0, num_add = 0, num_pixels = 0;

	delta_x = ABS(x2 - x1);
	delta_y = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1)	
	{	
		x_inc_1 = 1;	
		x_inc_2 = 1;	
	}
	else 			
	{	
		x_inc_1 = -1;	
		x_inc_2 = -1;	
	}

	if (y2 >= y1) 	
	{	
		y_inc_1 = 1;	
		y_inc_2 = 1;	
	}
	else 			
	{	
		y_inc_1 = -1;	
		y_inc_2 = -1;	
	}

	if (delta_x >= delta_y)
	{
		x_inc_1 = 0;
		y_inc_2 = 0;
		den = delta_x;
		num = delta_x / 2;
		num_add = delta_y;
		num_pixels = delta_x;
	}
	else
	{
		x_inc_2 = 0;
		y_inc_1 = 0;
		den = delta_y;
		num = delta_y / 2;
		num_add = delta_x;
		num_pixels = delta_y;
	}

	for (int16_t cur_pixel = 0; cur_pixel <= num_pixels; cur_pixel++)
	{
		OLED_DrawLine(x, y, x3, y3);

		num += num_add;
		if (num >= den)
		{
			num -= den;
			x += x_inc_1;
			y += y_inc_1;
		}
		x += x_inc_2;
		y += y_inc_2;
	}
}

//---------------------------------------------------------------------//
void OLED_DrawCircle(int16_t x0, int16_t y0, uint8_t r)
{
	int16_t x = 0;
	int16_t y = r;
	int16_t ddF_y = - (r << 1);
	int16_t ddF_x = 1;
	int16_t f = 1 - r;

	OLED_DrawPixel(x0, y0 + r);
	OLED_DrawPixel(x0, y0 - r);
	OLED_DrawPixel(x0 + r, y0);
	OLED_DrawPixel(x0 - r, y0);

	while(x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x;

		OLED_DrawPixel(x0 + x, y0 + y);
		OLED_DrawPixel(x0 - x, y0 + y);
		OLED_DrawPixel(x0 + x, y0 - y);
		OLED_DrawPixel(x0 - x, y0 - y);
		OLED_DrawPixel(x0 + y, y0 + x);
		OLED_DrawPixel(x0 - y, y0 + x);
		OLED_DrawPixel(x0 + y, y0 - x);
		OLED_DrawPixel(x0 - y, y0 - x);
	}
}

//---------------------------------------------------------------------//
void OLED_DrawCircleFill(int16_t x0, int16_t y0, int16_t r)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	OLED_DrawPixel(x0, y0 + r);
	OLED_DrawPixel(x0, y0 - r);
	OLED_DrawPixel(x0 + r, y0);
	OLED_DrawPixel(x0 - r, y0);
	OLED_DrawLine(x0 - r, y0, x0 + r, y0);

	while (x < y) 
	{
		if (f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		OLED_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y);
		OLED_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y);

		OLED_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x);
		OLED_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x);
	}
}

//---------------------------------------------------------------------//
void OLED_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	uint16_t curr_dx = ABS(x1 - x0);
	uint16_t curr_dy = ABS(y1 - y0);
	uint8_t step = curr_dy > curr_dx;

	if (step)
	{
		SWAP(x0, y0);
		SWAP(x1, y1);
		SWAP(curr_dx, curr_dy);
	}

	if (x0 > x1)
	{
		SWAP(x0, x1);
		SWAP(y0, y1);
	}

	int16_t y_step = (y0 < y1) ? 1 : -1;
	int16_t err = curr_dx >> 1;

	for ( ; x0 <= x1; x0++)
	{
		if (step)	
		{	OLED_DrawPixel(y0, x0);	}
		else		
		{	OLED_DrawPixel(x0, y0);	}
		
		err -= curr_dy;
		if (err < 0)
		{
			y0 += y_step;
			err += curr_dx;
		}
	}
}

//-------------------------------UPDATE_SCREEN-------------------------------//
void OLED_UpdateScreen()
{
	for (uint8_t page_number = 0; page_number < NUMB_PAGE ; page_number++)
	{
		OLED_UpdateOnePage(page_number);
	}
}

//--------------------------------------------------------------------------//
void OLED_UpdateOnePage(uint8_t page_number)
{
	OLED_SendCommand(0x00 | (oled_start_column & 0x0F));	// lo tetrad starting column address
	OLED_SendCommand(0x10 | (oled_start_column >> 4));		// hi tetrad starting column address
	OLED_SendCommand(0xB0 | page_number);					// starting address of the page

	for (int16_t j = 0; j < OLED_WIDTH; j++)
	{	OLED_SendData(oled_buffer[page_number * OLED_WIDTH + j]);	}
}

//---------------------------------------------------------------------//
