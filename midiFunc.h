#pragma once

#include <MIDI.h>
#include "voice.h"
#include "audioFunc.h"
#include <USBHost_t36.h>

#define RESOLUTION 24 // 24ppq
#define MAX_ARP_NOTES 8

#define ARP_IDLE 0
#define ARP_PLAYING 1

#define ARP_OFF  0
#define ARP_UP   1
#define ARP_DOWN 2
#define ARP_CYCLE  3
#define ARP_CHORD 4

#define NR_ARP_MODES 4

#define ARP_DIRECTION_UP 0
#define ARP_DIRECTION_DOWN 1

#define SYS_BANK_A_MIDICHANNEL  0
#define SYS_BANK_B_MIDICHANNEL  1
#define SYS_BANK_A_LIMIT_LOW    2
#define SYS_BANK_A_LIMIT_HIGH   3
#define SYS_BANK_B_LIMIT_LOW    4
#define SYS_BANK_B_LIMIT_HIGH   5
#define SYS_BANK_A_TRANSPOSE    6
#define SYS_BANK_B_TRANSPOSE    7

#define SYS_SIDECHAIN_CHANNEL 8
#define SYS_SIDECHAIN_NOTE 9

#define SYS_BANK_A_ARP_MODE 10
#define SYS_BANK_B_ARP_MODE 11
#define SYS_BPM 12
#define SYS_BANK_A_ARP_INTERVAL 13
#define SYS_BANK_B_ARP_INTERVAL 14
#define SYS_BANK_A_ARP_OFFSET 15
#define SYS_BANK_B_ARP_OFFSET 16
#define SYS_BANK_A_ARP_OCTAVES 17
#define SYS_BANK_B_ARP_OCTAVES 18


extern uint8_t noteStatus[128];
extern uint8_t midiActivity;
extern bool usbDeviceStatus;
extern bool usbPcStatus;
extern char arpModeNames[NR_ARP_MODES][6];

struct MidiSettings
{
  uint8_t bpm = 120;
  uint32_t oneTickUs = 1000 * 60000 / (120 * RESOLUTION);
  uint16_t masterClock = 0;
  
  uint8_t sideChain_channel = 9;
  uint8_t sideChain_note = 36;
};

extern MidiSettings midiSettings;

struct Event
{
  uint8_t type = 0;
  uint8_t note = 0;
  uint8_t velocity = 127;
};

class Arpeggiator
{
  private:
    uint8_t _notesPressed[MAX_ARP_NOTES];
    uint8_t _nrNotesPressed = 0;
    uint16_t _playedTime = 0;
    uint8_t _step = 0;
    uint8_t _octave = 0;
    uint8_t _direction = ARP_DIRECTION_UP;
    bool _oldStepTrigger = false;
    uint8_t _notePlaying = 0;
    MidiSettings * _midiSettings;
    VoiceBank * _voiceBank;
    void _printNotes();
  
  public:
    //Arpeggiator(MidiSettings * settings, VoiceBank * voiceBank, uint8_t * arpMode, uint8_t * intervalTicks);
    Arpeggiator(MidiSettings * midiSettings, VoiceBank * voiceBank);
    //void tick();
    void update();
    void addNote(uint8_t note);
    void removeNote(uint8_t note);
    //uint8_t * offsetTicks;
    void reset();
};

//extern Arpeggiator arpeggiator;

void setupMidi();
void updateMidi();

void checkUsbStatus();

void myNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void myNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void myControlChange(uint8_t channel, uint8_t control, uint8_t value);
void myPitchBend(uint8_t channel, int pitchBend);
void myProgramChange(uint8_t channel, uint8_t program);
void mySystemExclusive(uint8_t *data, unsigned int length);

void myMIDIClock();
void myMIDIClockStart();
void myMIDIClockStop();

void tickMasterClock();
void updateArpeggiator();

void adjustMidiParameter(uint8_t parameter, int8_t delta);

void setArpeggiatorMode(uint8_t dummy, int8_t value);
void adjustBpm(uint8_t dummy, int8_t delta);

int compare (const void * a, const void * b);

#define CC_MODWHEEL 1
#define CC_PANIC    123