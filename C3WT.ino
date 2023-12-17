#include "ui.h"
#include "midiFunc.h"
#include "audioFunc.h"
#include "sdMgr.h"
#include "voice.h"

void setup()
{
  Serial.println("START");
  if (CrashReport) Serial.print(CrashReport);
  //setupHwControls();
  initSDcard();
  setupUI();
  setupAudio();
  setupMidi();
  //initSDcard();
  
}

void loop()
{
  updateVoices();
  updateMidi();
  //updateHwControls();
  updateUI();
  //displayResources();
}
