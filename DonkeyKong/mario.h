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

#define MARIO_START_X 	0
#define MARIO_START_Y 	217
typedef enum { UP, DOWN, LEFT, RIGHT } MarioDirection;

/* Mario Files */
typedef enum { STAND_IMG, WALK1_IMG, WALK2_IMG, CLIMB_IMG, DEAD1_IMG, DEAD2_IMG, DEAD3_IMG, DEAD4_IMG,
	HMR1_IMG, HMR2_IMG, HMR3_IMG, HMR4_IMG, HMR5_IMG, HMR6_IMG, NUM_IMGS } MarioImages;

/* Mario Animations */
typedef enum { STAND_LEFT, WALK1_LEFT, WALK2_LEFT, STAND_RIGHT,
			WALK1_RIGHT, WALK2_RIGHT, CLIMB1, CLIMB2, DEAD1, DEAD2, DEAD3, DEAD4,
			HMR1_RIGHT, HMR2_RIGHT, HMR3_RIGHT, HMR4_RIGHT, HMR5_RIGHT, HMR6_RIGHT,
			HMR1_LEFT, HMR2_LEFT, HMR3_LEFT, HMR4_LEFT, HMR5_LEFT, HMR6_LEFT, NUM_ANIMS } MarioAnims;

/* Mario States */
typedef enum { WALKING, JUMPING, FALLING, M_CLIMBING, LADDER_BOTTOM,
	LADDER_TOP, DEAD, HAMMERING, NUM_MARIO_STATES } MarioState;

#define MAX_JUMP 12

typedef struct
{
	BitmapHandle* handle;
	bool flip;
} MarioAnimMap;

typedef struct
{
	MarioAnimMap animation[NUM_ANIMS];
	int x, y;
	int speed;
	float current_frame;
	int jumpStart;
	int currentFloor;
	MarioState state;
	int lives;
} Mario;

void loadMario(int x, int y, int speed);
void drawMario(bool bothBuffers);

bool moveMario(MarioDirection dir);
Mario getMario(void);
Mario* getMarioRef(void);

bool moveUp(void);
bool moveDown(void);
bool moveLeft(void);
bool moveRight(void);

int getCurrentWidth(void);
int getCurrentHeight(void);
void setMarioJumpStart(int height);
int getMarioJumpStart(void);

int getMarioCurrentFloor(void);
void setMarioCurrentFloor(int currentFloor);

void changeMarioState(MarioState state);
MarioState getMarioState(void);
void setMarioLives(int lives);
void drawLives(void);
void eraseLives(void);

void setMarioX(int x);
void setMarioY(int y);
void setMarioCurrentFrame(float current_frame);

void setMarioJumpStart(int jumpStart);

void setMarioCurrentFloor(int floor);
#endif /* MARIO_H_ */
