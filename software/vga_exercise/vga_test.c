#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h";
#include <stdio.h>

int main()
{
	alt_up_pixel_buffer_dma_dev* pixel_buffer;
	alt_up_char_buffer_dev *char_buffer;

	pixel_buffer =
			alt_up_pixel_buffer_dma_open_dev("/dev/dma_buffer");

	char_buffer = alt_up_char_buffer_open_dev("/dev/char_drawer");

	// Initialize the character buffer.
	alt_up_char_buffer_init(char_buffer);

	// Set the background buffer address.
	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,
			PIXEL_BUFFER_BASE);

	// Swap background and foreground buffers
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);

	// Wait for the swap to complete
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

	// Clear the screen
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);

	// Draw a white line to the foreground buffer
	alt_up_pixel_buffer_dma_draw_line(pixel_buffer, 0, 0, 320, 240, 0xFFFF, 0);

	int tester = 0;

	int textX = 0;

	while (1) {

		if (tester > 125000) tester = 0;

		if (tester == 0) {
			alt_up_char_buffer_clear(char_buffer);
			if (textX >= 80) textX = 0;
			else textX++;
			alt_up_char_buffer_string(char_buffer, "Donkey Kong", textX, 10);
		}

		tester++;
	}
	return 0;
}

