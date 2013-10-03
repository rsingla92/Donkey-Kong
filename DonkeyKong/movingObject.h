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
			  ROLLING_BOTTOM_RIGHT
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
			  } FireImage;

/* Other Files */
typedef enum { HAMMER,
			   PLATFORM_DOWN,
			   PLATFORM_UP,
			   PEACH_HELP,
			   PEACH_HEARTBROKEN,
			   PEACH_HEART
			  } OtherImage;

/* Barrel Animations */
typedef enum {} BarrelAnim;

/* DonkeyKong Animations */
typedef enum {} DonkeyKongAnim;

/* Fire Animations */
typedef enum {} FireAnim;

/* Other Animations */
// I don't think there are any for these
// typedef enum {} OtherAnim;

/* Barrel States */
typedef enum {} BarrelState;

/* Donkey Kong States */
typedef enum {STANDING, THROWING, ANGRY, CLIMBING, UPSIDE} DonkeyKongState;

/* Fire States */
typedef enum {} FireState;

/* Other States */
typedef enum {} OtherState;

// Animation image
typedef struct
{
	BitmapHandle* handle;
	bool flip;
} AnimMap;

// Barrel
typedef struct
{
	AnimMap animation[];
	int x, y;
	unsigned int current_frame;
	BarrelState state;

} Barrel;

// Donkey Kong
typedef struct
{
	AnimMap animation[];
	int x, y;
	unsigned int current_frame;
	DonkeyKongState state;
} DonkeyKong;

void drawBarrel();
void loadBarrel(BarrelImage, int, int);

void drawDonkeyKong();
void loadDonkeyKong(DonkeyKongImage, int, int);

void drawFire();
void loadFire(FireImage, int, int);

void drawOtherObject();
void loadOtherObject(OtherImage, int, int);

#endif /* MOVING_OBJECT_H_ */
