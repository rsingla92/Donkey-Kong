/*
 * level1.h
 *
 *  Created on: Sep 24, 2013
 *      Author: lauren
 */

#ifndef LEVEL1_H_
#define LEVEL1_H_

void draw_level1();
int find_floor(int x, int y , double ref);
int is_ladder(int x, int y);
int find_ladder_floor (int x, int y);
int find_ladder_top (int x, int y);

void update_level1(void);
void init_level1(void);

#endif /* LEVEL1_H_ */
