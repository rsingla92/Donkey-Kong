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
	float slope;
} Plane;

static const Plane floors[] =
{
	{{137,62}, {205, 62},0 }, 	// top floor
	{{0, 91}, {205, 91}, 0}, 	// second fllor
	{{206,92},{228,92}, 0 },
	{{229,93},{251,93}, 0 },
	{{252,94},{273,94}, 0 },
	{{274,95},{296,95}, 0 },
	{{23,123},{67,123}, 0}, 	// third floor
	{{68,122}, {90, 122},0 },
	{{91,121}, {113, 121},0 },
	{{114,120}, {136, 120},0 },
	{{137,119}, {159, 119},0 },
	{{160,118}, {182, 118},0 },
	{{183,117}, {205, 117},0 },
	{{206,116}, {227, 116},0 },
	{{228,115}, {250, 115},0 },
	{{251,114}, {273, 114},0 },
	{{274,113}, {296, 113},0 },
	{{297,112}, {319,112},0 },

};

#define NUM_FLOORS (sizeof(floors)/sizeof(floors[0]))

int find_floor(int x, int y){
	int i;
	int f=0;
	for (i = 0; i < NUM_FLOORS; i++){
		if (y <= floors[i].end.y){
			f = i;
			if(x >= floors[f].start.x && x <= floors[f].end.x)
				return (floors[f].start.y - floors[f].slope*x);
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


