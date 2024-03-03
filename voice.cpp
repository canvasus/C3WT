#include "voice.h"
#include "src/waveTables/1_AudioSamplePlesant_.h"
#include "src/waveTables/2_AudioSampleMicrow02.h"
#include "src/waveTables/3_AudioSampleLerning.h"
#include "src/waveTables/4_AudioSampleFairligh.h"
#include "src/waveTables/5_AudioSampleAnalog.h"
#include "src/waveTables/6_AudioSampleEnsoniq.h"
#include "src/waveTables/7_AudioSampleDigital.h"
#include "src/waveTables/8_AudioSampleMorphing.h"
#include "src/waveTables/9_AudioSamplePpg_best.h"
#include "src/waveTables/10_AudioSampleMeravigl.h"
#include "src/waveTables/11_AudioSampleWavetable.h"
#include "src/waveTables/12_AudioSampleBraids01.h"
#include "src/waveTables/13_AudioSampleBraids02.h"
#include "src/waveTables/14_AudioSampleBraids03.h"
#include "src/waveTables/15_AudioSampleBraids04.h"
#include "src/waveTables/16_AudioSamplePlaits01.h"
#include "src/waveTables/17_AudioSamplePlaits02.h"
#include "src/waveTables/18_AudioSamplePlaits03.h"
#include "src/waveTables/AudioSampleRoland.h"
#include "src/waveTables/AudioSampleTransfor.h"

const float NOTEFREQS[128] = {8.176f, 8.662f, 9.177f, 9.723f, 10.301f, 10.913f, 11.562f, 12.25f, 12.978f, 13.75f, 14.568f, 15.434f, 16.352f, 17.324f, 18.354f, 19.445f, 20.602f, 21.827f, 23.125f, 24.5f, 25.957f, 27.5f, 29.135f, 30.868f, 32.703f, 34.648f, 36.708f, 38.891f, 41.203f, 43.654f, 46.249f, 48.999f, 51.913f, 55.0f, 58.27f, 61.735f, 65.406f, 69.296f, 73.416f, 77.782f, 82.407f, 87.307f, 92.499f, 97.999f, 103.826f, 110.0f, 116.541f, 123.471f, 130.813f, 138.591f, 146.832f, 155.563f, 164.814f, 174.614f, 184.997f, 195.998f, 207.652f, 220.0f, 233.082f, 246.942f, 261.626f, 277.183f, 293.665f, 311.127f, 329.628f, 349.228f, 369.994f, 391.995f, 415.305f, 440.0f, 466.164f, 493.883f, 523.251f, 554.365f, 587.33f, 622.254f, 659.255f, 698.456f, 739.989f, 783.991f, 830.609f, 880.0f, 932.328f, 987.767f, 1046.502f, 1108.731f, 1174.659f, 1244.508f, 1318.51f, 1396.913f, 1479.978f, 1567.982f, 1661.219f, 1760.0f, 1864.655f, 1975.533f, 2093.005f, 2217.461f, 2349.318f, 2489.016f, 2637.02f, 2793.826f, 2959.955f, 3135.963f, 3322.438f, 3520.0f, 3729.31f, 3951.066f, 4186.009f, 4434.922f, 4698.636f, 4978.032f, 5274.041f, 5587.652f, 5919.911f, 6271.927f, 6644.875f, 7040.0f, 7458.62f, 7902.133f, 8372.018f, 8869.844f, 9397.273f, 9956.063f, 10548.08f, 11175.3f, 11839.82f, 12543.85f};
uint8_t waveformList[NR_WAVEFORMS] = {WAVEFORM_SINE, WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE, WAVEFORM_BANDLIMIT_PULSE, WAVEFORM_ARBITRARY, WAVEFORM_BANDLIMIT_SAWTOOTH, WAVEFORM_BANDLIMIT_SQUARE, WAVEFORM_TRIANGLE_VARIABLE, WAVEFORM_SAMPLE_HOLD};
uint8_t nrWaveforms = sizeof(waveformList);
char waveformNames[NR_WAVEFORMS][6] = {"SIN", "SAWbr", "PLSb", "WTB", "SAWb", "SQRb", "TRIb", "S&H"};
//unused: WAVEFORM_SAWTOOTH, WAVEFORM_SAWTOOTH_REVERSE, WAVEFORM_SQUARE, WAVEFORM_TRIANGLE, WAVEFORM_PULSE

DMAMEM int16_t waveTable1_I16[WAVETABLE_LENGTH];
DMAMEM int16_t waveTable2_I16[WAVETABLE_LENGTH];
DMAMEM int16_t waveTable3_I16[WAVETABLE_LENGTH];
DMAMEM int16_t waveTable4_I16[WAVETABLE_LENGTH];

int16_t * activeWaveTables[4] = {waveTable1_I16, waveTable2_I16, waveTable3_I16, waveTable4_I16};

const unsigned int * waveTables[NR_WAVETABLES] = {AudioSamplePlesant_, AudioSampleMicrow02, AudioSampleLerning, AudioSampleFairligh,
                                                  AudioSampleAnalog, AudioSampleEnsoniq, AudioSampleDigital, AudioSampleMorphing,
                                                  AudioSamplePpg_best, AudioSampleMeravigl, AudioSampleWavetable,
                                                  AudioSampleBraids01, AudioSampleBraids02, AudioSampleBraids03, AudioSampleBraids04,
                                                  AudioSamplePlaits01, AudioSamplePlaits02, AudioSamplePlaits03, AudioSampleRoland,
                                                  AudioSampleTransfor};

const String waveTableNames[NR_WAVETABLES] = {"Pleasant", "MicroWave2 ", "Learning", "Fairlight",
                                        "Analog", "Ensoniq", "Digital", "Morphing",
                                        "PPG best", "Meravigl", "Chant",
                                        "Braids1", "Braids2", "Braids3", "Braids4",
                                        "Plaits1", "Plaits2", "Plaits3",
                                        "Roland", "Transform"};


char waveTableModes[2][6] = {"scan", "morph"};
char waveTableMovement[2][6] = {"loop", "up"};

float waveshaper_default[3] = {-1.0, 0.0, 1.0};
char waveShaperNames[NR_WAVESHAPE_MODELS][6] = {"off", "odr", "dist", "comp"};

