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
	{{0, 91}, {205, 91}, 0},
	{{137,62}, {205, 62},0 },
	{{206,92},{296,95}, -3/90 },
};

#define NUM_FLOORS (sizeof(floors)/sizeof(floors[0]))

int find_floor(int x){
	int i;
	for (i = 0; i < NUM_FLOORS; i++){
		if(x >= floors[i].start.x && x <= floors[i].end.x)
			return (floors[i].start.y + floors[i].slope*x);
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


