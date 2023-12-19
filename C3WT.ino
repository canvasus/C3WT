#include "ui.h"
#include "midiFunc.h"
#include "audioFunc.h"
#include "sdMgr.h"
#include "voice.h"

void setup()
{
  Serial.println("START");
  if (CrashReport) Serial.print(CrashReport);

  initSDcard();
  setupUI();
  setupAudio();
  setupMidi();

  changePatch(0, 0);
  
}

void loop()
{
  updateVoices();
  updateMidi();
  updateUI();
}