FLASHMEM Voice::Voice()
{
  _connectionIndex = 0;
  _connect(_modMixer_osc1_pitch, 0, _osc1, 0);
  _connect(_modMixer_osc2_pitch, 0, _osc2, 0);

  _connect(_modMixer_osc1_phase, 0, _osc1, 1);
  _connect(_modMixer_osc2_phase, 0, _osc2, 1);

  _connect(_modMixer_pwm, 0, _pwm, 0);

  _connect(_modMixer_am_pitch, 0, _osc_AM, 0);
  _connect(_modMixer_fm_pitch, 0, _osc_FM, 0);

  _connect(_osc1, 0, _oscMixer, 0);
  _connect(_osc2, 0, _oscMixer, 1);
  _connect(_pwm, 0, _oscMixer, 2);
  _connect(_noise, 0, _oscMixer, 3);

  _connect(_oscMixer, 0, _amplitudeModulation, 0);
  _connect(_amplitudeModulation, 0, _filter, 0);
  _connect(_filter, 0, _ampEnvelope, 0);
  _connect(_ampEnvelope, 0, output, 0);

  // Amplitude Modulation (post osc mixer)
  _connect(_envelopeDC, 0, _amplitudeModulationMixer, 0);
  _connect(_osc_AM, 0, _amplitudeModulationMixer, 1);
  _connect(_amplitudeModulationMixer, 0, _amplitudeModulation, 1);

  _pwm.amplitude(1.0);
  _noise.amplitude(0.5);

  output.gain(1.0);
  mod_lfo1.gain(1.0);
  mod_lfo2.gain(1.0);
  mod_dc.gain(1.0);

  _ampEnvelope.hold(0.0);
  _ampEnvelope.delay(0.0);
  _filterEnvelope.hold(0.0);
  _filterEnvelope.delay(0.0);
  _envelope3.hold(0.0);
  _envelope3.delay(0.0);

  _connect(_envelopeDC, 0, _filterEnvelope, 0);
  _connect(_filterEnvelope, 0, _filterModMixer, 0);
  _connect(_filterModMixer, 0, _filter, 1);
  
  _envelopeDC.amplitude(1.0);

  _filter.frequency(FILTER_MAX_CUTOFF);
  _filter.octaveControl(FILTER_OCTAVECONTROL);

  // internal pitch mod mixers to main mod mixers
  _connect(_modMixer_osc1_pitch_voiceInternal, 0, _modMixer_osc1_pitch, 0);
  _connect(_modMixer_osc2_pitch_voiceInternal, 0, _modMixer_osc2_pitch, 0);

  // envelope 3 to mod mixers
  _connect(_envelopeDC, 0, _envelope3, 0);
  _connect(_envelope3, 0, _modMixer_osc1_pitch_voiceInternal, 0);
  _connect(_envelope3, 0, _modMixer_osc2_pitch_voiceInternal, 0);
  _connect(_envelope3, 0, _modMixer_osc1_phase, 0);
  _connect(_envelope3, 0, _modMixer_osc2_phase, 0);
  _connect(_envelope3, 0, _modMixer_pwm, 0);
  _connect(_envelope3, 0, _modMixer_am_pitch, 0);
  _connect(_envelope3, 0, _modMixer_fm_pitch, 0);

  // FM oscillator to mod mixers
  _connect(_osc_FM, 0, _modMixer_osc1_pitch_voiceInternal, 1);
  _connect(_osc_FM, 0, _modMixer_osc2_pitch_voiceInternal, 1);

  // Env3 to lfo1 amplitude modulation
  _connect(_envelopeDC, 0, _lfo1_amplitudeModulationMixer, 0);
  _connect(_envelope3, 0, _lfo1_amplitudeModulationMixer, 1);
  _connect(_lfo1_amplitudeModulationMixer, 0, _lfo1_amplitudeModulation, 0);
  _connect(mod_lfo1, 0, _lfo1_amplitudeModulation, 1);

  // lfo1 to mod mixers
  _connect(_lfo1_amplitudeModulation, 0, _modMixer_osc1_pitch, 1);
  _connect(_lfo1_amplitudeModulation, 0, _modMixer_osc2_pitch, 1);
  _connect(_lfo1_amplitudeModulation, 0, _modMixer_osc1_phase, 1);
  _connect(_lfo1_amplitudeModulation, 0, _modMixer_osc2_phase, 1);
  _connect(_lfo1_amplitudeModulation, 0, _filterModMixer, 1);
  _connect(_lfo1_amplitudeModulation, 0, _modMixer_pwm, 1);
  _connect(_lfo1_amplitudeModulation, 0, _modMixer_am_pitch, 1);
  _connect(_lfo1_amplitudeModulation, 0, _modMixer_fm_pitch, 1);

    // Env3 to lfo2 amplitude modulation
  _connect(_envelopeDC, 0, _lfo2_amplitudeModulationMixer, 0);
  _connect(_envelope3, 0, _lfo2_amplitudeModulationMixer, 1);
  _connect(_lfo2_amplitudeModulationMixer, 0, _lfo2_amplitudeModulation, 0);
  _connect(mod_lfo2, 0, _lfo2_amplitudeModulation, 1);

  // lfo2 to mod mixers
  _connect(_lfo2_amplitudeModulation, 0, _modMixer_osc1_pitch, 2);
  _connect(_lfo2_amplitudeModulation, 0, _modMixer_osc2_pitch, 2);
  _connect(_lfo2_amplitudeModulation, 0, _modMixer_osc1_phase, 2);
  _connect(_lfo2_amplitudeModulation, 0, _modMixer_osc2_phase, 2);
  _connect(_lfo2_amplitudeModulation, 0, _filterModMixer, 2);
  _connect(_lfo2_amplitudeModulation, 0, _modMixer_pwm, 2);
  _connect(_lfo2_amplitudeModulation, 0, _modMixer_am_pitch, 2);
  _connect(_lfo2_amplitudeModulation, 0, _modMixer_fm_pitch, 2);

  // dc input to mod mixers
  _connect(mod_dc, 0, _modMixer_osc1_pitch, 3);
  _connect(mod_dc, 0, _modMixer_osc2_pitch, 3);
  _connect(mod_dc, 0, _filterModMixer, 3);
  //_connect(_lfo2_amplitudeModulation, 0, _filterModMixer, 3); // --???

  // oscillator 1 to mod mixers
  _connect(_osc1, 0, _modMixer_osc2_pitch_voiceInternal, 2);
  _connect(_osc1, 0, _modMixer_osc2_phase, 3);

  // oscillator 2 to mod mixers
  _connect(_osc2, 0, _modMixer_osc1_pitch_voiceInternal, 2);
  _connect(_osc2, 0, _modMixer_osc1_phase, 3);

  memset(waveTableMorph1_I16, 0, sizeof(waveTableMorph1_I16));
  memset(waveTableMorph2_I16, 0, sizeof(waveTableMorph2_I16));

  _osc1.arbitraryWaveform(&activeWaveTable1[0], AWFREQ);
  _osc2.arbitraryWaveform(&activeWaveTable2[0], AWFREQ);

  _osc_AM.arbitraryWaveform(&waveTable1_I16[0], AWFREQ);
  _osc_FM.arbitraryWaveform(&waveTable1_I16[0], AWFREQ);

}

void Voice::update()
{
  //if ( (_patch->osc1_sync == 1) && _osc2.getSync() ) _osc1.sync();
  //if ( (_patch->osc2_sync == 1) && _osc1.getSync() ) _osc2.sync();

  if( (waveformList[_patch->osc1_waveform] == WAVEFORM_ARBITRARY) && (osc1_waveTimer > _patch->osc1_waveTable_interval) )
  {
    osc1_waveTimer = 0;
    if (_patch->osc1_waveTable_mode == WAVETABLE_MODE_PHASESCAN) _updateWaveTable1_phaseScan();
    else if(_patch->osc1_waveTable_mode == WAVETABLE_MODE_MORPH) _updateWaveTable1_morph();
  }
  
  if( (waveformList[_patch->osc2_waveform] == WAVEFORM_ARBITRARY) && (osc2_waveTimer > _patch->osc2_waveTable_interval) )
  {
    osc2_waveTimer = 0;
    if (_patch->osc2_waveTable_mode == WAVETABLE_MODE_PHASESCAN) _updateWaveTable2_phaseScan();
    else if(_patch->osc2_waveTable_mode == WAVETABLE_MODE_MORPH) _updateWaveTable2_morph();
  }
}

void Voice::_updateWaveTable1_phaseScan()
{
 if (_scanDirection1 == RIGHT)
    {
      _waveOffset1 = _waveOffset1 + _patch->osc1_waveTable_stepSize;
      if (_waveOffset1 >= (_patch->osc1_waveTable_start + _patch->osc1_waveTable_length) ) _scanDirection1 = LEFT;
    }
    if (_scanDirection1 == LEFT)
    {
      _waveOffset1 = _waveOffset1 - _patch->osc1_waveTable_stepSize;
      if (_waveOffset1 <= _patch->osc1_waveTable_start) _scanDirection1 = RIGHT;
    }
    if (_waveOffset1 > (WAVETABLE_LENGTH * 2 - 256) ) _waveOffset1 = WAVETABLE_LENGTH * 2 - 256;
    
    _osc1.arbitraryWaveform(&activeWaveTable1[_waveOffset1 >> 1], AWFREQ);
    //_osc1.arbitraryWaveform(&waveTable1_I16[_waveOffset1 >> 1], AWFREQ);
    _osc1.begin(WAVEFORM_ARBITRARY);
    _osc1.amplitude(1.0);
}

void Voice::_updateWaveTable2_phaseScan()
{
  if (_scanDirection2 == RIGHT)
  {
    _waveOffset2  = _waveOffset2 + _patch->osc2_waveTable_stepSize;
    if (_waveOffset2 >= (_patch->osc2_waveTable_start + _patch->osc2_waveTable_length)) _scanDirection2 = LEFT;
  }
  if (_scanDirection2 == LEFT)
  {
    _waveOffset2 = _waveOffset2 - _patch->osc2_waveTable_stepSize;
    if (_waveOffset2 <= _patch->osc2_waveTable_start) _scanDirection2 = RIGHT;
  }
  if (_waveOffset2 > (ARBITRARY_LENGTH * 2 - 256) ) _waveOffset2 = ARBITRARY_LENGTH * 2 - 256;
  
  _osc2.arbitraryWaveform(&activeWaveTable2[_waveOffset2 >> 1], AWFREQ);
  //_osc2.arbitraryWaveform(&waveTable2_I16[_waveOffset2 >> 1], AWFREQ);
  _osc2.begin(WAVEFORM_ARBITRARY);
  _osc2.amplitude(1.0);
}

