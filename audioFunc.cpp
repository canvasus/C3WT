#include "audioFunc.h"

AudioEffectEnsemble     chorus;
AudioEffectPlateReverb  reverb;
AudioEffectPhaser       phaser;
AudioEffectDelay        delayL;
AudioEffectDelay        delayR;

AudioEffectGranular     granularL;
AudioEffectGranular     granularR;

AudioMixer4           chorusInputMixer;
AudioMixer4           reverbInputMixer_L;
AudioMixer4           reverbInputMixer_R;
AudioMixer4           delayInputMixer_L;
AudioMixer4           delayInputMixer_R;

AudioAmplifier        chorusPhaserBoost;

AudioMixer4           fxReturnMixerL;
AudioMixer4           fxReturnMixerR;

AudioMixer4           outputMixerL;
AudioMixer4           outputMixerR;
//AudioOutputUSB        output_usb;
AudioOutputI2S        output_i2s;

AudioAnalyzeFFT256    fft;
AudioAnalyzePeak      peak1;
AudioAnalyzePeak      peak2;

AudioConnection  *   patchCords[NR_PATCHCORDS_MAINBUS];

SM2k_AudioControlWM8731 codecControl1;

VoiceBank voiceBank1;
VoiceBank voiceBank2;

VoiceBank * voiceBanks[2] = {&voiceBank1, &voiceBank2};
uint8_t currentVoiceBank = 0;

AudioParameters audioParameters;

#define GRANULAR_MEMORY_SIZE 8000 
DMAMEM int16_t granularMemoryL[GRANULAR_MEMORY_SIZE];
DMAMEM int16_t granularMemoryR[GRANULAR_MEMORY_SIZE];

float peakLevels[2];

FLASHMEM void setupAudio()
{
  AudioMemory(600);

  codecControl1.setAddress(0x1A);
  codecControl1.enable();
  codecControl1.inputLevel(0.0);
  
  voiceBank1.id = 0;
  voiceBank2.id = 1;
  voiceBank1.configure();
  voiceBank2.configure();

  connect(voiceBank1.output_dry_L, 0, outputMixerL , 0);
  connect(voiceBank1.output_dry_R, 0, outputMixerR , 0);
  connect(voiceBank1.output_reverbSend_L, 0, reverbInputMixer_L , 0);
  connect(voiceBank1.output_reverbSend_R, 0, reverbInputMixer_R , 0);
  connect(voiceBank1.output_chorusSend, 0, chorusInputMixer , 0);
  connect(voiceBank1.output_delaySend, 0, delayInputMixer_L , 0);
  connect(voiceBank1.output_delaySend, 0, delayInputMixer_R , 0);

  connect(voiceBank2.output_dry_L, 0, outputMixerL , 1);
  connect(voiceBank2.output_dry_R, 0, outputMixerR , 1);
  connect(voiceBank2.output_reverbSend_L, 0, reverbInputMixer_L , 1);
  connect(voiceBank2.output_reverbSend_R, 0, reverbInputMixer_R , 1);
  connect(voiceBank2.output_chorusSend, 0, chorusInputMixer , 1);
  connect(voiceBank2.output_delaySend, 0, delayInputMixer_L , 1);
  connect(voiceBank2.output_delaySend, 0, delayInputMixer_R , 1);

  // REVERB ROUTING
  connect(reverbInputMixer_L, 0, reverb , 0);
  connect(reverbInputMixer_R, 0, reverb , 1);
  connect(reverb, 0, fxReturnMixerL , 0);
  connect(reverb, 1, fxReturnMixerR , 0);
  connect(voiceBank1.output_reverbSend_L, 0, granularL , 0);
  connect(voiceBank1.output_reverbSend_R, 0, granularR , 0);
  //connect(reverb, 0, granularL , 0);
  //connect(reverb, 1, granularR , 0);
  connect(granularL, 0, reverbInputMixer_L , 3);
  connect(granularR, 0, reverbInputMixer_R , 3);

  // PHASER + CHORUS ROUTING
  connect(chorusInputMixer, 0, phaser , 0); 
  connect(phaser, 0, chorusPhaserBoost , 0); 
  connect(chorusPhaserBoost, 0, chorus , 0); 
  connect(chorus, 0, fxReturnMixerL, 1);
  connect(chorus, 1, fxReturnMixerR, 1);
  chorusPhaserBoost.gain(1.5);

  // DELAY ROUTING
  connect(delayInputMixer_L, 0, delayL, 0);
  connect(delayInputMixer_R, 0, delayR, 0);
  connect(delayL, 0, delayInputMixer_R , 2); // ping pong L - R
  connect(delayR, 0, delayInputMixer_L , 2); // ping pong R - L
  connect(delayL, 0, delayInputMixer_L , 3); // feedback L 
  connect(delayR, 0, delayInputMixer_R , 3); // feedback R
  connect(delayL, 0, fxReturnMixerL , 3);
  connect(delayR, 0, fxReturnMixerR , 3);

  connect(fxReturnMixerL, 0, outputMixerL, 3);
  connect(fxReturnMixerR, 0, outputMixerR, 3);

  connect(outputMixerL, 0, output_i2s , 0);
  connect(outputMixerR, 0, output_i2s , 1);

  connect(outputMixerL, 0, fft , 0);
  fft.averageTogether(4);

  //connect(voiceBank1.output_dry_R, 0, peak1 , 0);
  //connect(voiceBank2.output_dry_R, 0, peak2 , 0);

  connect(voiceBank1.output_dry_L, 0, peak1 , 0);
  connect(voiceBank2.output_dry_L, 0, peak2 , 0);

  outputMixerL.gain(0, 0.9); // Voicebank 1 DRY L
  outputMixerR.gain(0, 0.9); // Voicebank 1 DRY R
  outputMixerL.gain(1, 0.9); // Voicebank 2 DRY L
  outputMixerR.gain(1, 0.9); // Voicebank 2 DRY R
  outputMixerL.gain(3, 1.0); // FX L
  outputMixerR.gain(3, 1.0); // FX R

  granularL.begin(granularMemoryL, GRANULAR_MEMORY_SIZE);
  granularR.begin(granularMemoryR, GRANULAR_MEMORY_SIZE);
  
  applyAudioParameters();
}

