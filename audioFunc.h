#pragma once

#include <Audio.h>
#include "src/effect_platervbstereo.h"
#include "src/effect_ensemble.h"
#include "src/effect_phaser.h"
#include "src/SM2k_control_wm8731.h"
#include "voice.h"

#define NR_PATCHCORDS_MAINBUS 64

#define REVERB_SIZE   10
#define REVERB_HIDAMP 11
#define REVERB_LODAMP 12
#define REVERB_LOWPASS 13
#define REVERB_DIFFUSION 14
#define REVERB_FEEDBACK 15

#define GRANULAR_SPEED 16
#define GRANULAR_LENGTH 17

#define CHORUS_LFORATE 20

#define PHASER_LFORATE  30
#define PHASER_STAGES   31
#define PHASER_FEEDBACK 32
#define PHASER_MIX      33
#define PHASER_DEPTH    34

#define DELAY_TYPE      35
#define DELAY_TIME      36
#define DELAY_FEEDBACK  37
#define DELAY_NORMAL 0
#define DELAY_PINGPONG 1

#define NR_VOICEBANKS 2
extern VoiceBank voiceBank1;
extern VoiceBank voiceBank2;
extern VoiceBank * voiceBanks[2];
extern uint8_t currentVoiceBank;
extern float mainLoopTime;

struct AudioParameters
{
  float hpVolume = 0.70;
  float usbGain = 1.5;
 
  float reverb_size = 0.75;
  float reverb_hidamp = 0.50;
  float reverb_lodamp = 0.10;
  float reverb_lowpass = 0.40;
  float reverb_diffusion = 0.65;
  float reverb_feedback = 0.0;

  float granular_speed = 2.0;
  uint8_t granular_length = 90;

  float chorus_lfoRate = 6.0;
  
  float phaser_lfoRate = 1.6;
  uint8_t phaser_stages = 0;
  float phaser_feedback = 0.0;
  float phaser_mix = 0.0;
  float phaser_depth = 0.4;
  
  float delay_feedback = 0;
  float delay_time = 100;
  uint8_t delay_type = DELAY_NORMAL;

  float reverb_masterLevel = 1.0;
  float chorus_masterLevel = 1.0;
  float delay_masterLevel = 1.0;
  float phaser_masterLevel = 1.0;

};

extern AudioParameters audioParameters;
extern AudioAnalyzeFFT256 fft;
extern float peakLevels[2];

void setupAudio();
void connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput);
void adjustAudioParameter(uint8_t parameter, int8_t delta);
void applyAudioParameters();
void updateVoices();

void setReverb();
void setReverbInputMixer();
void setChorus();
void setPhaser();
void setDelay();
void setGranular();