void Voice::_updateWaveTable1_morph()
{
  // assumes 2* 8192 = 16384 length
  if (_scanDirection1 == RIGHT)
  {
    _waveOffset1 = min(_waveOffset1 + _patch->osc1_waveTable_stepSize, _patch->osc1_waveTable_start + _patch->osc1_waveTable_length);
    if ( (_waveOffset1 >= (_patch->osc1_waveTable_start + _patch->osc1_waveTable_length) ) && (_patch->osc1_waveTable_movement == WAVETABLE_PLAYMODE_UPDOWN) ) _scanDirection1 = LEFT;
    if (_waveOffset1 > (2* WAVETABLE_LENGTH - 257) ) _waveOffset1 = 2*WAVETABLE_LENGTH - 257;
  }
  else if (_scanDirection1 == LEFT)
  {
    if (_patch->osc1_waveTable_stepSize > _waveOffset1) _waveOffset1 = 0;
    else _waveOffset1 = _waveOffset1 - _patch->osc1_waveTable_stepSize;
    if (_waveOffset1 <= _patch->osc1_waveTable_start) _scanDirection1 = RIGHT;
  }

  float crossFade = (_waveOffset1 % 256) / 256.0;
  uint16_t firstWaveStartPoint = (_waveOffset1 / 256) * 256;
  uint16_t secondWaveStartPoint = firstWaveStartPoint + 256;
  for (uint16_t index = 0; index < 256; index++)
  {
    // now adjust for 8192 length
    waveTableMorph1_I16[index] = activeWaveTable1[ (firstWaveStartPoint + index) >> 1] * (1.0 - crossFade) + activeWaveTable1[(secondWaveStartPoint + index) >> 1] * crossFade;
  }

  _osc1.arbitraryWaveform(waveTableMorph1_I16, AWFREQ);
  _osc1.begin(WAVEFORM_ARBITRARY);
  _osc1.amplitude(1.0);
}

void Voice::_updateWaveTable2_morph()
{
  if (_scanDirection2 == RIGHT)
  {
    _waveOffset2 = min(_waveOffset2 + _patch->osc2_waveTable_stepSize, _patch->osc2_waveTable_start + _patch->osc2_waveTable_length);
    if ( (_waveOffset2 >= (_patch->osc2_waveTable_start + _patch->osc2_waveTable_length) ) && (_patch->osc2_waveTable_movement == WAVETABLE_PLAYMODE_UPDOWN) ) _scanDirection2 = LEFT;
    if (_waveOffset2 > (2*WAVETABLE_LENGTH - 257) ) _waveOffset2 = 2*WAVETABLE_LENGTH - 257;
  }
  else if (_scanDirection2 == LEFT)
  {
    if (_patch->osc2_waveTable_stepSize > _waveOffset2) _waveOffset2 = 0;
    else _waveOffset2 = _waveOffset2 - _patch->osc2_waveTable_stepSize;
    if (_waveOffset2 <= _patch->osc2_waveTable_start) _scanDirection2 = RIGHT;
  }

  float crossFade = (_waveOffset2 % 256) / 256.0;
    
  uint16_t firstWaveStartPoint = (_waveOffset2 / 256) * 256;
  uint16_t secondWaveStartPoint = firstWaveStartPoint + 256;
  for (uint16_t index = 0; index < 256; index++)
  {
    //waveTableMorph2_I16[index] = waveTable2_I16[firstWaveStartPoint + index] * (1 - crossFade) + waveTable2_I16[secondWaveStartPoint + index] * crossFade;
    //waveTableMorph2_I16[index] = waveTable2_I16[ (firstWaveStartPoint + index) << 1] * (1.0 - crossFade) + waveTable2_I16[(secondWaveStartPoint + index) << 1] * crossFade;
    waveTableMorph2_I16[index] = activeWaveTable2[ (firstWaveStartPoint + index) >> 1] * (1.0 - crossFade) + activeWaveTable2[(secondWaveStartPoint + index) >> 1] * crossFade;
  }

  _osc2.arbitraryWaveform(waveTableMorph2_I16, AWFREQ);
  _osc2.begin(WAVEFORM_ARBITRARY);
  _osc2.amplitude(1.0);
}

void Voice::configure(uint8_t id, Patch * patchData)
{
  voiceId = id;
  _patch = patchData;
  applyPatchData();
}

void Voice::_connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput)
{
  _patchCords[_connectionIndex] = new AudioConnection(source, sourceOutput, destination , destinationInput);
  _connectionIndex++;
  if (_connectionIndex > NR_PATCHCORDS_VOICE - 1)
  {
    _connectionIndex = NR_PATCHCORDS_VOICE - 1;
    Serial.println("ERROR: Voice - no available patchcords");
  }
}

void Voice::noteOn(uint8_t note, uint8_t velocity)
{
  currentNote = note;
  updateFrequency();

  if (_patch->osc1_waveTable_movement == WAVETABLE_PLAYMODE_ONESHOT_UP)
  {
    _waveOffset1 = _patch->osc1_waveTable_start;
    _scanDirection1 = RIGHT;
  }
  if (_patch->osc2_waveTable_movement == WAVETABLE_PLAYMODE_ONESHOT_UP)
  {
    _waveOffset2 = _patch->osc1_waveTable_start;
    _scanDirection2 = RIGHT;
  } 
  
  _ampEnvelope.noteOn();
  _filterEnvelope.noteOn();
  _envelope3.noteOn();
}

void Voice::noteOff(uint8_t note, uint8_t velocity)
{
  currentNote = 0;
  _ampEnvelope.noteOff();
  _filterEnvelope.noteOff();
  _envelope3.noteOff();
}

void Voice::updateFrequency()
{
  float baseFrequency = NOTEFREQS[constrain(currentNote, 0, 127)] * pow(2, pitchBend);
  float baseFrequencyTransposed = NOTEFREQS[constrain(currentNote + _patch->transpose, 0, 127)] * pow(2, pitchBend);
  _osc1.frequency(baseFrequency * pow(2, _patch->detune / 100.0) * (1.0 + unisonDetune));
  _osc2.frequency(baseFrequencyTransposed * pow(2, -_patch->detune / 100.0) * (1.0 + unisonDetune));
  _pwm.frequency(baseFrequencyTransposed * pow(2, unisonDetune / 100.0));
  if (_patch->am_fixedFrequency == 0) _osc_AM.frequency(baseFrequency * _patch->am_frequency_multiplier * (1.0 + unisonDetune));
  else _osc_AM.frequency(_patch->am_frequency_multiplier);
  _osc_FM.frequency(baseFrequency * _patch->fm_frequency_multiplier * (1.0 + unisonDetune));
}

void Voice::applyPatchData()
{
  setOsc1Waveform();
  setOsc2Waveform();

  set_osc1_waveOffset();
  set_osc2_waveOffset();
  
  _osc1.amplitude(1.0);
  _osc2.amplitude(1.0);

  _osc_AM.begin(1.0, 440.0f, waveformList[_patch->osc_am_waveform]);
  _osc_FM.begin(1.0, 440.0f, waveformList[_patch->osc_fm_waveform]);
  _pwm.frequency(440.0f);

  _osc1.phaseModulation(_patch->phaseModulation);
  _osc2.phaseModulation(_patch->phaseModulation);
  _osc1.frequencyModulation(_patch->frequencyModulation);
  _osc2.frequencyModulation(_patch->frequencyModulation);
  
  _oscMixer.gain(0, _patch->osc1_level);
  _oscMixer.gain(1, _patch->osc2_level);
  _oscMixer.gain(2, _patch->pulse_level);
  _oscMixer.gain(3, _patch->noise_level);

  set_modMixer_filter();
  set_modMixer_osc1_pitch();
  set_modMixer_osc2_pitch();
  set_modMixer_osc1_phase();
  set_modMixer_osc2_phase();
  set_modMixer_pwm();
  set_modMixer_am_pitch();
  set_modMixer_fm_pitch();

  setAmpEnv();
  setFilterEnv();
  setEnv3();

  setFilter();
  setAmplitudeModulation();
  setFrequencyModulation();

  setLfo1AmplitudeModulation();
  setLfo2AmplitudeModulation();
}

void Voice::setOsc1Waveform()
{
  _osc1.begin(waveformList[_patch->osc1_waveform]);
  if (waveformList[_patch->osc1_waveform] == WAVEFORM_ARBITRARY)
      _osc1.arbitraryWaveform(&activeWaveTable1[(_patch->osc1_waveTable_start) >> 1], AWFREQ);
}

void Voice::setOsc2Waveform()
{
  _osc2.begin(waveformList[_patch->osc2_waveform]);
  if (waveformList[_patch->osc2_waveform] == WAVEFORM_ARBITRARY)
      _osc2.arbitraryWaveform(&activeWaveTable2[(_patch->osc2_waveTable_start) >> 1], AWFREQ);
}

void Voice::set_osc1_waveOffset()
{
  _waveOffset1 = _patch->osc1_waveTable_start;
}

void Voice::set_osc2_waveOffset()
{
  _waveOffset2 = _patch->osc2_waveTable_start;
}

uint32_t Voice::get_osc1_waveOffset()
{
  return 0;
}

uint32_t Voice::get_osc2_waveOffset()
{
  return 0;
}

