//#include "midi_Defs.h"
#include "midiFunc.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, serialMIDI);

MidiSettings midiSettings;

IntervalTimer masterClockTimer;

Arpeggiator arpeggiator_A(&midiSettings, &voiceBank1);
Arpeggiator arpeggiator_B(&midiSettings, &voiceBank2);

char arpModeNames[NR_ARP_MODES][6] = {"off", "up", "down", "cycle", "seq", "chrd"};

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

  voiceBank2.patch.midi_channel = 2;

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
  if ( (channel == voiceBank1.patch.midi_channel)
        && (note >= voiceBank1.patch.midi_lowLimit)
        && (note <= voiceBank1.patch.midi_highLimit) )
  {
    noteStatus[note] = velocity;
    if(voiceBank1.patch.arp_mode == ARP_OFF) voiceBank1.noteOn(note + voiceBank1.patch.midi_transpose, velocity);
    arpeggiator_A.addNote(note);
    midiActivity = 1;
  }

  if ( (channel == voiceBank2.patch.midi_channel)
        && (note >= voiceBank2.patch.midi_lowLimit)
        && (note <= voiceBank2.patch.midi_highLimit) )
  {
    noteStatus[note] = velocity;
    if(voiceBank2.patch.arp_mode == ARP_OFF) voiceBank2.noteOn(note + voiceBank2.patch.midi_transpose, velocity);
    arpeggiator_B.addNote(note);
    midiActivity = 1;
  }

  if ( (channel == midiSettings.sideChain_channel) && (note == midiSettings.sideChain_note) )
  {
    sideChain.trigger();
  }

}

void myNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
   if ( (channel == voiceBank1.patch.midi_channel)
        && (note >= voiceBank1.patch.midi_lowLimit)
        && (note <= voiceBank1.patch.midi_highLimit) )
  {
    noteStatus[note] = 0;
    if(voiceBank1.patch.arp_mode == ARP_OFF) voiceBank1.noteOff(note + voiceBank1.patch.midi_transpose, velocity);
    arpeggiator_A.removeNote(note);
    midiActivity = 0;
  }

  if ( (channel == voiceBank2.patch.midi_channel)
        && (note >= voiceBank2.patch.midi_lowLimit)
        && (note <= voiceBank2.patch.midi_highLimit) )
  {
    noteStatus[note] = 0;
    if(voiceBank2.patch.arp_mode == ARP_OFF) voiceBank2.noteOff(note + voiceBank2.patch.midi_transpose, velocity);
    arpeggiator_B.removeNote(note);
    midiActivity = 0;
  }
}

void myControlChange(uint8_t channel, uint8_t control, uint8_t value)
{
  if (channel == voiceBank1.patch.midi_channel)
  {
    switch (control)
    {
      case CC_MODWHEEL:
        voiceBank1.setParameter(FILTER_CUTOFF, value / 127.0);
        voiceBank1.modWheel = value / 127.0;
        return;
    }
  }

  if (channel == voiceBank2.patch.midi_channel)
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
  if (channel == voiceBank1.patch.midi_channel) voiceBank1.setPitchBend(pitchBend);
  if (channel == voiceBank2.patch.midi_channel) voiceBank2.setPitchBend(pitchBend);
}

void myProgramChange(uint8_t channel, uint8_t program)
{

}

