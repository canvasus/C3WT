#pragma once

#define PATCH_NAME_NR_CHARACTERS 11

#define ARBITRARY_LENGTH 8193

#define NR_PATCHCORDS_VOICE 90
#define NR_PATCHCORDS_VOICEBANK 60

#define NR_VOICES 8

#define POLY_FULL 1
#define POLY_2X   2
#define POLY_4X   4
#define POLY_MONO 8

#define FILTER_MAX_CUTOFF 13600
#define FILTER_OCTAVECONTROL 7

#define NR_WAVESHAPE_MODELS 4

// Next parameter index: 123
#define POLY_MODE 106
#define MONO_MODE 114

#define OSC1_WAVEFORM 0
#define OSC2_WAVEFORM 1
#define TRANSPOSE     2
#define DETUNE        3
#define OSC1_LEVEL    4
#define OSC2_LEVEL    5
#define PULSE_LEVEL   6
#define NOISE_LEVEL   7
#define ENV_ATTACK    8
#define ENV_DECAY     9
#define ENV_SUSTAIN   10
#define ENV_RELEASE   11
#define FILTER_ATTACK 12
#define FILTER_DECAY  13
#define FILTER_SUSTAIN 14
#define FILTER_RELEASE 15
#define FILTER_CUTOFF  16
#define FILTER_RESONANCE 17
#define FILTERENV_PWR 18

#define HPFILTER_CUTOFF 117

#define OSC1_SYNC 112
#define OSC2_SYNC 113

#define OSC1_WAVETABLE_INDEX    102
#define OSC1_WAVETABLE_MODE     104
#define OSC1_WAVETABLE_START    94
#define OSC1_WAVETABLE_LENGTH   95
#define OSC1_WAVETABLE_INTERVAL 96
#define OSC1_WAVETABLE_STEPSIZE 97
#define OSC1_WAVETABLE_MOVEMENT 115

#define OSC2_WAVETABLE_INDEX    103
#define OSC2_WAVETABLE_MODE     105
#define OSC2_WAVETABLE_START    98
#define OSC2_WAVETABLE_LENGTH   99
#define OSC2_WAVETABLE_INTERVAL 100
#define OSC2_WAVETABLE_STEPSIZE 101
#define OSC2_WAVETABLE_MOVEMENT 116

#define ENV3_ATTACK  108
#define ENV3_DECAY   109
#define ENV3_SUSTAIN 110
#define ENV3_RELEASE 111

#define MOD_ENV3_OSC1_PITCH   30
#define MOD_ENV3_OSC2_PITCH   31
#define MOD_ENV3_OSC1_PHASE   32
#define MOD_ENV3_OSC2_PHASE   33
#define MOD_ENV3_FILTER_CUTOFF  34
#define MOD_ENV3_PWM          35
#define MOD_ENV3_AM_PITCH     86
#define MOD_ENV3_FM_PITCH     87
#define MOD_ENV3_LFO1_AMPLITUDE 92
#define MOD_ENV3_LFO2_AMPLITUDE 93

#define MOD_LFO1_OSC1_PITCH   36
#define MOD_LFO1_OSC2_PITCH   37
#define MOD_LFO1_OSC1_PHASE   38
#define MOD_LFO1_OSC2_PHASE   39
#define MOD_LFO1_FILTER_CUTOFF 40
#define MOD_LFO1_PWM          41
#define MOD_LFO1_AM_PITCH     88
#define MOD_LFO1_FM_PITCH     89

#define MOD_LFO2_OSC1_PITCH   42
#define MOD_LFO2_OSC2_PITCH   43
#define MOD_LFO2_OSC1_PHASE   44
#define MOD_LFO2_OSC2_PHASE   45
#define MOD_LFO2_FILTER_CUTOFF 46
#define MOD_LFO2_PWM          47
#define MOD_LFO2_AM_PITCH     90
#define MOD_LFO2_FM_PITCH     91

#define MOD_VEL_OSC1_PITCH   48
#define MOD_VEL_OSC2_PITCH   49
#define MOD_VEL_OSC1_PHASE   50
#define MOD_VEL_OSC2_PHASE   51
#define MOD_VEL_FILTER_CUTOFF 52
#define MOD_VEL_PWM          53

#define MOD_WHL_OSC1_PITCH   54
#define MOD_WHL_OSC2_PITCH   55
#define MOD_WHL_OSC1_PHASE   56
#define MOD_WHL_OSC2_PHASE   57
#define MOD_WHL_FILTER_CUTOFF 58
#define MOD_WHL_PWM           59

#define MOD_OSC1_OSC2_PITCH   60
#define MOD_OSC1_OSC2_PHASE   61
#define MOD_OSC2_OSC1_PITCH   62
#define MOD_OSC2_OSC1_PHASE   63

