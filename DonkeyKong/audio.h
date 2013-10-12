/*
 * level1.h
 *
 *  Created on: Sep 24, 2013
 *      Author: Christopher (Viewtiful) Tan
 */

#ifndef AUDIO_H_
#define AUDIO_H_
#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_sd_card_avalon_interface.h"
#include "sdcard.h"
#include "sys/alt_irq.h"

#define FIFO_SIZE 112
#define AUDIO_ERROR -1


int checkInitSD (alt_up_sd_card_dev *device_reference);

int initAVConfig(alt_up_av_config_dev* av_config);

int initAudioCore(alt_up_audio_dev* audio);

int findWavSize(file_handle fileHandle);

unsigned int reduceVolume(unsigned int buffer);

int set_audio_interrupt(alt_up_audio_dev *audio, volatile int edge_pointer_thing);

int playBlockingMusic(char* audioFile);

void resetAudio(void);

void initAudio(void);

int loadMusic(char* audioFile, unsigned short loop);

unsigned char isMusicDone(void);

#endif /* AUDIO_H_ */
