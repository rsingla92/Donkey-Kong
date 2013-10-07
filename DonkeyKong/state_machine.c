/*
 * state_machine.c
 *
 *  Created on: 2013-10-06
 *      Author: Jeremy
 */

#include "state_machine.h"
#include "level1.h"

#define NULL 0

static State states[NUM_STATE_MACHINE_STATES] =
{
	{NULL, NULL, NULL},	/* MAIN_MENU */
	{NULL, NULL, NULL},	/* LOADING_SCREEN */
	{update_level1, NULL, NULL},	/* LEVEL1 */
};

static eState current_state = LEVEL1;

void changeState(eState new_state)
{
	if (states[current_state].destructor_func != NULL) {
		states[current_state].destructor_func();
	}

	current_state = new_state;

	if (states[current_state].init_func != NULL) {
		states[current_state].init_func();
	}
}

void runState()
{
	if (states[current_state].handler != NULL) {
		states[current_state].handler();
	}
}
