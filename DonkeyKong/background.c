/*
 * background.c
 *
 *  Created on: 2013-09-22
 *      Author: Jeremy
 */

#include "background.h"
#include "bitmap.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

static colour background_map[SCREEN_WIDTH][SCREEN_HEIGHT];

short int loadBackground(char *filename)
{
	BitmapHandle *bmp;

	short int ret = load_bmp(filename, &bmp);

	if (ret < 0) return ret;

	if (bmp->bmp_info_header->width > SCREEN_WIDTH ||
			bmp->bmp_info_header->height > SCREEN_HEIGHT) return -4;

	int col, row;

	for (row = 0; row < bmp->bmp_info_header->height; row++)
	{
		for (col = 0; col < bmp->bmp_info_header->width; col++)
		{
			int ind = row * bmp->bmp_info_header->width + col;
			background_map[col][SCREEN_HEIGHT-row-1] = bmp->pixel_map[ind];
		}
	}

	close_bmp(bmp);

	return 0;
}

void drawBackground()
{
	drawBackgroundSection(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
}

void drawBackgroundSection(int x0, int y0, int x1, int y1)
{
	int x, y;

	for (y = y0; y <= y1; y++)
		if (x0 < x1){
			for (x = x0; x <= x1; x++)
				draw_pixel(x, y, background_map[x][y]);
		}
		else {
			for (x = x1; x <= x0; x++)
				draw_pixel(x, y, background_map[x][y]);
		}
}
