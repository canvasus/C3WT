#include "midiFunc.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, serialMIDI);

MidiSettings midiSettings;

IntervalTimer masterClockTimer;
Arpeggiator arpeggiator(&midiSettings, &voiceBank1);


USBHost myusb;
USBHub hub1(myusb);
MIDIDevice_BigBuffer midi1(myusb);
//MIDIDevice_BigBuffer midi2(myusb);
//MIDIDevice_BigBuffer midi3(myusb);

uint8_t noteStatus[128];

void setupMidi()
{
  Serial.println("MIDI SETUP");

  myusb.begin();
  delay(200);
  
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleControlChange(myControlChange);
  usbMIDI.setHandlePitchChange(myPitchBend);
  serialMIDI.begin(1);
  serialMIDI.setHandleNoteOn(myNoteOn);
  serialMIDI.setHandleNoteOff(myNoteOff);
  serialMIDI.setHandleControlChange(myControlChange);
  serialMIDI.setHandlePitchBend(myPitchBend);

  midi1.setHandleNoteOn(myNoteOn);
  midi1.setHandleNoteOff(myNoteOff);
  midi1.setHandleControlChange(myControlChange);
  midi1.setHandlePitchChange(myPitchBend);

  masterClockTimer.begin(&tickMasterClock, midiSettings.oneTickUs);
}

void updateMidi()
{
  usbMIDI.read();
  serialMIDI.read();
  midi1.read();
  //updateArpeggiator();
}

void myNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
  noteStatus[note] = velocity;
  if(midiSettings.arp_mode == ARP_OFF) voiceBank1.noteOn(note, velocity);
  else arpeggiator.addNote(note);
}

void myNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  noteStatus[note] = 0;
  if(midiSettings.arp_mode == ARP_OFF) voiceBank1.noteOff(note, velocity);
  else arpeggiator.removeNote(note);
}

void myControlChange(uint8_t channel, uint8_t control, uint8_t value)
{
  switch (control)
  {
    case CC_MODWHEEL:
      voiceBank1.setFilterCutoff(value << 3);
      break;
  }
}

void myPitchBend(uint8_t channel, int pitchBend)
{
  voiceBank1.setPitchBend(pitchBend);
}

void tickMasterClock() { arpeggiator.tick(); }

void setArpeggiatorMode(uint8_t dummy, int8_t delta)
{
  midiSettings.arp_mode = constrain(midiSettings.arp_mode + delta, 0, NR_ARP_MODES - 1);
}

void adjustBpm(uint8_t dummy, int8_t delta)
{
  midiSettings.bpm = constrain(midiSettings.bpm + delta, 20, 300);
  midiSettings.oneTickUs = 1000 * 60000 / (midiSettings.bpm * 32);
  masterClockTimer.update(midiSettings.oneTickUs);
}

Arpeggiator::Arpeggiator(MidiSettings * settings, VoiceBank * voiceBank)
{
  _settings = settings;
  _voiceBank = voiceBank;
  memset(_notesPressed, 255, MAX_ARP_NOTES);
}

void Arpeggiator::tick()
{

  if ( (midiSettings.arp_mode != ARP_OFF) && (_nrNotesPressed > 0) )
  {
    _tick++;

    if( (_state == ARP_IDLE) && (_tick > midiSettings.arp_intervalTicks) )
    {
       _voiceBank->noteOn(_notesPressed[_step], 127);
       _state = ARP_PLAYING;
       _playedTime = 0;
       return;
    }
    
    if( (_state == ARP_PLAYING) && ( _playedTime > midiSettings.arp_noteLength) )
    {
       _voiceBank->noteOff(_notesPressed[_step], 0);
      _state = ARP_IDLE;
      if(_step < (_nrNotesPressed - 1) ) _step++;
      else _step = 0;
      //Serial.println(_step);
      return;
    }

    if (_state == ARP_PLAYING) _playedTime++;
  }
}

void Arpeggiator::addNote(uint8_t note)
{
  if (_nrNotesPressed < MAX_ARP_NOTES)
  {
    _notesPressed[MAX_ARP_NOTES - 1] = note;
    _nrNotesPressed++;
    qsort(_notesPressed, MAX_ARP_NOTES, sizeof(uint8_t), compare);
  }
 // _printNotes();
}

void Arpeggiator::removeNote(uint8_t note)
{
  _voiceBank->noteOff(note, 0);
  for(uint8_t i = 0; i < MAX_ARP_NOTES; i++)
  {
    if(_notesPressed[i] == note)
    {
      _notesPressed[i] = 255;
      _nrNotesPressed--;
      break;
    }
  }
  qsort(_notesPressed, MAX_ARP_NOTES, sizeof(uint8_t), compare);
  _step = min(_step, _nrNotesPressed - 1);
  //_printNotes();
}

void Arpeggiator::_printNotes()
{
  for(uint8_t i = 0; i < MAX_ARP_NOTES; i++)
  {
    Serial.print(_notesPressed[i]);
    Serial.print(", ");
  }
  Serial.print("_nrNotesPressed: ");
  Serial.println(_nrNotesPressed);
}

int compare (const void * a, const void * b)
{
  //return ( *(int*)a - *(int*)b );
  return ( *(uint8_t*)a - *(uint8_t*)b );
}