void mySystemExclusive(uint8_t *data, unsigned int length)
{
  
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
      targetValue_I8 = voiceBank1.patch.midi_channel + delta;
      voiceBank1.patch.midi_channel =  constrain(targetValue_I8, 0, 16);
      break;
    case SYS_BANK_B_MIDICHANNEL:
      targetValue_I8 = voiceBank2.patch.midi_channel + delta;
      voiceBank2.patch.midi_channel =  constrain(targetValue_I8, 0, 16);
      break;
    case SYS_BANK_A_LIMIT_LOW:
      targetValue_I8 = voiceBank1.patch.midi_lowLimit + delta;
      voiceBank1.patch.midi_lowLimit =  constrain(targetValue_I8, 0, voiceBank1.patch.midi_highLimit - 1);
      break;
    case SYS_BANK_A_LIMIT_HIGH:
      targetValue_I8 = voiceBank1.patch.midi_highLimit + delta;
      voiceBank1.patch.midi_highLimit =  constrain(targetValue_I8, voiceBank1.patch.midi_lowLimit + 1, 127);
      break;
    case SYS_BANK_B_LIMIT_LOW:
      targetValue_I8 = voiceBank2.patch.midi_lowLimit + delta;
      voiceBank2.patch.midi_lowLimit =  constrain(targetValue_I8, 0, voiceBank2.patch.midi_highLimit - 1);
      break;
    case SYS_BANK_B_LIMIT_HIGH:
      targetValue_I8 = voiceBank2.patch.midi_highLimit + delta;
      voiceBank2.patch.midi_highLimit =  constrain(targetValue_I8, voiceBank2.patch.midi_lowLimit + 1, 127);
      break;
    case SYS_BANK_A_TRANSPOSE:
      targetValue_I8 = voiceBank1.patch.midi_transpose + delta;
      voiceBank1.patch.midi_transpose =  constrain(targetValue_I8, -48, 48);
      break;
    case SYS_BANK_B_TRANSPOSE:
      targetValue_I8 = voiceBank2.patch.midi_transpose + delta;
      voiceBank2.patch.midi_transpose =  constrain(targetValue_I8, -48, 48);
      break;
          
    case SYS_SIDECHAIN_CHANNEL:
      targetValue_I8 = midiSettings.sideChain_channel + delta;
      midiSettings.sideChain_channel = constrain(targetValue_I8, 0, 16);
      break;
    case SYS_SIDECHAIN_NOTE:
      targetValue_I8 = midiSettings.sideChain_note + delta;
      midiSettings.sideChain_note = constrain(targetValue_I8, 1, 127);
      break;
    case SYS_BPM:
      targetValue_I8 = midiSettings.bpm + delta;
      midiSettings.bpm = constrain(targetValue_I8, 20, 300);
      break;

    case SYS_BANK_A_ARP_MODE:
      targetValue_I8 = voiceBank1.patch.arp_mode + delta;
      voiceBank1.patch.arp_mode = constrain(targetValue_I8, 0, NR_ARP_MODES - 1);
      if (voiceBank1.patch.arp_mode == ARP_OFF)
      {
        arpeggiator_A.reset();
        voiceBank1.panic();
      }
      break;
    case SYS_BANK_B_ARP_MODE:
      targetValue_I8 = voiceBank2.patch.arp_mode + delta;
      voiceBank2.patch.arp_mode = constrain(targetValue_I8, 0, NR_ARP_MODES - 1);
      if (voiceBank2.patch.arp_mode == ARP_OFF)
      {
        arpeggiator_B.reset();
        voiceBank2.panic();
      } 
      break;
    case SYS_BANK_A_ARP_INTERVAL:
      if (delta > 0) targetValue_I8 = voiceBank1.patch.arp_intervalTicks >> 1;
      else targetValue_I8 = voiceBank1.patch.arp_intervalTicks << 1;
      voiceBank1.patch.arp_intervalTicks = constrain(targetValue_I8, 1, RESOLUTION);
      break;
    case SYS_BANK_B_ARP_INTERVAL:
      if (delta > 0) targetValue_I8 = voiceBank2.patch.arp_intervalTicks >> 1;
      else targetValue_I8 = voiceBank2.patch.arp_intervalTicks << 1;
      voiceBank2.patch.arp_intervalTicks = constrain(targetValue_I8, 1, RESOLUTION);
      break;
    case SYS_BANK_A_ARP_OFFSET:
      targetValue_I8 = voiceBank1.patch.arp_offsetTicks + delta;
      voiceBank1.patch.arp_offsetTicks = constrain(targetValue_I8, 0, RESOLUTION);
      break;
    case SYS_BANK_B_ARP_OFFSET:
      targetValue_I8 = voiceBank2.patch.arp_offsetTicks + delta;
      voiceBank2.patch.arp_offsetTicks = constrain(targetValue_I8, 0, RESOLUTION);
      break;
     case SYS_BANK_A_ARP_OCTAVES:
      targetValue_I8 = voiceBank1.patch.arp_octaves + delta;
      voiceBank1.patch.arp_octaves = constrain(targetValue_I8, 0, 3);
      break;
    case SYS_BANK_B_ARP_OCTAVES:
      targetValue_I8 = voiceBank2.patch.arp_octaves + delta;
      voiceBank2.patch.arp_octaves = constrain(targetValue_I8, 0, 3);
      break;
    case SYS_BANK_A_ARP_KEYTRACK:
      if (voiceBank1.patch.arp_keyTrack == 0) voiceBank1.patch.arp_keyTrack = 1;
      else voiceBank1.patch.arp_keyTrack = 0;
      break;
    case SYS_BANK_B_ARP_KEYTRACK:
      if (voiceBank2.patch.arp_keyTrack == 0) voiceBank2.patch.arp_keyTrack = 1;
      else voiceBank2.patch.arp_keyTrack = 0;
      break;


  }
}


Arpeggiator::Arpeggiator(MidiSettings * settings, VoiceBank * voiceBank)
{
  _midiSettings = settings;
  _voiceBank = voiceBank;
  memset(_notesPressed, 255, MAX_ARP_NOTES);
  //memset(_sequencerOffsets, 0, 16);
  //for (uint8_t i = 0; i < 16; i++) _sequencerOffsets[i] = i; // TEMP test
}

