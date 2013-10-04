#include "movingObject.h"

/*
 * Need to maintain a list of moving objects
 * Need to implement random AI
 * Need to rough plan this out
 */
static DonkeyKong donkeyKong;
static char* donkeyKong_list[NUM_DONKEYKONG_IMGS] = {"M9.BMP", "M10.BMP", "M11.BMP"};
static colour donkeyKong_alpha = { 0x1F, 0x00, 0x1F };

static Barrel* barrelListHead;
static char* barrel_list[NUM_BARREL_IMGS] = {"B1.BMP", "B2.BMP", "B3.BMP", "B4.BMP", "B5.BMP"};
static AnimMap barrel_anim_list[NUM_BARREL_IMGS];
static colour barrel_alpha = { 0x1F, 0x00, 0x1F };

void drawBarrel(Barrel* barrel)
{
	draw_bmp(barrel_anim_list[barrel->current_frame].handle, barrel->x,
			barrel->y, true, barrel_alpha);
}

void drawBarrels()
{
	Barrel* barrelItr = barrelListHead;
	while(barrelItr != NULL)
	{
		drawBarrel(barrelItr);
	}
}

void loadBarrel( int x, int y)
{
	Barrel* barrelItr = barrelListHead;
	while(barrelItr->next != NULL)
	{
		barrelItr = barrelItr->next;
	}

	Barrel* newBarrel = malloc(sizeof(Barrel));
	loadBarrel(newBarrel, x, y);
}

void addBarrel(Barrel* newBarrel, int x, int y)
{
	newBarrel->x = x;
	newBarrel->y = y;
	newBarrel->speed = 1;
	newBarrel->current_frame = FLAT_BARREL;
	newBarrel->state = LAYING;
}

void loadBarrels()
{
	load_bmp(barrel_list[FLAT_BARREL], &(barrel_anim_list[FLAT_BARREL].handle));
	load_bmp(barrel_list[ROLLING_TOP_LEFT], &(barrel_anim_list[ROLLING_TOP_LEFT].handle));
	load_bmp(barrel_list[ROLLING_TOP_RIGHT], &(barrel_anim_list[ROLLING_TOP_RIGHT].handle));
	load_bmp(anim_list[ROLLING_BOTTOM_LEFT], &(barrel_anim_list[ROLLING_BOTTOM_LEFT].handle));
	load_bmp(anim_list[ROLLING_BOTTOM_RIGHT], &(barrel_anim_list[ROLLING_BOTTOM_RIGHT].handle));
}











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
