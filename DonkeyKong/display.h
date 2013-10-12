/*
 * display.h
 *
 *  Created on: 2013-09-18
 *      Author: Jeremy
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"

typedef struct {
	unsigned char r, g, b;
} colour;

void init_display();
void swap_buffers();
void clear_pixel_buffer();
void clear_character_buffer();
void clear_display();
int colourToInt(colour col);
colour intToColour(int col_val);
void draw_string(const char *str, unsigned int x, unsigned int y);
void draw_line(int x0, int y0, int x1, int y1, colour col, int backbuffer);
void draw_box(int x0, int y0, int x1, int y1, colour col, int backbuffer);
colour makeCol(unsigned char r, unsigned char g, unsigned char b);
int draw_pixel_fast(alt_up_pixel_buffer_dma_dev *pixel_buffer, unsigned int color, unsigned int x, unsigned int y);

#endif /* DISPLAY_H_ */
