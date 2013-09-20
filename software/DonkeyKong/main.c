#include <stdio.h>
#include "sdcard.h"
#include "bitmap.h"
#include "display.h"

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

int main(void) {
	init_display();
	sdcard_handle *sd_dev = init_sdcard();
	BitmapHandle* images[NUM_FILES];

	clear_display();

	if (sd_dev != NULL)
	{
		printf("Card connected.\n");

		int i;
		for (i = 0; i < NUM_FILES; i++)
		{
			short int ret = load_bmp(file_list[i], &(images[i]));
			if (ret < 0)
			{
				printf("Invalid file! ret: %d, file_list[%d]: %s\n", ret, i, file_list[i]);
				return -1;
			}
		}

		colour col = { 0x1F, 0x00, 0x1F };

		for (i = 0; i < NUM_FILES; i++)
		{
			int x = (i % 10) * 32;
			int y = (i / 10) * 24;
			draw_bmp(images[i], x, y, true, col);
		}
	}
	return 0;
}
