/*
 * sdcard.h
 *
 */

#ifndef SDCARD_H_
#define SDCARD_H_
#include "altera_up_sd_card_avalon_interface.h"

typedef alt_up_sd_card_dev sdcard_handle;
typedef short int file_handle;
typedef unsigned char byte;

sdcard_handle* init_sdcard();
short int card_connected();
short int is_FAT16();
file_handle open_file(char *filename, bool create);
byte read_file(file_handle file);

#endif /* SDCARD_H_ */
