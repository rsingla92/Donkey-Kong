/*
 * game_over.c
 *
 *  Created on: Oct 13, 2013
 *      Author: singla
 */

#include "game_over.h"
#include "bitmap.h"
#include "state_machine.h"
#include "input.h"
#include "sdcard.h"
#include "background.h"

#define FINAL_SCORE_X 50
#define FINAL_SCORE_Y 30

#define HIGH_SCORE_X 50
#define HIGH_SCORE_Y 34

static BitmapHandle* background_bmp;
static BitmapHandle* cursor_bmp;

static colour back_alpha = {0x00, 0x00, 0x00};
extern unsigned char button_states[4];
extern controller_buttons controller_state;
extern controller_buttons prev_controller_state;

void updateGameOver(void)
{
	static int firstUpdate = 0;
	if(!firstUpdate)
	{
		file_handle scores = open_file("scores.txt", false);
		if( scores == -1)
		{
			printf("UH OH...\n");
		}
		byte dataByte = alt_up_sd_card_read(scores);
		int i = 0;
		char buf[50];
		while(dataByte >= 0 && i < 15)
		{
			buf[i] = dataByte;
			i++;
			dataByte = alt_up_sd_card_read(scores);
		}
		char* test = "99999";
		draw_string(test, FINAL_SCORE_X, FINAL_SCORE_Y);
		draw_string(test, HIGH_SCORE_X, HIGH_SCORE_Y);
		firstUpdate = 1;

	}

	draw_box( 90,  130,  110,  140, back_alpha,  1);
    draw_bmp(cursor_bmp, 0, 240-cursor_bmp->bmp_info_header->height,
    		false, back_alpha, 1);

	if(!button_states[0] ||
			(!prev_controller_state.B_BUTTON && controller_state.B_BUTTON ))
	{
		close_bmp(background_bmp);
		changeState(MAIN_MENU);
	}
	else if(!button_states[1] ||
			(!prev_controller_state.Y_BUTTON && controller_state.Y_BUTTON))
	{
		close_bmp(background_bmp);
		changeState(LOADING_SCREEN);
	}

}

void draw_gameover(void)
{
	short int ret = load_bmp("GO.BMP", &background_bmp);
    short int ret1 = load_bmp("CURSOR.BMP", &cursor_bmp);

	// Draw background to both buffers

	draw_bmp(background_bmp, 0, 240 - background_bmp->bmp_info_header->height,
			false, back_alpha, 1);

	swap_buffers();

	draw_box( 90,  90,  110,  110, back_alpha,  1);
	draw_bmp(background_bmp, 0, 240 - background_bmp->bmp_info_header->height,
				false, back_alpha, 1);

}
