#pragma once

#include <Audio.h>
#include "src/P3_synth_waveform.h"

// note: imported waveTables assumed to be 8193 samples, use wav2sketch 44100Hz/PCM

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

// 117
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

#define MODMIXER_ENV3_CH  0
#define MODMIXER_LFO1_CH  1
#define MODMIXER_LFO2_CH  2
#define MODMIXER_OSCx_CH  4

#define LEFT  0
#define RIGHT 1

#define NR_WAVETABLES 18
#define WAVETABLE_MODE_PHASESCAN  0
#define WAVETABLE_MODE_MORPH      1

#define WAVETABLE_PLAYMODE_UPDOWN     0
#define WAVETABLE_PLAYMODE_ONESHOT_UP 1

#define WAVETABLE_LENGTH 2 * (ARBITRARY_LENGTH - 1)

const uint8_t MAX_WAVEFORM_INDEX = 3;

extern int16_t waveTable1_I16[WAVETABLE_LENGTH];
extern int16_t waveTable2_I16[WAVETABLE_LENGTH];

extern const String waveTableNames[NR_WAVETABLES];

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

  
};

class Voice
{
  private:
    P3_AudioSynthWaveformModulated    _osc1;
    P3_AudioSynthWaveformModulated    _osc2;
    AudioSynthWaveformPWM             _pwm;
    AudioSynthNoiseWhite              _noise;
    AudioMixer4                       _oscMixer;
    AudioEffectEnvelope               _ampEnvelope;
    AudioEffectEnvelope               _filterEnvelope;
    AudioEffectEnvelope               _envelope3;

    AudioSynthWaveformDc              _envelopeDC;
    AudioFilterStateVariable          _filter;
    AudioMixer4                       _filterModMixer;

    AudioMixer4 _modMixer_osc1_pitch;
    AudioMixer4 _modMixer_osc2_pitch;
    AudioMixer4 _modMixer_osc1_pitch_voiceInternal;
    AudioMixer4 _modMixer_osc2_pitch_voiceInternal;
    AudioMixer4 _modMixer_osc1_phase;
    AudioMixer4 _modMixer_osc2_phase;
    AudioMixer4 _modMixer_am_pitch;
    AudioMixer4 _modMixer_fm_pitch;
    AudioMixer4 _modMixer_pwm;

    // AM and FM

    AudioMixer4                   _amplitudeModulationMixer;
    AudioEffectMultiply           _amplitudeModulation;
    P3_AudioSynthWaveformModulated _osc_AM;
    P3_AudioSynthWaveformModulated _osc_FM;

    AudioMixer4                   _lfo1_amplitudeModulationMixer;
    AudioEffectMultiply           _lfo1_amplitudeModulation;

    AudioMixer4                   _lfo2_amplitudeModulationMixer;
    AudioEffectMultiply           _lfo2_amplitudeModulation;
  
    // Wavetables

    AudioConnection  *   _patchCords[NR_PATCHCORDS_VOICE];
    uint16_t _connectionIndex = 0;
    Patch * _patch;

    elapsedMillis osc1_waveTimer;
    elapsedMillis osc2_waveTimer;
    uint16_t _waveOffset1 = 0;
    uint16_t _waveOffset2 = 0;
    uint8_t _scanDirection1 = RIGHT;
    uint8_t _scanDirection2 = RIGHT;
    void _updateWaveTable1_phaseScan();
    void _updateWaveTable2_phaseScan();
    void _updateWaveTable1_morph();
    void _updateWaveTable2_morph();
   
    void _connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput);
    
  public:
    Voice();
    uint8_t voiceId;
    uint8_t currentNote;
    float unisonDetune = 0.0;
    float pitchBend = 0.0;

    AudioAmplifier  output;
    AudioAmplifier  mod_lfo1;
    AudioAmplifier  mod_lfo2;
    AudioAmplifier  mod_dc;

    int16_t waveTableMorph1_I16[256];
    int16_t waveTableMorph2_I16[256];

    void update();
    void configure(uint8_t id, Patch * patchData);
    void applyPatchData();
    void noteOn(uint8_t note, uint8_t velocity);
    void noteOff(uint8_t note, uint8_t velocity);
    void setOsc1Waveform();
    void setOsc2Waveform();
    void setOscAmWaveform();
    void setOscFmWaveform();
    void setAmpEnv();
    void setFilterEnv();
    void setFilter();
    void setEnv3();
    void setAmplitudeModulation();
    void setLfo1AmplitudeModulation();
    void setLfo2AmplitudeModulation();
    void setFrequencyModulation();
    bool inRelease();
    void updateFrequency();
    void setOscMixer(uint8_t oscillatorId);
    void set_modMixer_osc1_pitch();
    void set_modMixer_osc2_pitch();
    void set_modMixer_osc1_phase();
    void set_modMixer_osc2_phase();
    void set_modMixer_filter();
    void set_modMixer_pwm();
    void set_modMixer_am_pitch();
    void set_modMixer_fm_pitch();
    void set_osc1_waveOffset();
    void set_osc2_waveOffset();
    uint32_t get_osc1_waveOffset();
    uint32_t get_osc2_waveOffset();
};