FLASHMEM void connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput)
{
  static uint16_t connectionIndex = 0;
  patchCords[connectionIndex] = new AudioConnection(source, sourceOutput, destination , destinationInput);
  connectionIndex++;
  if (connectionIndex > NR_PATCHCORDS_MAINBUS - 1)
  {
    connectionIndex = NR_PATCHCORDS_MAINBUS - 1;
    Serial.println("ERROR: Audio main bus - no available patchcords");
  }
}

elapsedMillis peakTimer;

void updateVoices()
{
  if (peakTimer > 20)
  {
    peakTimer = 0;
    if (peak1.available()) peakLevels[0] = 0.5 * peakLevels[0] + 0.5 * peak1.read();
    else if (peakLevels[0] > 0) peakLevels[0] = peakLevels[0] - 0.01;
    if (peak2.available()) peakLevels[1] = 0.5 * peakLevels[1] + 0.5 * peak2.read();
    else if (peakLevels[1] > 0) peakLevels[1] = peakLevels[1] - 0.01;
  }

  voiceBank1.update();
  voiceBank2.update();
}

void adjustAudioParameter(uint8_t parameter, int8_t delta)
{
  uint8_t targetValueU8 = 0;
  //uint16_t targetValueU16 = 0;
  float targetValueF = 0.0;

  switch (parameter)
  {
    case REVERB_SIZE:
      targetValueF = audioParameters.reverb_size + delta * 0.05;
      audioParameters.reverb_size = constrain(targetValueF, 0.0, 1.0); 
      setReverb();
      break;
    case REVERB_HIDAMP:
      targetValueF = audioParameters.reverb_hidamp + delta * 0.05;
      audioParameters.reverb_hidamp = constrain(targetValueF, 0.0, 1.0); 
      setReverb();
      break;
    case REVERB_LODAMP:
      targetValueF = audioParameters.reverb_lodamp + delta * 0.05;
      audioParameters.reverb_lodamp = constrain(targetValueF, 0.0, 1.0); 
      setReverb();
      break;
    case REVERB_LOWPASS:
      targetValueF = audioParameters.reverb_lowpass + delta * 0.05;
      audioParameters.reverb_lowpass = constrain(targetValueF, 0.0, 1.0); 
      setReverb();
      break;
    case REVERB_DIFFUSION:
      targetValueF = audioParameters.reverb_diffusion + delta * 0.05;
      audioParameters.reverb_diffusion = constrain(targetValueF, 0.0, 1.0); 
      setReverb();
      break;
    case REVERB_FEEDBACK:
      targetValueF = audioParameters.reverb_feedback + delta * 0.05;
      audioParameters.reverb_feedback = constrain(targetValueF, 0.0, 1.0); 
      setReverbInputMixer();
      break;
    case GRANULAR_SPEED:
      targetValueF = audioParameters.granular_speed + delta * 0.2;
      audioParameters.granular_speed = constrain(targetValueF, 0.2, 4.0); 
      setGranular();
      break;
    case GRANULAR_LENGTH:
      targetValueU8 = audioParameters.granular_length + delta * 10.0;
      audioParameters.granular_length = constrain(targetValueU8, 0.0, 100.0); 
      setGranular();
      break;

    case CHORUS_LFORATE:
      targetValueF = audioParameters.chorus_lfoRate + delta * 0.5;
      audioParameters.chorus_lfoRate = constrain(targetValueF, 0.0, 20.0); 
      setChorus();
      break;

    case PHASER_LFORATE:
      targetValueF = audioParameters.phaser_lfoRate + delta * 0.1;
      audioParameters.phaser_lfoRate = constrain(targetValueF, 0, 5); 
      setPhaser();
      break;
    case PHASER_STAGES:
      targetValueU8 = audioParameters.phaser_stages + delta * 2;
      targetValueU8 = constrain(targetValueU8, 2, 12);
      audioParameters.phaser_stages = targetValueU8;
      setPhaser();
      break;
    case PHASER_FEEDBACK:
      targetValueF = audioParameters.phaser_feedback + delta * 0.1;
      audioParameters.phaser_feedback = constrain(targetValueF, 0, 1); 
      setPhaser();
      break;
    case PHASER_MIX:
      targetValueF = audioParameters.phaser_mix + delta * 0.1;
      audioParameters.phaser_mix = constrain(targetValueF, 0, 1); 
      setPhaser();
      break;
    case PHASER_DEPTH:
      targetValueF = audioParameters.phaser_depth + delta * 0.1;
      audioParameters.phaser_depth = constrain(targetValueF, 0, 1); 
      setPhaser();
      break;

    case DELAY_TYPE:
      targetValueU8 = audioParameters.delay_type + delta;
      audioParameters.delay_type = constrain(targetValueU8, 0, 1);
      setDelay();
      break;
    case DELAY_TIME:
      targetValueF = audioParameters.delay_time + delta * 20;
      audioParameters.delay_time = constrain(targetValueF, 20, 400);
      setDelay();
      break;
    case DELAY_FEEDBACK:
      targetValueF = audioParameters.delay_feedback + delta * 0.05;
      audioParameters.delay_feedback = constrain(targetValueF, 0, 2.0);
      setDelay();
      break;

    default:
      break;
  }
}

