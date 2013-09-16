#include "altera_up_avalon_character_lcd.h"
int main(void)
{
	alt_up_character_lcd_dev * char_lcd_dev;
	// open the Character LCD port
	char_lcd_dev = alt_up_character_lcd_open_dev ("/dev/character_lcd_0");
	if ( char_lcd_dev == NULL)
		alt_printf ("Error: could not open character LCD device\n");
	else
		alt_printf ("Opened character LCD device\n");
	/* Initialize the character display */
	alt_up_character_lcd_init (char_lcd_dev);
	/* Write "Welcome to" in the ﬁrst row */
	alt_up_character_lcd_string(char_lcd_dev, "Welcome to");
	/* Write "the DE2 board" in the second row */
	char second_row[] = "the DE2 board\0";
	alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
	alt_up_character_lcd_string(char_lcd_dev, second_row);
}
