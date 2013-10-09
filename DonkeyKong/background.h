/*
 * background.h
 *
 *  Created on: 2013-09-22
 *      Author: Jeremy
 */

#ifndef BACKGROUND_H_
#define BACKGROUND_H_

typedef struct sEraseRect {
	int x0, y0;
	int x1, y1;
} EraseRect;

typedef struct sEraseStack {
	struct sEraseStack* next;
	EraseRect rect;
} EraseStack;

short int loadBackground(char *filename);
void drawBackground();
void drawBackgroundSection(int x0, int y0, int x1, int y1);

/* Erase queue */
void pushEraseNode(int x0, int y0, int x1, int y1);
EraseRect popEraseNode();
void eraseAll();
void eraseAllNoPop();

#endif /* BACKGROUND_H_ */
