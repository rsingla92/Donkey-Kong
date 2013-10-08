/*
 * input.c
 *
 *  Created on: 2013-10-03
 *      Author: Jeremy
 */

#include "input.h"
#include "system.h"
#include "io.h"

/*
 * getButton
 * Parameter(s): buttonNum - The button to check (0 to 4).
 *
 * Checks the state of one of the buttons (buttonNum goes from 0 to 4).
 * A 0 represents a pressed button.
 */
unsigned char getButton(int buttonNum) {
	if (buttonNum >= BUTTONS_DATA_WIDTH || buttonNum < 0) return 0;

	unsigned char mask = 1 << buttonNum;

	return (IORD_8DIRECT(buttons, 0) & mask) >> buttonNum;
}
