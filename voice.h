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

// 108
#define POLY_MODE 106

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

#define OSC1_WAVETABLE_INDEX    102
#define OSC1_WAVETABLE_MODE     104
#define OSC1_WAVETABLE_START    94
#define OSC1_WAVETABLE_LENGTH   95
#define OSC1_WAVETABLE_INTERVAL 96
#define OSC1_WAVETABLE_STEPSIZE 97

#define OSC2_WAVETABLE_INDEX    103
#define OSC2_WAVETABLE_MODE     105
#define OSC2_WAVETABLE_START    98
#define OSC2_WAVETABLE_LENGTH   99
#define OSC2_WAVETABLE_INTERVAL 100
#define OSC2_WAVETABLE_STEPSIZE 101

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

#define WAVETABLE_LENGTH 2 * (ARBITRARY_LENGTH - 1)

const uint8_t MAX_WAVEFORM_INDEX = 3;

extern int16_t waveTable1_I16[WAVETABLE_LENGTH];
extern int16_t waveTable2_I16[WAVETABLE_LENGTH];

extern String waveTableNames[NR_WAVETABLES];

struct Patch
{
  uint8_t osc1_waveform = 1;
  uint8_t osc2_waveform = 1;

  int8_t   transpose = 0;
  float detune = 0.20;

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
  
  uint8_t osc2_waveTable_mode = 0;
  uint8_t osc2_waveTable_index = 0;
  uint16_t osc2_waveTable_start = 0;
  uint16_t osc2_waveTable_length = 255;
  uint16_t osc2_waveTable_interval = 10;
  uint16_t osc2_waveTable_stepSize = 1;

  uint8_t polyMode = POLY_FULL;
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
    void setAmFrequency();
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

    AudioFilterStateVariable _lfo1Filter;
    AudioFilterStateVariable _lfo2Filter;
    AudioSynthWaveformDc _modDc;
    uint16_t _connectionIndex = 0;
    void _connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput);
    float _getPannedLevel(float baseLevel, uint8_t channel);

  public:
    Patch patch;
    int16_t * currentWaveform_I16 = nullptr;
    //int16_t waveTable1_I16[256];
    //int16_t waveTable2_I16[256];
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
    AudioAmplifier  output_phaserSend;

    void update();
    void configure();
    void noteOn(uint8_t note, uint8_t velocity);
    void noteOff(uint8_t note, uint8_t velocity);
    void setPitchBend(int pitchBend);

    void adjustParameter(uint8_t parameter, int8_t delta);
    void setParameter(uint8_t parameter, float value);

    void setEfx();
    void applyPatchData();
    void importWaveTable(const unsigned int * waveTableI32, int16_t * waveTableI16);
};

