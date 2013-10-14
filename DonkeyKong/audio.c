/*
 * audio.c
 *
 *  Created on: 2013-10-03
 *      Author: Christopher (El Magnifico) Tan
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "audio.h"
#include "system.h"

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
static void playMusicISR (void* context);
#else
static void playMusicISR (void* context, alt_u32 id);
#endif

// The main music Buffer
static int* interruptMusicBuffer = 0;
static unsigned int interruptBufSize = 0;

// The additive sound buffer:
// Currently we only support adding one sound.
static int* soundBuffer = 0;
static unsigned int soundBufSize = 0;
static unsigned int soundBufferSample = 0;
static unsigned char addSound = 0;

// The music swap buffer (can completely swap music buffers).
// This is more efficient than loading new music on demand. Instead,
// we simply do a pointer assignment.
static int* musicSwapBuffer = 0;
static unsigned int bufSizeSwap = 0;
static unsigned char musicLoop = 0;
static unsigned char swapLoop = 0;
static unsigned char musicDone = 0;
static unsigned char swapDone = 0;
static unsigned int interruptSample = 0;
static unsigned int swapSample = 0;
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

	return fileLength;
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
	int i;

	if (space > interruptBufSize - interruptSample)
	{
		// Don't need to fully fill the rest of the buffer.
		space = interruptBufSize - interruptSample;
	}

	if (space > 0)
	{
		if (addSound)
		{
			/* Add a sound in-- must add word by word. */
			for (i = 0; i < space; i++)
			{
				int currentWord = interruptMusicBuffer[interruptSample++];
				if (soundBufferSample < soundBufSize)
				{
					currentWord += soundBuffer[soundBufferSample++];
				}
				else
				{
					removeSound();
				}

				alt_up_audio_write_fifo(audio, &currentWord, 1, ALT_UP_AUDIO_LEFT);
				alt_up_audio_write_fifo(audio, &currentWord, 1, ALT_UP_AUDIO_RIGHT);
			}
		}
		else
		{
			sample = &(interruptMusicBuffer[interruptSample]);

			alt_up_audio_write_fifo(audio, sample, space, ALT_UP_AUDIO_LEFT);
			alt_up_audio_write_fifo(audio, sample, space, ALT_UP_AUDIO_RIGHT);
			interruptSample += space;
		}
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

			if (bufSizeSwap != 0)
			{
				swapOutSound();
			}
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

void swapOutSound(void)
{
	interruptMusicBuffer = musicSwapBuffer;
	interruptBufSize = bufSizeSwap;
	musicLoop = swapLoop;
	interruptSample = swapSample;
	musicDone = swapDone;

	swapSample = 0;
	swapLoop = 0;
	bufSizeSwap = 0;
	musicSwapBuffer = 0;
	swapDone = 0;

	if (!musicDone)
	{
		alt_up_audio_enable_write_interrupt(audio);
	}
}

void addInSound(int* buf, int len)
{
	addSound = 1;
	soundBuffer = buf;
	soundBufSize = len;
	soundBufferSample = 0;
}

void removeSound(void)
{
	addSound = 0;
	soundBuffer = 0;
	soundBufSize = 0;
	soundBufferSample = 0;
}

void swapInSound(int* buf, int len)
{
	if (bufSizeSwap == 0)
	{
		musicSwapBuffer = interruptMusicBuffer;
		bufSizeSwap = interruptBufSize;
		swapSample = interruptSample;
		swapLoop = musicLoop;
		swapDone = musicDone;

		interruptMusicBuffer = buf;
		interruptBufSize = len;
		interruptSample = 0;
		musicLoop = 0;
		musicDone = 0;

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
				alt_up_audio_enable_write_interrupt(audio);
			}
		}
		else
		{
			printf("Enabling interrupt.");
			// Enable the write interrupt
			alt_up_audio_enable_write_interrupt(audio);
		}
	}
}

// Note: The user is responsible for freeing buffer allocated in loadSound.
// filename - The name of the sound file in the SD card.
// buf - A pointer to the newly allocated buffer.
// len - Returns the length of the newly allocated memory (in words)
int loadSound(char* audioFile, int** buf, float audioVolume)
{
	int i;
	file_handle fileHandle = open_file(audioFile, false);

	if (fileHandle < 0) {
		printf("Reading file failed \n");
		return AUDIO_ERROR;
	}

	int fileLength = findWavSize(fileHandle);

	// Allocate a buffer that is the same byte size as the file
	*buf = (unsigned int*) malloc((fileLength-32) * 2);

	int bufSize = (fileLength-32)/2;

	if (audioVolume <= 0) audioVolume = 1.0;

	for (i = 0; i < bufSize; i++)
	{
		// Extract data and store in the buf.
		char firstByte = read_file(fileHandle);
		char secondByte = read_file(fileHandle);
		short val = ( (unsigned char) secondByte << 8) | (unsigned char) firstByte;
		val = val * audioVolume;
		(*buf)[i] = val;
	}

	// Close the file on the sd card.
	alt_up_sd_card_fclose(fileHandle);

	return bufSize;
}

void setSoundVolume(int* buf, int bufSize, float factor)
{
	int i;

	for (i = 0; i < bufSize; i++)
	{
		short val = buf[i];
		val = val * factor;
		buf[i] = val;
	}
}

// Note: You should have the interrupt disabled before calling.
void setMusicVolume(float factor)
{
	int i;

	for (i = 0; i < interruptBufSize; i++)
	{
		// Extract data and store in the buf.
		short val = interruptMusicBuffer[i];
		val = val * factor;
		interruptMusicBuffer[i] = val;
	}
}

int loadMusic(char* audioFile, unsigned short loop, float volumeFactor)
{
	int i = 0;
	unsigned int *sample;

	alt_up_audio_disable_write_interrupt(audio);

	printf("Opening file\n");

	file_handle fileHandle = open_file(audioFile, false);

	if (fileHandle < 0) {
		printf("Reading file failed \n");
		return AUDIO_ERROR;
	}

	int fileLength = findWavSize(fileHandle);

	// Discard header-- we are making an assumption about
	// how the data is stored to make it easier to
	// add sound to the music.
	for (i = 0; i < 32; i++) read_file(fileHandle);

	// Allocate the main music buffer to be the size of the file.
	if (interruptMusicBuffer != 0) free(interruptMusicBuffer);
	musicLoop = loop;
	musicDone = 0;
	interruptMusicBuffer = (int*) malloc((fileLength-32) * 2);

	interruptBufSize = (fileLength-32)/2;
	interruptSample = 0;

	if (volumeFactor <= 0) volumeFactor = 1;

	for (i = 0; i < interruptBufSize; i++)
	{
		// Extract data and store in the buf.
		char firstByte = read_file(fileHandle);
		char secondByte = read_file(fileHandle);
		short val = ( (unsigned char) secondByte << 8) | (unsigned char) firstByte;

		val = val * volumeFactor;
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
