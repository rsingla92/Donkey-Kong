/*
 * level1.c
 *
 *  Created on: Sep 24, 2013
 *      Author: lauren
 */
#include "level1.h"
#include "background.h"
#include "bitmap.h"
#include "mario.h"
#include "movingObject.h"
#include "sys/alt_timestamp.h"
#include "input.h"
#include "game_over.h"
#include "state_machine.h"

#define LADDER_ERROR	2
#define MAX_POINTS		300

static colour hmr_alpha = { 0x00, 0x00, 0x00 };
extern unsigned char button_states[4];
extern unsigned char prev_state[4];
extern controller_buttons controller_state;
extern controller_buttons prev_controller_state;

static unsigned char deadFlag = 0;
static unsigned char jumpHang = 0;
static alt_timestamp_type start_time;
static bool firstMove = true;
int bonus = MAX_POINTS;
int points = 0;

Point barrels_die = {0,200};
Point hammer1_location = {300,190};
Point hammer2_location = {10, 106};

static const Plane floors[] =
{
	{{137,62}, {205,62}, 0 }, 	// top floor
	{{-1,91}, {205,91}, 0}, 	// second floor
	{{206,92}, {228,92}, 0 },
	{{229,93}, {251,93}, 0 },
	{{252,94}, {273,94}, 0 },
	{{274,95}, {296,95}, 0 },
	{{23,123}, {67,123}, 0}, 	// third floor
	{{68,122}, {90,122}, 0 },
	{{91,121}, {113,121}, 0 },
	{{114,120}, {136,120}, 0 },
	{{137,119}, {159,119}, 0 },
	{{160,118}, {182,118}, 0 },
	{{183,117}, {205,117}, 0 },
	{{206,116}, {227,116}, 0 },
	{{228,115}, {250,115}, 0 },
	{{251,114}, {273,114}, 0 },
	{{274,113}, {296,113}, 0 },
	{{297,112}, {320,112}, 0 },
	{{-1,141}, {22,141}, 0 },	// fourth floor
	{{23,142}, {45,142}, 0 },
	{{46,143}, {90,143}, 0 },
	{{91,144}, {113,144}, 0 },
	{{114,145}, {136,145}, 0 },
	{{137,146}, {159,146}, 0 },
	{{160,147}, {182,147}, 0 },
	{{183,148}, {205,148}, 0 },
	{{206,149}, {227,149}, 0 },
	{{228,150}, {250,150}, 0 },
	{{251,151}, {273,151}, 0 },
	{{274,152}, {296,152}, 0 },
	{{23,180}, {45,180}, 0 },	//fifth floor
	{{46,179}, {68,179}, 0 },
	{{69,178}, {91,178}, 0 },
	{{92,177}, {113,177}, 0 },
	{{114,176}, {136,176}, 0 },
	{{137,175}, {159,175}, 0 },
	{{160,174}, {205,174}, 0 },
	{{206,173}, {228,173}, 0 },
	{{229,172}, {250,172}, 0 },
	{{251,171}, {273,171}, 0 },
	{{274,170}, {296,170}, 0 },
	{{297,169}, {320,169}, 0 },
	{{-1,198}, {22,198}, 0 },	//sixth floor
	{{23,199}, {45,199}, 0 },
	{{46,200}, {90,200}, 0 },
	{{91,201}, {113,201}, 0 },
	{{114,202}, {136,202}, 0 },
	{{137,203}, {159,203}, 0 },
	{{160,204}, {182,204}, 0 },
	{{183,205}, {205,205}, 0 },
	{{206,206}, {227,206}, 0 },
	{{228,207}, {250,207}, 0 },
	{{251,208}, {273,208}, 0 },
	{{274,209}, {296,209}, 0 },
	{{-1,232}, {159,232}, 0 }, // bottom floor
	{{160,231}, {182,231}, 0 },
	{{183,230}, {205,230}, 0 },
	{{206,229}, {228,229}, 0 },
	{{229,228}, {273,228}, 0 },
	{{274,227}, {296,227}, 0 },
	{{297,226}, {320,226}, 0 },
};

