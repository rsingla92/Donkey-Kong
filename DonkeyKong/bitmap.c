/*
 * bitmap.c
 *
 *  Created on: 2013-09-18
 *      Author: Jeremy
 */
#include "bitmap.h"
#include <stdlib.h>
#include <stdio.h>

static BmpFileHeader* fill_bmp_header(file_handle file);
static BmpInfoHeader* fill_info_header(file_handle file);
static short int verify_bmp_type(BmpFileHeader* bmp_file_header);

void draw_bmp(BitmapHandle* handle, int x, int y, bool alpha_enable, colour alpha_col, int backbuffer)
{
	if (handle->pixel_map == NULL || handle->bmp_info_header == NULL) return;

	int col, row;
	BmpInfoHeader *bmp_info_header = handle->bmp_info_header;
	colour *pixel_map = handle->pixel_map;

	for (row = 0; row < bmp_info_header->height; row++)
	{
		for (col = 0; col < bmp_info_header->width; col++)
		{
			int ind = row * bmp_info_header->width + col;

			/* The BMP file format stores the image upside down and backwards--
			 * must correct for that here by drawing from bottom-left to
			 * top-right. */
			int pixel_x = x + col;//(bmp_info_header->width - 1 - col);
			int pixel_y = y + (bmp_info_header->height - 1 - row);

			if (pixel_x >= 0 && pixel_x <= 320 && pixel_y >=0 && pixel_y <= 240)
			{
				if (alpha_enable == false || !(pixel_map[ind].r == alpha_col.r &&
						pixel_map[ind].g == alpha_col.g && pixel_map[ind].b == alpha_col.b))
				{
					//draw_pixel(pixel_x, pixel_y, pixel_map[ind]);
					draw_line(pixel_x, pixel_y, pixel_x, pixel_y, pixel_map[ind], backbuffer );
				} else if (alpha_enable == true && pixel_map[ind].r == alpha_col.r &&
						pixel_map[ind].g == alpha_col.g && pixel_map[ind].b == alpha_col.b) {
					drawBackgroundSection(pixel_x, pixel_y, pixel_x, pixel_y);
				}
			}
		}
	}
}

void draw_flipped_bmp(BitmapHandle* handle, int x, int y, bool alpha_enable, colour alpha_col, int backbuffer)
{
	if (handle->pixel_map == NULL || handle->bmp_info_header == NULL) return;

	int col, row;
	BmpInfoHeader *bmp_info_header = handle->bmp_info_header;
	colour *pixel_map = handle->pixel_map;

	for (row = 0; row < bmp_info_header->height; row++)
	{
		for (col = 0; col < bmp_info_header->width; col++)
		{
			int ind = row * bmp_info_header->width + col;

			int pixel_x = x + (bmp_info_header->width - 1 - col);//(bmp_info_header->width - 1 - col);
			int pixel_y = y + (bmp_info_header->height - 1 - row);

			if (pixel_x >= 0 && pixel_x <= 320 && pixel_y >=0 && pixel_y <= 240)
			{
				if (alpha_enable == false || !(pixel_map[ind].r == alpha_col.r &&
						pixel_map[ind].g == alpha_col.g && pixel_map[ind].b == alpha_col.b))
				{
					//draw_pixel(pixel_x, pixel_y, pixel_map[ind]);
					draw_line(pixel_x, pixel_y, pixel_x, pixel_y, pixel_map[ind], backbuffer );
				} else if (alpha_enable == true && pixel_map[ind].r == alpha_col.r &&
						pixel_map[ind].g == alpha_col.g && pixel_map[ind].b == alpha_col.b) {
					drawBackgroundSection(pixel_x, pixel_y, pixel_x, pixel_y);
				}
			}
		}
	}
}

