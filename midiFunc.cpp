#include "midi_Defs.h"
#include "midiFunc.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, serialMIDI);

MidiSettings midiSettings;

IntervalTimer masterClockTimer;
Arpeggiator arpeggiator_A(&midiSettings, &voiceBank1, &midiSettings.bank_A_arpMode, &midiSettings.bank_A_arpIntervalTicks);
Arpeggiator arpeggiator_B(&midiSettings, &voiceBank2, &midiSettings.bank_B_arpMode, &midiSettings.bank_B_arpIntervalTicks);

USBHost myusb;
USBHub hub1(myusb);
MIDIDevice_BigBuffer midi1(myusb);
//MIDIDevice_BigBuffer midi2(myusb);
//MIDIDevice_BigBuffer midi3(myusb);

uint8_t noteStatus[128];
uint8_t midiActivity = 0;
bool usbDeviceStatus = false;
bool usbPcStatus = false;

FLASHMEM void setupMidi()
{
  Serial.println("MIDI SETUP");

  myusb.begin();
  delay(200);
  
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleControlChange(myControlChange);
  usbMIDI.setHandlePitchChange(myPitchBend);
  usbMIDI.setHandleClock(myMIDIClock);
  usbMIDI.setHandleStart(myMIDIClockStart);
  usbMIDI.setHandleStop(myMIDIClockStop);

  serialMIDI.begin(MIDI_CHANNEL_OMNI);
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
  updateArpeggiator();
  checkUsbStatus();
}

void checkUsbStatus()
{
  if (midi1) usbDeviceStatus = true;
  else usbDeviceStatus = false;

  if (!bitRead(USB1_PORTSC1,7)) usbPcStatus = true;
  else usbPcStatus = false;
}

void myNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
  if ( (channel == midiSettings.bank_A_channel)
        && (note >= midiSettings.bank_A_lowLimit)
        && (note <= midiSettings.bank_A_highLimit) )
  {
    noteStatus[note] = velocity;
    //voiceBank1.noteOn(note + midiSettings.bank_A_transpose, velocity);
    if(midiSettings.bank_A_arpMode == ARP_OFF) voiceBank1.noteOn(note + midiSettings.bank_A_transpose, velocity);
    else arpeggiator_A.addNote(note + midiSettings.bank_A_transpose);
    midiActivity = 1;
  }

  if ( (channel == midiSettings.bank_B_channel)
        && (note >= midiSettings.bank_B_lowLimit)
        && (note <= midiSettings.bank_B_highLimit) )
  {
    noteStatus[note] = velocity;
    if(midiSettings.bank_B_arpMode == ARP_OFF) voiceBank2.noteOn(note + midiSettings.bank_B_transpose, velocity);
    else arpeggiator_B.addNote(note + midiSettings.bank_B_transpose);
    midiActivity = 1;
  }

  if ( (channel == midiSettings.sideChain_channel) && (note == midiSettings.sideChain_note) )
  {
    sideChain.trigger();
  }

}

void myNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
   if ( (channel == midiSettings.bank_A_channel)
        && (note >= midiSettings.bank_A_lowLimit)
        && (note <= midiSettings.bank_A_highLimit) )
  {
    noteStatus[note] = 0;
    if(midiSettings.bank_A_arpMode == ARP_OFF) voiceBank1.noteOff(note + midiSettings.bank_A_transpose, velocity);
    else arpeggiator_A.removeNote(note + midiSettings.bank_A_transpose);
    midiActivity = 0;
  }

  if ( (channel == midiSettings.bank_B_channel)
        && (note >= midiSettings.bank_B_lowLimit)
        && (note <= midiSettings.bank_B_highLimit) )
  {
    noteStatus[note] = 0;
    if(midiSettings.bank_B_arpMode == ARP_OFF) voiceBank2.noteOff(note + midiSettings.bank_B_transpose, velocity);
    else arpeggiator_B.removeNote(note + midiSettings.bank_B_transpose);
    midiActivity = 0;
  }
}