static const Plane ladders[] =
{
	{{108,26}, {108,91}, 13 },
	{{123,26}, {123,91}, 13 },
	{{192,62}, {192,91}, 13 },
	{{249,93}, {249,114}, 13 },
	{{135,91}, {135,104}, 13 },
	{{135,113}, {135,119}, 13 },
	{{44,123}, {44,143}, 13 },
	{{112,120}, {112,145}, 13 },
	{{226,115}, {226,128}, 13 },
	{{226,139}, {226,150}, 13 },
	{{89,144}, {89,157}, 13 },
	{{89,162}, {89,177}, 13 },
	{{158,147}, {158,175}, 13 },
	{{249,151}, {249,171}, 13 },
	{{44,178}, {44,200}, 13 },
	{{135,175}, {135,203}, 13 },
	{{112,202}, {112,215}, 13 },
	{{112,221}, {112,232}, 13 },
	{{249,207}, {249,228}, 13 },
};

/* The following array is for efficiency. It maps a floor number to
 * the index in ladders that corresponds to the ladders for that floor. */
static int floorToLadderMap[7] = {0, 0, 3, 6, 10, 14, 16};

#define NUM_FLOORS (sizeof(floors)/sizeof(floors[0]))
#define NUM_LADDERS (sizeof(ladders)/sizeof(ladders[0]))

/* For efficiency in searching the floor list */
#define FIRST_FLOOR_IND		0
#define SECOND_FLOOR_IND	1
#define THIRD_FLOOR_IND		6
#define FOURTH_FLOOR_IND	18
#define FIFTH_FLOOR_IND		30
#define SIXTH_FLOOR_IND		42
#define SEVENTH_FLOOR_IND	54

#define FIRST_FLOOR_Y				62
#define FIRST_FLOOR_X_LOW_BOUND		137
#define FIRST_FLOOR_X_HIGH_BOUND	205
#define SECOND_FLOOR_Y				95
#define THIRD_FLOOR_Y				123
#define FOURTH_FLOOR_Y				152
#define FIFTH_FLOOR_Y				180
#define SIXTH_FLOOR_Y				209
#define SEVENTH_FLOOR_Y				232
#define FLOOR_X_LOW_BOUND			23
#define FLOOR_X_HIGH_BOUND			296

Plane getLaddersElement(int index)
{
	return ladders[index];
}

int is_ladder (int x, int y, int height, int current_floor){
	int i;
	for (i = floorToLadderMap[current_floor]; i < NUM_LADDERS; i++){
		if (y >= (ladders[i].start.y - height) && y <= (ladders[i].end.y - height))
			if (x >= ladders[i].start.x && x <= (ladders[i].start.x) + (ladders[i].width/3))
				return i;
	}
	return -1;
}

int find_ladder_floor (int x, int y, int current_floor) {
	int i;
	if (current_floor == 0) return -1;
	for (i = floorToLadderMap[current_floor]; i < NUM_LADDERS; i++){
		if (y <= ladders[i].end.y){
			if(x >= ladders[i].start.x && x <= (ladders[i].start.x)+(ladders[i].width)/3)
				return (ladders[i].end.y);
		}
	}
	return -1;
}

int find_ladder_top (int x, int y, int height, int current_floor){
	int i;
	if (current_floor == 6) return -1;

	for (i = floorToLadderMap[current_floor + 1]; i < NUM_LADDERS; i++){
		if (y <= ladders[i].start.y && y + height >= ladders[i].start.y - 2){
			if(x >= ladders[i].start.x && x <= (ladders[i].start.x)+(ladders[i].width)/3)
				return (ladders[i].start.y);
		}
	}
	return -1;
}

