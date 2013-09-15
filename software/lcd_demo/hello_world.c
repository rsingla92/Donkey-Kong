#include "altera_up_avalon_character_lcd.h"
#include <stdio.h>

#define switches (volatile char *) SWITCHES_BASE
#define leds (char *) LEDS_BASE
#define buttons (volatile char *) BUTTONS_BASE

/* Prototypes */
void turnAllLightsOff();
void turnAllLightsOn();
void turnLightOn(int lightNo);
void turnLightOff(int lightNo);

/* Light Functions: */

void turnAllLightsOff() {
	*leds = 0;
}

void turnAllLightsOn() {
	*leds = 0xFF;
}

void turnLightOn(int lightNo) {
	if (lightNo >= SWITCHES_DATA_WIDTH || lightNo < 0) return;

	unsigned char mask = 1 << lightNo;

	printf("LightNo: %x, Mask: %x.\n", lightNo, mask);

	*leds = (*leds) | mask;
}

void turnLightOff(int lightNo) {
	if (lightNo >= SWITCHES_DATA_WIDTH) return;

	unsigned char mask = ~(1 << lightNo);

	*leds = (*leds) & mask;
}

/* Button Functions */

unsigned char getButton(int buttonNum) {
	if (buttonNum >= BUTTONS_DATA_WIDTH || buttonNum < 0) return 0;

	unsigned char mask = 1 << buttonNum;

	return ((*buttons) & mask) >> buttonNum;
}

int main(void)
{
	alt_up_character_lcd_dev * char_lcd_dev;

	// Turn all LEDs off initially.
	turnAllLightsOff();

	// open the Character LCD port
	char_lcd_dev = alt_up_character_lcd_open_dev ("/dev/character_lcd_0");

	if ( char_lcd_dev == NULL)
		alt_printf ("Error: could not open character LCD device\n");
	else
		alt_printf ("Opened character LCD device\n");

	/* Initialize the character display */
	alt_up_character_lcd_init (char_lcd_dev);

    unsigned char button_states[4] = {1, 1, 1, 1};
    unsigned char prev_state[4] = {1, 1, 1, 1};
    int i;
    int char_count = 0;

	while (1) {
		for (i = 0; i < 4; i++)
			prev_state[i] = button_states[i];

		for (i = 0; i < 4; i++)
			button_states[i] = getButton(i);

		for (i = 0; i < 4; i++) {
			if (prev_state[i] ==  1 && button_states[i] == 0) {
				char ascii_val[1] = {i + '0'};
				if (++char_count > 16) alt_up_character_lcd_shift_display(char_lcd_dev, -1);
				alt_up_character_lcd_write(char_lcd_dev, ascii_val, 1);
			}
		}
	}

	return 0;
}
