/*
 * state_machine.c
 *
 *  Created on: 2013-10-06
 *      Author: Jeremy
 */

#include "state_machine.h"
#include "level1.h"
#include "load_screen.h"
#include "mainmenu.h"
#include "game_over.h"
#include "bitmap.h"
#include "win_game.h"

#define NULL 0

static State states[NUM_STATE_MACHINE_STATES] =
{
	{updateMainMenu, draw_main_menu, NULL, 0},	/* MAIN_MENU */
	{updateLoadScreen, NULL, NULL, 0},	/* LOADING_SCREEN */
	{update_level1, NULL, NULL, 0},	/* LEVEL1 */
	{updateGameOver, draw_gameover, clear_display, 0}, /* GAME_OVER */
	{updateWinGame, draw_wingame, clear_display, 0} /* WIN_GAME */
};

static eState current_state = MAIN_MENU;

void changeState(eState new_state)
{
	if (states[current_state].destructor_func != NULL)
	{
		states[current_state].destructor_func();
	}

	states[current_state].initialized = 0;
	current_state = new_state;
}

void runState()
{
	if (!states[current_state].initialized &&
			states[current_state].init_func != NULL)
	{
		states[current_state].initialized = 1;
		states[current_state].init_func();
	}

	if (states[current_state].handler != NULL)
	{
		states[current_state].handler();
	}
}
