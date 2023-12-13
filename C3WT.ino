#include "ui.h"
#include "midiFunc.h"
#include "audioFunc.h"
#include "sdMgr.h"

void setup()
{
  Serial.println("START");
  if (CrashReport) Serial.print(CrashReport);
  //setupHwControls();
  setupUI();
  setupAudio();
  setupMidi();
  initSDcard();
  
}

void loop()
{
  updateVoices();
  updateMidi();
  //updateHwControls();
  updateUI();
  //displayResources();
}
