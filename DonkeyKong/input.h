/*
 * input.h
 *
 *  Created on: 2013-10-03
 *      Author: Jeremy
 */

#ifndef INPUT_H_
#define INPUT_H_

#define buttons (volatile char *) BUTTONS_BASE

typedef struct s_controller_buttons {
	unsigned char UP_ARROW;
	unsigned char RIGHT_ARROW;
	unsigned char DOWN_ARROW;
	unsigned char LEFT_ARROW;
	unsigned char SELECT;
	unsigned char START;
	unsigned char X_BUTTON;
	unsigned char A_BUTTON;
	unsigned char B_BUTTON;
	unsigned char Y_BUTTON;
	unsigned char L_BUTTON;
	unsigned char R_BUTTON;
} controller_buttons;

void copyController(controller_buttons* copy_to, controller_buttons copy_from);
unsigned char getButton(int buttonNum);
controller_buttons getControllerButtons(unsigned short dat);
void initController(controller_buttons* controller);

#endif /* INPUT_H_ */
