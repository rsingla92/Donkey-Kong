/*
 * input.c
 *
 *  Created on: 2013-10-03
 *      Author: Jeremy
 */

#include "input.h"
#include "system.h"
#include <stdio.h>
#include "io.h"

void initController(controller_buttons* controller) {
	controller->UP_ARROW = 0;
	controller->RIGHT_ARROW = 0;
	controller->DOWN_ARROW = 0;
	controller->LEFT_ARROW = 0;
	controller->SELECT = 0;
	controller->START = 0;
	controller->X_BUTTON = 0;
	controller->A_BUTTON = 0;
	controller->B_BUTTON = 0;
	controller->Y_BUTTON = 0;
	controller->L_BUTTON = 0;
	controller->R_BUTTON = 0;
}

void copyController(controller_buttons* copy_to, controller_buttons copy_from)
{
	copy_to->UP_ARROW = copy_from.UP_ARROW;
	copy_to->RIGHT_ARROW = copy_from.RIGHT_ARROW;
	copy_to->DOWN_ARROW = copy_from.DOWN_ARROW;
	copy_to->LEFT_ARROW = copy_from.LEFT_ARROW;
	copy_to->SELECT = copy_from.SELECT;
	copy_to->START = copy_from.START;
	copy_to->X_BUTTON = copy_from.X_BUTTON;
	copy_to->A_BUTTON = copy_from.A_BUTTON;
	copy_to->B_BUTTON = copy_from.B_BUTTON;
	copy_to->Y_BUTTON = copy_from.Y_BUTTON;
	copy_to->L_BUTTON = copy_from.L_BUTTON;
	copy_to->R_BUTTON = copy_from.R_BUTTON;
}

/* Note that X, A, L and R are not used. */
controller_buttons getControllerButtons(unsigned short dat)
{
	unsigned short normalizedDat = ((~(dat << 8)) >> 8) | (dat & 0xFF00);
	controller_buttons controller;

	initController(&controller);

	/* Arrow Key Hierarchy: Right > Left > Up > Down in priority. */
	if (!((dat >> 8) & 1)) {
		/* Right */
		controller.RIGHT_ARROW = 1;
	//	printf("Right Arrow Pressed\n");
	}
	else if (!((dat >> 9) & 1)) {
		/* Left */
		controller.LEFT_ARROW = 1;
	//	printf("Left Arrow Pressed\n");
	}
	else if (!((dat >> 11) & 1))
	{
		/* Up Button*/
		controller.UP_ARROW = 1;
	//	printf("Up Arrow Pressed\n");
	}
	else if (!((dat >> 10) & 1))
	{
		/* Down Button */
		controller.DOWN_ARROW = 1;
		//printf("Down Arrow Pressed\n");
	}

	if (!((dat >> 15) & 1)) {
		/* B */
		controller.B_BUTTON = 1;
		printf("B Button Pressed\n");
	}

	if (!((dat >> 14) & 1)) {
		/* Y */
		controller.Y_BUTTON = 1;
		printf("Y Button Pressed\n");
	}

	if (!((dat >> 12) & 1)) {
		/* Start button */
		controller.START = 1;
		printf("Start Button Pressed\n");
	}

	if (!((dat >> 13) & 1)) {
		/* Select button */
		controller.SELECT = 1;
	//	printf("Select Button Pressed\n");
	}

	return controller;
}

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
