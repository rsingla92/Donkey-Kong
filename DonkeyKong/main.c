#include <stdio.h>
#include "random.h"
#include "sdcard.h"
#include "bitmap.h"
#include "display.h"
#include "level1.h"
#include "background.h"
#include "priv/alt_busy_sleep.h"
#include "sys/alt_alarm.h"
#include "io.h"
#include "sys/alt_timestamp.h"
#include "mario.h"
#include "movingObject.h"
#include "state_machine.h"
#include "audio.h"
#include "input.h"

#define NUM_FILES 44
// Controller Out: Bits: 000000AB
// A is the Controller P/S latch
// B is the Controller Clock
#define controller_out (char *) CONTROLLER_OUTPUT_BASE

// Controller In: Only least significant bit matters.
#define controller_in (volatile char *) CONTROLLER_INPUT_BASE

#define leds (volatile char *) LEDS_BASE

static char* file_list[NUM_FILES] = { "4B.BMP", "B1.BMP", "B2.BMP", "B3.BMP",
		"B4.BMP", "B5.BMP", "DK1.BMP", "DK2.BMP", "DK3.BMP", "DK4.BMP",
		"DK5.BMP", "DK6.BMP", "DK7.BMP", "DK8.BMP", "DK9.BMP", "DK10.BMP",
		"DK11.BMP", "FIRE.BMP", "FIRE1.BMP", "FIRE2.BMP", "FIRE3.BMP",
		"HMR.BMP", "M1.BMP", "M2.BMP", "M3.BMP", "M4.BMP", "M5.BMP", "M6.BMP",
		"M7.BMP", "M8.BMP", "M9.BMP", "M10.BMP", "M11.BMP", "M12.BMP",
		"M13.BMP", "M14.BMP", "M15.BMP", "P1.BMP", "P2.BMP", "PP1.BMP",
		"PP2.BMP", "PP3.BMP", "PURSE.BMP", "UMBRLA.BMP", "MM1.BMP", "MM2.BMP"};
static BitmapHandle* bmp;
static alt_u32 ticks_per_sec;
static alt_u32 num_ticks;
static alt_32 update(void *context);
static void readDat();

/* Global Variables */
unsigned char button_states[4] = {1, 1, 1, 1};
unsigned char prev_state[4] = {1, 1, 1, 1};
controller_buttons controller_state;
controller_buttons prev_controller_state;

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)

static void readDat(){
	unsigned short accumulatedData = 0;
	int i;
	copyController(&prev_controller_state, controller_state);

	IOWR_8DIRECT(controller_out, 0, 0x01);
	IOWR_8DIRECT(controller_out, 0, 0x03);
	alt_busy_sleep(12);
	IOWR_8DIRECT(controller_out, 0, 0x01);
	alt_busy_sleep(6);

	accumulatedData = IORD_8DIRECT(controller_in, 0);

	for (i = 0; i < 16; i++)
	{
		IOWR_8DIRECT(controller_out, 0, 0x00);
		alt_busy_sleep(6);
		accumulatedData <<= 1;
		accumulatedData += IORD_8DIRECT(controller_in, 0);
		IOWR_8DIRECT(controller_out, 0, 0x01); // Pulse clock
		alt_busy_sleep(6);
	}

	IOWR_8DIRECT(leds, 0, accumulatedData);

	copyController(&controller_state, getControllerButtons(accumulatedData));
}

int main(void) {
	// Start the timestamp -- will be used for seeding the random number generator.

	alt_timestamp_start();
	sdcard_handle *sd_dev = init_sdcard();
	initAudio();

	printf("Loading\n");
	loadMusic("Title2.wav", 1, 0.25);

	printf("Done loading\n");
	// Set latch and clock to 0.
	IOWR_8DIRECT(controller_out, 0, 0x00);

	init_display();

	clear_display();

	if (sd_dev == NULL)
		return 1;

	printf("Card connected.\n");

	ticks_per_sec = alt_ticks_per_second();

	seed(alt_timestamp());

	alt_u32 tickCount = alt_nticks();
	num_ticks = ticks_per_sec / 60;
	//alt_alarm *update_alarm = malloc(sizeof(alt_alarm));
	//alt_alarm_start(update_alarm, num_ticks, update, (void*)0);

	while (true)
	{
		if (alt_nticks() - tickCount >= num_ticks)
		{
			tickCount = alt_nticks();
			update(0);
		}
	}

	return 0;
}

alt_32 update(void *context) {
	int i;
	for (i = 0; i < 4; i++) prev_state[i] = button_states[i];
	for (i = 0; i < 4; i++) button_states[i] = getButton(i);

	readDat();
	runState();
	return 1;

}