void myControlChange(uint8_t channel, uint8_t control, uint8_t value)
{
  if (channel == midiSettings.bank_A_channel)
  {
    switch (control)
    {
      case CC_MODWHEEL:
        voiceBank1.setParameter(FILTER_CUTOFF, value / 127.0);
        voiceBank1.modWheel = value / 127.0;
        return;
    }
  }

  if (channel == midiSettings.bank_B_channel)
  {
    switch (control)
    {
      case CC_MODWHEEL:
        voiceBank2.setParameter(FILTER_CUTOFF, value / 127.0);
        voiceBank2.modWheel = value / 127.0;
        return;
    }
  }

  switch (control)
    {
      case CC_PANIC:
        voiceBank1.panic();
        voiceBank2.panic();
        memset(noteStatus, 0, 128);
        midiActivity = 0;
        return;
    }
}

void myPitchBend(uint8_t channel, int pitchBend)
{
  if (channel == midiSettings.bank_A_channel) voiceBank1.setPitchBend(pitchBend);
  if (channel == midiSettings.bank_B_channel) voiceBank1.setPitchBend(pitchBend);
}

void myMIDIClock()
{
  static uint8_t counter = 0;
  static elapsedMicros midiClockTimer;
  counter++;
  if (counter > 23)
  {
    counter = 0;
    midiSettings.oneTickUs = midiClockTimer / 24.0;
    midiSettings.bpm = 24 * midiSettings.oneTickUs / (1000 * 60000);
    midiClockTimer = 0;
    Serial.println(midiSettings.oneTickUs);
    Serial.println(midiSettings.bpm);
  }
}

void myMIDIClockStart()
{

}

void myMIDIClockStop()
{

}


void tickMasterClock() { midiSettings.masterClock++; }

void updateArpeggiator()
{
  arpeggiator_A.update();
  arpeggiator_B.update();
}

void setArpeggiatorMode(uint8_t bank, int8_t delta)
{
  if (bank == 0) midiSettings.bank_A_arpMode = constrain(midiSettings.bank_A_arpMode + delta, 0, NR_ARP_MODES - 1);
  else midiSettings.bank_B_arpMode = constrain(midiSettings.bank_B_arpMode + delta, 0, NR_ARP_MODES - 1);
}

void adjustBpm(uint8_t dummy, int8_t delta)
{
  midiSettings.bpm = constrain(midiSettings.bpm + delta, 20, 255);
  midiSettings.oneTickUs = 1000 * 60000 / (midiSettings.bpm * RESOLUTION);
  masterClockTimer.update(midiSettings.oneTickUs);
}

