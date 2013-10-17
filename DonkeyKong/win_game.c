/*
 * win_game.c
 *
 *  Created on: 2013-10-15
 *      Author: Jeremy
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

static colour back_alpha = {0x00, 0x00, 0x00};
extern unsigned char button_states[4];
extern controller_buttons controller_state;
extern controller_buttons prev_controller_state;
extern int points;
extern int bonus;

int* winSoundBuf;
int winSoundBufLen;

extern int* menuSoundBuf;
extern int menuSoundBufLen;

void updateWinGame(void)
{
	if(!button_states[0] ||
			(!prev_controller_state.B_BUTTON && controller_state.B_BUTTON ))
	{
		close_bmp(background_bmp);
		swapInSound(menuSoundBuf, menuSoundBufLen, 1);
		changeState(MAIN_MENU);
	}
	else if(!button_states[1] ||
			(!prev_controller_state.Y_BUTTON && controller_state.Y_BUTTON))
	{
		close_bmp(background_bmp);
		changeState(LOADING_SCREEN);
	}
}

void draw_wingame(void)
{
	short int ret = load_bmp("WIN.BMP", &background_bmp);

	pauseMusic();
	clear_display();

	// Draw background to both buffers
	draw_bmp(background_bmp, 0, 240 - background_bmp->bmp_info_header->height,
			false, back_alpha, 1);

	swap_buffers();

	//draw_box( 90,  90,  110,  110, back_alpha,  1);
	draw_bmp(background_bmp, 0, 240 - background_bmp->bmp_info_header->height,
				false, back_alpha, 1);

	swapInSound(winSoundBuf, winSoundBufLen, 1);

	file_handle scores = open_file("s.txt", false);

	if( scores == -1)
	{
		printf("UH OH...\n");
	}

	byte dataByte = alt_up_sd_card_read(scores);
	int i = 0;
	char buf[4];

	while(dataByte >= 0 && i < 3)
	{
		buf[i] = dataByte;
		i++;
		dataByte = alt_up_sd_card_read(scores);
	}

	close_file(scores);

	int high_score = ((buf[0] - '0') * 100) + ((buf[1] - '0')*10) + (buf[2] - '0');

	char score_buf[4];
	int total_points = points + bonus;
	sprintf(score_buf, "%3d", total_points);

	if (high_score < total_points)
	{
		high_score = total_points;

		scores = open_file("s.txt", false);
		write_byte(scores, score_buf[0]);
		write_byte(scores, score_buf[1]);
		write_byte(scores, score_buf[2]);
		close_file(scores);
	}

	sprintf(buf, "%3d", high_score);
	draw_string(score_buf, FINAL_SCORE_X, FINAL_SCORE_Y);
	draw_string(buf, HIGH_SCORE_X, HIGH_SCORE_Y);

	resetLevel();
	bonus = 300;
}
