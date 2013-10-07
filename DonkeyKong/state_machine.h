/*
 * state_machine.h
 *
 *  Created on: 2013-10-06
 *      Author: Jeremy
 */

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

typedef void (*handler_func)(void);
typedef void (*pre_func)(void);
typedef void (*after_func)(void);

typedef struct {
	handler_func handler;
	pre_func init_func;
	after_func destructor_func;
} State;

typedef enum {MAIN_MENU, LOADING_SCREEN, LEVEL1, NUM_STATE_MACHINE_STATES} eState;

void changeState(eState new_state);
void runState();

#endif /* STATE_MACHINE_H_ */