short int load_bmp(char *filename, BitmapHandle** bmp_handle)
{
	BitmapHandle* handle = (BitmapHandle *) malloc(sizeof(BitmapHandle));
	BmpInfoHeader *bmp_info_header = NULL;
	BmpFileHeader *bmp_file_header = NULL;
	colour *pixel_map = NULL;
	file_handle file = 	open_file(filename, false);

	if (file < 0)
	{
		return file;
	}

	bmp_file_header = fill_bmp_header(file);
	bmp_info_header = fill_info_header(file);

	if (bmp_file_header == NULL || bmp_info_header == NULL)
	{
		return -1;
	}

	if (!verify_bmp_type(bmp_file_header))
	{
		return -2;
	}

	/* Read pixel data. */
	int bmp_alloc_size = bmp_info_header->width * bmp_info_header->height * 3;
	pixel_map = (colour *) malloc(bmp_alloc_size);

	colour *it;
	int i, j;

	/* Take into account padding */
	int byte_width = bmp_info_header->width*3;
	int padding = 0;

	if (byte_width % 4)
	{
		/* Image width is not divisible by 4-- there will be padding. */
		byte_width += 4 - (byte_width % 4);
	}

	padding = byte_width - (bmp_info_header->width*3);

	for (it = pixel_map, i = 0;
			i < bmp_info_header->width*bmp_info_header->height; i++, it++)
	{
		/* Must account for the fact that the display takes 5 bits for r,
		 * 6 bits for g and 5 bits for b.
		 */
		it->b = (read_file(file) >> 3);
		it->g = (read_file(file) >> 2);
		it->r = (read_file(file) >> 3);

		/* Discard padding at the end of the row. */
		if ((i+1) % bmp_info_header->width == 0)
			for (j = 0; j < padding; j++) read_file(file);
	}

	alt_up_sd_card_fclose(file);

	handle->bmp_file_header = bmp_file_header;
	handle->bmp_info_header = bmp_info_header;
	handle->pixel_map = pixel_map;

	(*bmp_handle) = handle;
	return 0;
}

void close_bmp(BitmapHandle* handle)
{
	if (handle == NULL) return;
	if (handle->bmp_file_header != NULL) free(handle->bmp_file_header);
	if (handle->bmp_info_header != NULL) free(handle->bmp_info_header);
	if (handle->pixel_map != NULL) free(handle->pixel_map);
	free(handle);
}

void dump_header_info(BmpFileHeader* bmp_file_header, BmpInfoHeader* bmp_info_header)
{
	if (bmp_file_header == NULL) return;

	printf("File Type: %x\nFile Size: %d\nOffset to image information: %d\n", bmp_file_header->header_field,
			bmp_file_header->size, bmp_file_header->offset);

	if(bmp_info_header == NULL) return;

	printf("Information Header Size: %d\nImage Width: %d\nImage Height: %d\n", bmp_info_header->size,
			bmp_info_header->width, bmp_info_header->height);
	printf("Number of Bits Per Pixel: %d\nImage Size: %d\nCompression: %d\n", bmp_info_header->bit_count,
			bmp_info_header->img_size, bmp_info_header->compression);
	printf("Horizontal Resolution: %d\nVertical Resolution: %d\n", bmp_info_header->x_ppm, bmp_info_header->y_ppm);
	printf("Number of important colours used: %d\nNumber of colours in palette: %d", bmp_info_header->important_colours,
			bmp_info_header->colours_used);
}

static BmpFileHeader* fill_bmp_header(file_handle file)
{
	byte *byte_list = (byte *) malloc(sizeof(BmpFileHeader));
	byte *it = byte_list;
	BmpFileHeader* bmp_file_header;
	int i;

	// Fill the header with bytes.
	for (i = 0; i < sizeof(BmpFileHeader); i++)
	{
		*it = read_file(file);
		it++;
	}

	bmp_file_header = (BmpFileHeader*) byte_list;
	return bmp_file_header;
}

static BmpInfoHeader* fill_info_header(file_handle file)
{
	byte *byte_list = (byte *) malloc(sizeof(BmpInfoHeader));
	byte *it = byte_list;
	BmpInfoHeader* bmp_info_header;
	int i;

	// Fill the header with bytes.
	for (i = 0; i < sizeof(BmpInfoHeader); i++)
	{
		*it = read_file(file);
		it++;
	}

	bmp_info_header = (BmpInfoHeader*) byte_list;
	return bmp_info_header;
}

static short int verify_bmp_type(BmpFileHeader* bmp_file_header)
{
	if((bmp_file_header->header_field & 0x00FF) != BITMAP_TYPE_BYTE1 ||
		((bmp_file_header->header_field & 0xFF00) >> 8)  != BITMAP_TYPE_BYTE2)
	{
		return 0;
	}

	return 1;
}
