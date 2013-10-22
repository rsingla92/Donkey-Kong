/*
 * display.c
 *
 *  Created on: 2013-09-18
 *      Author: Jeremy
 */
#include <stdio.h>
#include "display.h"
#include "io.h"

volatile static alt_up_pixel_buffer_dma_dev* pixel_buffer = NULL;
static alt_up_char_buffer_dev *char_buffer = NULL;
static unsigned int pixel_buffer_addr1, pixel_buffer_addr2;

/* The following function was taken from Jeff's optimization code: */
int draw_pixel_fast(alt_up_pixel_buffer_dma_dev *pixel_buffer,
		unsigned int color, unsigned int x, unsigned int y) {
	unsigned int addr;

	addr = ((x & pixel_buffer->x_coord_mask) << 1);
	addr += (((y & pixel_buffer->y_coord_mask) * 320) << 1);

	IOWR_16DIRECT(pixel_buffer->back_buffer_start_address, addr, color);

	return 0;
}

// Set the display up
void init_display()
{
	pixel_buffer =
			alt_up_pixel_buffer_dma_open_dev("/dev/dma_buffer");

	char_buffer = alt_up_char_buffer_open_dev("/dev/char_drawer");

	// Initialize the character buffer.
	alt_up_char_buffer_init(char_buffer);

	pixel_buffer_addr1 = PIXEL_BUFFER_BASE;
	pixel_buffer_addr2 = PIXEL_BUFFER_BASE + (320 * 240 * 2);

	// Set the background buffer address.
	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,
			pixel_buffer_addr1);

	// Swap background and foreground buffers
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);

	// Wait for the swap to complete
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

	// Set the 2nd buffer address
	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,
			pixel_buffer_addr2);

	// Clear both buffers (this makes all pixels black)
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 1);
}

//Switch between back and fore buffers
void swap_buffers()
{
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
}

// Clear everything
void clear_display()
{
	clear_pixel_buffer();
	clear_character_buffer();
}

// Clear the text
void clear_character_buffer()
{
	if (char_buffer == NULL) return;

	alt_up_char_buffer_clear(char_buffer);
}

// Clear pixels
void clear_pixel_buffer()
{
	if (pixel_buffer == NULL) return;

	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);

	// Swap background and foreground buffers
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);

	// Wait for the swap to complete
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

	// Clear the screen
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
}

// Make a line at the specified coordinates
void draw_line(int x0, int y0, int x1, int y1, colour col, int backbuffer)
{
	if (pixel_buffer == NULL) return;
	int intCol = colourToInt(col);
	//printf("%x ", intCol);
	alt_up_pixel_buffer_dma_draw_line(pixel_buffer, x0, y0, x1, y1,
			intCol, backbuffer);
}

// Draw a bow at the specified location
void draw_box(int x0, int y0, int x1, int y1, colour col, int backbuffer)
{
	if (pixel_buffer == NULL) return;
	int intCol = colourToInt(col);

    alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x0, y0, x1, y1,
			intCol, backbuffer);
}

// Mangle together a colour
colour makeCol(unsigned char r, unsigned char g, unsigned char b)
{
	colour col = {r, g, b};
	return col;
}

// Write some text
void draw_string(const char *str, unsigned int x, unsigned int y)
{
	alt_up_char_buffer_string(char_buffer, str, x, y);
}

// Draw a single coloured pixel
void draw_pixel(int x, int y, colour col)
{
	if (pixel_buffer == NULL) return;

	int intCol = colourToInt(col);
	//alt_up_pixel_buffer_dma_draw(pixel_buffer, intCol, x, y);
	draw_pixel_fast(pixel_buffer, intCol, x, y);
}

// Make the colour a single int
int colourToInt(colour col)
{
	int col_val = 0;
	int r = (int) col.r;
	int g = (int) col.g;
	int b = (int) col.b;

	col_val = (r << 11) | ((g & 0x003F) << 5) | (b & 0x001F);
	return col_val;
}

// Determine colour from an int
colour intToColour(int col_val)
{
	colour col;
	col.r = (col_val & 0xF800) >> 11;
	col.g = (col_val & 0x07E0) >> 5;
	col.b = (col_val & 0x001F);
}