void Voice::setAmpEnv()
{
  _ampEnvelope.attack(_patch->ampEnvelope_attack);
  _ampEnvelope.decay(_patch->ampEnvelope_decay);
  _ampEnvelope.sustain(_patch->ampEnvelope_sustain);
  _ampEnvelope.release(_patch->ampEnvelope_release);
}

void Voice::setFilterEnv()
{
  _filterEnvelope.attack(_patch->filterEnvelope_attack);
  _filterEnvelope.decay(_patch->filterEnvelope_decay);
  _filterEnvelope.sustain(_patch->filterEnvelope_sustain);
  _filterEnvelope.release(_patch->filterEnvelope_release);
  //_filterModMixer.gain(0, _patch->envToFilter);
}

void Voice::setEnv3()
{
  _envelope3.attack(_patch->envelope3_attack);
  _envelope3.decay(_patch->envelope3_decay);
  _envelope3.sustain(_patch->envelope3_sustain);
  _envelope3.release(_patch->envelope3_release);
}

void Voice::setFilter()
{
  _filter.frequency( (FILTER_MAX_CUTOFF / 9) * (pow(10, _patch->cutoff) - 1) );
  _filter.resonance(0.7 + 4.3 * _patch->resonance);  // Chamberlin
  //_filter.resonance(1.8 * _patch->resonance);  // Ladder
}

void Voice::setOscMixer(uint8_t oscillatorId)
{
  if (oscillatorId == OSC1_LEVEL) _oscMixer.gain(0, _patch->osc1_level);
  if (oscillatorId == OSC2_LEVEL) _oscMixer.gain(1, _patch->osc2_level);
  if (oscillatorId == PULSE_LEVEL) _oscMixer.gain(2, _patch->pulse_level);
  if (oscillatorId == NOISE_LEVEL) _oscMixer.gain(3, _patch->noise_level);
}

void Voice::setAmplitudeModulation()
{
  _amplitudeModulationMixer.gain(0, 1.0 - _patch->am_level);
  _amplitudeModulationMixer.gain(1, _patch->am_level);
}

void Voice::setLfo1AmplitudeModulation()
{
  _lfo1_amplitudeModulationMixer.gain(0, 1.0 - _patch->mod_env3_lfo1_amplitude);
  _lfo1_amplitudeModulationMixer.gain(1, _patch->mod_env3_lfo1_amplitude);
}

void Voice::setLfo2AmplitudeModulation()
{
  _lfo2_amplitudeModulationMixer.gain(0, 1.0 - _patch->mod_env3_lfo2_amplitude);
  _lfo2_amplitudeModulationMixer.gain(1, _patch->mod_env3_lfo2_amplitude);
}

void Voice::setFrequencyModulation()
{
  _osc_FM.offset(_patch->fm_offset);
  //_modMixer_osc1_pitch_voiceInternal.gain(0, _patch->mod_env3_osc1_pitch);
  //_modMixer_osc2_pitch_voiceInternal.gain(0, _patch->mod_env3_osc2_pitch);
  _modMixer_osc1_pitch_voiceInternal.gain(1, _patch->fm_level);
  _modMixer_osc2_pitch_voiceInternal.gain(1, _patch->fm_level);
}

void Voice::setOscAmWaveform()
{
  _osc_AM.begin(waveformList[_patch->osc_am_waveform]);
}

void Voice::setOscFmWaveform()
{
  _osc_FM.begin(waveformList[_patch->osc_fm_waveform]);
}

void Voice::set_modMixer_osc1_pitch()
{
   _modMixer_osc1_pitch_voiceInternal.gain(0, _patch->mod_env3_osc1_pitch);
  _modMixer_osc1_pitch.gain(1, _patch->mod_lfo1_osc1_pitch);
  _modMixer_osc1_pitch.gain(2, _patch->mod_lfo2_osc1_pitch);
  //_modMixer_osc1_pitch.gain(3, _patch->mod_osc2_osc1_pitch);
  _modMixer_osc1_pitch_voiceInternal.gain(2, _patch->mod_osc2_osc1_pitch);
}

void Voice::set_modMixer_osc2_pitch()
{
  _modMixer_osc2_pitch_voiceInternal.gain(0, _patch->mod_env3_osc2_pitch);
  _modMixer_osc2_pitch.gain(1, _patch->mod_lfo1_osc2_pitch);
  _modMixer_osc2_pitch.gain(2, _patch->mod_lfo2_osc2_pitch);
  //_modMixer_osc2_pitch.gain(3, _patch->mod_osc1_osc2_pitch);
  _modMixer_osc2_pitch_voiceInternal.gain(2, _patch->mod_osc1_osc2_pitch);
}

void Voice::set_modMixer_osc1_phase()
{
  _modMixer_osc1_phase.gain(0, _patch->mod_env3_osc1_phase);
  _modMixer_osc1_phase.gain(1, _patch->mod_lfo1_osc1_phase);
  _modMixer_osc1_phase.gain(2, _patch->mod_lfo2_osc1_phase);
  _modMixer_osc1_phase.gain(3, _patch->mod_osc2_osc1_phase);
}

void Voice::set_modMixer_osc2_phase()
{
  _modMixer_osc2_phase.gain(0, _patch->mod_env3_osc2_phase);
  _modMixer_osc2_phase.gain(1, _patch->mod_lfo1_osc2_phase);
  _modMixer_osc2_phase.gain(2, _patch->mod_lfo2_osc2_phase);
  _modMixer_osc2_phase.gain(3, _patch->mod_osc1_osc2_phase);
}

void Voice::set_modMixer_filter()
{
  _filterModMixer.gain(0, _patch->envToFilter); // FILTER ENV
  _filterModMixer.gain(1, _patch->mod_lfo1_filter_cutoff); // LFO1
  _filterModMixer.gain(2, _patch->mod_lfo2_filter_cutoff); // LFO2
  _filterModMixer.gain(3, 0.0);
}

void Voice::set_modMixer_pwm()
{
  _modMixer_pwm.gain(0, _patch->mod_env3_pwm); // ENV3
  _modMixer_pwm.gain(1, _patch->mod_lfo1_pwm); // LFO1
  _modMixer_pwm.gain(2, _patch->mod_lfo2_pwm); // LFO2
  _modMixer_pwm.gain(3, 0.0);
}

void Voice::set_modMixer_am_pitch()
{
  _modMixer_am_pitch.gain(0, _patch->mod_env3_am_pitch); // FILTER ENV
  _modMixer_am_pitch.gain(1, _patch->mod_lfo1_am_pitch); // LFO1
  _modMixer_am_pitch.gain(2, _patch->mod_lfo2_am_pitch); // LFO2
  _modMixer_am_pitch.gain(3, 0.0);
}

void Voice::set_modMixer_fm_pitch()
{
  _modMixer_fm_pitch.gain(0, _patch->mod_env3_fm_pitch); // FILTER ENV
  _modMixer_fm_pitch.gain(1, _patch->mod_lfo1_fm_pitch); // LFO1
  _modMixer_fm_pitch.gain(2, _patch->mod_lfo2_fm_pitch); // LFO2
  _modMixer_fm_pitch.gain(3, 0.0);
}

bool Voice::inRelease()
{
  return (_ampEnvelope.isActive() || _filterEnvelope.isActive());
}

void VoiceBank::configure()
{
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++)
  {
    uint8_t mixerId = voiceIndex / 4;
    uint8_t channelId = voiceIndex % 4;
    voices[voiceIndex].activeWaveTable1 = activeWaveTables[2 * id];      // Ugly
    voices[voiceIndex].activeWaveTable2 = activeWaveTables[2 * id + 1];
    voices[voiceIndex].configure(voiceIndex, &patch);
    
    _connect(voices[voiceIndex].output, 0, _voiceMixer[mixerId], channelId);
    _voiceMixer[mixerId].gain(channelId, 0.35); // probably should be (1 / NR_VOICES)
    _connect(_lfo1, 0, voices[voiceIndex].mod_lfo1, 0);
    _connect(_lfo2, 0, voices[voiceIndex].mod_lfo2, 0);
  }
  _connect(_voiceMixer[0], 0, _voiceMixer[2], 0);
  _connect(_voiceMixer[1], 0, _voiceMixer[2], 1);

  // DC offset removal

  _connect(_voiceMixer[2], 0, _dcOffsetFilter, 0);
  _dcOffsetFilter.octaveControl(1.0f);
  _dcOffsetFilter.frequency(patch.hpfilter_cutoff);

  _connect(_dcOffsetFilter, 2, _waveShaper, 0);
  _waveShaper.shape(waveshaper_default, 3);

  _connect(_waveShaper, 0, output_dry_L, 0);
  _connect(_waveShaper, 0, output_dry_R, 0);
  _connect(_waveShaper, 0, output_reverbSend_L, 0);
  _connect(_waveShaper, 0, output_reverbSend_R, 0);
  _connect(_waveShaper, 0, output_chorusSend, 0);
  _connect(_waveShaper, 0, output_delaySend, 0);

  //_connect(_dcOffsetFilter, 2, output_dry_L, 0);
  //_connect(_dcOffsetFilter, 2, output_dry_R, 0);
  //_connect(_dcOffsetFilter, 2, output_reverbSend_L, 0);
  //_connect(_dcOffsetFilter, 2, output_reverbSend_R, 0);
  //_connect(_dcOffsetFilter, 2, output_chorusSend, 0);
  //_connect(_dcOffsetFilter, 2, output_delaySend, 0);

  _voiceMixer[2].gain(0, 0.5);
  _voiceMixer[2].gain(1, 0.5);

  _lfo1.begin(0.2, 1.0, waveformList[0]);
  _lfo2.begin(0.9, 1.0, waveformList[0]);
  //_lfo1.offset(-0.5);
  //_lfo2.offset(-0.5);
  _modDc.amplitude(0.0);

  setEfx();

  importWaveTable(waveTables[0], waveTable1_I16);
  importWaveTable(waveTables[0], waveTable2_I16);
  importWaveTable(waveTables[0], waveTable3_I16);
  importWaveTable(waveTables[0], waveTable4_I16);
  
  currentWaveform_I16 = &voices[0].waveTableMorph1_I16[0];
}

