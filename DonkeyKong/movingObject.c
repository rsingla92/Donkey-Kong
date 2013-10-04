#include "movingObject.h"

/*
 * Need to maintain a list of moving objects
 * Need to implement random AI
 * Need to rough plan this out
 */
static DonkeyKong donkeyKong;
static char* donkeyKong_list[NUM_DONKEYKONG_IMGS] = {"M9.BMP", "M10.BMP", "M11.BMP"};
static colour donkeyKong_alpha = { 0x1F, 0x00, 0x1F };

static MovingObject* barrelListHead;
static char* barrel_list[NUM_BARREL_IMGS] = {"B1.BMP", "B2.BMP", "B3.BMP", "B4.BMP", "B5.BMP"};
static AnimMap barrel_anim_list[NUM_BARREL_IMGS];
static colour barrel_alpha = { 0x1F, 0x00, 0x1F };

static MovingObject* fireListHead;
static char* fire_list[NUM_FIRE_IMGS] = {"FIRE.BMP", "FIRE1.BMP", "FIRE2.BMP", "FIRE3.BMP"};
static AnimMap fire_anim_list[NUM_FIRE_IMGS];
static colour fire_alpha = { 0x1F, 0x00, 0x1F };

static Peach peach;
static char* peach_list[NUM_PEACH_IMGS] = {"PP1.BMP", "PP2.BMP", "PP3.BMP"};
static colour peach_alpha = { 0x1F, 0x00, 0x1F };

void drawFire(MovingObject* fire)
{
	draw_bmp(fire_anim_list[fire->current_frame].handle, fire->x, fire->y,true, fire_alpha, 1);
}

void drawFires()
{
	MovingObject* fireItr = fireListHead;
	while(fireItr != NULL)
	{
		drawFire(fireItr);
	}
}

void loadFire( int x, int y)
{
	MovingObject* fireItr = fireListHead;
	while(fireItr->next != NULL)
	{
		fireItr = fireItr->next;
	}

	MovingObject* newFire = malloc(sizeof(MovingObject));
	addFire(newFire, x, y);
}

void addFire(MovingObject* newFire, int x, int y)
{
	newFire->x = x;
	newFire->y = y;
	newFire->speed = 1;
	newFire->current_frame = FIRE_BUCKET;
	newFire->state = STILL;
}

void loadFires()
{
	load_bmp(fire_list[FIRE_BUCKET], &(fire_anim_list[FIRE_BUCKET].handle));
	load_bmp(fire_list[FIRE_BALL_ONE], &(fire_anim_list[FIRE_BALL_ONE].handle));
	load_bmp(fire_list[FIRE_BALL_TWO], &(fire_anim_list[FIRE_BALL_TWO].handle));
	load_bmp(fire_list[FIRE_BALL_THREE], &(fire_anim_list[FIRE_BALL_THREE].handle));
}

void drawBarrel(MovingObject* barrel)
{
	draw_bmp(barrel_anim_list[barrel->current_frame].handle, barrel->x,
			barrel->y, true, barrel_alpha, 1);
}

void drawBarrels()
{
	MovingObject* barrelItr = barrelListHead;
	while(barrelItr != NULL)
	{
		drawBarrel(barrelItr);
	}
}

void loadBarrel( int x, int y)
{
	MovingObject* barrelItr = barrelListHead;
	while(barrelItr->next != NULL)
	{
		barrelItr = barrelItr->next;
	}

	MovingObject* newBarrel = malloc(sizeof(MovingObject));
	addBarrel(newBarrel, x, y);
}

void addBarrel(MovingObject* newBarrel, int x, int y)
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
	load_bmp(barrel_list[ROLLING_BOTTOM_LEFT], &(barrel_anim_list[ROLLING_BOTTOM_LEFT].handle));
	load_bmp(barrel_list[ROLLING_BOTTOM_RIGHT], &(barrel_anim_list[ROLLING_BOTTOM_RIGHT].handle));
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

void drawPeach(void)
{
	draw_bmp(peach.animation[peach.current_frame].handle,
			 peach.x, peach.y, true, peach_alpha, 1);
}

void loadPeach(void)
{
	load_bmp(peach_list[PEACH_HELP], &(peach.animation[PEACH_HELP].handle));
	load_bmp(peach_list[PEACH_HEARTBROKEN], &(peach.animation[PEACH_HEARTBROKEN].handle));
	load_bmp(peach_list[PEACH_HEART], &(peach.animation[PEACH_HEART].handle));
}
