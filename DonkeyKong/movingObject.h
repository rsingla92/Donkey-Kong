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
			  ANGRY_LEFT,
			  ANGRY_RIGHT,
			  CLIMBING_LEFT,
			  CLIMBING_RIGHT,
			  UPSIDE_DOWN_LEFT,
			  UPDATE_DOWN_RIGHT,
			  WALK_RIGHT,
			  SUPER_ANGRY,
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

/* Barrel Animations */
 typedef enum {SIDE, ROLL_1, ROLL_2, ROLL_3, ROLL_4} BarrelAnim;

/* DonkeyKong Animations */
// typedef enum {} DonkeyKongAnim;

/* Fire Animations */
// typedef enum {} FireAnim;

/* Other Animations */
// I don't think there are any for these
// typedef enum {} OtherAnim;

/* MovingObject States */
 typedef enum {LAYING, ROLLING, STILL, MOVING} MovingObjectState;

/* Donkey Kong States */
typedef enum {STANDING, THROWING, ANGRY, CLIMBING, UPSIDE} DonkeyKongState;

/* Peach States */
typedef enum {HEART, HEARTBROKEN, HELP} PeachState;

/* Other States */
// typedef enum {} OtherState;

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

void drawDonkeyKong();
void loadDonkeyKong(int, int);

void drawFire(MovingObject* fire);
void drawFires();
void addFire(MovingObject*, int, int);
void loadFire(int, int);
void loadFires();

void drawPeach(void);
void loadPeach(int, int);

void drawOtherObject();
void loadOtherObject(int, int);

#endif /* MOVING_OBJECT_H_ */
