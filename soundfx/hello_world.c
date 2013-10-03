/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_sd_card_avalon_interface.h"

#define FIFO_SIZE 112

int main() {
	printf("Hello from Nios II!\n");

	char* audioFile = "Title2.wav";

	playMusic(audioFile);

	return 0;
}

void playMusic(char* audioFile) {

	alt_up_sd_card_dev *device_reference = NULL;
	device_reference = alt_up_sd_card_open_dev("/dev/sdcard_interface");

	//Check if SD Card interface is ready
	if (device_reference != NULL)
		printf("SD card reader is opened\n");
	else
		printf("SD card reader is null\n");

	//Check if there is an SD Card
	if (alt_up_sd_card_is_Present())
		printf("SD Card is present.\n");
	else
		printf("SD Card is not present.\n");

	//Check if SD card is FAT16 format
	if (!alt_up_sd_card_is_FAT16()) {
		printf("Not FAT16\n");
	}


	alt_up_av_config_dev* av_config = alt_up_av_config_open_dev("/dev/audio_and_video_config");


	if (av_config != NULL)
		printf("AV is opened\n");
	else
		printf("AV is null\n");


	while (!alt_up_av_config_read_ready(av_config)) {
	}
	printf("Configuration read ready!\n");

	//alt_up_av_config_enable_interrupt(av_config);
	//printf("Config interrupt enabled\n");

	alt_up_audio_dev* audio = alt_up_audio_open_dev("/dev/audio_0");


	if (audio != NULL)
		printf("Audio dev is opened (not null)\n");
	else
		printf("Audio is null \n");

	short int fileHandle = alt_up_sd_card_fopen(audioFile, false);


	if (fileHandle >= 0)
		printf("File is opened\n");
	else
		printf("File failed\n");

	int i, j = 0;
	short int cardRead = 0;
	unsigned char firstPart;
	unsigned char secondPart;
	int fileLength = 0;

	alt_up_audio_reset_audio_core(audio);

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
	unsigned int *buf = (unsigned int*) malloc(fileLength* sizeof(unsigned int));

	// Reopen the SD card
	fileHandle = alt_up_sd_card_fopen(audioFile, false);

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

		// lator gator
	}


	alt_up_audio_reset_audio_core(audio);

	/*
		int currentSample = 0;
		unsigned int sample;
		while (currentSample < fileLength) {
			if (alt_up_audio_write_fifo_space(audio, ALT_UP_AUDIO_RIGHT) > 2) {
				sample = buf[currentSample];
				//printf("Putting space into the FIFO\n");
				alt_up_audio_write_fifo(audio, &sample, 1, ALT_UP_AUDIO_LEFT);
				alt_up_audio_write_fifo(audio, &sample, 1, ALT_UP_AUDIO_RIGHT);
				currentSample++;
			}

		}
		*/

		int currentSample = 0;
		int length = FIFO_SIZE;
		unsigned int *sample;
		while (currentSample < fileLength) {
			if (alt_up_audio_write_fifo_space(audio, ALT_UP_AUDIO_RIGHT) > FIFO_SIZE+1) {
				sample = &(buf[currentSample]);
				//printf("Putting space into the FIFO\n");
				if (currentSample + FIFO_SIZE > fileLength) {
					length = fileLength - currentSample;
				}
				alt_up_audio_write_fifo(audio, sample, length, ALT_UP_AUDIO_LEFT);
				alt_up_audio_write_fifo(audio, sample, length, ALT_UP_AUDIO_RIGHT);
				currentSample+=FIFO_SIZE;
			}

		}

		/*
		unsigned int fifo_buffer[FIFO_SIZE];

		for (i = 0; i < (fileLength/FIFO_SIZE);i++) {
			if (alt_up_audio_write_fifo_space(audio, ALT_UP_AUDIO_RIGHT)> FIFO_SIZE+1) {
				for (j = 0; j < FIFO_SIZE; j++) {
					fifo_buffer[j] = buf[j+(FIFO_SIZE*i)];
				}
				//printf("Putting space into the FIFO\n");
				alt_up_audio_write_fifo(audio, fifo_buffer, FIFO_SIZE, ALT_UP_AUDIO_LEFT);
				alt_up_audio_write_fifo(audio, fifo_buffer, FIFO_SIZE, ALT_UP_AUDIO_RIGHT);
			}
		}
		*/

		printf("Sound output complete \n");

}
