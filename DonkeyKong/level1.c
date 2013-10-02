/*
 * level1.c
 *
 *  Created on: Sep 24, 2013
 *      Author: lauren
 */

#include "level1.h"
#include "background.h"
#include "bitmap.h"

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
	{{-1,91}, {205,91}, 0}, 		// second floor
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
	{{110,26-12}, {110,91-12}, 11 },// must subtract 12 from top level. height of mario
	{{125,26-12}, {125,91-12}, 11 },
	{{194,62-12}, {194,91-12}, 11 },
	{{251,94-12}, {251,114-12}, 11 },
	{{137,91-12}, {137,104-12}, 11 },
	{{137,113-12}, {137,119-12}, 11 },
	{{46,123-12}, {46,143-12}, 11 },
	{{114,120-12}, {114,145-12}, 11 },
	{{228,115-12}, {228,128-12}, 11 },
	{{228,139-12}, {228,150-12}, 11 },
	{{91,144-12}, {91,157-12}, 11 },
	{{91,162-12}, {91,177-12}, 11 },
	{{160,147-12}, {46,175-12}, 11 },
	{{251,151-12}, {251,171-12}, 11 },
	{{46,178-12}, {46,200-12}, 11 },
	{{137,175-12}, {137,203-12}, 11 },
	{{114,202-12}, {114,215-12}, 11 },
	{{114,221-12}, {114,232-12}, 11 },
	{{251,207-12}, {251,228-12}, 11 },
};

#define NUM_FLOORS (sizeof(floors)/sizeof(floors[0]))
#define NUM_LADDERS (sizeof(ladders)/sizeof(ladders[0]))

int is_ladder (int x, int y){
	int i;
	for (i = 0; i < NUM_LADDERS; i++){
		if (y >= ladders[i].start.y && y <= ladders[i].end.y)
			if (x >= ladders[i].start.x && x <= (ladders[i].start.x) + (ladders[i].width/3))
				return 1;
	}
	return 0;
}

int find_ladder_floor (int x, int y){
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

int find_floor(int x, int y){
	int i;
	for (i = 0; i < NUM_FLOORS; i++){
		if (y <= floors[i].end.y){
			if(x >= floors[i].start.x && x <= floors[i].end.x)
				return (floors[i].start.y);
		}
	}
	return -1;
}

void draw_level1() {
	printf("loading level1...\n");
	short int ret = loadBackground("LVL1.BMP");
	if (ret < 0) {
			printf("Could not load level1. Ret: %d\n", ret);
		}

	// Draw the background to both buffers.
	drawBackground();
	swap_buffers();
	drawBackground();
}


