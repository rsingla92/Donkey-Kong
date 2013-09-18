/*
 * bitmap.h
 *
 *  Created on: 2013-09-18
 *      Author: Jeremy
 */

#ifndef BITMAP_H_
#define BITMAP_H_

#include "sdcard.h"
#include "display.h"

// Header field type for a bitmap
#define BITMAP_TYPE_BYTE1 0x42
#define BITMAP_TYPE_BYTE2 0x4D

#pragma pack(2)

typedef struct
{
    unsigned short header_field;
    unsigned int   size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int   offset;
} BmpFileHeader;

#pragma pack()

typedef struct
{
    unsigned int   size;           /* Header size */
    int            width;          /* Bitmap Width */
    int            height;         /* Bitmap Height */
    unsigned short planes;         /* Colour plane numbers */
    unsigned short bit_count;       /* # bits per pixel */
    unsigned int   compression;    /* Compression type */
    unsigned int   img_size;      /* Image data size */
    int            x_ppm;  /* X pixels/meter */
    int            y_ppm;  /* Y pixels/meter */
    unsigned int   colours_used;        /* Number of colours */
    unsigned int   important_colours;   /* Number of important colours */
} BmpInfoHeader;

short int load_bmp(file_handle file);
void draw_bmp(int x, int y, bool alpha_enable, colour alpha_col);
void clear_data();
void dump_header_info();

#endif /* BITMAP_H_ */
