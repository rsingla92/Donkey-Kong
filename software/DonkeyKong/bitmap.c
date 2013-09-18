/*
 * bitmap.c
 *
 *  Created on: 2013-09-18
 *      Author: Jeremy
 */
#include "bitmap.h"
#include <stdlib.h>
#include <stdio.h>

static BmpFileHeader *bmp_file_header = NULL;
static BmpInfoHeader *bmp_info_header = NULL;
static colour *pixel_map = NULL;

static void fill_bmp_header(file_handle file);
static void fill_info_header(file_handle file);
static short int verify_bmp_type();

void draw_bmp(int x, int y, bool alpha_enable, colour alpha_col)
{
	if (pixel_map == NULL || bmp_info_header == NULL) return;

	int col, row;

	for (row = 0; row < bmp_info_header->height; row++)
	{
		for (col = 0; col < bmp_info_header->width; col++)
		{
			int ind = row * bmp_info_header->width + col;

			/* The BMP file format stores the image upside down and backwards--
			 * must correct for that here by drawing from bottom-left to
			 * top-right. */
			int pixel_x = x + (bmp_info_header->width - 1 - col);
			int pixel_y = y + (bmp_info_header->height - 1 - row);

			if (alpha_enable == false || !(pixel_map[ind].r == alpha_col.r &&
					pixel_map[ind].g == alpha_col.g && pixel_map[ind].b == alpha_col.b))
			{
				draw_line(pixel_x, pixel_y,
						pixel_x, pixel_y, pixel_map[ind], 0);
			}
		}
	}
}

short int load_bmp(file_handle file)
{
	clear_data();
	fill_bmp_header(file);
	fill_info_header(file);

	if (bmp_file_header == NULL || bmp_info_header == NULL)
	{
		printf("Error loading bmp headers.\n");
		return -1;
	}

	if (!verify_bmp_type())
	{
		printf("Not a proper 24-bit bitmap.\n");
		return -2;
	}

	dump_header_info();

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
	printf("\nPadding: %d\n", padding);

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

	printf("First pixel: R: %x, G: %x, B: %x\n", pixel_map[0].r,
			pixel_map[0].g, pixel_map[0].b);
	printf("Second pixel: R: %x, G: %x, B: %x\n", pixel_map[1].r,
			pixel_map[1].g, pixel_map[1].b);

	return 0;
}

void clear_data()
{
	if (bmp_file_header != NULL) free(bmp_file_header);
	if (bmp_info_header != NULL) free(bmp_info_header);
	if (pixel_map != NULL) free(pixel_map);
}

void dump_header_info()
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

static void fill_bmp_header(file_handle file)
{
	byte *byte_list = (byte *) malloc(sizeof(BmpFileHeader));
	byte *it = byte_list;
	int i;

	// Fill the header with bytes.
	for (i = 0; i < sizeof(BmpFileHeader); i++)
	{
		*it = read_file(file);
		it++;
	}

	bmp_file_header = (BmpFileHeader*) byte_list;
}

static void fill_info_header(file_handle file)
{
	byte *byte_list = (byte *) malloc(sizeof(BmpInfoHeader));
	byte *it = byte_list;
	int i;

	// Fill the header with bytes.
	for (i = 0; i < sizeof(BmpInfoHeader); i++)
	{
		*it = read_file(file);
		it++;
	}

	bmp_info_header = (BmpInfoHeader*) byte_list;
}

static short int verify_bmp_type()
{
	if((bmp_file_header->header_field & 0x00FF) != BITMAP_TYPE_BYTE1 ||
		((bmp_file_header->header_field & 0xFF00) >> 8)  != BITMAP_TYPE_BYTE2)
	{
		printf("Wrong file type for bitmap.\n");
		return 0;
	}

	return 1;
}
