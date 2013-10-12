/*
 * sdcard.c
 *
 */
#include "sdcard.h"
#include <stdio.h>
#include <stdlib.h>

// File Variables:
static short int FAT16;
static short int sdcard_connected;

short int checkConnectedAndFat();

sdcard_handle* init_sdcard(void)
{
	sdcard_handle *device_reference = NULL;
	device_reference = alt_up_sd_card_open_dev(SDCARD_INTERFACE_NAME);

	if (!(sdcard_connected = card_connected()))
	{
		printf("Error: sd card not connected.\n");
		return NULL;
	}

	if (!(FAT16 = is_FAT16()))
	{
		printf("Error: Unknown file system.\n");
		return NULL;
	}

	return device_reference;
}

short int card_connected(void)
{
	return alt_up_sd_card_is_Present();
}

short int is_FAT16(void)
{
	return alt_up_sd_card_is_FAT16();
}

file_handle open_file(char *filename, bool create)
{
	if (!checkConnectedAndFat()) return -2;

	return alt_up_sd_card_fopen(filename, create);
}

void close_file(file_handle file)
{
	alt_up_sd_card_fclose(file);
}

byte read_file(file_handle file)
{
	if (!checkConnectedAndFat()) return -2;

	return alt_up_sd_card_read(file);
}

short int checkConnectedAndFat(void)
{
	if (!sdcard_connected || !FAT16)
	{
		printf("Error: Cannot open file: SD Card not connected properly.\n");
		return 0;
	}
	else
	{
		return 1;
	}
}
