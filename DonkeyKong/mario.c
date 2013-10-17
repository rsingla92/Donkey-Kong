/*
 * mario.c
 *
 *  Created on: 2013-10-01
 *      Author: Jeremy
 */
#include "mario.h"
#include "audio.h"
#include "level1.h"
#include "background.h"
#include <math.h>

#define FRAME_SPEED		0.05

static Mario mario;
static char* anim_list[NUM_IMGS] = {"M9.BMP", "M10.BMP", "M11.BMP", "M12.BMP", "D1.BMP", "D2.BMP", "D3.BMP",
		"D4.BMP", "M2.BMP", "M3.BMP","M4.BMP","M5.BMP","M6.BMP","M7.BMP"};
static colour mario_alpha = { 0x00, 0x00, 0x00 };
static double frame_dir = FRAME_SPEED;
static float past_frame = STAND_LEFT;
extern Point hammer1_location;
//Point hammer1_location = {300,190};
// Global Jump Buffer:
unsigned int* jumpSoundBuf = 0;
int jumpSoundBufLen = 0;

Mario getMario(void) {
	return mario;
}

void loadMario(int x, int y, int speed)
{
	/* Prepare the animation map. */
	load_bmp(anim_list[STAND_IMG], &(mario.animation[STAND_LEFT].handle));
	load_bmp(anim_list[WALK1_IMG], &(mario.animation[WALK1_LEFT].handle));
	load_bmp(anim_list[WALK2_IMG], &(mario.animation[WALK2_LEFT].handle));
	load_bmp(anim_list[CLIMB_IMG], &(mario.animation[CLIMB1].handle));
	load_bmp(anim_list[DEAD1_IMG], &(mario.animation[DEAD1].handle));
	load_bmp(anim_list[DEAD2_IMG], &(mario.animation[DEAD2].handle));
	load_bmp(anim_list[DEAD3_IMG], &(mario.animation[DEAD3].handle));
	load_bmp(anim_list[DEAD4_IMG], &(mario.animation[DEAD4].handle));
	load_bmp(anim_list[HMR1_IMG], &(mario.animation[HMR1_LEFT].handle));
	load_bmp(anim_list[HMR2_IMG], &(mario.animation[HMR2_LEFT].handle));
	load_bmp(anim_list[HMR3_IMG], &(mario.animation[HMR3_LEFT].handle));
	load_bmp(anim_list[HMR4_IMG], &(mario.animation[HMR4_LEFT].handle));
	load_bmp(anim_list[HMR5_IMG], &(mario.animation[HMR5_LEFT].handle));
	load_bmp(anim_list[HMR6_IMG], &(mario.animation[HMR6_LEFT].handle));

	/* The right-facing animations use the same images. */
	mario.animation[STAND_RIGHT].handle = mario.animation[STAND_LEFT].handle;
	mario.animation[WALK1_RIGHT].handle = mario.animation[WALK1_LEFT].handle;
	mario.animation[WALK2_RIGHT].handle = mario.animation[WALK2_LEFT].handle;
	mario.animation[CLIMB2].handle = mario.animation[CLIMB1].handle;
	mario.animation[HMR1_RIGHT].handle = mario.animation[HMR1_LEFT].handle;
	mario.animation[HMR2_RIGHT].handle = mario.animation[HMR2_LEFT].handle;
	mario.animation[HMR3_RIGHT].handle = mario.animation[HMR3_LEFT].handle;
	mario.animation[HMR4_RIGHT].handle = mario.animation[HMR4_LEFT].handle;
	mario.animation[HMR5_RIGHT].handle = mario.animation[HMR5_LEFT].handle;
	mario.animation[HMR6_RIGHT].handle = mario.animation[HMR6_LEFT].handle;

	mario.animation[STAND_LEFT].flip = false;
	mario.animation[WALK1_LEFT].flip = false;
	mario.animation[WALK2_LEFT].flip = false;
	mario.animation[HMR1_LEFT].flip = false;
	mario.animation[HMR2_LEFT].flip = false;
	mario.animation[HMR3_LEFT].flip = false;
	mario.animation[HMR4_LEFT].flip = false;
	mario.animation[HMR5_LEFT].flip = false;
	mario.animation[HMR6_LEFT].flip = false;
	mario.animation[STAND_RIGHT].flip = true;
	mario.animation[WALK1_RIGHT].flip = true;
	mario.animation[WALK2_RIGHT].flip = true;
	mario.animation[CLIMB2].flip = true;
	mario.animation[HMR1_RIGHT].flip = true;
	mario.animation[HMR2_RIGHT].flip = true;
	mario.animation[HMR3_RIGHT].flip = true;
	mario.animation[HMR4_RIGHT].flip = true;
	mario.animation[HMR5_RIGHT].flip = true;
	mario.animation[HMR6_RIGHT].flip = true;

	mario.current_frame = STAND_RIGHT;
	mario.state = WALKING;
	mario.x = x;
	mario.y = y;
	mario.jumpStart = y;
	mario.speed = speed;
	mario.lives = 3;
	mario.currentFloor = 6;
}

