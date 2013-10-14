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

sdcard_handle* init_sdcard(void);
short int card_connected(void);
short int is_FAT16(void);
file_handle open_file(char *filename, bool create);
void close_file(file_handle file);
byte read_file(file_handle file);
short int write_byte_to_file( short int file_handle, byte byte_of_data);

#endif /* SDCARD_H_ */
