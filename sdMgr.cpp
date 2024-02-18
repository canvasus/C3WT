#include "sdMgr.h"

PatchInfo patchInfo;
String patchNameUI[NR_VOICEBANKS] = {"000: INIT PATCH", "000: INIT PATCH"};
String peekPatchNameUI = "INIT PATCH";
uint8_t currentPatchNr[NR_VOICEBANKS] = {0, 0};
uint8_t peekPatchNr = 0;
uint8_t currentCharPosition = 0;

FLASHMEM void initSDcard()
{
  delay(200);
  Serial.print(F("Init SD card..."));
  while (!SD.begin(CS_SD)) delay(200);
  checkFileSystem();
  Serial.println(F("done"));
}

//https://arduinojson.org/v6/example/config/

FLASHMEM uint8_t loadPatch(uint8_t patchNr)
{
  char fileName[14];
  sprintf(fileName, "P3_PATCHJ_%03d", patchNr);

  File file = SD.open(fileName);
  if (!file) return FILE_NOT_EXIST;

  StaticJsonDocument<5208> doc;
  Patch patch;
  AudioParameters tempAudioPar;

  // // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    Serial.println(F("Failed to read file, using default configuration"));
    return INVALID_FORMAT;
  } 
  // // Copy values from the JsonDocument to the Config

  strlcpy(patchInfo.name,                  // <- destination
          doc["name"] | "INIT PATCH",  // <- source
          sizeof(patchInfo.name)); 

  peekPatchNameUI = patchInfo.name;

  patch.osc1_waveform =doc["osc1_waveform"];
  patch.osc2_waveform = doc["osc2_waveform"];

  patch.transpose = doc["transpose"];
  patch.detune = doc["detune"];
  patch.polyMode = doc["polyMode"];
  patch.mono_mode = doc["mono_mode"];

  patch.osc1_level = doc["osc1_level"];
  patch.osc2_level = doc["osc2_level"];
  patch.pulse_level =  doc["pulse_level"];
  patch.noise_level = doc["noise_level"];

  patch.phaseModulation = doc["phaseModulation"];
  patch.frequencyModulation = doc["frequencyModulation"];

  patch.ampEnvelope_attack = doc["ampEnvelope_attack"];
  patch.ampEnvelope_decay = doc["ampEnvelope_decay"];
  patch.ampEnvelope_sustain = doc["ampEnvelope_sustain"];
  patch.ampEnvelope_release = doc["ampEnvelope_release"];

  patch.filterEnvelope_attack = doc["filterEnvelope_attack"];
  patch.filterEnvelope_decay = doc["filterEnvelope_decay"];
  patch.filterEnvelope_sustain = doc["filterEnvelope_sustain"];
  patch.filterEnvelope_release = doc["filterEnvelope_release"];
  patch.octaveControl = doc["octaveControl"];
  patch.envToFilter = doc["envToFilter"];
  patch.cutoff = doc["cutoff"];
  patch.resonance = doc["resonance"];
  patch.hpfilter_cutoff = doc["hpfilter_cutoff"]; 
  
  patch.envelope3_attack = doc["envelope3_attack"];
  patch.envelope3_decay = doc["envelope3_decay"];
  patch.envelope3_sustain = doc["envelope3_sustain"];
  patch.envelope3_release = doc["envelope3_release"];
  
  patch.mod_env3_osc1_pitch = doc["mod_env3_osc1_pitch"];
  patch.mod_env3_osc2_pitch = doc["mod_env3_osc2_pitch"];
  patch.mod_env3_osc1_phase = doc["mod_env3_osc1_phase"];
  patch.mod_env3_osc2_phase = doc["mod_env3_osc2_phase"];
  patch.mod_env3_filter_cutoff = doc["mod_env3_filter_cutoff"];
  patch.mod_env3_pwm = doc["mod_env3_pwm"];
  patch.mod_env3_am_pitch = doc["mod_env3_am_pitch"];
  patch.mod_env3_fm_pitch = doc["mod_env3_fm_pitch"];
  patch.mod_env3_lfo1_amplitude = doc["mod_env3_lfo1_ampl"];
  patch.mod_env3_lfo2_amplitude = doc["mod_env3_lfo2_ampl"];
  
  patch.mod_lfo1_osc1_pitch = doc["mod_lfo1_osc1_pitch"];
  patch.mod_lfo1_osc2_pitch = doc["mod_lfo1_osc2_pitch"];
  patch.mod_lfo1_osc1_phase = doc["mod_lfo1_osc1_phase"];
  patch.mod_lfo1_osc2_phase = doc["mod_lfo1_osc2_phase"];
  patch.mod_lfo1_filter_cutoff = doc["mod_lfo1_filter_cutoff"];
  patch.mod_lfo1_pwm = doc["mod_lfo1_pwm"];
  patch.mod_lfo1_am_pitch = doc["mod_lfo1_am_pitch"];
  patch.mod_lfo1_fm_pitch = doc["mod_lfo1_fm_pitch"];

  patch.mod_lfo2_osc1_pitch = doc["mod_lfo2_osc1_pitch"];
  patch.mod_lfo2_osc2_pitch = doc["mod_lfo2_osc2_pitch"];
  patch.mod_lfo2_osc1_phase = doc["mod_lfo2_osc1_phase"];
  patch.mod_lfo2_osc2_phase = doc["mod_lfo2_osc2_phase"];
  patch.mod_lfo2_filter_cutoff = doc["mod_lfo2_filter_cutoff"];
  patch.mod_lfo2_pwm = doc["mod_lfo2_pwm"];
  patch.mod_lfo2_am_pitch = doc["mod_lfo2_am_pitch"];
  patch.mod_lfo2_fm_pitch = doc["mod_lfo2_fm_pitch"];

  patch.mod_osc1_osc2_pitch = doc["mod_osc1_osc2_pitch"];
  patch.mod_osc2_osc1_pitch = doc["mod_osc2_osc1_pitch"];
  patch.mod_osc1_osc2_phase = doc["mod_osc1_osc2_phase"];
  patch.mod_osc2_osc1_phase = doc["mod_osc2_osc1_phase"];

  patch.mod_velocity_osc1_pitch = doc["mod_velocity_osc1_pitch"];
  patch.mod_velocity_osc2_pitch = doc["mod_velocity_osc2_pitch"];
  patch.mod_velocity_osc1_phase = doc["mod_velocity_osc1_phase"];
  patch.mod_velocity_osc2_phase = doc["mod_velocity_osc2_phase"];
  patch.mod_velocity_filter_cutoff = doc["mod_velocity_filter_cutoff"];
  patch.mod_velocity_pwm = doc["mod_velocity_pwm"];

  patch.mod_wheel_osc1_pitch = doc["mod_wheel_osc1_pitch"];
  patch.mod_wheel_osc2_pitch = doc["mod_wheel_osc2_pitch"];
  patch.mod_wheel_osc1_phase = doc["mod_wheel_osc1_phase"];
  patch.mod_wheel_osc2_phase = doc["mod_wheel_osc2_phase"];
  patch.mod_wheel_filter_cutoff = doc["mod_wheel_filter_cutoff"];
  patch.mod_wheel_pwm = doc["mod_wheel_pwm"];

  patch.lfo1Frequency = doc["lfo1Frequency"];
  patch.lfo2Frequency = doc["lfo2Frequency"];
  patch.lfo1_waveform = doc["lfo1_waveform"];
  patch.lfo2_waveform = doc["lfo2_waveform"];
  patch.lfo1Level = doc["lfo1Level"];
  patch.lfo2Level = doc["lfo2Level"];

  patch.fm_frequency_multiplier = doc["fm_frequency_multiplier"];
  patch.fm_level = doc["fm_level"];
  patch.osc_fm_waveform = doc["osc_fm_waveform"];
  patch.fm_offset = doc["fm_offset"];

  patch.am_frequency_multiplier = doc["am_frequency_multiplier"];
  patch.am_level = doc["am_level"];
  patch.osc_am_waveform = doc["osc_am_waveform"];
  patch.am_fixedFrequency = doc["am_fixedFrequency"];

  patch.dryLevel = doc["dryLevel"];
  patch.pan = doc["pan"];
  patch.reverbSend = doc["reverbSend"];
  patch.chorusSend = doc["chorusSend"];
  patch.delaySend = doc["delaySend"];
  patch.phaserSend = doc["phaserSend"];

  patch.osc1_waveTable_mode = doc["osc1_waveTable_mode"];
  patch.osc1_waveTable_index = doc["osc1_waveTable_index"];
  patch.osc1_waveTable_start = doc["osc1_waveTable_start"];
  patch.osc1_waveTable_length = doc["osc1_waveTable_length"];
  patch.osc1_waveTable_interval = doc["osc1_waveTable_interval"];
  patch.osc1_waveTable_stepSize = doc["osc1_waveTable_stepSize"];
  patch.osc1_waveTable_movement = doc["osc1_waveTable_movement"];

  patch.osc2_waveTable_mode = doc["osc2_waveTable_mode"];
  patch.osc2_waveTable_index = doc["osc2_waveTable_index"];
  patch.osc2_waveTable_start = doc["osc2_waveTable_start"];
  patch.osc2_waveTable_length = doc["osc2_waveTable_length"];
  patch.osc2_waveTable_interval = doc["osc2_waveTable_interval"];
  patch.osc2_waveTable_stepSize = doc["osc2_waveTable_stepSize"];
  patch.osc2_waveTable_movement = doc["osc2_waveTable_movement"];
  
  patch.midi_channel = doc["midi_channel"] | 1;
  patch.midi_lowLimit = doc["midi_lowLimit"] | 0;
  patch.midi_highLimit = doc["midi_highLimit"] | 127;
  patch.midi_transpose = doc["midi_transpose"] | 0; 
  patch.arp_mode = doc["arp_mode"] | 0;
  patch.arp_intervalTicks = doc["arp_intervalTicks"] | 24;
  patch.arp_offsetTicks = doc["arp_offsetTicks"] | 0;
  patch.arp_octaves = doc["arp_octaves"] | 0;


  // AUDIO PARAMETERS
  tempAudioPar.hpVolume =doc["hpVolume"];
  tempAudioPar.usbGain = doc["usbGain"];
  tempAudioPar.reverb_size = doc["reverb_size"];
  tempAudioPar.reverb_hidamp = doc["reverb_hidamp"];
  tempAudioPar.reverb_lodamp = doc["reverb_lodamp"];
  tempAudioPar.reverb_lowpass = doc["reverb_lowpass"];
  tempAudioPar.reverb_diffusion =doc["reverb_diffusion"];
  tempAudioPar.reverb_feedback = doc["reverb_feedback"];
  tempAudioPar.granular_speed = doc["granular_speed"];
  //tempAudioPar.granular_length = doc["granular_length"];
  tempAudioPar.chorus_lfoRate = doc["chorus_lfoRate"];
  tempAudioPar.phaser_lfoRate = doc["phaser_lfoRate"];
  tempAudioPar.phaser_stages = doc["phaser_stages"];
  tempAudioPar.phaser_feedback = doc["phaser_feedback"];
  tempAudioPar.phaser_mix = doc["phaser_mix"];
  tempAudioPar.delay_feedback = doc["delay_feedback"];
  tempAudioPar.delay_time = doc["delay_time"];
  tempAudioPar.delay_type = doc["delay_type"];
  tempAudioPar.reverb_masterLevel = doc["reverb_masterLevel"];
  tempAudioPar.chorus_masterLevel = doc["chorus_masterLevel"];
  tempAudioPar.delay_masterLevel = doc["delay_masterLevel"];
  tempAudioPar.phaser_masterLevel = doc["phaser_masterLevel"];

  file.close();

  voiceBanks[currentVoiceBank]->patch = patch;
  voiceBanks[currentVoiceBank]->applyPatchData();
    
  if (currentVoiceBank == 0)  // NOTE TEMPORARY FIX
  {
    audioParameters = tempAudioPar;
    applyAudioParameters();
  }

  char  buffer[16];
  sprintf(buffer, "%03d: %s", patchNr, patchInfo.name);
  patchNameUI[currentVoiceBank] = buffer;
  currentPatchNr[currentVoiceBank] = patchNr;
  peekPatchNr = patchNr;
  return LOAD_OK;
}