void Arpeggiator::update()
{
  if ( (_voiceBank->patch.arp_mode != ARP_OFF) && ( (_nrNotesPressed > 0) || (_voiceBank->patch.arp_keyTrack == 1) ))
  {
    bool stepTrigger = ( (midiSettings.masterClock % _voiceBank->patch.arp_intervalTicks) == _voiceBank->patch.arp_offsetTicks);

    if ( stepTrigger && !_oldStepTrigger) // rising edge
    {
      // new step
      _voiceBank->noteOff(_notePlaying, 0); // for mono modes

      // if ( _voiceBank->patch.arp_mode == ARP_SEQUENCER ) _notePlaying = (uint8_t)(_notesPressed[0] + _voiceBank->patch.arp_offsets[_step]);
      // else
      // {
      //   if (_octave > _voiceBank->patch.arp_octaves) _octave = 0;
      //   _notePlaying = _notesPressed[_step] + _octave * 12 + _voiceBank->patch.midi_transpose;
      // }
      // _voiceBank->noteOn(_notePlaying, 127);

      switch (_voiceBank->patch.arp_mode)
      {
        case ARP_UP:
          if(_step < (_nrNotesPressed - 1) ) _step++;
          else
          {
            _step = 0;
            _octave++;
          } 
          break;
        case ARP_DOWN:
          if(_step > 0 ) _step--;
          else
          {
            _step = _nrNotesPressed - 1;
            _octave++;
          }
          break;
        case ARP_CYCLE:
          if ( (_direction == ARP_DIRECTION_UP) && (_step < (_nrNotesPressed - 1)) ) _step++;
          else if ( (_direction == ARP_DIRECTION_DOWN) && (_step > 0 ) ) _step--;
          if (_step >= (_nrNotesPressed - 1) ) _direction = ARP_DIRECTION_DOWN;
          if (_step == 0) _direction = ARP_DIRECTION_UP;
          break;
        case ARP_SEQUENCER:
          if(_step < (NR_ARP_SEQUENCER_STEPS - 1) ) _step++;
          else _step = 0;
          break;
        case ARP_CHORD:
          break;
      }

       if ( _voiceBank->patch.arp_mode == ARP_SEQUENCER )
       {
        _notePlaying = (uint8_t)(_baseNote + _voiceBank->patch.arp_offsets[_step]);
        //_notePlaying = (uint8_t)(_notesPressed[0] + _voiceBank->patch.arp_offsets[_step]);
        if (_voiceBank->patch.arp_velocities[_step] > 0) _voiceBank->noteOn(_notePlaying, 127);
       } 
       else
       {
        if (_octave > _voiceBank->patch.arp_octaves) _octave = 0;
        _notePlaying = _notesPressed[_step] + _octave * 12 + _voiceBank->patch.midi_transpose;
        _voiceBank->noteOn(_notePlaying, 127);
       }
    
    }

    else   
    {
      // no step change - just handle note off after noteLength (TBA)
    }

    _oldStepTrigger = stepTrigger;
  } 
}
  
void Arpeggiator::addNote(uint8_t note)
{
  if (_nrNotesPressed < MAX_ARP_NOTES)
  {
    _notesPressed[MAX_ARP_NOTES - 1] = note;
    _nrNotesPressed++;
    qsort(_notesPressed, MAX_ARP_NOTES, sizeof(uint8_t), compare);
    _baseNote = _notesPressed[0];
  }
  //_printNotes();
}

void Arpeggiator::removeNote(uint8_t note)
{
  if ((_voiceBank->patch.arp_mode != ARP_OFF) && (_voiceBank->patch.arp_keyTrack != 1)) _voiceBank->noteOff(note + _octave * 12, 0);

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
  if (_voiceBank->patch.arp_keyTrack == 1) return;
  if (_nrNotesPressed > 0) _step = min(_step, _nrNotesPressed - 1);
  else if (_voiceBank->patch.arp_mode == ARP_SEQUENCER) _step = NR_ARP_SEQUENCER_STEPS - 1;
  //else if (_voiceBank->patch.arp_keyTrack == 1) return;
  else _step = 0;
  //_printNotes();
}

void Arpeggiator::reset()
{
  for(uint8_t i = 0; i < MAX_ARP_NOTES; i++)
  {
    if(_notesPressed[i] < 255)
    {
      _voiceBank->noteOff(_notesPressed[i] + _octave * 12, 0);
      _notesPressed[i] = 255;
    }
  }
  if (_voiceBank->patch.arp_mode == ARP_SEQUENCER) _step = NR_ARP_SEQUENCER_STEPS - 1;
  else _step = 0;
  //memset(_notesPressed, 255, MAX_ARP_NOTES);
  _nrNotesPressed = 0;
  _notePlaying = 0;
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
  // for(uint8_t i = 0; i < NR_ARP_SEQUENCER_STEPS; i++)
  // {
  //   Serial.print(_sequencerOffsets[i]);
  //   Serial.print(", ");
  // }
  // Serial.println("");
}

int compare (const void * a, const void * b)
{
  //return ( *(int*)a - *(int*)b );
  return ( *(uint8_t*)a - *(uint8_t*)b );
}