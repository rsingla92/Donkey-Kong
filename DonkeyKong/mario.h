/*
 * mario.h
 *
 *  Created on: 2013-10-01
 *      Author: Jeremy
 */

#ifndef MARIO_H_
#define MARIO_H_

#include "display.h"
#include "bitmap.h"

/* Mario Files */
typedef enum { STAND_IMG, WALK1_IMG, WALK2_IMG, NUM_IMGS } MarioImages;

/* Mario Animations */
typedef enum { STAND_LEFT, WALK1_LEFT, WALK2_LEFT, STAND_RIGHT,
			WALK1_RIGHT, WALK2_RIGHT, NUM_ANIMS } MarioAnims;

/* Mario States */
typedef enum { NORMAL, CLIMBING, DEAD, NUM_STATES } MarioState;

typedef struct
{
	BitmapHandle* handle;
	bool flip;
} AnimMap;

typedef struct
{
	AnimMap animation[NUM_ANIMS];
	int x, y;
	unsigned int current_frame;
	MarioState state;
} Mario;

void loadMario();

#endif /* MARIO_H_ */
