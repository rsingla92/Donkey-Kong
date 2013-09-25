#include <stdio.h>
#include "sdcard.h"
#include "bitmap.h"
#include "display.h"
#include "background.h"
#include "level1.h"
#include "sys/alt_alarm.h"

#define NUM_FILES 44

static char* file_list[NUM_FILES] = {
		"4B.BMP", "B1.BMP", "B2.BMP", "B3.BMP", "B4.BMP", "B5.BMP", "DK1.BMP",
		"DK2.BMP", "DK3.BMP", "DK4.BMP", "DK5.BMP", "DK6.BMP", "DK7.BMP", "DK8.BMP",
		"DK9.BMP", "DK10.BMP", "DK11.BMP", "FIRE.BMP", "FIRE1.BMP", "FIRE2.BMP",
		"FIRE3.BMP", "HMR.BMP", "M1.BMP", "M2.BMP", "M3.BMP", "M4.BMP", "M5.BMP",
		"M6.BMP", "M7.BMP", "M8.BMP", "M9.BMP", "M10.BMP", "M11.BMP", "M12.BMP",
		"M13.BMP", "M14.BMP", "M15.BMP", "P1.BMP", "P2.BMP", "PP1.BMP", "PP2.BMP",
		"PP3.BMP", "PURSE.BMP", "UMBRLA.BMP"
};

static 	BitmapHandle* bmp;
static alt_u32 ticks_per_sec;
static alt_u32 num_ticks;

static alt_32 update(void *context);

int main(void) {
	init_display();
	sdcard_handle *sd_dev = init_sdcard();
	//BitmapHandle* images[NUM_FILES];

	clear_display();

	if (sd_dev == NULL) return 1;

	printf("Card connected.\n");

	int i;
	load_bmp("M1.BMP", &bmp);

	draw_level1();
	/*short int ret = loadBackground("LVL1.BMP");

	if (ret < 0) {
		printf("Could not load background. Ret: %d\n", ret);
	}
	// Draw the background to both buffers.
	drawBackground();
	swap_buffers();
	drawBackground();
	 */

	int count = 0;
	int x = 0;
	int y = 72;

	ticks_per_sec = alt_ticks_per_second();
	num_ticks = ticks_per_sec/60;

	alt_alarm *update_alarm;
	alt_alarm_start(update_alarm, num_ticks, update, NULL);


	/*
	while (true) {
		if (!count) {
			if (x > 320)
			{
				x = 0;
			}
			else
			{
				drawBackgroundSection(x-1, y, x, y + bmp->bmp_info_header->height);
				//draw_box(x-1, y, x, y + bmp->bmp_info_header->height, makeCol(0, 0, 0), 1);
				x++;
			}

			draw_bmp(bmp, x, y, true, col);

			swap_buffers();
		} else {
			count = (count > 100000) ? 0 : count + 1;
		}
	}*/

	return 0;
}

static alt_32 update(void *context)
{
	static int x = 0;
	int y = 72;
	colour col = { 0x1F, 0x00, 0x1F };

	if (x > 320)
	{
		x = 0;
	}
	else
	{
		drawBackgroundSection(x-1, y, x, y + bmp->bmp_info_header->height);
	}

	draw_bmp(bmp, x, y, true, col);
	x++;

	swap_buffers();
}