void setMarioX(int x)
{
	mario.x = x;
}
void setMarioY(int y)
{
	mario.y = y;
}

void setMarioCurrentFrame(float current_frame)
{
	mario.current_frame = current_frame;
}

void drawMario(bool bothBuffers)
{
	int cur_frame = (int) round(mario.current_frame);
	static int deadCount = 0;
	static int hammerCount = 0;

	if (mario.state == DEAD)
	{
		deadCount++;
		if (deadCount > 300)
		{
			mario.state = WALKING;
			drawMarioBackground(mario.x, mario.y,
					mario.x + getCurrentWidth(), mario.y + getCurrentHeight());
			mario.x = MARIO_START_X;
			mario.y = MARIO_START_Y;
			mario.current_frame = STAND_RIGHT;
			mario.jumpStart = MARIO_START_Y;
			mario.currentFloor = 6;
			deadCount = 0;
			return;
		}

		animate(DEAD1, DEAD4);
	}

	if(mario.state == HAMMERING)
	{
		hammerCount++;
		if (hammerCount > 500)
		{
			mario.state = WALKING;
			drawMarioBackground(mario.x, mario.y,
								mario.x + getCurrentWidth(), mario.y + getCurrentHeight());
			mario.current_frame = STAND_RIGHT;
			hammerCount = 0;
			return;
		}
		if(mario.current_frame >= HMR1_RIGHT && mario.current_frame <= HMR6_RIGHT)
			animate(HMR1_RIGHT, HMR6_RIGHT);
		else
			animate(HMR1_LEFT, HMR6_LEFT);
	}

	if ((cur_frame >= STAND_RIGHT && cur_frame <= WALK2_RIGHT) || cur_frame == CLIMB2 ||
			(cur_frame >= HMR1_RIGHT && cur_frame <= HMR6_RIGHT))
	{
		draw_flipped_bmp(mario.animation[cur_frame].handle,
				mario.x, mario.y, true, mario_alpha, 1);
		if (bothBuffers == true)
		{
			swap_buffers();
			draw_flipped_bmp(mario.animation[cur_frame].handle,
					mario.x, mario.y, true, mario_alpha, 1);
		}
	}
	else
	{
		draw_bmp(mario.animation[cur_frame].handle,
				mario.x, mario.y, true, mario_alpha, 1);
		if (bothBuffers == true)
		{
			swap_buffers();
			draw_bmp(mario.animation[cur_frame].handle,
					mario.x, mario.y, true, mario_alpha, 1);
		}
	}
}

bool moveMario(MarioDirection dir)
{
	bool ret = true;
	switch (dir)
	{
	case UP:
		ret = moveUp();
		break;
	case DOWN:
		ret = moveDown();
		break;
	case LEFT:
		ret = moveLeft();
		break;
	case RIGHT:
		ret = moveRight();
		break;
	}

	return ret;
}

void animate(MarioAnims lowFrame, MarioAnims highFrame)
{
	past_frame = mario.current_frame;

	if (mario.current_frame < lowFrame || mario.current_frame > highFrame) {
		mario.current_frame = lowFrame;
	} else {
		mario.current_frame += frame_dir;
	}

	if (mario.current_frame > highFrame || mario.current_frame < lowFrame) {
		if (mario.state != DEAD)
		{
			frame_dir = -frame_dir;
		}
		else
		{
			if (frame_dir > 0) mario.current_frame = lowFrame;
			else mario.current_frame = highFrame;
		}
	}

	int past_width = getPastWidth();
	int cur_width = getCurrentWidth();
	if (past_width > cur_width)
	{
		// Went from a larger frame to smaller-- erase background it might have left.
		int cur_height = getCurrentHeight();
		pushEraseNode(mario.x + cur_width, mario.y,
				mario.x + past_width, mario.y + cur_height);
	}
}

void move(int x, int y, MarioAnims lowFrame, MarioAnims highFrame, bool flip) {
	//double tmp_frame = mario.current_frame;
	mario.x += x;
	mario.y += y;

	if (mario.state == WALKING || mario.state == LADDER_TOP ||
			mario.state == LADDER_BOTTOM || mario.state == HAMMERING) {
		animate(lowFrame, highFrame);
	}
}

void drawMarioBackground(int x0, int y0, int x1, int y1) {
	pushEraseNode(x0, y0, x1, y1);
}

int getCurrentWidth(void)
{
	return mario.animation[(int) round(mario.current_frame)].handle->bmp_info_header->width;
}