#define AM_FREQ_MULTIPLIER 64
#define AM_LEVEL           65
#define AM_WAVEFORM        66
#define AM_FIXEDFREQUENCY   107

#define FM_FREQ_MULTIPLIER 67
#define FM_LEVEL           68
#define FM_WAVEFORM        69
#define FM_OFFSET           70

#define EFX_SEND_DRY          71
#define EFX_SEND_REVERB       72
#define EFX_SEND_CHORUS       73
#define EFX_SEND_PHASER       74
#define EFX_SEND_DELAY        75

#define LFO1_WAVEFORM 76
#define LFO2_WAVEFORM 77
#define LFO1_FREQUENCY 78
#define LFO2_FREQUENCY 79
#define LFO1_AMPLITUDE 80
#define LFO2_AMPLITUDE 81
#define LFO1_FILTER_MODE 82
#define LFO2_FILTER_MODE 83
#define LFO1_FILTER_CUTOFF 84
#define LFO2_FILTER_CUTOFF 85
#define LFO1_OFFSET 121
#define LFO2_OFFSET 122

#define OSC1_PAR_A 123
#define OSC1_PAR_B 124
#define OSC2_PAR_A 125
#define OSC2_PAR_B 126

#define VELOCITY_TO_AMP_ENV_AMPL 127
#define VELOCITY_TO_FILTER_ENV_AMPL 128

#define FILTERMIX_LP 129
#define FILTERMIX_BP 130
#define FILTERMIX_HP 131

#define EFX_PAN 118
#define WAVESHAPER_INDEX 119
#define WAVESHAPER_LEVEL 120

#define MODMIXER_ENV3_CH  0
#define MODMIXER_LFO1_CH  1
#define MODMIXER_LFO2_CH  2
#define MODMIXER_OSCx_CH  4

#define LEFT  0
#define RIGHT 1

#define NR_WAVEFORMS 14

#define NR_WAVETABLES 20
#define WAVETABLE_MODE_PHASESCAN  0
#define WAVETABLE_MODE_MORPH      1

#define WAVETABLE_PLAYMODE_UPDOWN     0
#define WAVETABLE_PLAYMODE_ONESHOT_UP 1
#define WAVETABLE_PLAYMODE_LFO1       2
#define WAVETABLE_PLAYMODE_LFO2       3
#define NR_WT_PLAYMODES 4

#define WAVETABLE_LENGTH 8192

#define NR_ARP_SEQUENCER_STEPS 16

struct Patch
{
  uint8_t osc1_waveform = 1;
  uint8_t osc2_waveform = 1;

  int8_t   transpose = 0;
  float detune = 0.20;
  uint8_t mono_mode = 0;
  uint8_t polyMode = POLY_FULL;

  uint8_t osc1_sync = 0;
  uint8_t osc2_sync = 0;

  uint16_t osc1_parA = 0;
  uint16_t osc1_parB = 0;
  uint16_t osc2_parA = 0;
  uint16_t osc2_parB = 0;

  float osc1_level = 0.8;
  float osc2_level = 0.0;
  float pulse_level = 0.0;
  float noise_level = 0.0;
  
  float phaseModulation = 180;
  uint8_t frequencyModulation = 4;

  float ampEnvelope_attack = 0.0;
  float ampEnvelope_decay = 200;
  float ampEnvelope_sustain = 0.9;
  float ampEnvelope_release = 0.0;

  float filterEnvelope_attack = 0;
  float filterEnvelope_decay = 200;
  float filterEnvelope_sustain = 0.9;
  float filterEnvelope_release = 500.0;
  float octaveControl = 7.0;
  float envToFilter = 0.0;

  float cutoff = 1.0;
  float resonance = 0.0;
  float hpfilter_cutoff = 0.0;

  float filterMix_LP = 1.0;
  float filterMix_BP = 0.0;
  float filterMix_HP = 0.0;

  float velocityToLevel = 0.0;
  float velocityToFilterEnvelope = 0.0;

  float envelope3_attack = 0.0;
  float envelope3_decay = 200;
  float envelope3_sustain = 0.9;
  float envelope3_release = 0.0;

  float mod_env3_osc1_pitch = 0.0;
  float mod_env3_osc2_pitch = 0.0;
  float mod_env3_osc1_phase = 0.0;
  float mod_env3_osc2_phase = 0.0;
  float mod_env3_filter_cutoff = 0.0;
  float mod_env3_pwm = 0.0;
  float mod_env3_am_pitch = 0.0;
  float mod_env3_fm_pitch = 0.0;
  
  float mod_env3_lfo1_amplitude = 0.0;
  float mod_env3_lfo2_amplitude = 0.0;

  float mod_lfo1_osc1_pitch = 0.0;
  float mod_lfo1_osc2_pitch = 0.0;
  float mod_lfo1_osc1_phase = 0.0;
  float mod_lfo1_osc2_phase = 0.0;
  float mod_lfo1_filter_cutoff = 0.0;
  float mod_lfo1_pwm = 0.0;
  float mod_lfo1_am_pitch = 0.0;
  float mod_lfo1_fm_pitch = 0.0;

