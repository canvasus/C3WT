#include <SD.h>

#define CS_SD BUILTIN_SDCARD

uint16_t keys[14] = [0,1,2,3,4,5,6,7,8,9,10,11,12,13];

uint8_t nr_keys = 14;

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
};

class KeyValuePairParser
{
  public:
    uint8_t readFile(File * file);
    uint16_t getNextKey();
    uint8_t getValue(float * parameterPtr);
    uint8_t getValue(uint8_t * parameterPtr);
    uint8_t getValue(int8_t * parameterPtr);
    uint8_t getValue(char* parameterPtr);
    uint8_t writeKeyValue(uint16_t key, float value);
};

uint8_t KeyValuePairParser::readFile(File * file)
{

}

KeyValuePairParser parser;

void setup()
{
  Serial.print(F("Init SD card..."));
  while (!SD.begin(CS_SD)) delay(200);
  Serial.println(F("done"));
}

void loop()
{
  // put your main code here, to run repeatedly:

}

void writeFile(uint8_t patchNr)
{
  char fileName[17];
  sprintf(fileName, "P3_PATCH_KVP_%03d", patchNr);
  SD.remove(fileName);
  
  File file = SD.open(fileName, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  for(uint16_t key = 0; key < nr_keys, key++)
  {
    
  }

}

void readFile()
{

}