void VoiceBank::update()
{
  // update audiostream --> number modulation etc
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].update();
  
}

void VoiceBank::noteOn(uint8_t note, uint8_t velocity)
{
  const float unisonModifiers[8] = {-0.001, 0.001, -0.002, 0.002, -0.003, 0.003, -0.004, 0.004};
  uint8_t voicesAssigned = 0;

  if (note > 127) note = 127;

  if (patch.mono_mode > 0) for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].noteOff(voices[voiceIndex].currentNote, 0);
  // noteManager.get

  // Prio 1: idle voices
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++)
  {
    if (voices[voiceIndex].currentNote == 0 && !voices[voiceIndex].inRelease())
    {
      voicesAssigned++;
      if (patch.polyMode != POLY_FULL) voices[voiceIndex].unisonDetune = unisonModifiers[voicesAssigned];
      else voices[voiceIndex].unisonDetune = 0.0;
      voices[voiceIndex].noteOn(note, velocity);
      voicesActive++;
      if (voicesAssigned >= patch.polyMode) return;
    }
  }
  
  // Prio 2: voices idle or in release
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++)
  {
    if (voices[voiceIndex].currentNote == 0)
    {
      voicesAssigned++;
      if (patch.polyMode != POLY_FULL) voices[voiceIndex].unisonDetune = unisonModifiers[voicesAssigned];
      else voices[voiceIndex].unisonDetune = 0.0;
      voices[voiceIndex].noteOn(note, velocity);
      voicesActive++;
      if (voicesAssigned >= patch.polyMode) return;
    }
  }

  // Prio 3: steal lowest note
  uint8_t lowestNote = 128;
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) lowestNote = min(lowestNote, voices[voiceIndex].currentNote);
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++)
  {
    if (voices[voiceIndex].currentNote == lowestNote)
    {
      voicesAssigned++;
      if (patch.polyMode != POLY_FULL) voices[voiceIndex].unisonDetune = unisonModifiers[voicesAssigned];
      else voices[voiceIndex].unisonDetune = 0.0;
      voices[voiceIndex].noteOn(note, velocity);
      if (voicesAssigned >= patch.polyMode) return;
    }
  }
}

void VoiceBank::noteOff(uint8_t note, uint8_t velocity)
{
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++)
  {
    if (voices[voiceIndex].currentNote == note) voices[voiceIndex].noteOff(note, velocity);
    voicesActive--;
  }
}

void VoiceBank::panic()
{
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++)
  {
    voices[voiceIndex].noteOff(voices[voiceIndex].currentNote, 0);
  }
  voicesActive = 0;
}

void VoiceBank::_connect(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput)
{
  _patchCords[_connectionIndex] = new AudioConnection(source, sourceOutput, destination , destinationInput);
  _connectionIndex++;
  if (_connectionIndex > NR_PATCHCORDS_VOICEBANK - 1)
  {
    _connectionIndex = NR_PATCHCORDS_VOICEBANK - 1;
    Serial.println("ERROR: Voice Bank - no available patchcords");
  }
}

