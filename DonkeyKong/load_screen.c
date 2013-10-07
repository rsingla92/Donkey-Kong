/*
 * load_screen.c
 *
 *  Created on: 2013-10-06
 *      Author: Jeremy
 */

#include "load_screen.h"
#include "bitmap.h"
#include "mario.h"
#include "state_machine.h"

static BitmapHandle* background_bmp;
static colour back_alpha = { 0x00, 0x00, 0x00 };

void updateLoadScreen(void)
{
	short int ret = load_bmp("LOAD.BMP", &background_bmp);
	if (ret < 0)
	{
			printf("Could not load the load screen. Ret: %d\n", ret);
	}

	draw_load_screen();

	loadMario(0, 217, 1);
	loadPeach(137, 43);
	loadDonkeyKong(85, 71);
	loadBarrels();
	loadBarrel(20, 70);
	loadBarrel(50, 70);

	draw_level1();
	drawMario(false);
	drawPeach();
	drawDonkeyKong();
	drawBarrels();

	close_bmp(background_bmp);

	swap_buffers();
	draw_level1();
	drawPeach();
	drawDonkeyKong();
	drawBarrels();

	changeState(LEVEL1);
}

void draw_load_screen(void)
{
	// Draw the background to both buffers.
	draw_bmp(background_bmp, 0, 240 - background_bmp->bmp_info_header->height,
			false, back_alpha, 1);
	swap_buffers();
	draw_bmp(background_bmp, 0, 240 - background_bmp->bmp_info_header->height,
			false, back_alpha, 1);
}
