/*
 * movingObject.
 *
 *  Created on: 2013-10-01
 *      Author: Ricky
 */

#ifndef MOVING_OBJECT_H_
#define MOVING_OBJECT_H_

#include "display.h"
#include "bitmap.h"
#include "background.h"

/* Barrel Files */
typedef enum {FLAT_BARREL,
			  ROLLING_TOP_LEFT,
			  ROLLING_TOP_RIGHT,
			  ROLLING_BOTTOM_LEFT,
			  ROLLING_BOTTOM_RIGHT,
			  NUM_BARREL_IMGS
} BarrelImage;

/* Donkey Kong Files */
typedef enum {STANDING_STILL,
			  GRABBING_BARREL,
			  CARRYING_BARREL,
			  ROLLING_BARREL,
			  ANGRY_LEFT,
			  ANGRY_RIGHT,
			  CLIMBING_LEFT,
			  CLIMBING_RIGHT,
			  UPSIDE_DOWN_LEFT,
			  UPDATE_DOWN_RIGHT,
			  NUM_DONKEYKONG_IMGS
} DonkeyKongImage;

/* Fire Files */
typedef enum {FIRE_BUCKET,
			  FIRE_BALL_ONE,
			  FIRE_BALL_TWO,
			  FIRE_BALL_THREE,
			  NUM_FIRE_IMGS
			  } FireImage;
/* Peach Files */
typedef enum{ PEACH_HELP,
	   	   	  PEACH_HEARTBROKEN,
	   	   	  PEACH_HEART,
	   	   	  NUM_PEACH_IMGS
			} PeachImage;

/* Other Files */
typedef enum { HAMMER,
			   PLATFORM_DOWN,
			   PLATFORM_UP,
			  } OtherImage;


/* MovingObject States */
 typedef enum {LAYING, ROLLING, STILL, MOVING, THROWABLE, OBJ_FALLING} MovingObjectState;

/* Donkey Kong States */
typedef enum {STANDING, THROWING, ANGRY, CLIMBING, UPSIDE} DonkeyKongState;

/* Peach States */
typedef enum {HEART, HEARTBROKEN, HELP} PeachState;

// Animation image
typedef struct
{
	BitmapHandle* handle;
	bool flip;
} AnimMap;

// MovingObject
typedef struct MovingObject
{
	int x, y, speed;
	float current_frame;
	float past_frame;
	int currentFloor;
	int byLadder;
	MovingObjectState state;
	struct MovingObject* prev;
	struct MovingObject* next;
} MovingObject;

// Donkey Kong
typedef struct
{
	AnimMap animation[NUM_DONKEYKONG_IMGS];
	int x, y;
	float current_frame;
	float past_frame;
	DonkeyKongState state;
} DonkeyKong;

// Peach
typedef struct
{
	AnimMap animation[NUM_PEACH_IMGS];
	int x, y;
	float current_frame;
	PeachState state;
} Peach;

void drawBarrel(MovingObject*);
void drawBarrels();
void loadBarrel(int, int);
void addBarrel(MovingObject*, int, int);
void loadBarrels();
void moveBarrels(BarrelImage lowFrame, BarrelImage highFrame);
int MOgetCurrentWidth(MovingObject* itr);
int MOgetCurrentHeight(MovingObject* itr);
int MOgetPastWidth(MovingObject* itr);
int MOgetPastHeight(MovingObject* itr);
void MOdrawBackground(int x0, int y0, int x1, int y1);

void drawDonkeyKong();
void loadDonkeyKong(int, int);
void animateDonkeyKong(DonkeyKongImage lowFrame, DonkeyKongImage highFrame);
void drawFire(MovingObject* fire);
void drawFires();
void addFire(MovingObject*, int, int);
void loadFire(int, int);
void loadFires();

void drawPeach(void);
void loadPeach(int, int);

void drawOtherObject();
void loadOtherObject(int, int);
MovingObject* getBarrelListHead(void);

#endif /* MOVING_OBJECT_H_ */
