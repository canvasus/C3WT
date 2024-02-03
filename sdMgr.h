#pragma once

#include <SD.h>
#include <ArduinoJson.h>
#include "voice.h"
#include "audioFunc.h"

#define CS_SD BUILTIN_SDCARD

#define NR_PATCHES 100
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

// struct wav_header {
//     // RIFF Header
//     char riff_header[4] = {0,0,0,0};    // 00 - 03 - Contains "RIFF"
//     int header_chunk_size = 0;  // 04 - 07 - Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
//     char wave_header[4] = {0,0,0,0};    // 08 - 11 - Contains "WAVE"

//     // Format Header
//     char fmt_header[4] = {0,0,0,0};     // 12 - 15 - Contains "fmt " (includes trailing space)
//     int fmt_chunk_size = 0;     // 16 - 19 - Should be 16 for PCM
//     short audio_format = 0;     // 20 - 21 - Should be 1 for PCM. 3 for IEEE Float
//     short num_channels = 0;     // 22 - 23
//     int sample_rate = 0;        // 24 - 27
//     int byte_rate = 0;          // 28 - 31
//     short sample_alignment = 0; // 32 - 33
//     short bit_depth  = 0;        // 34 - 35
// };

// struct wav_data_header {
//     // Data
//     char data_header[4] = {0,0,0,0};    // 36 - 39
//     unsigned int data_bytes = 0;// 40 - 43
// };

// bool readInfoTags(unsigned char *buffer, size_t offset, unsigned &infoTagsSize);
// bool readWaveHeader(const char *filename, wav_header &header, wav_data_header &wav_data_header);