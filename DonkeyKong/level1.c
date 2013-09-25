/*
 * level1.c
 *
 *  Created on: Sep 24, 2013
 *      Author: lauren
 */

#include "level1.h"
#include "background.h"
#include "bitmap.h"

void draw_level1() {
	printf("loading level1...\n");
	short int ret = loadBackground("LVL1.BMP");
	if (ret < 0) {
			printf("Could not load level1. Ret: %d\n", ret);
		}

	// Draw the background to both buffers.
	drawBackground();
	swap_buffers();
	drawBackground();

}