  float mod_lfo2_osc1_pitch = 0.0;
  float mod_lfo2_osc2_pitch = 0.0;
  float mod_lfo2_osc1_phase = 0.0;
  float mod_lfo2_osc2_phase = 0.0;
  float mod_lfo2_filter_cutoff = 0.0;
  float mod_lfo2_pwm = 0.0;
  float mod_lfo2_am_pitch = 0.0;
  float mod_lfo2_fm_pitch = 0.0;

  float mod_osc1_osc2_pitch = 0.0;
  float mod_osc2_osc1_pitch = 0.0;
  float mod_osc1_osc2_phase = 0.0;
  float mod_osc2_osc1_phase = 0.0;
 
  float mod_velocity_osc1_pitch = 0.0;
  float mod_velocity_osc2_pitch = 0.0;
  float mod_velocity_osc1_phase = 0.0;
  float mod_velocity_osc2_phase = 0.0;
  float mod_velocity_filter_cutoff = 0.0;
  float mod_velocity_pwm = 0.0;

  float mod_wheel_osc1_pitch = 0.0;
  float mod_wheel_osc2_pitch = 0.0;
  float mod_wheel_osc1_phase = 0.0;
  float mod_wheel_osc2_phase = 0.0;
  float mod_wheel_filter_cutoff = 0.0;
  float mod_wheel_pwm = 0.0;

  float lfo1Frequency = 0.4;
  float lfo2Frequency = 0.4;
  uint8_t lfo1_waveform = 0;
  uint8_t lfo2_waveform = 0;
  float lfo1Level = 0.0;
  float lfo2Level = 0.0;
  float lfo1Offset = 0.0;
  float lfo2Offset = 0.0;
  uint8_t lfo1FilterMode = 0;
  uint8_t lfo2FilterMode = 0;
  float lfo1FilterCutoff = FILTER_MAX_CUTOFF;
  float lfo2FilterCutoff = FILTER_MAX_CUTOFF;

  float fm_frequency_multiplier = 2;
  float fm_level = 0.0;
  float fm_offset = 0.0;
  uint8_t osc_fm_waveform = 0;

  float am_frequency_multiplier = 2;
  float am_level = 0.0;
  uint8_t osc_am_waveform = 0;
  uint8_t am_fixedFrequency = 0;

  uint8_t waveshaper_index = 0;
  float waveshaper_level = 0.0;

  float dryLevel = 0.8;
  float pan = 0.0; // center
  float reverbSend = 0.0;
  float chorusSend = 0.0;
  float delaySend = 0.0;
  float phaserSend = 0.0;
  
  uint8_t osc1_waveTable_mode = 0;
  uint8_t osc1_waveTable_index = 0;
  uint16_t osc1_waveTable_start = 0;
  uint16_t osc1_waveTable_length = 255;
  uint16_t osc1_waveTable_interval = 10;
  uint16_t osc1_waveTable_stepSize = 1;
  uint8_t osc1_waveTable_movement = WAVETABLE_PLAYMODE_UPDOWN;
  
  uint8_t osc2_waveTable_mode = 0;
  uint8_t osc2_waveTable_index = 0;
  uint16_t osc2_waveTable_start = 0;
  uint16_t osc2_waveTable_length = 255;
  uint16_t osc2_waveTable_interval = 10;
  uint16_t osc2_waveTable_stepSize = 1;
  uint8_t osc2_waveTable_movement = WAVETABLE_PLAYMODE_UPDOWN;

  uint8_t midi_channel = 1;
  uint8_t midi_lowLimit = 0;
  uint8_t midi_highLimit = 127;
  int8_t midi_transpose = 0;
  
  uint8_t arp_mode = 0;
  uint8_t arp_intervalTicks = 24;
  uint8_t arp_offsetTicks = 0;
  uint8_t arp_octaves = 0;
  uint16_t arp_noteLengthMs = 120;
  uint8_t arp_keyTrack = 0;
  int8_t  arp_offsets[NR_ARP_SEQUENCER_STEPS] = {0,0,12,0,0,12,0,0,3,0,0,3,0,0,0,0};
  uint8_t  arp_velocities[NR_ARP_SEQUENCER_STEPS] = {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};

  char name[PATCH_NAME_NR_CHARACTERS] = {'I', 'N', 'I', 'T', ' ', 'P', 'A', 'T', 'C', 'H'};
};



#define wraparound(amt, low, high)\
  ({\
    typeof(amt) _amt = (amt);\
    typeof(low) _low = (low);\
    typeof(high) _high = (high);\
    (_amt < _low) ? _high : ((_amt > _high) ? _low : _amt);\
     })


