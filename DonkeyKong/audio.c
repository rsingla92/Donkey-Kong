/*
 * audio.c
 *
 *  Created on: 2013-10-03
 *      Author: Christopher (El Magnifico) Tan
 */
#include <stdlib.h>
#include <stdio.h>
#include "audio.h"
#include "system.h"

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
static void playMusicISR (void* context);
#else
static void playMusicISR (void* context, alt_u32 id);
#endif

static unsigned int* interruptMusicBuffer = 0;
static unsigned int interruptBufSize = 0;
static unsigned char musicLoop = 0;
static unsigned char musicDone = 0;
static unsigned int interruptSample = 0;
volatile int context = 0;

static alt_up_av_config_dev* av_config;
static alt_up_audio_dev* audio;

int initAVConfig(alt_up_av_config_dev* av_config) {

	if (av_config == NULL) {
		return AUDIO_ERROR;
	}

	alt_up_av_config_reset(av_config);

	/*
	if (alt_up_av_config_enable_interrupt(av_config) == 0) {
		printf("Config interrupt enabled\n");
	}
	*/

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
int findWavSize(file_handle fileHandle)
{
	int i = 0;
	int fileLength = 0;
	short int cardRead = 0;

	//Read the file and obtain the length of a .wav file
	unsigned char tempBuf[8];
	for (i = 0; i < 8; i++) {
		cardRead = read_file(fileHandle);
		tempBuf[i] = cardRead;
	}

	//Do shifting math to get .wav file length
	fileLength = (tempBuf[7] << 24) | (tempBuf[6] << 16) | (tempBuf[5] << 8) | tempBuf[4];
//	fileLength /= 2;

	return fileLength;
}

//Reduces the volume by halfing the sample.
unsigned int reduceVolume(unsigned int buffer) {
	return (buffer/2);
}

void pauseMusic(void)
{
	// Stops writing to the audio buffer.
	alt_up_audio_disable_write_interrupt(audio);
}

void resumeMusic(void)
{
	alt_up_audio_enable_write_interrupt(audio);
}

void restartMusic(void)
{
	interruptSample = 0;
	alt_up_audio_enable_write_interrupt(audio);
}

unsigned char isMusicDone(void)
{
	return musicDone;
}

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
static void playMusicISR (void* context)
#else
static void playMusicISR (void* context, alt_u32 id)
#endif
{
	/* Interrupt for writing*/
	unsigned int space = alt_up_audio_write_fifo_space(audio, ALT_UP_AUDIO_RIGHT);
	unsigned int* sample;

	if (space > interruptBufSize - interruptSample)
	{
		// Don't need to fully fill the rest of the buffer.
		space = interruptBufSize - interruptSample;
	}

	if (space > 0)
	{
		sample = &(interruptMusicBuffer[interruptSample]);
		alt_up_audio_write_fifo(audio, sample, space, ALT_UP_AUDIO_LEFT);
		alt_up_audio_write_fifo(audio, sample, space, ALT_UP_AUDIO_RIGHT);
		interruptSample += space;
	}

	if (interruptSample >= interruptBufSize)
	{
		if (musicLoop)
		{
			interruptSample = 0;
		}
		else
		{
			musicDone = 1;
			alt_up_audio_disable_write_interrupt(audio);
		}
	}
}

int set_audio_interrupt(alt_up_audio_dev *audio, volatile int edge_capture_thing)
{
    // Need to disable both audio interrupts before setting them up
    // otherwise you get stuck in them when they are setup
    alt_up_audio_disable_read_interrupt(audio);
    alt_up_audio_disable_write_interrupt(audio);

    void *edge_pointer = (void*)&edge_capture_thing;
	#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
	return alt_ic_isr_register(AUDIO_CORE_IRQ_INTERRUPT_CONTROLLER_ID,
			AUDIO_CORE_IRQ, playMusicISR, edge_pointer, 0x0);
	#else
	return alt_irq_register(AUDIO_CORE_IRQ, edge_pointer, playMusicISR);
	#endif
}

void initAudio(void)
{
	//Set up audio and sd parts
	av_config = alt_up_av_config_open_dev("/dev/audio_and_video_config");
	audio = alt_up_audio_open_dev("/dev/audio_core");

	if (initAVConfig(av_config)== -1) {
		printf("AV Failed \n");
		return AUDIO_ERROR;
	}

	if (initAudioCore(audio)== -1) {
		printf("Audio Failed \n");
		return AUDIO_ERROR;
	}

	set_audio_interrupt(audio, context);
}

void resetAudio(void)
{
	alt_up_audio_reset_audio_core(audio);
}

int loadMusic(char* audioFile, unsigned short loop)
{
	int i = 0;
	unsigned int *sample;

	alt_up_audio_disable_write_interrupt(audio);

	file_handle fileHandle = open_file(audioFile, false);

	if (fileHandle < 0) {
		printf("Reading file failed \n");
		return AUDIO_ERROR;
	}

	int fileLength = findWavSize(fileHandle);

	// Allocate the main music buffer to be the size of the file.
	if (interruptMusicBuffer != 0) free(interruptMusicBuffer);
	musicLoop = loop;
	musicDone = 0;
	interruptMusicBuffer = (unsigned int*) malloc(fileLength * 2);
	interruptBufSize = fileLength/2;
	interruptSample = 0;

	for (i = 0; i < interruptBufSize; i++)
	{
		// Extract data and store in the buf.
		unsigned char firstByte = read_file(fileHandle);
		unsigned char secondByte = read_file(fileHandle);
		unsigned short val = (secondByte << 8) | firstByte;
		interruptMusicBuffer[i] = val;
	}

	alt_up_sd_card_fclose(fileHandle);

	unsigned int space = alt_up_audio_write_fifo_space(audio, ALT_UP_AUDIO_RIGHT);

	if (space > interruptBufSize - interruptSample)
	{
		// Don't need to fully fill the rest of the buffer.
		space = interruptBufSize - interruptSample;
	}

	if (space > 0)
	{
		sample = &(interruptMusicBuffer[interruptSample]);
		alt_up_audio_write_fifo(audio, sample, space, ALT_UP_AUDIO_LEFT);
		alt_up_audio_write_fifo(audio, sample, space, ALT_UP_AUDIO_RIGHT);
		interruptSample += space;
	}

	if (interruptSample >= interruptBufSize)
	{
		if (musicLoop)
		{
			interruptSample = 0;
			alt_up_audio_enable_write_interrupt(audio);
		}
	}
	else
	{
		printf("Enabling interrupt.");
		// Enable the write interrupt
		alt_up_audio_enable_write_interrupt(audio);
	}

	return 0;
}

// Requires:
// Effects: Plays an audio .wav file, blocking the CPU until the song has completed.
int playBlockingMusic(char* audioFile)
{
	int i = 0;
	int currentSample = 0;
	unsigned int *sample;

	file_handle fileHandle = open_file(audioFile, false);

	if (fileHandle < 0) {
		printf("Reading file failed \n");
		return AUDIO_ERROR;
	}

	int fileLength = findWavSize(fileHandle);

	// Allocate a buffer that is the same byte size as the file
	unsigned int *buf = (unsigned int*) malloc(fileLength * 2);

	int bufSize = fileLength/2;

	for (i = 0; i < bufSize; i++)
	{
		// Extract data and store in the buf.
		unsigned char firstByte = read_file(fileHandle);
		unsigned char secondByte = read_file(fileHandle);
		unsigned short val = (secondByte << 8) | firstByte;
		buf[i] = val;
	}

	// Close the file on the sd card.
	alt_up_sd_card_fclose(fileHandle);

	while (currentSample < bufSize)
	{
		unsigned int space = alt_up_audio_write_fifo_space(audio, ALT_UP_AUDIO_RIGHT);

		if (space > bufSize - currentSample)
		{
			// Don't need to fully fill the rest of the buffer.
			space = bufSize - currentSample;
		}

		if (space > 0)
		{
			sample = &(buf[currentSample]);
			alt_up_audio_write_fifo(audio, sample, space, ALT_UP_AUDIO_LEFT);
			alt_up_audio_write_fifo(audio, sample, space, ALT_UP_AUDIO_RIGHT);
			currentSample += space;
		}
	}

	free(buf);
	return 0;
}