int getCurrentHeight(void)
{
	return mario.animation[(int) round(mario.current_frame)].handle->bmp_info_header->height;
}

int getPastWidth(void)
{
		return mario.animation[(int) round(past_frame)].handle->bmp_info_header->width;
}

int getPastHeight(void)
{
	return mario.animation[(int) round(past_frame)].handle->bmp_info_header->height;
}

void setMarioJumpStart(int height)
{
	mario.jumpStart = height;
}

int getMarioJumpStart(void)
{
	return mario.jumpStart;
}

bool moveLeft(void)
{
	if (mario.x - mario.speed > 0) {
		if (mario.state == HAMMERING){
			move(-mario.speed, 0, HMR1_LEFT, HMR6_LEFT, false);
		}
		else{
			move(-mario.speed, 0, STAND_LEFT, WALK2_LEFT, false);
		}
		drawMarioBackground(mario.x + getCurrentWidth(), mario.y,
				mario.x + getPastWidth() + mario.speed, mario.y + getCurrentHeight());

		return true;
	}

	return false;
}

bool moveRight(void)
{
	if (mario.x + getCurrentWidth() + mario.speed < 320) {
		if (mario.state == HAMMERING){
			move(mario.speed, 0, HMR1_RIGHT, HMR6_RIGHT, true);
		}
		else {
			move(mario.speed, 0, STAND_RIGHT, WALK2_RIGHT, true);
		}
		drawMarioBackground(mario.x-mario.speed, mario.y,
				mario.x, mario.y + getCurrentHeight());
		return true;
	}

	return false;
}

bool moveDown(void)
{
	mario.y += mario.speed;
	drawMarioBackground(mario.x, mario.y - mario.speed,
			mario.x + getCurrentWidth(), mario.y);

	if (mario.state == M_CLIMBING) {
		animate(CLIMB1, CLIMB2);
	}

	return true;
}

bool moveUp(void)
{
	mario.y -= mario.speed;
	drawMarioBackground(mario.x, mario.y + getCurrentHeight(),
			mario.x + getCurrentWidth(), mario.y + getCurrentHeight() + mario.speed);

	if (mario.state == M_CLIMBING) {
		animate(CLIMB1, CLIMB2);
	}

	return true;
}

void changeMarioState(MarioState state)
{
	mario.state = state;

	float frame = round(mario.current_frame);

	if (mario.state == JUMPING) {
		addInSound(jumpSoundBuf, jumpSoundBufLen);
	}

	if (frame >= STAND_RIGHT && frame <= WALK2_RIGHT)
	{
		if (mario.state == JUMPING || mario.state == FALLING) {
			mario.current_frame = WALK1_RIGHT;
		}
	}
	else if(frame >= STAND_LEFT && frame <= WALK2_LEFT)
	{
		if (mario.state == JUMPING || mario.state == FALLING) {
			mario.current_frame = WALK1_LEFT;
		}
	}
	if (mario.state == HAMMERING){
		mario.current_frame = HMR1_RIGHT;
	}
	if (mario.state == M_CLIMBING) {
		mario.current_frame = CLIMB1;
	}

	if (mario.state == DEAD)
	{
		mario.lives--;
		mario.current_frame = DEAD2;
		eraseLives();
	}

}

MarioState getMarioState(void)
{
	return mario.state;
}

Mario* getMarioRef(void)
{
	return &mario;
}

int getMarioCurrentFloor(void)
{
	return mario.currentFloor;
}

void setMarioCurrentFloor(int currentFloor)
{
	if (currentFloor < 0 || currentFloor > 6) return;

	mario.currentFloor = currentFloor;
}

void setMarioLives(int lives)
{
	mario.lives = lives;
}

void drawLives(void)
{
	BitmapHandle* lifeHandle;

	load_bmp("LIFE.BMP", &lifeHandle);
	draw_bmp(lifeHandle, 0, 20, 0, mario_alpha, 1);
	draw_bmp(lifeHandle, 23, 20, 0, mario_alpha, 1);
	draw_bmp(lifeHandle, 46, 20, 0, mario_alpha, 1);
	close_bmp(lifeHandle);
}

void eraseLives(void)
{
	if (mario.lives == 2)
	{
		// Erase first one
		draw_box(46, 20, 68, 42, mario_alpha, 1);
		draw_box(46, 20, 68, 42, mario_alpha, 0);
	}
	else if(mario.lives == 1)
	{
		// Erase second one
		draw_box(23, 20, 45, 42, mario_alpha, 1);
		draw_box(23, 20, 45, 42, mario_alpha, 0);
	}
	else if(mario.lives == 0)
	{
		// Erase last one
		draw_box(0, 20, 22, 42, mario_alpha, 1);
		draw_box(0, 20, 22, 42, mario_alpha, 0);
	}
}