class VoiceBank
{
  private:
    AudioConnection  *   _patchCords[NR_PATCHCORDS_VOICEBANK];
    Voice voices[NR_VOICES];
    AudioMixer4 _voiceMixer[3];
    AudioSynthWaveform _lfo1;
    AudioSynthWaveform _lfo2;
    AudioAnalyzePeak    _lfo1Peak;
    AudioAnalyzePeak    _lfo2Peak;
    AudioSynthWaveformDc _modDc;
    
    AudioFilterStateVariable dcOffsetFilter;
    
    uint16_t _connectionIndex = 0;
    void _connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput);
    float _getPannedLevel(float baseLevel, uint8_t channel);

  public:
    Patch patch;
    float modWheel = 0.0;
    int16_t * currentWaveform_I16 = nullptr;

    uint8_t waveTableScanMode = 0;
    uint16_t waveTableScanStep = 1;
    uint16_t waveTableScanInterval = 10;
    uint8_t waveTableVoiceSyncMode = 0;

    AudioAmplifier  output_dry_L;
    AudioAmplifier  output_dry_R;
    AudioAmplifier  output_reverbSend_L;
    AudioAmplifier  output_reverbSend_R;
    AudioAmplifier  output_chorusSend;
    AudioAmplifier  output_delaySend;
    //AudioAmplifier  output_phaserSend;

    AudioSynthWaveformSine overTone;

    void update();
    void configure();
    void noteOn(uint8_t note, uint8_t velocity);
    void noteOff(uint8_t note, uint8_t velocity);
    void setPitchBend(int pitchBend);
    void panic();

    void adjustParameter(uint8_t parameter, int8_t delta);
    void setParameter(uint8_t parameter, float value);

    void setEfx();
    void applyPatchData();
    void importWaveTable(const unsigned int * waveTableI32, int16_t * waveTableI16);
};

const int16_t PARABOLIC_WAVE[256] = { -26092, -26053, -25939, -25748, -25486, -25153, -24753, -24289, -23768, -23192, -22570, -21905, -21204, -20472, -19715, -18940, -18153, -17356, -16558, -15761, -14969, -14186, -13415, -12658, -11916, -11192, -10485, -9795, -9123, -8466, -7824, -7196, -6579, -5972, -5373, -4779, -4191, -3602, -3015, -2426, -1836, -1240, -642, -38, 569, 1182, 1798, 2417, 3039, 3661, 4283, 4903, 5520, 6133, 6740, 7339, 7928, 8509, 9078, 9637, 10183, 10717, 11238, 11746, 12242, 12726, 13199, 13662, 14114, 14558, 14994, 15423, 15846, 16263, 16676, 17085, 17490, 17893, 18292, 18689, 19082, 19473, 19859, 20241, 20619, 20990, 21356, 21714, 22065, 22406, 22737, 23058, 23369, 23668, 23953, 24227, 24489, 24738, 24974, 25198, 25410, 25610, 25799, 25978, 26146, 26305, 26456, 26599, 26733, 26861, 26982, 27098, 27207, 27311, 27410, 27503, 27591, 27673, 27749, 27820, 27884, 27942, 27992, 28000, 28000, 28000, 28000, 28000, 28000, 28000, 28000, 28000, 28000, 28000, 27992, 27942, 27884, 27820, 27749, 27673, 27591, 27503, 27410, 27311, 27207, 27098, 26982, 26861, 26733, 26599, 26456, 26305, 26146, 25978, 25799, 25610, 25410, 25198, 24974, 24738, 24489, 24227, 23953, 23668, 23369, 23058, 22737, 22406, 22065, 21714, 21356, 20990, 20619, 20241, 19859, 19473, 19082, 18689, 18292, 17893, 17490, 17085, 16676, 16263, 15846, 15423, 14994, 14558, 14114, 13662, 13199, 12726, 12242, 11746, 11238, 10717, 10183, 9637, 9078, 8509, 7928, 7339, 6740, 6133, 5520, 4903, 4283, 3661, 3039, 2417, 1798, 1182, 569, -38, -642, -1240, -1836, -2426, -3015, -3602, -4191, -4779, -5373, -5972, -6579, -7196, -7824, -8466, -9123, -9795, -10485, -11192, -11916, -12658, -13415, -14186, -14969, -15761, -16558, -17356, -18153, -18940, -19715, -20472, -21204, -21905, -22570, -23192, -23768, -24289, -24753, -25153, -25486, -25748, -25939, -26053};
const float AWFREQ = 172.0f;
