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

static EraseStack* head = 0;

void pushEraseNode(int x0, int y0, int x1, int y1)
{
	EraseRect newRect;
	EraseStack* newNode = head;
	newRect.x0 = x0;
	newRect.x1 = x1;
	newRect.y0 = y0;
	newRect.y1 = y1;

	newNode = (EraseStack*) malloc(sizeof(EraseStack));
	newNode->next = head;
	newNode->rect = newRect;
	head = newNode;
}

EraseRect popEraseNode()
{
	EraseRect retRect = {-1, -1, -1, -1};
	if (head == 0) return retRect;

	EraseStack* tmp = head;
	head = head->next;
	retRect.x0 = tmp->rect.x0;
	retRect.y0 = tmp->rect.y0;
	retRect.x1 = tmp->rect.x1;
	retRect.y1 = tmp->rect.y1;

	free(tmp);
	return retRect;
}

void eraseAllNoPop()
{
	EraseStack* it = head;

	while (it != NULL)
	{
		EraseRect rect = it->rect;
		if (rect.x0 < 0) break;

		drawBackgroundSection(rect.x0, rect.y0, rect.x1, rect.y1);
		it = it->next;
	}
}

void eraseAll()
{
	while (1)
	{
		EraseRect rect = popEraseNode();
		if (rect.x0 < 0) break;

		drawBackgroundSection(rect.x0, rect.y0, rect.x1, rect.y1);
	}
}

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

	for (y = y0; y <= y1; y++) {
		if (x0 <= x1){
			for (x = x0; x <= x1; x++)
				draw_pixel(x, y, background_map[x][y]);
		}
		else {
			for (x = x1; x <= x0; x++)
				draw_pixel(x, y, background_map[x][y]);
		}
	}
}