void VoiceBank::adjustParameter(uint8_t parameter, int8_t delta)
{
  uint16_t targetValueU16 = 0;
  //uint32_t targetValueI32 = 0;
  uint8_t targetValueU8 = 0;
  int8_t targetValueI8 = 0;
  float targetValueF = 0.0;

  switch (parameter)
  {
    case OSC1_WAVEFORM:
      targetValueU8 = patch.osc1_waveform + delta;
      //patch.osc1_waveform = constrain(targetValueU8, 0, (NR_WAVEFORMS - 1) );
      if (targetValueU8 > (NR_WAVEFORMS - 1) ) targetValueU8 = 0;
      patch.osc1_waveform = targetValueU8;
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setOsc1Waveform();
      break;
    case OSC2_WAVEFORM:
      targetValueU8 = patch.osc2_waveform + delta;
      if (targetValueU8 > (NR_WAVEFORMS - 1) ) targetValueU8 = 0;
      patch.osc2_waveform = targetValueU8;
      //patch.osc2_waveform = constrain(targetValueU8, 0, (NR_WAVEFORMS - 1) );
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setOsc2Waveform();
      break;
    case TRANSPOSE:
      targetValueI8 = patch.transpose + delta;
      patch.transpose = constrain(targetValueI8, -24, 24);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].updateFrequency();
      break;
    case DETUNE:
      targetValueF = patch.detune + delta * 0.05;
      patch.detune = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].updateFrequency();
      break;
    case MONO_MODE:
      targetValueI8 = patch.mono_mode + delta;
      patch.mono_mode = constrain(targetValueI8, 0, 1);
      break;
    case OSC1_LEVEL:
      targetValueF = patch.osc1_level + delta * 0.05;
      patch.osc1_level = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setOscMixer(OSC1_LEVEL);
      break;
    case OSC2_LEVEL:
      targetValueF = patch.osc2_level + delta * 0.05;
      patch.osc2_level = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setOscMixer(OSC2_LEVEL);
      break;
    case PULSE_LEVEL:
      targetValueF = patch.pulse_level + delta * 0.05;
      patch.pulse_level = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setOscMixer(PULSE_LEVEL);
      break;
    case NOISE_LEVEL:
      targetValueF = patch.noise_level + delta * 0.05;
      patch.noise_level = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setOscMixer(NOISE_LEVEL);
      break;
    case OSC1_SYNC:
      targetValueI8 = patch.osc1_sync + delta;
      patch.osc1_sync = constrain(targetValueI8, 0, 1);
      break;
    case OSC2_SYNC:
      targetValueI8 = patch.osc2_sync + delta;
      patch.osc2_sync = constrain(targetValueI8, 0, 1);
      break;


    case ENV_ATTACK:
      if (patch.ampEnvelope_attack < 10) targetValueF = patch.ampEnvelope_attack + delta;
      else targetValueF = patch.ampEnvelope_attack + delta * 10;
      patch.ampEnvelope_attack = constrain(targetValueF, 0.0, 2000.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setAmpEnv();
      break;
    case ENV_DECAY:
      if (patch.ampEnvelope_decay < 10) targetValueF = patch.ampEnvelope_decay + delta;
      else targetValueF = patch.ampEnvelope_decay + delta * 10;
      patch.ampEnvelope_decay = constrain(targetValueF, 0.0, 2000.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setAmpEnv();
      break;
    case ENV_SUSTAIN:
      targetValueF = patch.ampEnvelope_sustain + delta * 0.1;
      patch.ampEnvelope_sustain = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setAmpEnv();
      break;
    case ENV_RELEASE:
      if (patch.ampEnvelope_release < 10) targetValueF = patch.ampEnvelope_release + delta;
      else if (patch.ampEnvelope_release < 100) targetValueF = patch.ampEnvelope_release + delta * 10;
      else targetValueF = patch.ampEnvelope_release + delta * 100;
      patch.ampEnvelope_release = constrain(targetValueF, 0.0, 5000.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setAmpEnv();
      break;
    case FILTER_ATTACK:
      if (patch.filterEnvelope_attack < 10) targetValueF = patch.filterEnvelope_attack + delta;
      else targetValueF = patch.filterEnvelope_attack + delta * 10;
      patch.filterEnvelope_attack = constrain(targetValueF, 0.0, 2000.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setFilterEnv();
      break;
    case FILTER_DECAY:
      if (patch.filterEnvelope_decay < 10) targetValueF = patch.filterEnvelope_decay + delta;
      else targetValueF = patch.filterEnvelope_decay + delta * 10;
      patch.filterEnvelope_decay = constrain(targetValueF, 0.0, 2000.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setFilterEnv();
      break;
    case FILTER_SUSTAIN:
      targetValueF = patch.filterEnvelope_sustain + delta * 0.1;
      patch.filterEnvelope_sustain = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setFilterEnv();
      break;
    case FILTER_RELEASE:
      if (patch.filterEnvelope_release < 10) targetValueF = patch.filterEnvelope_release + delta;
      else if (patch.filterEnvelope_release < 100) targetValueF = patch.filterEnvelope_release + delta * 10;
      else targetValueF = patch.filterEnvelope_release + delta * 100;
      patch.filterEnvelope_release = constrain(targetValueF, 0.0, 5000.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setFilterEnv();
      break;
    
    case FILTERENV_PWR:
      targetValueF = patch.envToFilter + delta * 0.1;
      patch.envToFilter = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_filter();
      break;

    case FILTER_CUTOFF:
      // if (patch.cutoff < 1) targetValueF = patch.cutoff + delta * 0.1;
      // else if (patch.cutoff < 10) targetValueF = patch.cutoff + delta;
      // else if (patch.cutoff < 100) targetValueF = patch.cutoff + delta * 10;
      // else if (patch.cutoff < 1000) targetValueF = patch.cutoff + delta * 100;
      // else targetValueF = patch.cutoff + delta * 500;
      targetValueF = patch.cutoff + delta * 0.02;
      //patch.cutoff = constrain(targetValueF, 0.0, FILTER_MAX_CUTOFF);
      patch.cutoff = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setFilter();
      break;
    case FILTER_RESONANCE:
      targetValueF = patch.resonance + delta * 0.05;
      patch.resonance = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setFilter();
      break;
    
    case ENV3_ATTACK:
      if (patch.envelope3_attack < 11) targetValueF = patch.envelope3_attack + delta;
      else targetValueF = patch.envelope3_attack + delta * 10;
      patch.envelope3_attack = constrain(targetValueF, 0.0, 2000.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setEnv3();
      break;
    case ENV3_DECAY:
      if (patch.envelope3_decay < 11) targetValueF = patch.envelope3_decay + delta;
      else targetValueF = patch.envelope3_decay + delta * 10;
      patch.envelope3_decay = constrain(targetValueF, 0.0, 2000.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setEnv3();
      break;
    case ENV3_SUSTAIN:
      targetValueF = patch.envelope3_sustain + delta * 0.1;
      patch.envelope3_sustain = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setEnv3();
      break;
    case ENV3_RELEASE:
      if (patch.envelope3_release < 11) targetValueF = patch.envelope3_release + delta;
      else targetValueF = patch.envelope3_release + delta * 10;
      patch.envelope3_release = constrain(targetValueF, 0.0, 5000.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setEnv3();
      break;
    

    case MOD_ENV3_OSC1_PITCH:
      targetValueF = patch.mod_env3_osc1_pitch + delta * 0.02;
      patch.mod_env3_osc1_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc1_pitch();
      break;
    case MOD_ENV3_OSC2_PITCH:
      targetValueF = patch.mod_env3_osc2_pitch + delta * 0.02;
      patch.mod_env3_osc2_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc2_pitch();
      break;
    case MOD_ENV3_OSC1_PHASE:
      targetValueF = patch.mod_env3_osc1_phase + delta * 0.02;
      patch.mod_env3_osc1_phase = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc1_phase();
      break;
    case MOD_ENV3_OSC2_PHASE:
      targetValueF = patch.mod_env3_osc2_phase + delta * 0.02;
      patch.mod_env3_osc2_phase = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc2_phase();
      break;
    case MOD_ENV3_FILTER_CUTOFF:
      targetValueF = patch.mod_env3_filter_cutoff + delta * 0.02;
      patch.mod_env3_filter_cutoff = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_filter();
      break;
    case MOD_ENV3_PWM:
      targetValueF = patch.mod_env3_pwm + delta * 0.02;
      patch.mod_env3_pwm = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_pwm();
      break;
    case MOD_ENV3_AM_PITCH:
      targetValueF = patch.mod_env3_am_pitch + delta * 0.02;
      patch.mod_env3_am_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_am_pitch();
      break;
    case MOD_ENV3_FM_PITCH:
      targetValueF = patch.mod_env3_fm_pitch + delta * 0.02;
      patch.mod_env3_fm_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_fm_pitch();
      break;
    
    case MOD_LFO1_OSC1_PITCH:
      targetValueF = patch.mod_lfo1_osc1_pitch + delta * 0.01;
      patch.mod_lfo1_osc1_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc1_pitch();
      break;
    case MOD_LFO1_OSC2_PITCH:
      targetValueF = patch.mod_lfo1_osc2_pitch + delta * 0.01;
      patch.mod_lfo1_osc2_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc2_pitch();
      break;
    case MOD_LFO1_OSC1_PHASE:
      targetValueF = patch.mod_lfo1_osc1_phase + delta * 0.02;
      patch.mod_lfo1_osc1_phase = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc1_phase();
      break;
    case MOD_LFO1_OSC2_PHASE:
      targetValueF = patch.mod_lfo1_osc2_phase + delta * 0.02;
      patch.mod_lfo1_osc2_phase = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc2_phase();
      break;
    case MOD_LFO1_FILTER_CUTOFF:
      targetValueF = patch.mod_lfo1_filter_cutoff + delta * 0.02;
      patch.mod_lfo1_filter_cutoff = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_filter();
      break;
    case MOD_LFO1_PWM:
      targetValueF = patch.mod_lfo1_pwm + delta * 0.02;
      patch.mod_lfo1_pwm = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_pwm();
      break;
    case MOD_LFO1_AM_PITCH:
      targetValueF = patch.mod_lfo1_am_pitch + delta * 0.02;
      patch.mod_lfo1_am_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_am_pitch();
      break;
    case MOD_LFO1_FM_PITCH:
      targetValueF = patch.mod_lfo1_fm_pitch + delta * 0.02;
      patch.mod_lfo1_fm_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_fm_pitch();
      break;

    case MOD_LFO2_OSC1_PITCH:
      targetValueF = patch.mod_lfo2_osc1_pitch + delta * 0.02;
      patch.mod_lfo2_osc1_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc1_pitch();
      break;
    case MOD_LFO2_OSC2_PITCH:
      targetValueF = patch.mod_lfo2_osc2_pitch + delta * 0.02;
      patch.mod_lfo2_osc2_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc2_pitch();
      break;
    case MOD_LFO2_OSC1_PHASE:
      targetValueF = patch.mod_lfo2_osc1_phase + delta * 0.02;
      patch.mod_lfo2_osc1_phase = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc1_phase();
      break;
    case MOD_LFO2_OSC2_PHASE:
      targetValueF = patch.mod_lfo2_osc2_phase + delta * 0.02;
      patch.mod_lfo2_osc2_phase = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc2_phase();
      break;
    case MOD_LFO2_FILTER_CUTOFF:
      targetValueF = patch.mod_lfo2_filter_cutoff + delta * 0.02;
      patch.mod_lfo2_filter_cutoff = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_filter();
      break;
    case MOD_LFO2_PWM:
      targetValueF = patch.mod_lfo2_pwm + delta * 0.02;
      patch.mod_lfo2_pwm = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_pwm();
      break;
    case MOD_LFO2_AM_PITCH:
      targetValueF = patch.mod_lfo2_am_pitch + delta * 0.02;
      patch.mod_lfo2_am_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_am_pitch();
      break;
    case MOD_LFO2_FM_PITCH:
      targetValueF = patch.mod_lfo2_fm_pitch + delta * 0.02;
      patch.mod_lfo2_fm_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_fm_pitch();
      break;
    
    case MOD_OSC1_OSC2_PITCH:
      targetValueF = patch.mod_osc1_osc2_pitch + delta * 0.01;
      patch.mod_osc1_osc2_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc2_pitch();
      break;
    case MOD_OSC2_OSC1_PITCH:
      targetValueF = patch.mod_osc2_osc1_pitch + delta * 0.01;
      patch.mod_osc2_osc1_pitch = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc1_pitch();
      break;
    case MOD_OSC1_OSC2_PHASE:
      targetValueF = patch.mod_osc1_osc2_phase + delta * 0.02;
      patch.mod_osc1_osc2_phase = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc2_phase();
      break;
    case MOD_OSC2_OSC1_PHASE:
      targetValueF = patch.mod_osc2_osc1_phase + delta * 0.02;
      patch.mod_osc2_osc1_phase = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_modMixer_osc1_phase();
      break;

    case AM_FREQ_MULTIPLIER:
      targetValueF = patch.am_frequency_multiplier + delta * 0.02;
      patch.am_frequency_multiplier = constrain(targetValueF, 0, 16.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].updateFrequency();
      break;
    case AM_LEVEL:
      targetValueF = patch.am_level + delta * 0.05;
      patch.am_level = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setAmplitudeModulation();
      break;
    case AM_WAVEFORM:
      targetValueU8 = patch.osc_am_waveform + delta;
      patch.osc_am_waveform = constrain(targetValueU8, 0, (nrWaveforms - 1) );
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setOscAmWaveform();
      break;
    case AM_FIXEDFREQUENCY:
      targetValueU8 = patch.am_fixedFrequency + delta;
      patch.am_fixedFrequency = constrain(targetValueU8, 0, 1);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].updateFrequency();
      break;
    
    case FM_FREQ_MULTIPLIER:
      targetValueF = patch.fm_frequency_multiplier + delta * 0.02;
      patch.fm_frequency_multiplier = constrain(targetValueF, 0, 16);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].updateFrequency();
      break;
    case FM_LEVEL:
      targetValueF = patch.fm_level + delta * 0.01;
      patch.fm_level = constrain(targetValueF, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setFrequencyModulation();
      break;
    case FM_WAVEFORM:
      targetValueU8 = patch.osc_fm_waveform + delta;
      patch.osc_fm_waveform = constrain(targetValueU8, 0, (nrWaveforms - 1) );
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setOscFmWaveform();
      break;
    case FM_OFFSET:
      targetValueF = patch.fm_offset + delta * 0.01;
      patch.fm_offset = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setFrequencyModulation();
      break;

    case EFX_SEND_DRY:
      targetValueF = patch.dryLevel + delta * 0.05;
      patch.dryLevel = constrain(targetValueF, 0.0, 1.0);
      output_dry_L.gain(_getPannedLevel(patch.dryLevel, LEFT));
      output_dry_R.gain(_getPannedLevel(patch.dryLevel, RIGHT));
      break;
    case EFX_SEND_REVERB:
      targetValueF = patch.reverbSend + delta * 0.05;
      patch.reverbSend = constrain(targetValueF, 0.0, 1.0);
      output_reverbSend_L.gain(patch.reverbSend);
      output_reverbSend_R.gain(patch.reverbSend);
      break;
    case EFX_SEND_CHORUS:
      targetValueF = patch.chorusSend + delta * 0.05;
      patch.chorusSend = constrain(targetValueF, 0.0, 1.0);
      output_chorusSend.gain(patch.chorusSend);
      break;
    //case EFX_SEND_PHASER:
      //targetValueF = patch.phaserSend + delta * 0.05;
      //patch.phaserSend = constrain(targetValueF, 0.0, 2.0);
      //output_phaserSend.gain(patch.phaserSend);
      //break;
    case EFX_SEND_DELAY:
      targetValueF = patch.delaySend + delta * 0.05;
      patch.delaySend = constrain(targetValueF, 0.0, 1.0);
      output_delaySend.gain(patch.delaySend);
      break;
    case EFX_PAN:
      targetValueF = patch.pan + delta * 0.05;
      patch.pan = constrain(targetValueF, -1.0, 1.0);
      setEfx();
      break;
    case LFO1_WAVEFORM:
      targetValueU8 = patch.lfo1_waveform + delta;
      if (targetValueU8 > (nrWaveforms - 1) ) targetValueU8 = 0;
      patch.lfo1_waveform = targetValueU8;
      _lfo1.begin(waveformList[patch.lfo1_waveform]);
      break;
    case LFO2_WAVEFORM:
      targetValueU8 = patch.lfo2_waveform + delta;
      if (targetValueU8 > (nrWaveforms - 1) ) targetValueU8 = 0;
      patch.lfo2_waveform = targetValueU8;
      _lfo2.begin(waveformList[patch.lfo2_waveform]);
      break;
    case LFO1_FREQUENCY:
      targetValueF = patch.lfo1Frequency + delta * 0.1;
      patch.lfo1Frequency = constrain(targetValueF, 0.0, 20.0);
      _lfo1.frequency(patch.lfo1Frequency);
      break;
    case LFO2_FREQUENCY:
      targetValueF = patch.lfo2Frequency + delta * 0.1;
      patch.lfo2Frequency = constrain(targetValueF, 0.0, 20.0);
      _lfo2.frequency(patch.lfo2Frequency);
      break;
    case LFO1_AMPLITUDE:
      targetValueF = patch.lfo1Level + delta * 0.05;
      patch.lfo1Level = constrain(targetValueF, 0.0, 1.0);
      _lfo1.amplitude(patch.lfo1Level);
      break;
    case LFO2_AMPLITUDE:
      targetValueF = patch.lfo2Level + delta * 0.05;
      patch.lfo2Level = constrain(targetValueF, 0.0, 1.0);
      _lfo2.amplitude(patch.lfo2Level);
      break;
  
    case MOD_ENV3_LFO1_AMPLITUDE:
      targetValueF = patch.mod_env3_lfo1_amplitude + delta * 0.05;
      patch.mod_env3_lfo1_amplitude = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setLfo1AmplitudeModulation();
      break;
    case MOD_ENV3_LFO2_AMPLITUDE:
      targetValueF = patch.mod_env3_lfo2_amplitude + delta * 0.05;
      patch.mod_env3_lfo2_amplitude = constrain(targetValueF, -1.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setLfo2AmplitudeModulation();
      break;


    case OSC1_WAVETABLE_INDEX:
      targetValueI8 = patch.osc1_waveTable_index + delta;
      if (targetValueI8 > (NR_WAVETABLES - 1) ) targetValueI8 = 0;
      if (targetValueI8 < 0) targetValueI8 = NR_WAVETABLES - 1;
      patch.osc1_waveTable_index = (uint8_t)targetValueI8;
      importWaveTable(waveTables[patch.osc1_waveTable_index], waveTable1_I16); // FIX for multitimbral
      break;
    case OSC1_WAVETABLE_MODE:
      targetValueI8 = patch.osc1_waveTable_mode + delta;
      if (targetValueI8 > 1 ) targetValueI8 = 0;
      if (targetValueI8 < 0) targetValueI8 = 1;
      patch.osc1_waveTable_mode = (uint8_t)targetValueI8;
      break;
    case OSC1_WAVETABLE_START:
    {
      targetValueU16 = patch.osc1_waveTable_start + delta * 64;
      uint16_t maxValue = 2 * WAVETABLE_LENGTH - patch.osc1_waveTable_length - 1;
      if (targetValueU16 > maxValue) targetValueU16 = 0;
      patch.osc1_waveTable_start = targetValueU16;
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_osc1_waveOffset();
      break;
    }
    case OSC1_WAVETABLE_LENGTH:
    {
      targetValueU16 = patch.osc1_waveTable_length + delta * 16;
      uint16_t maxValue = 2* WAVETABLE_LENGTH - patch.osc1_waveTable_start - 1;
      if (targetValueU16 > maxValue) targetValueU16 = 0;
      patch.osc1_waveTable_length = targetValueU16;
      break;
    }
    case OSC1_WAVETABLE_INTERVAL:
      targetValueU16 = patch.osc1_waveTable_interval + delta;
      if (targetValueU16 > 300) targetValueU16 = 0;
      patch.osc1_waveTable_interval = targetValueU16;
      break;
    case OSC1_WAVETABLE_STEPSIZE:
      targetValueU16 = patch.osc1_waveTable_stepSize + delta;
      if (targetValueU16 > 256) targetValueU16 = 0;
      patch.osc1_waveTable_stepSize = targetValueU16;
      break;
    case OSC1_WAVETABLE_MOVEMENT:
      targetValueU8 = patch.osc1_waveTable_movement + delta;
      targetValueU8 = constrain(targetValueU8, 0,1);
      patch.osc1_waveTable_movement = targetValueU8;
      break;

    case OSC2_WAVETABLE_INDEX:
      targetValueI8 = patch.osc2_waveTable_index + delta;
      if (targetValueI8 > (NR_WAVETABLES - 1) ) targetValueI8 = 0;
      if (targetValueI8 < 0) targetValueI8 = NR_WAVETABLES - 1;
      patch.osc2_waveTable_index = (uint8_t)targetValueI8;
      importWaveTable(waveTables[patch.osc2_waveTable_index], waveTable2_I16); // FIX for multitimbral
      break;
    case OSC2_WAVETABLE_MODE:
      targetValueI8 = patch.osc2_waveTable_mode + delta;
      if (targetValueI8 > 1 ) targetValueI8 = 0;
      if (targetValueI8 < 0) targetValueI8 = 1;
      patch.osc2_waveTable_mode = (uint8_t)targetValueI8;
      break;
    case OSC2_WAVETABLE_START:
    {
      targetValueU16 = patch.osc2_waveTable_start + delta * 64;
      uint16_t maxValue = 2 * WAVETABLE_LENGTH - patch.osc2_waveTable_length - 1;
      if (targetValueU16 > maxValue) targetValueU16 = 0;
      patch.osc2_waveTable_start = targetValueU16;
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].set_osc2_waveOffset();
      break;
    }
    case OSC2_WAVETABLE_LENGTH:
    {
      targetValueU16 = patch.osc2_waveTable_length + delta * 16;
      uint16_t maxValue = 2 * WAVETABLE_LENGTH - patch.osc2_waveTable_start - 1;
      if (targetValueU16 > maxValue) targetValueU16 = 0;
      patch.osc2_waveTable_length = targetValueU16;
      break;
    }
    case OSC2_WAVETABLE_INTERVAL:
      targetValueU16 = patch.osc2_waveTable_interval + delta;
      if (targetValueU16 > 300) targetValueU16 = 0;
      patch.osc2_waveTable_interval = targetValueU16;
      break;
    case OSC2_WAVETABLE_STEPSIZE:
      targetValueU16 = patch.osc2_waveTable_stepSize + delta;
      if (targetValueU16 > 256) targetValueU16 = 0;
      patch.osc2_waveTable_stepSize = targetValueU16;
      break;
    case OSC2_WAVETABLE_MOVEMENT:
      targetValueU8 = patch.osc2_waveTable_movement + delta;
      targetValueU8 = constrain(targetValueU8, 0,1);
      patch.osc2_waveTable_movement = targetValueU8;
      break;


    case POLY_MODE:
      if (delta > 0) targetValueU8 = patch.polyMode << 1;
      else targetValueU8 = patch.polyMode >> 1;
      if (targetValueU8 < 1) targetValueU8 = 1;
      if (targetValueU8 > 8) targetValueU8 = 8;
      patch.polyMode = targetValueU8;
      break;
    
    case HPFILTER_CUTOFF:
      targetValueF = patch.hpfilter_cutoff + delta * 0.02;
      patch.hpfilter_cutoff = constrain(targetValueF, 0.0, 1.0);
      _dcOffsetFilter.frequency(12.0 + patch.hpfilter_cutoff * 400.0);
      break;
    case WAVESHAPER_INDEX:
      targetValueU8 = patch.waveshaper_index + delta;
      patch.waveshaper_index = constrain(targetValueU8, 0, NR_WAVESHAPE_MODELS);
      _setWaveshaper();
      break;
    case WAVESHAPER_LEVEL:
      targetValueF = patch.waveshaper_level + delta * 0.05;
      patch.waveshaper_level = constrain(targetValueF, 0.0, 1.0);
      _setWaveshaper();
      break;
  }
}

void VoiceBank::_setWaveshaper()
{
  const uint16_t shapeArrayLength = 65;
  float maxLevel = 1.0 - patch.waveshaper_level * 0.5;

  switch (patch.waveshaper_index)
  {
    case 0:
      // off
      _waveShaper.shape(waveshaper_default, 3);
      break;
    case 1:
      // "odr" crossfade linear and sine 
      for (uint16_t i = 0; i < shapeArrayLength; i++)
      {
        float inputF = -1.0 + i * (2.0 / (shapeArrayLength - 1) ); // x : [-1, 1]
        waveShapeArray[i] = (1 - patch.waveshaper_level) * inputF + patch.waveshaper_level * sin(1.570796 * inputF); // (1-level) * x + level * sin(pi * x)
        waveShapeArray[i] = constrain(waveShapeArray[i], -1.0, 1.0);
        //Serial.println(shapeArray[i]);
      }
      _waveShaper.shape(waveShapeArray, shapeArrayLength);
      break;

    case 2:
      // "dist" squeeze sine in x direction, both ways
      if (patch.waveshaper_level < 0.01)
      {
        _waveShaper.shape(waveshaper_default, 3);
        break;
      }

      
      for (uint16_t i = 0; i < shapeArrayLength; i++)
      {
        float x = -1.0 + i * (2.0 / (shapeArrayLength - 1) ); // x : [-1, 1]
        if (x < (-1.0 + patch.waveshaper_level) ) waveShapeArray[i] = -1.0 * maxLevel;
        else if ( x > (1.0 - patch.waveshaper_level) ) waveShapeArray[i] = 1.0 * maxLevel;
        else waveShapeArray[i] = maxLevel * sin(1.570796 * x / (1.0 - patch.waveshaper_level) );
        //waveShapeArray[i] = constrain(waveShapeArray[i], -1.0, 1.0);
        waveShapeArray[i] = constrain(waveShapeArray[i], -maxLevel, maxLevel);
        //Serial.println(shapeArray[i]);
      }
      _waveShaper.shape(waveShapeArray, shapeArrayLength);
      break;

    case 3:
      // experiments
      if (patch.waveshaper_level < 0.05)
      {
        _waveShaper.shape(waveshaper_default, 3);
        break;
      }

      for (uint16_t i = 0; i < shapeArrayLength; i++)
      {
        float inputF = -1.0 + i * (2.0 / (shapeArrayLength - 1) ); // x : [-1, 1]
        waveShapeArray[i] = (1 - patch.waveshaper_level) * inputF + patch.waveshaper_level * sin(3.1415926 * inputF); // (1-level) * x + level * sin(pi * x)
        waveShapeArray[i] = constrain(waveShapeArray[i], -1.0, 1.0);
        //Serial.println(shapeArray[i]);
      }

      _waveShaper.shape(waveShapeArray, shapeArrayLength);
      break;

  }
}

void VoiceBank::setParameter(uint8_t parameter, float value)
{
  switch (parameter)
  {
    case FILTER_CUTOFF:
      patch.cutoff = constrain(value, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setFilter();
      break;
    case FILTER_RESONANCE:
      patch.resonance = constrain(value, 0.0, 1.0);
      for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].setFilter();
      break;
  }
}

void VoiceBank::setPitchBend(int pitchBend)
{
  
  float bendFactor = pitchBend / 8192.0;
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++)
  {
    voices[voiceIndex].pitchBend = bendFactor;
    voices[voiceIndex].updateFrequency();
  }
}

float VoiceBank::_getPannedLevel(float baseLevel, uint8_t channel)
{
  float level_L = baseLevel;
  float level_R = baseLevel;
  if (patch.pan > 0.01) level_L = baseLevel * (1 - abs(patch.pan));   // pan right
  if (patch.pan < 0.01) level_R = baseLevel * (1 - abs(patch.pan));   // pan right
  
  if (channel == LEFT) return level_L;
  else return level_R;
}

void VoiceBank::setEfx()
{
  output_dry_L.gain(_getPannedLevel(patch.dryLevel, LEFT));
  output_dry_R.gain(_getPannedLevel(patch.dryLevel, RIGHT));
  output_reverbSend_L.gain(patch.reverbSend);
  output_reverbSend_R.gain(patch.reverbSend);
  output_chorusSend.gain(patch.chorusSend);
  //output_phaserSend.gain(patch.phaserSend);
  output_delaySend.gain(patch.delaySend);
}

void VoiceBank::applyPatchData()
{
  _lfo1.begin(waveformList[patch.lfo1_waveform]);
  _lfo2.begin(waveformList[patch.lfo2_waveform]);
  _lfo1.amplitude(patch.lfo1Level);
  _lfo2.amplitude(patch.lfo2Level);
  _lfo1.frequency(patch.lfo1Frequency);
  _lfo2.frequency(patch.lfo2Frequency);
  _dcOffsetFilter.frequency(patch.hpfilter_cutoff);
  _setWaveshaper();
  setEfx();
  importWaveTable(waveTables[patch.osc1_waveTable_index], activeWaveTables[2 * id]);
  importWaveTable(waveTables[patch.osc2_waveTable_index], activeWaveTables[2 * id + 1]);
  
  for (uint8_t voiceIndex = 0; voiceIndex < NR_VOICES; voiceIndex++) voices[voiceIndex].applyPatchData();
}

void VoiceBank::importWaveTable(const unsigned int * waveTableI32, int16_t * waveTableI16)
{
  for (uint32_t index = 1; index < ARBITRARY_LENGTH; index++) waveTableI16[index - 1] = waveTableI32[index];// first value is identifier so skip
  // for (uint32_t index = 1; index < ARBITRARY_LENGTH; index++)  // first value is identifier so skip
  // {
  //   const int32_t preScaler = 65536;
  //   int32_t sample1 = waveTableI32[index];
  //   int32_t sample2 = waveTableI32[index + 1];
  //   waveTableI16[2 * index - 1] = sample1 / preScaler;
  //   if (index == (ARBITRARY_LENGTH - 1)) waveTableI16[2 * index] = sample1 / preScaler;
  //   else waveTableI16[2 * index] = (sample1 / (2* preScaler)) + (sample2 / (2* preScaler));
  // }
}