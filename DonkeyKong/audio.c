/*
 * audio.c
 *
 *  Created on: 2013-10-03
 *      Author: Christopher (El Magnifico) Tan
 */

#include "audio.h"
#include "system.h"



// Requires:
// Effects: Plays an audio .wav file. Returns 0 on success or -1 for error.
//
//
//

int playMusic(char* audioFile) {

	alt_up_sd_card_dev *device_reference = NULL;
	device_reference = alt_up_sd_card_open_dev("/dev/sdcard_interface");

	//Check if SD Card interface is ready
	if (device_reference != NULL) {
		printf("SD card reader is opened\n");
	}
	else {
		printf("SD card reader is null\n");
		return AUDIO_ERROR;
	}

	//Check if there is an SD Card
	if (alt_up_sd_card_is_Present()) {
		printf("SD Card is present.\n");
	}
	else {
		printf("SD Card is not present.\n");
		return AUDIO_ERROR;
	}

	//Check if SD card is FAT16 format
	if (!alt_up_sd_card_is_FAT16()) {
		printf("Not FAT16\n");
		return AUDIO_ERROR;
	}


	alt_up_av_config_dev* av_config = alt_up_av_config_open_dev("/dev/audio_and_video_config");


	if (av_config != NULL) {
		printf("AV is opened\n");
	}
	else {
		printf("AV is null\n");
		return AUDIO_ERROR;
	}


	while (!alt_up_av_config_read_ready(av_config)) {
	}
	printf("Configuration read ready!\n");

	if (alt_up_av_config_enable_interrupt(av_config) == 0) {
		printf("Config interrupt enabled\n");
	}

	alt_up_audio_dev* audio = alt_up_audio_open_dev("/dev/audio_core");


	if (audio != NULL) {
		printf("Audio dev is opened (not null)\n");
	}
	else {
		printf("Audio is null \n");
		return AUDIO_ERROR;
	}

	short int fileHandle = alt_up_sd_card_fopen(audioFile, false);


	if (fileHandle >= 0)
		printf("File is opened\n");
	else {
		printf("File failed\n");
		return AUDIO_ERROR;
	}

	int i, j = 0;
	short int cardRead = 0;
	unsigned char firstPart;
	unsigned char secondPart;
	int fileLength = 0;

	//Read the file and obtain the length of a .wav file
	short tempBuf[8];
	for (i = 0; i < 7; i++) {
		cardRead = alt_up_sd_card_read(fileHandle);
		tempBuf[i] = cardRead;
	}

	fileLength = (tempBuf[7] << 24) | (tempBuf[6] << 16) | (tempBuf[5] << 8) | tempBuf[4];
	fileLength /= 2;

	// Close the SD card
	alt_up_sd_card_fclose(fileHandle);

	// Allocate a buffer that is the same byte size as the file
	unsigned int *buf = (unsigned int*) malloc(fileLength * sizeof(unsigned int));

	int currentSample = 0;
	int length = FIFO_SIZE;
	unsigned int *sample;
	int remainder = 0;

	// Reopen the SD card
	fileHandle = alt_up_sd_card_fopen(audioFile, false);

	alt_up_audio_reset_audio_core(audio);

	// Set up the buffer
	for (i = 0; i < fileLength+4; ++i) {
		// Get the first part of the PCM sample
		cardRead = alt_up_sd_card_read(fileHandle);
		firstPart = cardRead;

		// Get the second part of the PCM sample
		cardRead = alt_up_sd_card_read(fileHandle);
		secondPart = cardRead;

		// Ignore the upper halves. Get the REAL PCM sample
		buf[i] = (secondPart << 8) | firstPart;
		//buf[i] = combineHalves;
		//printf("Byte %i: %x \n",i,buf[i]); // sanity check

		if (currentSample + FIFO_SIZE > fileLength) {
			length = fileLength - currentSample;
		}

		//if ( ( (i % FIFO_SIZE) == (FIFO_SIZE - 1) ) || length < FIFO_SIZE )
		if ( ( (i / FIFO_SIZE) > 10 ) || length < FIFO_SIZE )
		{
//			unsigned int *cursor;
//			int toWrite = getFifoSpace();
//			if (toWrite > 0) {
//				write(cursor, toWrite);
//				cursor += toWrite;
//			}
			if (alt_up_audio_write_fifo_space(audio, ALT_UP_AUDIO_RIGHT) > FIFO_SIZE+1) {
				sample = &(buf[currentSample]);
				//printf("Putting space into the FIFO\n");
				alt_up_audio_write_fifo(audio, sample, length, ALT_UP_AUDIO_LEFT);
				alt_up_audio_write_fifo(audio, sample, length, ALT_UP_AUDIO_RIGHT);
				currentSample+=FIFO_SIZE;
			}
		}

		// This code will bestow ultimate power upon he/she who believes
	}

	alt_up_sd_card_fclose(fileHandle);
	alt_up_audio_reset_audio_core(audio);
	return 0;
}