int find_floor(int x, int y, double ref, int* current_floor)
{
	int i, start;

	/* The following checks greatly improve efficiency, by making sure we loop through at most
	 * 6 times. */
	if (y + ref <= FIRST_FLOOR_Y && x >= FIRST_FLOOR_X_LOW_BOUND && x <= FIRST_FLOOR_X_HIGH_BOUND)
	{
		start = FIRST_FLOOR_IND;
		*current_floor = 0;

		resetLevel();
		changeState(WIN_GAME);
	}
	else if (y + ref <= SECOND_FLOOR_Y && x <= FLOOR_X_HIGH_BOUND)
	{
		*current_floor = 1;
		if (x < floors[SECOND_FLOOR_IND + 3].start.x)
		{
			start = SECOND_FLOOR_IND;
		}
		else
		{
			start = SECOND_FLOOR_IND + 3;
		}
	}
	else if (y + ref <= THIRD_FLOOR_Y && x >= FLOOR_X_LOW_BOUND)
	{
		*current_floor = 2;
		if (x < floors[THIRD_FLOOR_IND + 6].start.x)
		{
			start = THIRD_FLOOR_IND;
		}
		else
		{
			start = THIRD_FLOOR_IND + 6;
		}
	}
	else if (y+ref <= FOURTH_FLOOR_Y && x <= FLOOR_X_HIGH_BOUND)
	{
		*current_floor = 3;
		if (x < floors[FOURTH_FLOOR_IND + 6].start.x)
		{
			start = FOURTH_FLOOR_IND;
		}
		else
		{
			start = FOURTH_FLOOR_IND + 6;
		}
	}
	else if (y+ref <= FIFTH_FLOOR_Y && x >= FLOOR_X_LOW_BOUND)
	{
		*current_floor = 4;
		if (x < floors[FIFTH_FLOOR_IND + 6].start.x)
		{
			start = FIFTH_FLOOR_IND;
		}
		else
		{
			start = FIFTH_FLOOR_IND + 6;
		}
	}
	else if (y+ref <= SIXTH_FLOOR_Y && x <= FLOOR_X_HIGH_BOUND)
	{
		*current_floor = 5;
		if (x < floors[SIXTH_FLOOR_IND + 6].start.x)
		{
			start = SIXTH_FLOOR_IND;
		}
		else
		{
			start = SIXTH_FLOOR_IND + 6;
		}
	}
	else
	{
		*current_floor = 6;
		if (x < floors[SEVENTH_FLOOR_IND + 3].start.x)
		{
			start = SEVENTH_FLOOR_IND;
		}
		else
		{
			start = SEVENTH_FLOOR_IND + 3;
		}
	}

	for (i = start; i < NUM_FLOORS; i++){
		if (y + ref <= floors[i].end.y){
			if(x >= floors[i].start.x && x <= floors[i].end.x)
				return (floors[i].start.y);
		}
	}
	return -1;
}

void draw_level1(void) {
	short int ret = loadBackground("LVL1.BMP");
	if (ret < 0) {
			printf("Could not load level1. Ret: %d\n", ret);
		}

	// Draw the background
	drawBackground();
	drawLives();
	drawHammer();
}

void drawHammer(void)
{
	BitmapHandle* hammerHandle;
	load_bmp("HMR.BMP", &hammerHandle);
	draw_bmp(hammerHandle, hammer1_location.x, hammer1_location.y, 0, hmr_alpha, 1);
	draw_bmp(hammerHandle, hammer2_location.x, hammer2_location.y, 0, hmr_alpha, 1);
	close_bmp(hammerHandle);
}

void hitHammer(void)
{

	if (( (getMario().x >= hammer1_location.x && getMario().x <= hammer1_location.x+7) ||
			(getMario().x+getCurrentWidth() >= hammer1_location.x && getMario().x+getCurrentWidth() <= hammer1_location.x+7))
			&& (getMario().y >= hammer1_location.y && getMario().y <= hammer1_location.y+8))
	{
		eraseHammer(1);
		changeMarioState(HAMMERING);
	}

	if (( (getMario().x >= hammer2_location.x && getMario().x <= hammer2_location.x+7) ||
				(getMario().x+getCurrentWidth() >= hammer2_location.x && getMario().x+getCurrentWidth() <= hammer2_location.x+7))
				&& (getMario().y >= hammer2_location.y && getMario().y <= hammer2_location.y+8))
		{
			eraseHammer(2);
			changeMarioState(HAMMERING);
		}
}

