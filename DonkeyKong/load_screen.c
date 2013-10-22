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
#include "movingObject.h"

static BitmapHandle* background_bmp;
static colour back_alpha = { 0x00, 0x00, 0x00 };
static unsigned char alreadyLoaded = 0;

static void draw_load_screen(void);

// From Mario:
extern int* jumpSoundBuf;
extern int jumpSoundBufLen;
extern int* deadSoundBuf;
extern int deadSoundBufLen;
extern int* winSoundBuf;
extern int winSoundBufLen;
extern unsigned int* lvlSoundBuf;
extern int lvlSoundBufLen;
extern int* hammerSoundBuf;
extern int hammerSoundBufLen;

static int* gameStartBuf;
static int gameStartBufLen;

// Tran
void updateLoadScreen(void)
{
	short int ret = load_bmp("LOAD.BMP", &background_bmp);
	if (ret < 0)
	{
			printf("Could not load the load screen. Ret: %d\n", ret);
	}

	draw_load_screen();

	if (alreadyLoaded)
	{
		swapInSound(lvlSoundBuf, lvlSoundBufLen, 1);
		swapInSound(gameStartBuf, gameStartBufLen, 0);
	}
	else
	{
		gameStartBufLen = loadSound("gs1.wav", &gameStartBuf, 0.1);
		lvlSoundBufLen = loadSound("thm.wav", &lvlSoundBuf, 0.5);
		swapInSound(lvlSoundBuf, lvlSoundBufLen, 1);
		swapInSound(gameStartBuf, gameStartBufLen, 0);
		jumpSoundBufLen = loadSound("boin.wav", &jumpSoundBuf, 0.5);
		deadSoundBufLen = loadSound("dead.wav", &deadSoundBuf, 0.5);
		winSoundBufLen = loadSound("end.wav", &winSoundBuf, 0.5);
		hammerSoundBufLen = loadSound("ht.wav", &hammerSoundBuf, 0.5);
		loadMario(MARIO_START_X, MARIO_START_Y, 1);
		loadPeach(137, 43);
		loadDonkeyKong(76, 71);
		loadBarrels();
		loadBarrel(20, 70);
		loadBarrel(50, 70);
		loadBarrel(210, 30);
		loadBarrel(240, 30);
		loadBarrel(240, 30);
		loadBarrel(240, 30);
		//loadBarrel(240, 30);
		//loadBarrel(240, 30);
		//loadBarrel(240, 30);
		//loadBarrel(240, 30);
	}

	alreadyLoaded = 1;

	draw_level1();
	drawMario(false);
	drawPeach();
	drawDonkeyKong();
	drawBarrels();

	swap_buffers();
	draw_level1();
	drawPeach();
	drawDonkeyKong();
	drawBarrels();

	close_bmp(background_bmp);

	changeState(LEVEL1);
}

// Initializer function for the load state
static void draw_load_screen(void)
{
	// Draw the background to both buffers.
	draw_box( 0,  50,  320,  170, back_alpha,  1);
	draw_bmp(background_bmp, 0, 240 - background_bmp->bmp_info_header->height,
			false, back_alpha, 1);
	swap_buffers();
	draw_box( 0,  50,  320,  170, back_alpha,  1);
	draw_bmp(background_bmp, 0, 240 - background_bmp->bmp_info_header->height,
			false, back_alpha, 1);
}
