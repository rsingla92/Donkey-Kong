/*
 * audio.c
 *
 *  Created on: 2013-10-03
 *      Author: Christopher (El Magnifico) Tan
 */

#include "audio.h"
#include "system.h"

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
static void playMusicISR (void* context);
#else
static void playMusicISR (void* context, alt_u32 id);
#endif

//Checks if SD Card is usable
int checkInitSD (alt_up_sd_card_dev *device_reference) {

	//Check if SD Card interface is ready
	if (device_reference == NULL) {
		return AUDIO_ERROR;
	}

	//Check if there is an SD Card
	if (!alt_up_sd_card_is_Present()) {
		return AUDIO_ERROR;
	}

	//Check if SD card is FAT16 format
	if (!alt_up_sd_card_is_FAT16()) {
		return AUDIO_ERROR;
	}

	return 0;
}

int initAVConfig(alt_up_av_config_dev* av_config) {

	if (av_config == NULL) {
		return AUDIO_ERROR;
	}

	alt_up_av_config_reset(av_config);

	/*
	if (alt_up_av_config_enable_interrupt(av_config) == 0) {
		printf("Config interrupt enabled\n");
	}
	*/b

	while (!alt_up_av_config_read_ready(av_config)) {
	}

	return 0;

}

int initAudioCore(alt_up_audio_dev* audio) {

	alt_up_audio_reset_audio_core(audio);

	if (audio == NULL) {
		return AUDIO_ERROR;
	}


	alt_up_audio_enable_write_interrupt(audio);


	return 0;

}

//Finds size of wav file
int findWavSize(char* audioFile) {

	alt_up_sd_card_dev* device_reference = alt_up_sd_card_open_dev("/dev/sdcard_interface");
	if (checkInitSD (device_reference) == -1) {
		printf("SD Failed \n");
		return AUDIO_ERROR;
	}

	int i = 0;
	int fileLength = 0;
	short int cardRead = 0;

	short int fileHandle = alt_up_sd_card_fopen(audioFile, false);

	if (fileHandle == -1) {
		printf("Reading file failed \n");
		return AUDIO_ERROR;
	}

	//Read the file and obtain the length of a .wav file
	short tempBuf[8];
	for (i = 0; i < 8; i++) {
		cardRead = alt_up_sd_card_read(fileHandle);
		tempBuf[i] = cardRead;
	}

	//Do shifting math to get .wav file length
	fileLength = (tempBuf[7] << 24) | (tempBuf[6] << 16) | (tempBuf[5] << 8) | tempBuf[4];
	fileLength /= 2;

	// Close the SD card
	alt_up_sd_card_fclose(fileHandle);

	return fileLength;
}

//Reduces the volume by halfing the sample.
unsigned int reduceVolume(unsigned int buffer) {
	return (buffer/2);
}

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
static void playMusicISR (void* context)
#else
static void playMusicISR (void* context, alt_u32 id)
#endif
{

}

int set_audio_interrupt(alt_up_audio_dev *audio, volatile int edge_capture_thing)
{

    // Need to disable both audio interrupts before setting them up
    // otherwise you get stuck in them when they are setup
    alt_up_audio_disable_read_interrupt(audio);
    alt_up_audio_disable_write_interrupt(audio);

    void *edge_pointer = (void*)&edge_capture_thing;
	#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
	return alt_ic_isr_register(AUDIO_CORE_IRQ_INTERRUPT_CONTROLLER_ID, AUDIO_CORE_IRQ, playMusicISR, edge_pointer, 0x0);
	#else
	return alt_irq_register(AUDIO_CORE_IRQ, edge_pointer, playMusicISR);
	#endif
}


// Requires:
// Effects: Plays an audio .wav file. Returns 0 on success or -1 for error.
//
//
//
int playMusic(char* audioFile) {

	int i = 0;
	int currentSample = 0;
	int length = FIFO_SIZE;
	unsigned int *sample;
	unsigned char firstPart;
	unsigned char secondPart;

	//Set up audio and sd parts
	alt_up_sd_card_dev* device_reference = alt_up_sd_card_open_dev("/dev/sdcard_interface");
	alt_up_av_config_dev* av_config = alt_up_av_config_open_dev("/dev/audio_and_video_config");
	alt_up_audio_dev* audio = alt_up_audio_open_dev("/dev/audio_core");

	//Initialize and check them
	if (checkInitSD (device_reference) == -1) {
		printf("SD Failed \n");
		return AUDIO_ERROR;
	}

	if (initAVConfig(av_config)== -1) {
		printf("AV Failed \n");
		return AUDIO_ERROR;
	}

	if (initAudioCore(audio)== -1) {
		printf("Audio Failed \n");
		return AUDIO_ERROR;
	}

	short int cardRead = 0;

	int fileLength = findWavSize(audioFile);

	// Allocate a buffer that is the same byte size as the file
	unsigned int *buf = (unsigned int*) malloc(fileLength * sizeof(unsigned int));

	// Reopen the SD card
	int fileHandle = alt_up_sd_card_fopen(audioFile, false);

	// Set up the buffer
	for (i = 0; i < fileLength; ++i) {
		// Get the first part of the PCM sample
		cardRead = alt_up_sd_card_read(fileHandle);
		firstPart = cardRead;

		// Get the second part of the PCM sample
		cardRead = alt_up_sd_card_read(fileHandle);
		secondPart = cardRead;

		// Ignore the upper halves. Get the REAL PCM sample
		buf[i] = (secondPart << 8) | firstPart;
		//Now, suppose we want to reduce the volume. We	could divide the sample by two

		if (currentSample + FIFO_SIZE > fileLength) {
			length = fileLength - currentSample;
		}

		//Start playing (filling audio buffer) Currently being filled faster than buffer loading
		if ( ( (i / FIFO_SIZE) > 100 ) || length < FIFO_SIZE )
		//if ((i >= (fileLength*0.0008)) || length < FIFO_SIZE )
		{
//			unsigned int *cursor;
//			int toWrite = getFifoSpace();
//			if (toWrite > 0) {
//				write(cursor, toWrite);
//				cursor += toWrite;
//			}

			if (alt_up_audio_write_fifo_space(audio, ALT_UP_AUDIO_RIGHT) > FIFO_SIZE+1) {
				sample = &(buf[currentSample]);
				alt_up_audio_write_fifo(audio, sample, length, ALT_UP_AUDIO_LEFT);
				alt_up_audio_write_fifo(audio, sample, length, ALT_UP_AUDIO_RIGHT);
				currentSample+=FIFO_SIZE;
			}
		}
	}

	alt_up_sd_card_fclose(fileHandle);
	alt_up_audio_reset_audio_core(audio);
	return 0;
}