void eraseHammer(int number)
{
	if(number == 1)
	{
		draw_box(hammer1_location.x, hammer1_location.y, hammer1_location.x + 7, hammer1_location.y + 8, hmr_alpha, 1);
		draw_box(hammer1_location.x, hammer1_location.y, hammer1_location.x + 7, hammer1_location.y + 8, hmr_alpha, 0);
		hammer1_location.x = 0;
		hammer1_location.y = 0;
	}
	else if(number == 2)
	{
		draw_box(hammer2_location.x, hammer2_location.y, hammer2_location.x + 7, hammer2_location.y + 8, hmr_alpha, 1);
		draw_box(hammer2_location.x, hammer2_location.y, hammer2_location.x + 7, hammer2_location.y + 8, hmr_alpha, 0);
		hammer2_location.x = 0;
		hammer2_location.y = 0;
	}
}

void resetLevel(void)
{
	setDonkeyKongState(THROWING);
	setDonkeyKongFrame(STANDING_STILL);
	deadFlag = 0;
	jumpHang = 0;
	firstMove = true;
	bonus = MAX_POINTS;
	points = 0;
	setMarioLives(3);
	setMarioX(MARIO_START_X);
	setMarioY(MARIO_START_Y);
	setMarioCurrentFrame(STAND_RIGHT);
	setMarioJumpStart(MARIO_START_Y);
	setMarioCurrentFloor(6);
	stopBarrels();
	hammer1_location.x = 300;
	hammer1_location.y = 190;
	hammer2_location.x = 10;
	hammer2_location.y = 106;
}

bool is_num_in_range(int num, int lowBound, int highBound) {
	return (num >= lowBound && num <= highBound);
}

void init_level1(void) {

}

int should_barrel_die(int x, int y){
	if (x <= barrels_die.x && y >= barrels_die.y)
		return 1;
	return 0;
}