const int16_t PARABOLIC_WAVE[256] = { -26092, -26053, -25939, -25748, -25486, -25153, -24753, -24289, -23768, -23192, -22570, -21905, -21204, -20472, -19715, -18940, -18153, -17356, -16558, -15761, -14969, -14186, -13415, -12658, -11916, -11192, -10485, -9795, -9123, -8466, -7824, -7196, -6579, -5972, -5373, -4779, -4191, -3602, -3015, -2426, -1836, -1240, -642, -38, 569, 1182, 1798, 2417, 3039, 3661, 4283, 4903, 5520, 6133, 6740, 7339, 7928, 8509, 9078, 9637, 10183, 10717, 11238, 11746, 12242, 12726, 13199, 13662, 14114, 14558, 14994, 15423, 15846, 16263, 16676, 17085, 17490, 17893, 18292, 18689, 19082, 19473, 19859, 20241, 20619, 20990, 21356, 21714, 22065, 22406, 22737, 23058, 23369, 23668, 23953, 24227, 24489, 24738, 24974, 25198, 25410, 25610, 25799, 25978, 26146, 26305, 26456, 26599, 26733, 26861, 26982, 27098, 27207, 27311, 27410, 27503, 27591, 27673, 27749, 27820, 27884, 27942, 27992, 28000, 28000, 28000, 28000, 28000, 28000, 28000, 28000, 28000, 28000, 28000, 27992, 27942, 27884, 27820, 27749, 27673, 27591, 27503, 27410, 27311, 27207, 27098, 26982, 26861, 26733, 26599, 26456, 26305, 26146, 25978, 25799, 25610, 25410, 25198, 24974, 24738, 24489, 24227, 23953, 23668, 23369, 23058, 22737, 22406, 22065, 21714, 21356, 20990, 20619, 20241, 19859, 19473, 19082, 18689, 18292, 17893, 17490, 17085, 16676, 16263, 15846, 15423, 14994, 14558, 14114, 13662, 13199, 12726, 12242, 11746, 11238, 10717, 10183, 9637, 9078, 8509, 7928, 7339, 6740, 6133, 5520, 4903, 4283, 3661, 3039, 2417, 1798, 1182, 569, -38, -642, -1240, -1836, -2426, -3015, -3602, -4191, -4779, -5373, -5972, -6579, -7196, -7824, -8466, -9123, -9795, -10485, -11192, -11916, -12658, -13415, -14186, -14969, -15761, -16558, -17356, -18153, -18940, -19715, -20472, -21204, -21905, -22570, -23192, -23768, -24289, -24753, -25153, -25486, -25748, -25939, -26053};
//const int16_t HARMONIC_WAVE[256] = { 0, 3934, 7773, 11428, 14813, 17853, 20484, 22655, 24332, 25493, 26137, 26276, 25937, 25162, 24004, 22525, 20793, 18882, 16865, 14816, 12801, 10882, 9113, 7536, 6183, 5074, 4218, 3612, 3244, 3092, 3127, 3314, 3617, 3996, 4414, 4834, 5226, 5563, 5826, 6003, 6087, 6082, 5994, 5838, 5631, 5395, 5151, 4923, 4732, 4595, 4527, 4538, 4633, 4808, 5059, 5372, 5730, 6113, 6497, 6858, 7171, 7413, 7564, 7606, 7529, 7325, 6993, 6539, 5974, 5314, 4581, 3797, 2992, 2191, 1423, 714, 87, -440, -852, -1141, -1303, -1344, -1271, -1101, -851, -546, -209, 132, 451, 724, 929, 1048, 1067, 980, 783, 482, 85, -393, -932, -1511, -2104, -2685, -3228, -3710, -4109, -4407, -4592, -4655, -4595, -4415, -4124, -3736, -3270, -2747, -2192, -1628, -1080, -571, -122, 252, 537, 727, 820, 819, 735, 579, 370, 127, -127, -370, -579, -735, -819, -820, -727, -537, -252, 122, 571, 1080, 1628, 2192, 2747, 3270, 3736, 4124, 4415, 4595, 4655, 4592, 4407, 4109, 3710, 3228, 2685, 2104, 1511, 932, 393, -85, -482, -783, -980, -1067, -1048, -929, -724, -451, -132, 209, 546, 851, 1101, 1271, 1344, 1303, 1141, 852, 440, -87, -714, -1423, -2191, -2992, -3797, -4581, -5314, -5974, -6539, -6993, -7325, -7529, -7606, -7564, -7413, -7171, -6858, -6497, -6113, -5730, -5372, -5059, -4808, -4633, -4538, -4527, -4595, -4732, -4923, -5151, -5395, -5631, -5838, -5994, -6082, -6087, -6003, -5826, -5563, -5226, -4834, -4414, -3996, -3617, -3314, -3127, -3092, -3244, -3612, -4218, -5074, -6183, -7536, -9113, -10882, -12801, -14816, -16865, -18882, -20793, -22525, -24004, -25162, -25937, -26276, -26137, -25493, -24332, -22655, -20484, -17853, -14813, -11428, -7773, -3934, 0};
//const int16_t PPG_WAVE[256] = {455, 4257, 12654, 21524, 27042, 29297, 30527, 30599, 28691, 25352, 22613, 20841, 19570, 18729, 19317, 21097, 23149, 24638, 25735, 26000, 24846, 21902, 17751, 12607, 6711, 399, -5419, -10409, -14393, -17262, -18601, -18616, -17616, -15827, -13201, -10328, -7929, -6346, -5257, -4661, -4831, -5865, -7209, -8331, -9023, -9326, -9135, -8295, -6692, -4347, -1469, 1531, 4149, 5977, 6679, 6172, 4685, 2771, 785, -1134, -3014, -4655, -5872, -6344, -5928, -4655, -3053, -1457, 104, 1848, 3452, 4607, 5265, 5768, 5967, 5738, 5238, 5046, 5120, 5251, 5386, 5936, 6781, 7676, 8446, 9285, 9879, 9979, 9593, 9073, 8135, 6588, 4663, 2955, 1254, -665, -2465, -3371, -3660, -3996, -4518, -4612, -4385, -4300, -4440, -4253, -3895, -3830, -4099, -4091, -3857, -3754, -3768, -3370, -2749, -2310, -1941, -1073, -66, 429, 542, 1154, 2163, 2759, 2616, 2285, 1759, 648, -904, -2015, -2541, -2872, -3015, -2419, -1410, -798, -685, -190, 817, 1685, 2054, 2493, 3114, 3512, 3498, 3601, 3835, 3843, 3574, 3639, 3997, 4184, 4044, 4129, 4356, 4262, 3740, 3404, 3115, 2209, 409, -1510, -3211, -4919, -6844, -8391, -9329, -9849, -10235, -10135, -9541, -8702, -7932, -7037, -6192, -5642, -5507, -5376, -5302, -5494, -5994, -6223, -6024, -5521, -4863, -3708, -2104, -360, 1201, 2797, 4399, 5672, 6088, 5616, 4399, 2758, 878, -1041, -3027, -4941, -6428, -6935, -6233, -4405, -1787, 1213, 4091, 6436, 8039, 8879, 9070, 8767, 8075, 6953, 5609, 4575, 4405, 5001, 6090, 7673, 10072, 12945, 15571, 17360, 18360, 18345, 17006, 14137, 10153, 5163, -655, -6966, -12863, -18007, -22158, -25102, -26256, -25991, -24894, -23405, -21353, -19573, -18985, -19826, -21097, -22869, -25608, -28947, -19538, -22187, -22187, -22383, -22054, -17988, -12202, 252};
const float AWFREQ = 172.0f;
