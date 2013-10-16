/*
 * level1.h
 *
 *  Created on: Sep 24, 2013
 *      Author: lauren
 */

#ifndef LEVEL1_H_
#define LEVEL1_H_

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	Point start;
	Point end;
	int width; // for ladder
} Plane;

void draw_level1();
int find_floor(int x, int y , double ref, int* current_floor);
int is_ladder(int x, int y, int height, int current_floor);
int find_ladder_floor (int x, int y, int current_floor);
int find_ladder_top (int x, int y, int height, int current_floor);

void update_level1(void);
void init_level1(void);
int should_barrel_die(int x, int y);
Plane getLaddersElement(int index);
void resetLevel(void);

#endif /* LEVEL1_H_ */