void update_level1(void) {
	int floor = 0;
	int ladder_ind = 0;

	if (!firstMove)
	{
		alt_timestamp_type end_time = alt_timestamp();
		char buf[50];
		char score_buf[50];

		if ((2*(end_time - start_time)/alt_timestamp_freq()) > 1)
		{
			alt_timestamp_start();
			start_time = alt_timestamp();
			bonus = bonus - 1;
			if (bonus < 0)
				bonus = 0;
			sprintf(buf, "Bonus: %03d", bonus);
			sprintf(score_buf, "Score: %03d", points);
			draw_string(buf, 0, 0);
			draw_string(score_buf, 0, 2);
		}
	}
	else
	{
		char buf[50];
		char score_buf[50];

		sprintf(buf, "Bonus: %03d", MAX_POINTS);
		sprintf(score_buf, "Score: %03d", points);
		draw_string(buf, 0, 0);
		draw_string(score_buf, 0, 2);
	}

	if (deadFlag && getMarioState() != DEAD)
	{
		if (getMario().lives > 0)
		{
			// Mario resurrected.
			setDonkeyKongState(THROWING);
			setDonkeyKongFrame(STANDING_STILL);
			deadFlag = 0;
			firstMove = true;
			bonus = MAX_POINTS;
			jumpHang = 0;
		}
		else
		{
			resetLevel();
			changeState(GAME_OVER);
		}
	}

	hitHammer(); // handles hammer collision

	if (getMarioState() == JUMPING)
	{
		/* Mario is jumping */
		if (getMario().y <= getMarioJumpStart() - MAX_JUMP)
		{
			if (jumpHang > 5)
			{
				changeMarioState(FALLING);
				jumpHang = 0;
			}
			else
			{
				jumpHang++;
			}
		}
		else if (!jumpHang)
		{
			moveMario(UP);
		}
  	}
	else if (getMarioState() != FALLING && getMarioState() != DEAD && getMarioState() != HAMMERING && (ladder_ind = is_ladder(getMario().x, getMario().y, getCurrentHeight(), getMario().currentFloor)) != -1)
	{
		int ladder_end_y = ladders[ladder_ind].end.y;
		int ladder_begin_y = ladders[ladder_ind].start.y;

		if ( is_num_in_range(getMario().y + getCurrentHeight(),
				ladder_end_y-LADDER_ERROR, ladder_end_y+LADDER_ERROR))
		{
			changeMarioState(LADDER_BOTTOM);

		}
		else if( is_num_in_range(getMario().y + getCurrentHeight(),
						ladder_begin_y-LADDER_ERROR, ladder_begin_y+LADDER_ERROR))
		{
			changeMarioState(LADDER_TOP);
		}

		if (getMarioState() == LADDER_BOTTOM &&
				(button_states[1] == 0 || controller_state.UP_ARROW) )
		{
			/* Mario is beginning to climb. */
			changeMarioState(M_CLIMBING);
		}
		else if (getMarioState() == LADDER_TOP &&
				(button_states[0] == 0 || controller_state.DOWN_ARROW))
		{
			changeMarioState(M_CLIMBING);
		}
		else if (getMarioState() == LADDER_TOP &&
				(button_states[1] == 0 || controller_state.B_BUTTON))
		{
			changeMarioState(JUMPING);
			setMarioJumpStart(getMario().y);
		}

		if (getMarioState() == M_CLIMBING)
		{
			if (button_states[1] == 0 || controller_state.UP_ARROW)
			{
				moveMario(UP);
			}
			else if (button_states[0] == 0 || controller_state.DOWN_ARROW)
			{
				moveMario(DOWN);
			}
		}
	}
	else if (getMarioState() != DEAD)
	{
		/* Drop to the correct floor: */
		floor = find_floor(getMario().x, getMario().y, 3*(getCurrentHeight()/4), &(getMarioRef()->currentFloor)) - getCurrentHeight();
		if (getMario().y < floor) moveMario(DOWN);
		else if (getMario().y > floor) moveMario(UP);

		if (getMario().y > floor - 2 && getMario().y < floor + 2)
		{
			if (getMarioState() != HAMMERING)
			{
				changeMarioState(WALKING);
				if (button_states[1] == 0 || controller_state.B_BUTTON)
				{
					changeMarioState(JUMPING);
					setMarioJumpStart(getMario().y);
				}
			}
		}
	}

	if ((button_states[3] == 0 || button_states[2] == 0 ||
			controller_state.RIGHT_ARROW || controller_state.LEFT_ARROW) && firstMove)
	{
		alt_timestamp_start();
		start_time = alt_timestamp();
		firstMove = false;
	}

	if (getMarioState() == WALKING || getMarioState() == LADDER_BOTTOM ||
			getMarioState() == LADDER_TOP ||
			getMarioState() == JUMPING || getMarioState() == FALLING || getMarioState() == HAMMERING)
	{
		if (button_states[3] == 0 || controller_state.LEFT_ARROW) moveMario(LEFT);
		else if (button_states[2] == 0 || controller_state.RIGHT_ARROW) moveMario(RIGHT);
	}

	if (moveBarrels(ROLLING_TOP_LEFT, ROLLING_BOTTOM_RIGHT))
	{
		// Mario was hit.
		deadFlag = 1;
	}

	int kongWidth = getKong().animation[(int) round(getKong().current_frame)].handle->bmp_info_header->width;
	int kongHeight = getKong().animation[(int) round(getKong().current_frame)].handle->bmp_info_header->height;

	if (getMario().x <= getKong().x + kongWidth &&
			getMario().x + getCurrentWidth() >= getKong().x &&
			getMario().y <= getKong().y + kongHeight &&
			getMario().y + getCurrentHeight() >= getKong().y)
	{
		// Collision with DK
		changeMarioState(DEAD);
		stopBarrels();
		setDonkeyKongState(ANGRY);
		deadFlag = 1;
	}

	drawDonkeyKong();
	drawBarrels();
	drawMario(false);
	eraseAllNoPop();
	swap_buffers();
	eraseAll();
}