void applyAudioParameters()
{
  setReverb();
  setGranular();
  setChorus();
  setPhaser();
  setDelay();
  setReverbInputMixer();
}

void setReverb()
{
  reverb.size(audioParameters.reverb_size );
  reverb.diffusion(audioParameters.reverb_diffusion);
  reverb.hidamp(audioParameters.reverb_hidamp);
  reverb.lodamp(audioParameters.reverb_lodamp);
}

void setReverbInputMixer()
{
  reverbInputMixer_L.gain(0, 1.0); // VoiceBank1
  reverbInputMixer_L.gain(1, 1.0); // VoiceBank2
  reverbInputMixer_L.gain(2, 0.0);
  reverbInputMixer_L.gain(3, audioParameters.reverb_feedback);
  
  reverbInputMixer_R.gain(0, 1.0); // VoiceBank1
  reverbInputMixer_R.gain(1, 1.0); // VoiceBank2
  reverbInputMixer_R.gain(2, 0.0);
  reverbInputMixer_R.gain(3, audioParameters.reverb_feedback);
}

void setGranular()
{
  granularL.setSpeed(audioParameters.granular_speed);
  granularR.setSpeed(audioParameters.granular_speed);
  granularL.beginPitchShift(audioParameters.granular_length);
  granularR.beginPitchShift(audioParameters.granular_length);
}

void setChorus()
{
  chorus.lfoRate(audioParameters.chorus_lfoRate);
}

void setPhaser()
{
  if (audioParameters.phaser_mix < 0.1) phaser.bypass(true);
  else phaser.bypass(false);
  phaser.feedback(audioParameters.phaser_feedback);
  phaser.depth(0.5 - audioParameters.phaser_depth, 0.5 + audioParameters.phaser_depth);
  phaser.lfo_rate(audioParameters.phaser_lfoRate);
  phaser.mix(audioParameters.phaser_mix);
  phaser.stages(audioParameters.phaser_stages);
  
}

void setDelay()
{
  if (audioParameters.delay_type == DELAY_NORMAL)
  {
    delayL.delay(0, audioParameters.delay_time);
    delayR.delay(0, audioParameters.delay_time - 1);
    delayInputMixer_L.gain(0, 1.0); //direct input
    delayInputMixer_R.gain(0, 1.0);
    delayInputMixer_L.gain(2, 0.0); // ping pong
    delayInputMixer_R.gain(2, 0.0);
    delayInputMixer_L.gain(3, audioParameters.delay_feedback);
    delayInputMixer_R.gain(3, audioParameters.delay_feedback);
  }

  if (audioParameters.delay_type == DELAY_PINGPONG)
  {
    delayL.delay(0, audioParameters.delay_time);
    delayR.delay(0, audioParameters.delay_time);
    delayInputMixer_L.gain(1, 1.0); //direct input
    delayInputMixer_R.gain(0, 0.0);
    delayInputMixer_L.gain(2, audioParameters.delay_feedback);
    delayInputMixer_R.gain(2, audioParameters.delay_feedback);
    delayInputMixer_L.gain(3, 0.0);
    delayInputMixer_R.gain(3, 0.0);
  }
}