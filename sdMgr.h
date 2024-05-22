#pragma once

#include <SD.h>
#include <ArduinoJson.h>
#include "voice.h"
#include "audioFunc.h"
#include "ui.h"

#define CS_SD BUILTIN_SDCARD

#define NR_PATCHES 150
#define PATCH_NAME_NR_CHARACTERS 11

#define LOAD_OK         0
#define INVALID_FORMAT  1
#define FILE_NOT_EXIST  2

struct PatchInfo
{
  uint16_t dataSize = 0;
  char name[PATCH_NAME_NR_CHARACTERS] = {'I', 'N', 'I', 'T', ' ', 'P', 'A', 'T', 'C', 'H'};
};

#define DATA_SIZE sizeof(Patch) + sizeof(AudioParameters)
#define JSON_DOC_SIZE 5800

extern PatchInfo patchInfo;
extern String patchNameUI[NR_VOICEBANKS];
extern String peekPatchNameUI;
extern uint8_t currentPatchNr[NR_VOICEBANKS];
extern uint8_t peekPatchNr;
extern uint8_t currentCharPosition;

void initSDcard();
void checkFileSystem();
uint8_t peekPatchName(uint8_t patchNr);
uint8_t loadPatch(uint8_t patchNr);
void savePatch(uint8_t patchNr);
Patch populatePatchFromDoc(const JsonDocument& doc);
AudioParameters populateAudioParametersFromDoc(const JsonDocument& doc);

void updateSerial();
void sendPatchData(uint8_t bankNr);
void receivePatchData(String buffer, uint8_t bankNr);
void dumpAllPatchData();