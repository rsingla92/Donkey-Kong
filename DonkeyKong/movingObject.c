#include "movingObject.h"

/*
 * Need to maintain a list of moving objects
 * Need to implement random AI
 * Need to rough plan this out
 */
static DonkeyKong donkeyKong;
static char* donkeyKong_list[NUM_DONKEYKONG_IMGS] = {"M9.BMP", "M10.BMP", "M11.BMP"};
static colour donkeyKong_alpha = { 0x1F, 0x00, 0x1F };

void loadDonkeyKong(int x, int y)
{
	// Animation list for Donkey Kong
	load_bmp(donkeyKong_list[STANDING_STILL], &(donkeyKong.animation[STANDING_STILL].handle));
	load_bmp(donkeyKong_list[GRABBING_BARREL], &(donkeyKong.animation[GRABBING_BARREL].handle));
	load_bmp(donkeyKong_list[ANGRY_LEFT], &(donkeyKong.animation[ANGRY_LEFT].handle));
	load_bmp(donkeyKong_list[ANGRY_RIGHT], &(donkeyKong.animation[ANGRY_RIGHT].handle));
	load_bmp(donkeyKong_list[CLIMBING_LEFT], &(donkeyKong.animation[CLIMBING_LEFT].handle));
	load_bmp(donkeyKong_list[CLIMBING_RIGHT], &(donkeyKong.animation[CLIMBING_RIGHT].handle));
	load_bmp(donkeyKong_list[UPSIDE_DOWN_LEFT], &(donkeyKong.animation[UPSIDE_DOWN_LEFT].handle));
	load_bmp(donkeyKong_list[UPDATE_DOWN_RIGHT], &(donkeyKong.animation[UPDATE_DOWN_RIGHT].handle));
	load_bmp(donkeyKong_list[WALK_RIGHT], &(donkeyKong.animation[WALK_RIGHT].handle));
	load_bmp(donkeyKong_list[SUPER_ANGRY], &(donkeyKong.animation[SUPER_ANGRY].handle));

	// There are no right-facing animations. It's all either facing the player or not.
	// There are no flipping animations.

	donkeyKong.current_frame = STANDING_STILL;
	donkeyKong.x = x;
	donkeyKong.y = y;
	donkeyKong.state = STANDING;

}

void drawDonkeyKong(void)
{
	draw_bmp(donkeyKong.animation[donkeyKong.current_frame].handle,
			donkeyKong.x, donkeyKong.y, true, donkeyKong_alpha, 1);
}