// Saves the configuration to a file
FLASHMEM void savePatch(uint8_t patchNr)
{
  char fileName[14];
  sprintf(fileName, "P3_PATCHJ_%03d", patchNr);
  SD.remove(fileName);

  File file = SD.open(fileName, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  StaticJsonDocument<5208> doc;

  // Set the values in the document
  doc["name"] = patchInfo.name;

  doc["osc1_waveform"] = voiceBanks[currentVoiceBank]->patch.osc1_waveform;
  doc["osc2_waveform"] = voiceBanks[currentVoiceBank]->patch.osc2_waveform;

  doc["transpose"] = voiceBanks[currentVoiceBank]->patch.transpose;
  doc["detune"] = voiceBanks[currentVoiceBank]->patch.detune;
  doc["polyMode"] = voiceBanks[currentVoiceBank]->patch.polyMode;
  doc["mono_mode"] = voiceBanks[currentVoiceBank]->patch.mono_mode;
 
  doc["osc1_level"] = voiceBanks[currentVoiceBank]->patch.osc1_level;
  doc["osc2_level"] = voiceBanks[currentVoiceBank]->patch.osc2_level;
  doc["pulse_level"] = voiceBanks[currentVoiceBank]->patch.pulse_level;
  doc["noise_level"] = voiceBanks[currentVoiceBank]->patch.noise_level;

  doc["phaseModulation"] = voiceBanks[currentVoiceBank]->patch.phaseModulation;
  doc["frequencyModulation"] = voiceBanks[currentVoiceBank]->patch.frequencyModulation;

  doc["ampEnvelope_attack"] = voiceBanks[currentVoiceBank]->patch.ampEnvelope_attack;
  doc["ampEnvelope_decay"] = voiceBanks[currentVoiceBank]->patch.ampEnvelope_decay;
  doc["ampEnvelope_sustain"] = voiceBanks[currentVoiceBank]->patch.ampEnvelope_sustain;
  doc["ampEnvelope_release"] = voiceBanks[currentVoiceBank]->patch.ampEnvelope_release;

  doc["filterEnvelope_attack"] = voiceBanks[currentVoiceBank]->patch.filterEnvelope_attack;
  doc["filterEnvelope_decay"] = voiceBanks[currentVoiceBank]->patch.filterEnvelope_decay;
  doc["filterEnvelope_sustain"] = voiceBanks[currentVoiceBank]->patch.filterEnvelope_sustain;
  doc["filterEnvelope_release"] = voiceBanks[currentVoiceBank]->patch.filterEnvelope_release;
  doc["octaveControl"] = voiceBanks[currentVoiceBank]->patch.octaveControl;
  doc["envToFilter"] = voiceBanks[currentVoiceBank]->patch.envToFilter;
  doc["cutoff"] = voiceBanks[currentVoiceBank]->patch.cutoff;
  doc["resonance"] = voiceBanks[currentVoiceBank]->patch.resonance;
  doc["hpfilter_cutoff"] = voiceBanks[currentVoiceBank]->patch.hpfilter_cutoff;
    
  doc["envelope3_attack"] = voiceBanks[currentVoiceBank]->patch.envelope3_attack;
  doc["envelope3_decay"] = voiceBanks[currentVoiceBank]->patch.envelope3_decay;
  doc["envelope3_sustain"] = voiceBanks[currentVoiceBank]->patch.envelope3_sustain;
  doc["envelope3_release"] = voiceBanks[currentVoiceBank]->patch.envelope3_release;
  
  doc["mod_env3_osc1_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_env3_osc1_pitch;
  doc["mod_env3_osc2_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_env3_osc2_pitch;
  doc["mod_env3_osc1_phase"] = voiceBanks[currentVoiceBank]->patch.mod_env3_osc1_phase;
  doc["mod_env3_osc2_phase"] = voiceBanks[currentVoiceBank]->patch.mod_env3_osc2_phase;
  doc["mod_env3_filter_cutoff"] = voiceBanks[currentVoiceBank]->patch.mod_env3_filter_cutoff;
  doc["mod_env3_pwm"] = voiceBanks[currentVoiceBank]->patch.mod_env3_pwm;
  doc["mod_env3_am_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_env3_am_pitch;
  doc["mod_env3_fm_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_env3_fm_pitch;

  doc["mod_env3_lfo1_ampl"] = voiceBanks[currentVoiceBank]->patch.mod_env3_lfo1_amplitude;
  doc["mod_env3_lfo2_ampl"] = voiceBanks[currentVoiceBank]->patch.mod_env3_lfo2_amplitude;

  doc["mod_lfo1_osc1_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_lfo1_osc1_pitch;
  doc["mod_lfo1_osc2_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_lfo1_osc2_pitch;
  doc["mod_lfo1_osc1_phase"] = voiceBanks[currentVoiceBank]->patch.mod_lfo1_osc1_phase;
  doc["mod_lfo1_osc2_phase"] = voiceBanks[currentVoiceBank]->patch.mod_lfo1_osc2_phase;
  doc["mod_lfo1_filter_cutoff"] = voiceBanks[currentVoiceBank]->patch.mod_lfo1_filter_cutoff;
  doc["mod_lfo1_pwm"] = voiceBanks[currentVoiceBank]->patch.mod_lfo1_pwm;
  doc["mod_lfo1_am_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_lfo1_am_pitch;
  doc["mod_lfo1_fm_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_lfo1_fm_pitch;

  doc["mod_lfo2_osc1_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_lfo2_osc1_pitch;
  doc["mod_lfo2_osc2_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_lfo2_osc2_pitch;
  doc["mod_lfo2_osc1_phase"] = voiceBanks[currentVoiceBank]->patch.mod_lfo2_osc1_phase;
  doc["mod_lfo2_osc2_phase"] = voiceBanks[currentVoiceBank]->patch.mod_lfo2_osc2_phase;
  doc["mod_lfo2_filter_cutoff"] = voiceBanks[currentVoiceBank]->patch.mod_lfo2_filter_cutoff;
  doc["mod_lfo2_pwm"] = voiceBanks[currentVoiceBank]->patch.mod_lfo2_pwm;
  doc["mod_lfo2_am_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_lfo2_am_pitch;
  doc["mod_lfo2_fm_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_lfo2_fm_pitch;

  doc["mod_osc1_osc2_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_osc1_osc2_pitch;
  doc["mod_osc2_osc1_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_osc2_osc1_pitch;
  doc["mod_osc1_osc2_phase"] = voiceBanks[currentVoiceBank]->patch.mod_osc1_osc2_phase;
  doc["mod_osc2_osc1_phase"] = voiceBanks[currentVoiceBank]->patch.mod_osc2_osc1_phase;

  doc["mod_velocity_osc1_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_velocity_osc1_pitch;
  doc["mod_velocity_osc2_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_velocity_osc2_pitch;
  doc["mod_velocity_osc1_phase"] = voiceBanks[currentVoiceBank]->patch.mod_velocity_osc1_phase;
  doc["mod_velocity_osc2_phase"] = voiceBanks[currentVoiceBank]->patch.mod_velocity_osc2_phase;
  doc["mod_velocity_filter_cutoff"] = voiceBanks[currentVoiceBank]->patch.mod_velocity_filter_cutoff;
  doc["mod_velocity_pwm"] = voiceBanks[currentVoiceBank]->patch.mod_velocity_pwm;

  doc["mod_wheel_osc1_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_wheel_osc1_pitch;
  doc["mod_wheel_osc2_pitch"] = voiceBanks[currentVoiceBank]->patch.mod_wheel_osc2_pitch;
  doc["mod_wheel_osc1_phase"] = voiceBanks[currentVoiceBank]->patch.mod_wheel_osc1_phase;
  doc["mod_wheel_osc2_phase"] = voiceBanks[currentVoiceBank]->patch.mod_wheel_osc2_phase;
  doc["mod_wheel_filter_cutoff"] = voiceBanks[currentVoiceBank]->patch.mod_wheel_filter_cutoff;
  doc["mod_wheel_pwm"] = voiceBanks[currentVoiceBank]->patch.mod_wheel_pwm;

  doc["lfo1Frequency"] = voiceBanks[currentVoiceBank]->patch.lfo1Frequency;
  doc["lfo2Frequency"] = voiceBanks[currentVoiceBank]->patch.lfo2Frequency;
  doc["lfo1_waveform"] = voiceBanks[currentVoiceBank]->patch.lfo1_waveform;
  doc["lfo2_waveform"] = voiceBanks[currentVoiceBank]->patch.lfo2_waveform;
  doc["lfo1Level"] = voiceBanks[currentVoiceBank]->patch.lfo1Level;
  doc["lfo2Level"] = voiceBanks[currentVoiceBank]->patch.lfo2Level;

  doc["fm_frequency_multiplier"] = voiceBanks[currentVoiceBank]->patch.fm_frequency_multiplier;
  doc["fm_level"] = voiceBanks[currentVoiceBank]->patch.fm_level;
  doc["osc_fm_waveform"] = voiceBanks[currentVoiceBank]->patch.osc_fm_waveform;
  doc["fm_offset"] = voiceBanks[currentVoiceBank]->patch.fm_offset;

  doc["am_frequency_multiplier"] = voiceBanks[currentVoiceBank]->patch.am_frequency_multiplier;
  doc["am_level"] = voiceBanks[currentVoiceBank]->patch.am_level;
  doc["osc_am_waveform"] = voiceBanks[currentVoiceBank]->patch.osc_am_waveform;
  doc["am_fixedFrequency"] = voiceBanks[currentVoiceBank]->patch.am_fixedFrequency;


  doc["dryLevel"] = voiceBanks[currentVoiceBank]->patch.dryLevel;
  doc["pan"] = voiceBanks[currentVoiceBank]->patch.pan;
  doc["reverbSend"] = voiceBanks[currentVoiceBank]->patch.reverbSend;
  doc["chorusSend"] = voiceBanks[currentVoiceBank]->patch.chorusSend;
  doc["delaySend"] = voiceBanks[currentVoiceBank]->patch.delaySend;
  doc["phaserSend"] = voiceBanks[currentVoiceBank]->patch.phaserSend;

  doc["osc1_waveTable_mode"] = voiceBanks[currentVoiceBank]->patch.osc1_waveTable_mode;
  doc["osc1_waveTable_index"] = voiceBanks[currentVoiceBank]->patch.osc1_waveTable_index;
  doc["osc1_waveTable_start"] = voiceBanks[currentVoiceBank]->patch.osc1_waveTable_start;
  doc["osc1_waveTable_length"] = voiceBanks[currentVoiceBank]->patch.osc1_waveTable_length;
  doc["osc1_waveTable_interval"] = voiceBanks[currentVoiceBank]->patch.osc1_waveTable_interval;
  doc["osc1_waveTable_stepSize"] = voiceBanks[currentVoiceBank]->patch.osc1_waveTable_stepSize;
  doc["osc1_waveTable_movement"] = voiceBanks[currentVoiceBank]->patch.osc1_waveTable_movement;
  
  doc["osc2_waveTable_mode"] = voiceBanks[currentVoiceBank]->patch.osc2_waveTable_mode;
  doc["osc2_waveTable_index"] = voiceBanks[currentVoiceBank]->patch.osc2_waveTable_index;
  doc["osc2_waveTable_start"] = voiceBanks[currentVoiceBank]->patch.osc2_waveTable_start;
  doc["osc2_waveTable_length"] = voiceBanks[currentVoiceBank]->patch.osc2_waveTable_length;
  doc["osc2_waveTable_interval"] = voiceBanks[currentVoiceBank]->patch.osc2_waveTable_interval;
  doc["osc2_waveTable_stepSize"] = voiceBanks[currentVoiceBank]->patch.osc2_waveTable_stepSize;
  doc["osc2_waveTable_movement"] = voiceBanks[currentVoiceBank]->patch.osc2_waveTable_movement;

  doc["midi_channel"] = voiceBanks[currentVoiceBank]->patch.midi_channel;
  doc["midi_lowLimit"] = voiceBanks[currentVoiceBank]->patch.midi_lowLimit;
  doc["midi_highLimit"] = voiceBanks[currentVoiceBank]->patch.midi_highLimit;
  doc["midi_transpose"] = voiceBanks[currentVoiceBank]->patch.midi_transpose;
  doc["arp_mode"] = voiceBanks[currentVoiceBank]->patch.arp_mode;
  doc["arp_intervalTicks"] = voiceBanks[currentVoiceBank]->patch.arp_intervalTicks;
  doc["arp_offsetTicks"] = voiceBanks[currentVoiceBank]->patch.arp_offsetTicks;
  doc["arp_octaves"] = voiceBanks[currentVoiceBank]->patch.arp_octaves;


  // AUDIO PARAMETERS
  doc["hpVolume"] = audioParameters.hpVolume;
  doc["usbGain"] = audioParameters.usbGain;
  doc["reverb_size"] = audioParameters.reverb_size;
  doc["reverb_hidamp"] = audioParameters.reverb_hidamp;
  doc["reverb_lodamp"] = audioParameters.reverb_lodamp;
  doc["reverb_lowpass"] = audioParameters.reverb_lowpass;
  doc["reverb_diffusion"] = audioParameters.reverb_diffusion;
  doc["reverb_feedback"] = audioParameters.reverb_feedback;
  doc["granular_speed"] = audioParameters.granular_speed;
  //doc["granular_length"] = audioParameters.granular_length;
  doc["chorus_lfoRate"] = audioParameters.chorus_lfoRate;
  doc["phaser_lfoRate"] = audioParameters.phaser_lfoRate;
  doc["phaser_stages"] = audioParameters.phaser_stages;
  doc["phaser_feedback"] = audioParameters.phaser_feedback;
  doc["phaser_mix"] = audioParameters.phaser_mix;
  doc["delay_feedback"] = audioParameters.delay_feedback;
  doc["delay_time"] = audioParameters.delay_time;
  doc["delay_type"] = audioParameters.delay_type;
  doc["reverb_masterLevel"] = audioParameters.reverb_masterLevel;
  doc["chorus_masterLevel"] = audioParameters.chorus_masterLevel;
  doc["delay_masterLevel"] = audioParameters.delay_masterLevel;
  doc["phaser_masterLevel"] = audioParameters.phaser_masterLevel;
 

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
 
  char  buffer[16];
  sprintf(buffer, "%03d: %s", patchNr, patchInfo.name);
  patchNameUI[currentVoiceBank] = buffer;
}

uint8_t peekPatchName(uint8_t patchNr)
{
  peekPatchNr = patchNr;
  char fileName[14];
  sprintf(fileName, "P3_PATCHJ_%03d", patchNr);

  File file = SD.open(fileName);

  StaticJsonDocument<5208> doc;

  // // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error) Serial.println(F("Read json failed, using default"));

  char buf[PATCH_NAME_NR_CHARACTERS];
  strlcpy(buf,                  // <- destination
          doc["name"] | "INIT PATCH",  // <- source
          PATCH_NAME_NR_CHARACTERS); 

  peekPatchNameUI = String(buf);

  return 0;
}

FLASHMEM void checkFileSystem()
{
  for (uint8_t patchNr = 0; patchNr < NR_PATCHES; patchNr++)
  {
    char fileName[14];
    sprintf(fileName, "P3_PATCHJ_%03d", patchNr);
    if (!SD.exists(fileName)) savePatch(patchNr);
  }
}

// FLASHMEM bool readWaveHeader(const char *filename, wav_header &header, wav_data_header &wav_data_header)
// {
//   bool result = true;
//   File wavFile = SD.open(filename);
//   if (!wavFile) {
//       Serial.printf("Not able to open wave file... %s\n", filename);
//       result = false;
//   }
        
//   if (result) {
//       wavFile.seek(36);
//       unsigned char buffer[8];
//       size_t bytesRead = wavFile.read(buffer, 8);
//       if (bytesRead != 8) {
//           Serial.printf("Not able to read header... %s\n", filename);
//           result = false;
//       }

//       if (result) {
//           unsigned infoTagsSize;
//           result = readInfoTags(buffer, 0, infoTagsSize);

//           if (result) {
//               wavFile.seek(36 + infoTagsSize);
//               bytesRead = wavFile.read(buffer, 8);
//               if (bytesRead != 8) {
//                   Serial.printf("Not able to read header... %s\n", filename);
//                   return false;
//               }

//               result = readDataHeader(buffer, 0, wav_data_header);
//           }
//       }
//   }
//   wavFile.close();
//   return result;
// }

// FLASHMEM bool readInfoTags(unsigned char *buffer, size_t offset, unsigned &infoTagsSize)
// {
  //     if (    buffer[offset+0] == 'L' 
  //           && buffer[offset+1] == 'I' 
  //           && buffer[offset+2] == 'S' 
  //           && buffer[offset+3] == 'T') {
  //         infoTagsSize = static_cast<uint32_t>(buffer[offset+7] << 24 | buffer[offset+6] << 16 | buffer[offset+5] << 8 | buffer[offset+4]);    
  //         infoTagsSize += 8;
  //         return true;
  //     }

  //     if (    buffer[offset+0] == 'd' 
  //           && buffer[offset+1] == 'a' 
  //           && buffer[offset+2] == 't' 
  //           && buffer[offset+3] == 'a') {
  //         infoTagsSize = 0;
  //         return true;
  //     }

  //     Serial.println("expected 'data' or 'LIST'...");
  //     return false;
  // }