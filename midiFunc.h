#pragma once

#include <MIDI.h>
#include "voice.h"
#include "audioFunc.h"
#include <USBHost_t36.h>

#define RESOLUTION 4 // 1/16th
#define MAX_ARP_NOTES 8

#define ARP_IDLE 0
#define ARP_PLAYING 1

#define ARP_OFF  0
#define ARP_UP   1
#define ARP_DOWN 2

#define NR_ARP_MODES 3

#define SYS_MIDICHANNEL 0

extern uint8_t noteStatus[128];
extern uint8_t midiActivity;
extern bool usbDeviceStatus;
extern bool usbPcStatus;

struct MidiSettings
{
  uint8_t channel = 1;
  uint8_t bpm = 120;
  uint32_t oneTickUs = 1000 * 60000 / (120 * 24);
  uint16_t masterClock = 0;
  uint8_t arp_mode = ARP_OFF;
  uint16_t arp_intervalTicks = 16;
  uint8_t arp_noteLength = 8;
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
    uint32_t _tick = 0;
    uint16_t _playedTime = 0;
    uint8_t _step = 0;
    uint8_t _state = ARP_IDLE;
    MidiSettings * _settings;
    VoiceBank * _voiceBank;
    void _printNotes();
  
  public:
    Arpeggiator(MidiSettings * settings, VoiceBank * voiceBank); // { _settings = settings;}
    void tick();
    void addNote(uint8_t note);
    void removeNote(uint8_t note);
};

extern Arpeggiator arpeggiator;

void setupMidi();
void updateMidi();

void checkUsbStatus();

void myNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void myNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void myControlChange(uint8_t channel, uint8_t control, uint8_t value);
void myPitchBend(uint8_t channel, int pitchBend);

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