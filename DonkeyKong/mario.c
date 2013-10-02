/*
 * mario.c
 *
 *  Created on: 2013-10-01
 *      Author: Jeremy
 */
#include "mario.h"

static Mario mario;
static char* anim_list[NUM_IMGS] = {"M9.BMP", "M10.BMP", "M11.BMP"};
static colour mario_alpha = { 0x1F, 0x00, 0x1F };

void loadMario(int x, int y)
{
	/* Prepare the animation map. */
	load_bmp(anim_list[STAND_IMG], &(mario.animation[STAND_LEFT].handle));
	load_bmp(anim_list[WALK1_IMG], &(mario.animation[WALK1_LEFT].handle));
	load_bmp(anim_list[WALK2_IMG], &(mario.animation[WALK2_LEFT].handle));

	/* The right-facing animations use the same images. */
	mario.animation[STAND_RIGHT].handle = mario.animation[STAND_LEFT].handle;
	mario.animation[WALK1_RIGHT].handle = mario.animation[WALK1_LEFT].handle;
	mario.animation[WALK2_RIGHT].handle = mario.animation[WALK2_LEFT].handle;

	mario.animation[STAND_LEFT].flip = false;
	mario.animation[WALK1_LEFT].flip = false;
	mario.animation[WALK2_LEFT].flip = false;
	mario.animation[STAND_RIGHT].flip = true;
	mario.animation[WALK1_RIGHT].flip = true;
	mario.animation[WALK2_RIGHT].flip = true;

	mario.current_frame = STAND_RIGHT;
	mario.state = NORMAL;
	mario.x = x;
	mario.y = y;
}

void drawMario(void)
{
	draw_bmp(mario.animation[mario.current_frame].handle,
			mario.x, mario.y, true, mario_alpha);
}
