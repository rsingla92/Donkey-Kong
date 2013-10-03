/*
 * input.h
 *
 *  Created on: 2013-10-03
 *      Author: Jeremy
 */

#ifndef INPUT_H_
#define INPUT_H_

#define buttons (volatile char *) BUTTONS_BASE

unsigned char getButton(int buttonNum);

#endif /* INPUT_H_ */
