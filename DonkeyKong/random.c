/*
 * random.c
 *
 *  Created on: 2013-10-12
 *      Author: Jeremy
 *
 *  Note: This code is based on the description in the
 *  following link: http://en.wikipedia.org/wiki/Linear_congruential_generator
 *  Uses the parameters usually used in ANSI C (as listed in the table).
 */

#include "random.h"

// From ANSI C Standard:
#define RAND_MAX	32768

static unsigned long int random_val = 1;

/* Linear Congruential Generator */
int nextRand(void)
{
	random_val = random_val*1103515245 + 12345;
    return (unsigned int)(random_val/65536) % RAND_MAX;
}

void seed(unsigned int seed)
{
	random_val = seed;
}
