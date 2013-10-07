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
#include "sys/alt_timestamp.h"

#define LADDER_ERROR	2
#define MAX_POINTS		300

extern unsigned char button_states[4];
extern unsigned char prev_state[4];

static alt_timestamp_type start_time;
static int points = MAX_POINTS;

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	Point start;
	Point end;
	int width; // for ladder
} Plane;

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
	{{110,26}, {110,91}, 11 },
	{{125,26}, {125,91}, 11 },
	{{194,62}, {194,91}, 11 },
	{{251,94}, {251,114}, 11 },
	{{137,91}, {137,104}, 11 },
	{{137,113}, {137,119}, 11 },
	{{46,123}, {46,143}, 11 },
	{{114,120}, {114,145}, 11 },
	{{228,115}, {228,128}, 11 },
	{{228,139}, {228,150}, 11 },
	{{91,144}, {91,157}, 11 },
	{{91,162}, {91,177}, 11 },
	{{160,147}, {160,175}, 11 },
	{{251,151}, {251,171}, 11 },
	{{46,178}, {46,200}, 11 },
	{{137,175}, {137,203}, 11 },
	{{114,202}, {114,215}, 11 },
	{{114,221}, {114,232}, 11 },
	{{251,207}, {251,228}, 11 },
};

#define NUM_FLOORS (sizeof(floors)/sizeof(floors[0]))
#define NUM_LADDERS (sizeof(ladders)/sizeof(ladders[0]))

int is_ladder (int x, int y){
	int i;
	for (i = 0; i < NUM_LADDERS; i++){
		if (y >= (ladders[i].start.y - getCurrentHeight()) && y <= (ladders[i].end.y - getCurrentHeight()))
			if (x >= ladders[i].start.x && x <= (ladders[i].start.x) + (ladders[i].width/3))
				return i;
	}
	return -1;
}

int find_ladder_floor (int x, int y) {
	int i;
	for (i = 0; i < NUM_LADDERS; i++){
		if (y <= ladders[i].end.y){
			if(x >= ladders[i].start.x && x <= (ladders[i].start.x)+(ladders[i].width)/3)
				return (ladders[i].end.y);
		}
	}
	return -1;
}

int find_ladder_top (int x, int y){
	int i;
	for (i = 0; i < NUM_LADDERS; i++){
		if (y <= ladders[i].end.y){
			if(x >= ladders[i].start.x && x <= (ladders[i].start.x)+(ladders[i].width)/3)
				return (ladders[i].start.y);
		}
	}
	return -1;
}

int find_floor(int x, int y, double ref){
	int i;
	for (i = 0; i < NUM_FLOORS; i++){
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

	// Draw the background to both buffers.
	drawBackground();
	swap_buffers();
	drawBackground();
}

bool is_num_in_range(int num, int lowBound, int highBound) {
	return (num >= lowBound && num <= highBound);
}

void update_level1(void) {
	static bool firstMove = true;
	int floor = 0;
	int ladder_ind = 0;

	if (!firstMove) {
		alt_timestamp_type end_time = alt_timestamp();
		char buf[50];

		if ((2*(end_time - start_time)/alt_timestamp_freq()) > 1){
			alt_timestamp_start();
			start_time = alt_timestamp();
			points = points - 1;
			if (points < 0)
				points = 0;
		}
		sprintf(buf, "Score: %03d", points);
		draw_string(buf, 0, 0);
	}

	if (getMarioState() == JUMPING) {
		/* Mario is jumping */
		if (getMario().y <= getMarioJumpStart() - MAX_JUMP) {
			changeMarioState(FALLING);
		} else {
			moveMario(UP);
		}
  	} else if (getMarioState() != FALLING && (ladder_ind = is_ladder(getMario().x,getMario().y)) != -1) {
		int ladder_end_y = ladders[ladder_ind].end.y;
		int ladder_begin_y = ladders[ladder_ind].start.y;
		changeMarioState(M_CLIMBING);
		if ( is_num_in_range(getMario().y + getCurrentHeight(),
				ladder_end_y-LADDER_ERROR, ladder_end_y+LADDER_ERROR)) {
			changeMarioState(LADDER_BOTTOM);

		} else if( is_num_in_range(getMario().y + getCurrentHeight(),
						ladder_begin_y-LADDER_ERROR, ladder_begin_y+LADDER_ERROR)) {
			changeMarioState(LADDER_TOP);
		}

		if (getMarioState() == LADDER_BOTTOM && button_states[1] == 0 ) {
			/* Mario is beginning to climb. */
			changeMarioState(M_CLIMBING);
		} else if (getMarioState() == LADDER_TOP && button_states[0] == 0) {
			changeMarioState(M_CLIMBING);
		} else if (getMarioState() == LADDER_TOP && button_states[1] == 0) {
			changeMarioState(JUMPING);
			setMarioJumpStart(getMario().y);
		}

		if (getMarioState() == M_CLIMBING) {
			if (button_states[1] == 0) {
				moveMario(UP);
			} else if (button_states[0] == 0) {
				moveMario(DOWN);
			}
		}
	} else {
		// changeMarioState(WALKING);

		/* Drop to the correct floor: */
		floor = find_floor(getMario().x, getMario().y, 3*(getCurrentHeight()/4)) - getCurrentHeight();
		if (getMario().y < floor) moveMario(DOWN);
		else if (getMario().y > floor) moveMario(UP);

		if (getMario().y > floor - 2 && getMario().y < floor + 2) {
			changeMarioState(WALKING);
			if (button_states[1] == 0) {
				changeMarioState(JUMPING);
				setMarioJumpStart(getMario().y);
			}
		}
	}

	if ((button_states[3] == 0 || button_states[2] == 0) && firstMove) {
		alt_timestamp_start();
		start_time = alt_timestamp();
		firstMove = false;
	}

	if (getMarioState() == WALKING || getMarioState() == LADDER_BOTTOM ||
			getMarioState() == LADDER_TOP ||
			getMarioState() == JUMPING || getMarioState() == FALLING) {
		if (button_states[3] == 0) moveMario(LEFT);
		else if (button_states[2] == 0) moveMario(RIGHT);
	}

	drawMario();
}