void adjustMidiParameter(uint8_t parameter, int8_t delta)
{
  int8_t targetValue_I8 = 0;
  switch (parameter)
  {
    case SYS_BANK_A_MIDICHANNEL:
      targetValue_I8 = midiSettings.bank_A_channel + delta;
      midiSettings.bank_A_channel =  constrain(targetValue_I8, 0, 16);
      break;
    case SYS_BANK_B_MIDICHANNEL:
      targetValue_I8 = midiSettings.bank_B_channel + delta;
      midiSettings.bank_B_channel =  constrain(targetValue_I8, 0, 16);
      break;
    case SYS_BANK_A_LIMIT_LOW:
      targetValue_I8 = midiSettings.bank_A_lowLimit + delta;
      midiSettings.bank_A_lowLimit =  constrain(targetValue_I8, 0, midiSettings.bank_A_highLimit - 1);
      break;
    case SYS_BANK_A_LIMIT_HIGH:
      targetValue_I8 = midiSettings.bank_A_highLimit + delta;
      midiSettings.bank_A_highLimit =  constrain(targetValue_I8, midiSettings.bank_A_lowLimit + 1, 127);
      break;
    case SYS_BANK_B_LIMIT_LOW:
      targetValue_I8 = midiSettings.bank_B_lowLimit + delta;
      midiSettings.bank_B_lowLimit =  constrain(targetValue_I8, 0, midiSettings.bank_B_highLimit - 1);
      break;
    case SYS_BANK_B_LIMIT_HIGH:
      targetValue_I8 = midiSettings.bank_B_highLimit + delta;
      midiSettings.bank_B_highLimit =  constrain(targetValue_I8, midiSettings.bank_B_lowLimit + 1, 127);
      break;
    case SYS_BANK_A_TRANSPOSE:
      targetValue_I8 = midiSettings.bank_A_transpose + delta;
      midiSettings.bank_A_transpose =  constrain(targetValue_I8, -48, 48);
      break;
    case SYS_BANK_B_TRANSPOSE:
      targetValue_I8 = midiSettings.bank_B_transpose + delta;
      midiSettings.bank_B_transpose =  constrain(targetValue_I8, -48, 48);
      break;
    case SYS_SIDECHAIN_CHANNEL:
      targetValue_I8 = midiSettings.sideChain_channel + delta;
      midiSettings.sideChain_channel = constrain(targetValue_I8, 0, 16);
      break;
    case SYS_SIDECHAIN_NOTE:
      targetValue_I8 = midiSettings.sideChain_note + delta;
      midiSettings.sideChain_note = constrain(targetValue_I8, 1, 127);
      break;
    case SYS_BANK_A_ARP_MODE:
      targetValue_I8 = midiSettings.bank_A_arpMode + delta;
      midiSettings.bank_A_arpMode = constrain(targetValue_I8, 0, NR_ARP_MODES - 1);
      break;
    case SYS_BANK_B_ARP_MODE:
      targetValue_I8 = midiSettings.bank_B_arpMode + delta;
      midiSettings.bank_B_arpMode = constrain(targetValue_I8, 0, NR_ARP_MODES - 1);
      break;
    case SYS_BPM:
      targetValue_I8 = midiSettings.bpm + delta;
      midiSettings.bpm = constrain(targetValue_I8, 20, 300);
      break;
    case SYS_BANK_A_ARP_INTERVAL:
      if (delta > 0) targetValue_I8 = midiSettings.bank_A_arpIntervalTicks >> 1;
      else targetValue_I8 = midiSettings.bank_A_arpIntervalTicks << 1;
      midiSettings.bank_A_arpIntervalTicks = constrain(targetValue_I8, 1, RESOLUTION);
      break;
    case SYS_BANK_B_ARP_INTERVAL:
      if (delta > 0) targetValue_I8 = midiSettings.bank_B_arpIntervalTicks >> 1;
      else targetValue_I8 = midiSettings.bank_B_arpIntervalTicks << 1;
      midiSettings.bank_B_arpIntervalTicks = constrain(targetValue_I8, 1, RESOLUTION);
      break;
  }
}


Arpeggiator::Arpeggiator(MidiSettings * settings, VoiceBank * voiceBank, uint8_t * arpMode, uint8_t * intervalTicks)
{
  _settings = settings;
  _voiceBank = voiceBank;
  _arpMode = arpMode;
  _intervalTicks = intervalTicks;
  memset(_notesPressed, 255, MAX_ARP_NOTES);
}

void Arpeggiator::update()
{
  if ( (*_arpMode != ARP_OFF) && (_nrNotesPressed > 0) )
  {
    bool stepTrigger = ( (midiSettings.masterClock % *_intervalTicks) == 0);

    if (stepTrigger) Serial.printf("Step, %d\n", midiSettings.masterClock);
    if ( stepTrigger && !_oldStepTrigger) // rising edge
    {
      // new step
      //_oldStepTrigger = true;
      _voiceBank->noteOff(_notesPressed[_step], 0);
      if(_step < (_nrNotesPressed - 1) ) _step++;
      else _step = 0;
      _voiceBank->noteOn(_notesPressed[_step], 127);
    }

    else   
    {
      // just do note off after noteLength
      
    }

    _oldStepTrigger = stepTrigger;
  } 
}
  
    // if(_state == ARP_IDLE)
    // {
    //    _voiceBank->noteOn(_notesPressed[_step], 127);
    //    _state = ARP_PLAYING;
    //    _playedTime = 0;
    //    return;
    // }
    
    // if(_state == ARP_PLAYING) && ( _playedTime > midiSettings.arp_noteLength) )
    // {
    //    _voiceBank->noteOff(_notesPressed[_step], 0);
    //   _state = ARP_IDLE;
    //   if(_step < (_nrNotesPressed - 1) ) _step++;
    //   else _step = 0;
    //   //Serial.println(_step);
    //   return;
    //}

    //if (_state == ARP_PLAYING) _playedTime++;
  //}


void Arpeggiator::addNote(uint8_t note)
{
  if (_nrNotesPressed < MAX_ARP_NOTES)
  {
    _notesPressed[MAX_ARP_NOTES - 1] = note;
    _nrNotesPressed++;
    qsort(_notesPressed, MAX_ARP_NOTES, sizeof(uint8_t), compare);
  }
  //_printNotes();
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