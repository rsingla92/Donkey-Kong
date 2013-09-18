#include <stdio.h>
#include "sdcard.h"
#include "bitmap.h"
#include "display.h"

int main(void) {
	init_display();
	sdcard_handle *sd_dev = init_sdcard();

	clear_display();

	if (sd_dev != NULL)
	{
		printf("Card connected.\n");

		file_handle file = 	open_file("MARIO1.BMP", false);

		if (file >= 0)
		{
			/* Valid File  */
			load_bmp(file);
		}
		else
		{
			printf("Not a valid file. Return val: %d\n", file);
		}
	}

    colour col = { 0xFF, 0xFF, 0xFF };
	draw_bmp(20, 20, false, col);

	return 0;
}
