#include "sdMgr.h"

PatchInfo patchInfo;
String patchNameUI = "000: INIT PATCH";
String peekPatchNameUI = "INIT PATCH";
uint8_t currentPatchNr = 0;
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

  StaticJsonDocument<4096> doc;
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

  patch.osc2_waveTable_mode = doc["osc2_waveTable_mode"];
  patch.osc2_waveTable_index = doc["osc2_waveTable_index"];
  patch.osc2_waveTable_start = doc["osc2_waveTable_start"];
  patch.osc2_waveTable_length = doc["osc2_waveTable_length"];
  patch.osc2_waveTable_interval = doc["osc2_waveTable_interval"];
  patch.osc2_waveTable_stepSize = doc["osc2_waveTable_stepSize"];
  
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

  voiceBank1.patch =  patch;
  voiceBank1.applyPatchData();
  
  audioParameters = tempAudioPar;
  applyAudioParameters();

  char  buffer[16];
  sprintf(buffer, "%03d: %s", patchNr, patchInfo.name);
  patchNameUI = buffer;
  currentPatchNr = patchNr;
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

  StaticJsonDocument<4096> doc;

  // Set the values in the document
  doc["name"] = patchInfo.name;

  doc["osc1_waveform"] = voiceBank1.patch.osc1_waveform;
  doc["osc2_waveform"] = voiceBank1.patch.osc2_waveform;

  doc["transpose"] = voiceBank1.patch.transpose;
  doc["detune"] = voiceBank1.patch.detune;

  doc["osc1_level"] = voiceBank1.patch.osc1_level;
  doc["osc2_level"] = voiceBank1.patch.osc2_level;
  doc["pulse_level"] = voiceBank1.patch.pulse_level;
  doc["noise_level"] = voiceBank1.patch.noise_level;

  doc["phaseModulation"] = voiceBank1.patch.phaseModulation;
  doc["frequencyModulation"] = voiceBank1.patch.frequencyModulation;

  doc["ampEnvelope_attack"] = voiceBank1.patch.ampEnvelope_attack;
  doc["ampEnvelope_decay"] = voiceBank1.patch.ampEnvelope_decay;
  doc["ampEnvelope_sustain"] = voiceBank1.patch.ampEnvelope_sustain;
  doc["ampEnvelope_release"] = voiceBank1.patch.ampEnvelope_release;

  doc["filterEnvelope_attack"] = voiceBank1.patch.filterEnvelope_attack;
  doc["filterEnvelope_decay"] = voiceBank1.patch.filterEnvelope_decay;
  doc["filterEnvelope_sustain"] = voiceBank1.patch.filterEnvelope_sustain;
  doc["filterEnvelope_release"] = voiceBank1.patch.filterEnvelope_release;
  doc["octaveControl"] = voiceBank1.patch.octaveControl;
  doc["envToFilter"] = voiceBank1.patch.envToFilter;
  doc["cutoff"] = voiceBank1.patch.cutoff;
  doc["resonance"] = voiceBank1.patch.resonance;
  
  doc["envelope3_attack"] = voiceBank1.patch.envelope3_attack;
  doc["envelope3_decay"] = voiceBank1.patch.envelope3_decay;
  doc["envelope3_sustain"] = voiceBank1.patch.envelope3_sustain;
  doc["envelope3_release"] = voiceBank1.patch.envelope3_release;
  
  doc["mod_env3_osc1_pitch"] = voiceBank1.patch.mod_env3_osc1_pitch;
  doc["mod_env3_osc2_pitch"] = voiceBank1.patch.mod_env3_osc2_pitch;
  doc["mod_env3_osc1_phase"] = voiceBank1.patch.mod_env3_osc1_phase;
  doc["mod_env3_osc2_phase"] = voiceBank1.patch.mod_env3_osc2_phase;
  doc["mod_env3_filter_cutoff"] = voiceBank1.patch.mod_env3_filter_cutoff;
  doc["mod_env3_pwm"] = voiceBank1.patch.mod_env3_pwm;
  doc["mod_env3_am_pitch"] = voiceBank1.patch.mod_env3_am_pitch;
  doc["mod_env3_fm_pitch"] = voiceBank1.patch.mod_env3_fm_pitch;

  doc["mod_env3_lfo1_ampl"] = voiceBank1.patch.mod_env3_lfo1_amplitude;
  doc["mod_env3_lfo2_ampl"] = voiceBank1.patch.mod_env3_lfo2_amplitude;

  doc["mod_lfo1_osc1_pitch"] = voiceBank1.patch.mod_lfo1_osc1_pitch;
  doc["mod_lfo1_osc2_pitch"] = voiceBank1.patch.mod_lfo1_osc2_pitch;
  doc["mod_lfo1_osc1_phase"] = voiceBank1.patch.mod_lfo1_osc1_phase;
  doc["mod_lfo1_osc2_phase"] = voiceBank1.patch.mod_lfo1_osc2_phase;
  doc["mod_lfo1_filter_cutoff"] = voiceBank1.patch.mod_lfo1_filter_cutoff;
  doc["mod_lfo1_pwm"] = voiceBank1.patch.mod_lfo1_pwm;
  doc["mod_lfo1_am_pitch"] = voiceBank1.patch.mod_lfo1_am_pitch;
  doc["mod_lfo1_fm_pitch"] = voiceBank1.patch.mod_lfo1_fm_pitch;

  doc["mod_lfo2_osc1_pitch"] = voiceBank1.patch.mod_lfo2_osc1_pitch;
  doc["mod_lfo2_osc2_pitch"] = voiceBank1.patch.mod_lfo2_osc2_pitch;
  doc["mod_lfo2_osc1_phase"] = voiceBank1.patch.mod_lfo2_osc1_phase;
  doc["mod_lfo2_osc2_phase"] = voiceBank1.patch.mod_lfo2_osc2_phase;
  doc["mod_lfo2_filter_cutoff"] = voiceBank1.patch.mod_lfo2_filter_cutoff;
  doc["mod_lfo2_pwm"] = voiceBank1.patch.mod_lfo2_pwm;
  doc["mod_lfo2_am_pitch"] = voiceBank1.patch.mod_lfo2_am_pitch;
  doc["mod_lfo2_fm_pitch"] = voiceBank1.patch.mod_lfo2_fm_pitch;

  doc["mod_osc1_osc2_pitch"] = voiceBank1.patch.mod_osc1_osc2_pitch;
  doc["mod_osc2_osc1_pitch"] = voiceBank1.patch.mod_osc2_osc1_pitch;
  doc["mod_osc1_osc2_phase"] = voiceBank1.patch.mod_osc1_osc2_phase;
  doc["mod_osc2_osc1_phase"] = voiceBank1.patch.mod_osc2_osc1_phase;

  doc["mod_velocity_osc1_pitch"] = voiceBank1.patch.mod_velocity_osc1_pitch;
  doc["mod_velocity_osc2_pitch"] = voiceBank1.patch.mod_velocity_osc2_pitch;
  doc["mod_velocity_osc1_phase"] = voiceBank1.patch.mod_velocity_osc1_phase;
  doc["mod_velocity_osc2_phase"] = voiceBank1.patch.mod_velocity_osc2_phase;
  doc["mod_velocity_filter_cutoff"] = voiceBank1.patch.mod_velocity_filter_cutoff;
  doc["mod_velocity_pwm"] = voiceBank1.patch.mod_velocity_pwm;

  doc["mod_wheel_osc1_pitch"] = voiceBank1.patch.mod_wheel_osc1_pitch;
  doc["mod_wheel_osc2_pitch"] = voiceBank1.patch.mod_wheel_osc2_pitch;
  doc["mod_wheel_osc1_phase"] = voiceBank1.patch.mod_wheel_osc1_phase;
  doc["mod_wheel_osc2_phase"] = voiceBank1.patch.mod_wheel_osc2_phase;
  doc["mod_wheel_filter_cutoff"] = voiceBank1.patch.mod_wheel_filter_cutoff;
  doc["mod_wheel_pwm"] = voiceBank1.patch.mod_wheel_pwm;

  doc["lfo1Frequency"] = voiceBank1.patch.lfo1Frequency;
  doc["lfo2Frequency"] = voiceBank1.patch.lfo2Frequency;
  doc["lfo1_waveform"] = voiceBank1.patch.lfo1_waveform;
  doc["lfo2_waveform"] = voiceBank1.patch.lfo2_waveform;
  doc["lfo1Level"] = voiceBank1.patch.lfo1Level;
  doc["lfo2Level"] = voiceBank1.patch.lfo2Level;

  doc["fm_frequency_multiplier"] = voiceBank1.patch.fm_frequency_multiplier;
  doc["fm_level"] = voiceBank1.patch.fm_level;
  doc["osc_fm_waveform"] = voiceBank1.patch.osc_fm_waveform;
  doc["fm_offset"] = voiceBank1.patch.fm_offset;

  doc["am_frequency_multiplier"] = voiceBank1.patch.am_frequency_multiplier;
  doc["am_level"] = voiceBank1.patch.am_level;
  doc["osc_am_waveform"] = voiceBank1.patch.osc_am_waveform;
  doc["am_fixedFrequency"] = voiceBank1.patch.am_fixedFrequency;



  doc["dryLevel"] = voiceBank1.patch.dryLevel;
  doc["pan"] = voiceBank1.patch.pan;
  doc["reverbSend"] = voiceBank1.patch.reverbSend;
  doc["chorusSend"] = voiceBank1.patch.chorusSend;
  doc["delaySend"] = voiceBank1.patch.delaySend;
  doc["phaserSend"] = voiceBank1.patch.phaserSend;

  doc["osc1_waveTable_mode"] = voiceBank1.patch.osc1_waveTable_mode;
  doc["osc1_waveTable_index"] = voiceBank1.patch.osc1_waveTable_index;
  doc["osc1_waveTable_start"] = voiceBank1.patch.osc1_waveTable_start;
  doc["osc1_waveTable_length"] = voiceBank1.patch.osc1_waveTable_length;
  doc["osc1_waveTable_interval"] = voiceBank1.patch.osc1_waveTable_interval;
  doc["osc1_waveTable_stepSize"] = voiceBank1.patch.osc1_waveTable_stepSize;

  doc["osc2_waveTable_mode"] = voiceBank1.patch.osc2_waveTable_mode;
  doc["osc2_waveTable_index"] = voiceBank1.patch.osc2_waveTable_index;
  doc["osc2_waveTable_start"] = voiceBank1.patch.osc2_waveTable_start;
  doc["osc2_waveTable_length"] = voiceBank1.patch.osc2_waveTable_length;
  doc["osc2_waveTable_interval"] = voiceBank1.patch.osc2_waveTable_interval;
  doc["osc2_waveTable_stepSize"] = voiceBank1.patch.osc2_waveTable_stepSize;

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
  patchNameUI = buffer;
}

uint8_t peekPatchName(uint8_t patchNr)
{
  peekPatchNr = patchNr;
  char fileName[14];
  sprintf(fileName, "P3_PATCHJ_%03d", patchNr);

  File file = SD.open(fileName);

  StaticJsonDocument<4096> doc;

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