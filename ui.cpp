#include "ui.h"

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);
Encoder encoders[1] = {Encoder(ENC1_A, ENC1_B)};

uint16_t coordinates[5][2];

Page pages[NR_PAGES];

uint8_t currentPage = PAGE_PATCH;
uint8_t backgroundPage = PAGE_PATCH;
bool forceReload = false;
uint8_t brightness = 255;
uint16_t loadTimes[NR_PAGES];

void setupUI()
{
  Serial.println("UI SETUP");
  
  tft.begin(RA8875_800x480);
  tft.useCapINT(RA8875_INT);
  tft.fillWindow(MAIN_BG_COLOR);
  tft.enableCapISR(true);

  //showStartupScreen();
  configurePage_patch();
  configurePage_oscillator();
  configurePage_modulation();
  configurePage_modulation2();
  configurePage_effects();
  configurePage_patchName();
  configurePage_lfo();
  configurePage_wavetable();
  configurePage_wavetable2();
  configurePage_wavetableOsc1();
  configurePage_wavetableOsc2();
  configurePage_screenSaver();
  configurePage_envelope();
  configurePage_filter();
  configurePage_controls();
  configurePage_system();
}

void showStartupScreen()
{
  // for (uint8_t i = 0; i < 16; i++)
  // {
  //   tft.drawFastHLine(10, 80 - i * 4, 300, tft.color565(0, 0, 200 - i * 10));
  //   tft.drawFastHLine(10, 120 + i * 4, 300, tft.color565(0, 0, 200 - i * 10));
  //   delay(200);
  // }
  // tft.fillScreen(MAIN_BG_COLOR);
}

void updateUI()
{
  static elapsedMillis uiTimer = 0;
  const uint8_t uiInterval = 33;

  if (pages[currentPage].draw(false) == 1) // checks if current page is done drawing
  {
    if (uiTimer > uiInterval)
    {
      uiTimer = 0;
      static uint8_t oldPage = N_A;
      bool firstCall = (currentPage != oldPage) || forceReload;
      oldPage = currentPage;
      updatePage(currentPage, firstCall);
    }
  }
}

void updatePage(uint8_t pageId, bool firstCall)
{
  uint8_t selectedId = pages[pageId].selectedId;
  uint8_t nrWidgets = pages[pageId].nrWidgets;

  int newSelectedId = pages[pageId].selectedId;
  static elapsedMillis screenSaverTimer = 0;

  if (firstCall)
  {
    screenSaverTimer = 0;
    pages[pageId].draw(true);
    forceReload = false;
  }
  else pages[pageId].animate(false);

  if (tft.touched())
  {
    screenSaverTimer = 0;
    tft.updateTS();
    uint8_t touchState = tft.getTouchState();
    if (touchState == CTP_FINGER_DOWN || touchState == CTP_FINGER_UP)
    {
      tft.getTScoordinates(coordinates);
      newSelectedId = pages[pageId].checkTouch(coordinates[0][0], coordinates[0][1], touchState);
    }
    tft.enableCapISR(); // needed?
  }

  // This should be moved to page class:
  if ( (newSelectedId > -1) && (newSelectedId != selectedId) )
  {
    // pages[pageId].select(newSelectedId); // NOTE: change to this later
    pages[pageId].widgetPointers[selectedId]->draw(false);
    if (newSelectedId >= nrWidgets) newSelectedId = 0;
    selectedId = newSelectedId;
    pages[pageId].selectedId = selectedId;
    pages[pageId].widgetPointers[selectedId]->draw(true);
  } 
  
  int valueChange = getEncoderDirection(0);
  if (valueChange != 0 && (pages[pageId].widgetPointers[selectedId]->setI8 != nullptr))
  {
    screenSaverTimer = 0;
    // pages[pageId].valueChange(valueChange); // NOTE: change to this later
    pages[pageId].widgetPointers[selectedId]->setI8(pages[pageId].widgetPointers[selectedId]->id, valueChange);
    pages[pageId].widgetPointers[selectedId]->draw(true);
  }

  if ( (screenSaverTimer > SCREEN_SAVER_TIME) && (currentPage != PAGE_SCREENSAVER) )
  {
    pages[PAGE_SCREENSAVER].widgetPointers[0]->id = currentPage;
    currentPage = PAGE_SCREENSAVER;
  }
  //}
}

FLASHMEM void configurePage_screenSaver()
{
  // one full screen widget that returns to main window
  uint8_t PAGE = PAGE_SCREENSAVER;
  uint8_t widgetIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].animateFunction = &animateScreenSaver;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 10, 10, SCREEN_XRES-20, SCREEN_YRES-20);
  pages[PAGE].widgetPointers[widgetIndex]->color1 = MAIN_BG_COLOR;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = MAIN_BG_COLOR;
  pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_patch()
{
  uint8_t PAGE = PAGE_PATCH;
  uint8_t widgetIndex = 0;
  //uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateHomePage;

  const uint16_t column_w = 140;
  const uint16_t row_h = 60;
  const uint8_t padding = 4;
  const uint8_t labelOffsetX = 10;
  const uint8_t labelOffsetY = 15;

  widgetIndex = pages[PAGE].addWidget(0, 0, 0, 80, 60);
  pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &currentVoiceBank;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &setActiveVoiceBank;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 20;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = HEADER_COLOR;
  pages[PAGE].widgetPointers[widgetIndex]->textColor1 = 0xffff;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCHNAME, 84, 0, 400, 60);
  pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_str = &patchNameUI;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &changePatch;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 20;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = HEADER_COLOR;
  pages[PAGE].widgetPointers[widgetIndex]->textColor1 = 0xffff;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCHNAME, 504, 0, 100, 60);
  pages[PAGE].widgetPointers[widgetIndex]->label("SAVE");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = HEADER_COLOR;
  pages[PAGE].widgetPointers[widgetIndex]->textColor1 = 0xffff;

  widgetIndex = pages[PAGE].addWidget(PAGE_SCREENSAVER, SCREEN_XRES - 40, SCREEN_YRES - 40, 40, 40);
  pages[PAGE].widgetPointers[widgetIndex]->label("*");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = labelOffsetY;


  // ----------------------------------
  widgetIndex = pages[PAGE].addWidget(PAGE_OSCILLATOR, 0* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("OSC");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = 0xd6bf;

  widgetIndex = pages[PAGE].addWidget(PAGE_WAVETABLE_OSC1, 0* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("WT1");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = 0xb5be;

  widgetIndex = pages[PAGE].addWidget(PAGE_WAVETABLE_OSC2, 0* (column_w + padding), 4* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("WT2");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = 0x9d1f;


  widgetIndex = pages[PAGE].addWidget(PAGE_ENVELOPE, 1* (column_w + padding), 2* (row_h + padding), column_w, 3 * row_h + 2 * padding); 
  pages[PAGE].widgetPointers[widgetIndex]->label("ENV");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = 0xafff;

  widgetIndex = pages[PAGE].addWidget(PAGE_FILTER, 2* (column_w + padding), 2* (row_h + padding), column_w, 3 * row_h + 2 * padding); 
  pages[PAGE].widgetPointers[widgetIndex]->label("FLT");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = 0xb7f5;

  widgetIndex = pages[PAGE].addWidget(PAGE_LFO, 3* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("LFO");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = 0xc398;

  widgetIndex = pages[PAGE].addWidget(PAGE_MODULATION, 3* (column_w + padding), 3* (row_h + padding), column_w, 2 * row_h + padding); 
  pages[PAGE].widgetPointers[widgetIndex]->label("MOD");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = 0xc518;

  widgetIndex = pages[PAGE].addWidget(PAGE_EFFECTS, 4* (column_w + padding), 2* (row_h + padding), column_w, 3 * row_h + 2 * padding); 
  pages[PAGE].widgetPointers[widgetIndex]->label("EFX");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = labelOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = 0xfff0;
  

  widgetIndex = pages[PAGE].addWidget(PAGE_SYSTEM, 0* (column_w + padding), SCREEN_YRES - 40, column_w, 40); 
  pages[PAGE].widgetPointers[widgetIndex]->label("SYS");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = 5;

  widgetIndex = pages[PAGE].addWidget(PAGE_CONTROLS, 1* (column_w + padding), SCREEN_YRES - 40, column_w, 40); 
  pages[PAGE].widgetPointers[widgetIndex]->label("CTL");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->labelOffsetY = 5;
}

FLASHMEM void configurePage_oscillator()
{
  uint8_t PAGE = PAGE_OSCILLATOR;
  uint8_t widgetIndex = 0;
  uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;

  const uint16_t column_w = 136;
  const uint16_t row_h = 60;
  const uint8_t padding = 4;
  const uint8_t labelOffsetX = 10;
  const uint8_t labelOffsetY = 8;

  staticIndex = pages[PAGE].addStatic(0, 0* (column_w + padding), 0* (row_h + padding), column_w, 40);
  pages[PAGE].staticPointers[staticIndex]->label("OSC");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;
  pages[PAGE].staticPointers[staticIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].staticPointers[staticIndex]->labelOffsetY = labelOffsetY;

  staticIndex = pages[PAGE].addStatic(0, 1* (column_w + padding), 0* (row_h + padding), column_w, 40);
  pages[PAGE].staticPointers[staticIndex]->label("MIX");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;
  pages[PAGE].staticPointers[staticIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].staticPointers[staticIndex]->labelOffsetY = labelOffsetY;

  staticIndex = pages[PAGE].addStatic(0, 3* (column_w + padding), 0* (row_h + padding), column_w, 40);
  pages[PAGE].staticPointers[staticIndex]->label("AM");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;
  pages[PAGE].staticPointers[staticIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].staticPointers[staticIndex]->labelOffsetY = labelOffsetY;
  
  staticIndex = pages[PAGE].addStatic(0, 4* (column_w + padding), 0* (row_h + padding), column_w, 40);
  pages[PAGE].staticPointers[staticIndex]->label("FM");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;
  pages[PAGE].staticPointers[staticIndex]->labelOffsetX = labelOffsetX;
  pages[PAGE].staticPointers[staticIndex]->labelOffsetY = labelOffsetY;

  // staticIndex = pages[PAGE].addStatic(0, 4* (column_w + padding), 0* (row_h + padding), column_w, 40);
  // pages[PAGE].staticPointers[staticIndex]->label("SYNC");
  // pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;



  widgetIndex = pages[PAGE].addWidget(OSC1_WAVEFORM, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Wfm 1");
  pages[PAGE].widgetPointers[widgetIndex]->drawWaveform = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc1_waveform;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVEFORM, 0* (column_w + padding), 2* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Wfm 2");
  pages[PAGE].widgetPointers[widgetIndex]->drawWaveform = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc2_waveform;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  staticIndex = pages[PAGE].addStatic(0, 0* (column_w + padding), 3* (row_h + padding), column_w, row_h);
  pages[PAGE].staticPointers[staticIndex]->label("Pulse");
  pages[PAGE].staticPointers[staticIndex]->color2 = NO_CONTROL_COLOR;

  staticIndex = pages[PAGE].addStatic(0, 0* (column_w + padding), 4* (row_h + padding), column_w, row_h);
  pages[PAGE].staticPointers[staticIndex]->label("Noise");
  pages[PAGE].staticPointers[staticIndex]->color2 = NO_CONTROL_COLOR;

  widgetIndex = pages[PAGE].addWidget(TRANSPOSE, 0* (column_w + padding), 5* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Trp");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_i8 = &voiceBank1.patch.transpose;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(DETUNE, 0* (column_w + padding), 6* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Det");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.detune;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(POLY_MODE, 1* (column_w + padding), 6* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Uni:");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.polyMode;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(MONO_MODE, 2* (column_w + padding), 6* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Mono");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.mono_mode;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  // ------

  widgetIndex = pages[PAGE].addWidget(OSC1_LEVEL, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  //pages[PAGE].widgetPointers[widgetIndex]->label("");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = false;
  pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 4;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 18;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.osc1_level;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;

  widgetIndex = pages[PAGE].addWidget(OSC2_LEVEL, 1* (column_w + padding), 2* (row_h + padding), column_w, row_h);
  //pages[PAGE].widgetPointers[widgetIndex]->label("");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = false;
  pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 4;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 18;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.osc2_level;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;

  widgetIndex = pages[PAGE].addWidget(PULSE_LEVEL, 1* (column_w + padding), 3* (row_h + padding), column_w, row_h);
  //pages[PAGE].widgetPointers[widgetIndex]->label("");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = false;
  pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 4;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 18;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.pulse_level;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;

  widgetIndex = pages[PAGE].addWidget(NOISE_LEVEL, 1* (column_w + padding), 4* (row_h + padding), column_w, row_h);
  //pages[PAGE].widgetPointers[widgetIndex]->label("");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = false;
  pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 4;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 18;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.noise_level;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;


  widgetIndex = pages[PAGE].addWidget(AM_WAVEFORM, 3* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Wfm");
  pages[PAGE].widgetPointers[widgetIndex]->drawWaveform = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc_am_waveform;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(AM_FREQ_MULTIPLIER, 3* (column_w + padding), 2* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("F/xF");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 60;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.am_frequency_multiplier;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(AM_FIXEDFREQUENCY, 3* (column_w + padding), 3* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Fix F");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 60;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.am_fixedFrequency;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(AM_LEVEL, 3* (column_w + padding), 4* (row_h + padding), column_w, row_h);
  //pages[PAGE].widgetPointers[widgetIndex]->label("Lvl");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = false;
  pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 4;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 18;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.am_level;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;

  
  widgetIndex = pages[PAGE].addWidget(FM_WAVEFORM, 4* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Wfm");
  pages[PAGE].widgetPointers[widgetIndex]->drawWaveform = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc_fm_waveform;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FM_FREQ_MULTIPLIER, 4* (column_w + padding), 2* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("F/xF");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 60;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.fm_frequency_multiplier;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FM_OFFSET, 4* (column_w + padding), 3* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Offs");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 60;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.fm_offset;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FM_LEVEL, 4* (column_w + padding), 4* (row_h + padding), column_w, row_h);
  //pages[PAGE].widgetPointers[widgetIndex]->label("Lvl");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = false;
  pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 4;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 18;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.fm_level;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;

 

  // widgetIndex = pages[PAGE].addWidget(OSC1_SYNC, 4* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  // pages[PAGE].widgetPointers[widgetIndex]->label("Sync:");
  // pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  // pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 60;
  // pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  // pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc1_sync;
  // pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  // widgetIndex = pages[PAGE].addWidget(OSC2_SYNC, 4* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  // pages[PAGE].widgetPointers[widgetIndex]->label("Sync:");
  // pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  // pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 60;
  // pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  // pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc2_sync;
  // pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
 
}

FLASHMEM void configurePage_modulation()
{
  uint8_t PAGE = PAGE_MODULATION;
  uint8_t widgetIndex = 0;
  uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;

  // draw row headers
  const uint8_t nrRows = 7;
  const uint8_t nrColumns = 5;
  const uint8_t rowHeight = 46;
  const uint8_t padding = 4;
  const uint8_t columnWidth = 120;

  const String rowNames[nrRows] = {"Osc1", "Osc2", "Env3", "Lfo1", "Lfo2", "Vel", "Wheel"};

  for (uint8_t row = 0; row < nrRows; row++)
  {
    staticIndex = pages[PAGE].addStatic(0, 0, (1 + row) * (rowHeight + padding), columnWidth, rowHeight);
    pages[PAGE].staticPointers[staticIndex]->label(rowNames[row]);
    pages[PAGE].staticPointers[staticIndex]->color2 = MOD_SOURCE_COLOR;
  }

  // draw column headers
  
  const String columnNames[nrRows] = {"Osc1 P", "Osc2 P", "Osc1 PH", "Osc2 PH", "Cutoff"};
  for (uint8_t column = 0; column < nrColumns; column++)
  {
    staticIndex = pages[PAGE].addStatic(0, (1 + column) * (columnWidth + padding), 0, columnWidth, rowHeight);
    pages[PAGE].staticPointers[staticIndex]->label(columnNames[column]);
    pages[PAGE].staticPointers[staticIndex]->color2 = MOD_DESTINATION_COLOR;
  }

  staticIndex = pages[PAGE].addStatic(0, 2 * (columnWidth + padding), 434, columnWidth, rowHeight);
  pages[PAGE].staticPointers[staticIndex]->label("Source");
  pages[PAGE].staticPointers[staticIndex]->color2 = MOD_SOURCE_COLOR;

  staticIndex = pages[PAGE].addStatic(0, 3 * (columnWidth + padding), 434, columnWidth, rowHeight);
  pages[PAGE].staticPointers[staticIndex]->label("Dest");
  pages[PAGE].staticPointers[staticIndex]->color2 = MOD_DESTINATION_COLOR;

  
  // draw parameters, 5*5x
  uint8_t parameterIndexes[7][5] = {{N_A, MOD_OSC1_OSC2_PITCH, N_A, MOD_OSC1_OSC2_PHASE, N_A},
                                    {MOD_OSC2_OSC1_PITCH, N_A, MOD_OSC2_OSC1_PHASE, N_A, N_A},
                                    {MOD_ENV3_OSC1_PITCH, MOD_ENV3_OSC2_PITCH, MOD_ENV3_OSC1_PHASE, MOD_ENV3_OSC2_PHASE, MOD_ENV3_FILTER_CUTOFF},
                                    {MOD_LFO1_OSC1_PITCH, MOD_LFO1_OSC2_PITCH, MOD_LFO1_OSC1_PHASE, MOD_LFO1_OSC2_PHASE, MOD_LFO1_FILTER_CUTOFF},
                                    {MOD_LFO2_OSC1_PITCH, MOD_LFO2_OSC2_PITCH, MOD_LFO2_OSC1_PHASE, MOD_LFO2_OSC2_PHASE, MOD_LFO2_FILTER_CUTOFF},
                                    {MOD_VEL_OSC1_PITCH, MOD_VEL_OSC2_PITCH, MOD_VEL_OSC1_PHASE, MOD_VEL_OSC2_PHASE, MOD_VEL_FILTER_CUTOFF},
                                    {MOD_WHL_OSC1_PITCH, MOD_WHL_OSC2_PITCH, MOD_WHL_OSC1_PHASE, MOD_WHL_OSC2_PHASE, MOD_WHL_FILTER_CUTOFF}
                                    };
  
  float * varPointers[7][5] = {
                                {nullptr, &voiceBank1.patch.mod_osc1_osc2_pitch, nullptr, &voiceBank1.patch.mod_osc1_osc2_phase, nullptr},
                                {&voiceBank1.patch.mod_osc2_osc1_pitch, nullptr, &voiceBank1.patch.mod_osc2_osc1_phase, nullptr, nullptr},
                                {&voiceBank1.patch.mod_env3_osc1_pitch, &voiceBank1.patch.mod_env3_osc2_pitch, &voiceBank1.patch.mod_env3_osc1_phase, &voiceBank1.patch.mod_env3_osc2_phase, &voiceBank1.patch.mod_env3_filter_cutoff},
                                {&voiceBank1.patch.mod_lfo1_osc1_pitch, &voiceBank1.patch.mod_lfo1_osc2_pitch, &voiceBank1.patch.mod_lfo1_osc1_phase, &voiceBank1.patch.mod_lfo1_osc2_phase, &voiceBank1.patch.mod_lfo1_filter_cutoff},
                                {&voiceBank1.patch.mod_lfo2_osc1_pitch, &voiceBank1.patch.mod_lfo2_osc2_pitch, &voiceBank1.patch.mod_lfo2_osc1_phase, &voiceBank1.patch.mod_lfo2_osc2_phase, &voiceBank1.patch.mod_lfo2_filter_cutoff},
                                {&voiceBank1.patch.mod_velocity_osc1_pitch, &voiceBank1.patch.mod_velocity_osc2_pitch, &voiceBank1.patch.mod_velocity_osc1_phase, &voiceBank1.patch.mod_velocity_osc2_phase, &voiceBank1.patch.mod_velocity_filter_cutoff},
                                {&voiceBank1.patch.mod_wheel_osc1_pitch, &voiceBank1.patch.mod_wheel_osc2_pitch, &voiceBank1.patch.mod_wheel_osc1_phase, &voiceBank1.patch.mod_wheel_osc2_phase, &voiceBank1.patch.mod_wheel_filter_cutoff}
                              };

  for (uint8_t row = 0; row < nrRows; row++)
  {
    for (uint8_t column = 0; column < nrColumns; column++)
    {
      if (parameterIndexes[row][column] != N_A)
      {
        widgetIndex = pages[PAGE].addWidget(parameterIndexes[row][column], (column + 1) * (columnWidth + padding), (row + 1) * (rowHeight + padding), columnWidth, rowHeight);
        pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
        pages[PAGE].widgetPointers[widgetIndex]->drawVariable = false;
        pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 4;
        pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 12;
        pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = varPointers[row][column];
        pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
        pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
        pages[PAGE].widgetPointers[widgetIndex]->varMin = -1.0;
        pages[PAGE].widgetPointers[widgetIndex]->varMax = 1.0;
      }
    }
  }

  widgetIndex = pages[PAGE].addWidget(PAGE_MODULATION2, 700, 430, 100, 50);
  pages[PAGE].widgetPointers[widgetIndex]->label("PAGE2>");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 0, 430, 100, 50);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
  
}

FLASHMEM void configurePage_modulation2()
{
  uint8_t PAGE = PAGE_MODULATION2;
  uint8_t widgetIndex = 0;
  uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;

   // draw row headers
  const uint8_t nrRows = 7;
  const uint8_t nrColumns = 5;
  const uint8_t rowHeight = 46;
  const uint8_t padding = 4;
  const uint8_t columnWidth = 120;

  const String rowNames[nrRows] = {"Osc1", "Osc2", "Env3", "Lfo1", "Lfo2", "Vel", "Wheel"};

  for (uint8_t row = 0; row < nrRows; row++)
  {
    staticIndex = pages[PAGE].addStatic(0, 0, (1 + row) * (rowHeight + padding), columnWidth, rowHeight);
    pages[PAGE].staticPointers[staticIndex]->label(rowNames[row]);
    pages[PAGE].staticPointers[staticIndex]->color2 = MOD_SOURCE_COLOR;
  }

  // draw column headers
  const String columnNames[nrColumns] = {"Pwm", "AM P", "FM P", "Lfo1 Amp", "Lfo2 Amp"};
  for (uint8_t column = 0; column < nrColumns; column++)
  {
    staticIndex = pages[PAGE].addStatic(0, (1 + column) * (columnWidth + padding), 0, columnWidth, rowHeight);
    pages[PAGE].staticPointers[staticIndex]->label(columnNames[column]);
    pages[PAGE].staticPointers[staticIndex]->color2 = MOD_DESTINATION_COLOR;
  }

  // draw parameters
  uint8_t parameterIndexes[nrRows][nrColumns] = {{N_A, N_A, N_A, N_A, N_A},
                                    {N_A, N_A, N_A, N_A, N_A},
                                    {MOD_ENV3_PWM, MOD_ENV3_AM_PITCH, MOD_ENV3_FM_PITCH, MOD_ENV3_LFO1_AMPLITUDE, MOD_ENV3_LFO2_AMPLITUDE},
                                    {MOD_LFO1_PWM, MOD_LFO1_AM_PITCH, MOD_LFO1_FM_PITCH, N_A, N_A},
                                    {MOD_LFO2_PWM, MOD_LFO2_AM_PITCH, MOD_LFO2_FM_PITCH, N_A, N_A},
                                    {MOD_VEL_PWM, N_A, N_A, N_A, N_A},
                                    {MOD_WHL_PWM, N_A, N_A, N_A, N_A}
                                    };
  
  float * varPointers[nrRows][nrColumns] = {
                                {nullptr, nullptr, nullptr, nullptr, nullptr},
                                {nullptr, nullptr, nullptr, nullptr, nullptr},
                                {&voiceBank1.patch.mod_env3_pwm, &voiceBank1.patch.mod_env3_am_pitch, &voiceBank1.patch.mod_env3_fm_pitch, &voiceBank1.patch.mod_env3_lfo1_amplitude, &voiceBank1.patch.mod_env3_lfo2_amplitude},
                                {&voiceBank1.patch.mod_lfo1_pwm, &voiceBank1.patch.mod_lfo1_am_pitch, &voiceBank1.patch.mod_lfo1_fm_pitch},
                                {&voiceBank1.patch.mod_lfo2_pwm, &voiceBank1.patch.mod_lfo2_am_pitch, &voiceBank1.patch.mod_lfo2_fm_pitch},
                                {&voiceBank1.patch.mod_velocity_pwm, nullptr, nullptr, nullptr, nullptr},
                                {&voiceBank1.patch.mod_wheel_pwm, nullptr, nullptr, nullptr, nullptr}
                              };
  
  for (uint8_t row = 0; row < nrRows; row++)
  {
    for (uint8_t column = 0; column < nrColumns; column++)
    {
      if (parameterIndexes[row][column] != N_A)
      {
        widgetIndex = pages[PAGE].addWidget(parameterIndexes[row][column], (column + 1) * (columnWidth + padding), (row + 1) * (rowHeight + padding), columnWidth, rowHeight);
        pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
        pages[PAGE].widgetPointers[widgetIndex]->drawVariable = false;
        pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 4;
        pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 12;
        pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = varPointers[row][column];
        pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
        pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
        pages[PAGE].widgetPointers[widgetIndex]->varMin = -1.0;
        pages[PAGE].widgetPointers[widgetIndex]->varMax = 1.0;
      }
    }
  }

  widgetIndex = pages[PAGE].addWidget(PAGE_MODULATION, 700, 430, 100, 50);
  pages[PAGE].widgetPointers[widgetIndex]->label("PAGE1>");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 0, 430, 100, 50);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_effects()
{
  uint8_t PAGE = PAGE_EFFECTS;
  uint8_t widgetIndex = 0;
  uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;

  const uint16_t column_w = 140;
  const uint16_t row_h = 50;
  const uint8_t padding = 4;
  //const uint16_t varOffsetX = 80;
  const uint16_t varOffsetX = 4;
  const uint16_t varOffsetY = 24;
   
  staticIndex = pages[PAGE].addStatic(0, 0* (column_w + padding), 0* (row_h + padding), column_w, 40); 
  pages[PAGE].staticPointers[staticIndex]->label("MIX");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = 0xffff;

  staticIndex = pages[PAGE].addStatic(0, 1* (column_w + padding), 0* (row_h + padding), column_w, 40); 
  pages[PAGE].staticPointers[staticIndex]->label("REVERB");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = 0xffff;
  
  staticIndex = pages[PAGE].addStatic(0, 2* (column_w + padding), 0* (row_h + padding), column_w, 40); 
  pages[PAGE].staticPointers[staticIndex]->label("CHORUS");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = 0xffff;

  staticIndex = pages[PAGE].addStatic(0, 3* (column_w + padding), 0* (row_h + padding), column_w, 40); 
  pages[PAGE].staticPointers[staticIndex]->label("DELAY");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = 0xffff;

  staticIndex = pages[PAGE].addStatic(0, 4* (column_w + padding), 0* (row_h + padding), column_w, 40); 
  pages[PAGE].staticPointers[staticIndex]->label("PHASER");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = 0xffff;


  widgetIndex = pages[PAGE].addWidget(EFX_SEND_DRY, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Dry");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.dryLevel;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(EFX_SEND_REVERB, 0* (column_w + padding), 2* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Rev");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.reverbSend;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(EFX_SEND_CHORUS, 0* (column_w + padding), 3* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Ch/Ph");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.chorusSend;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(EFX_SEND_DELAY, 0* (column_w + padding), 4* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Del");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.delaySend;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;


  widgetIndex = pages[PAGE].addWidget(REVERB_SIZE, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Size");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.reverb_size;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(REVERB_HIDAMP, 1* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("HiD");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.reverb_hidamp;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(REVERB_LODAMP, 1* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("LoD");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.reverb_lodamp;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(REVERB_LOWPASS, 1* (column_w + padding), 4* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("LoP");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.reverb_lowpass;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(REVERB_DIFFUSION, 1* (column_w + padding), 5* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Diff");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.reverb_diffusion;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(REVERB_FEEDBACK, 1* (column_w + padding), 6* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Shimr");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.reverb_feedback;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(GRANULAR_SPEED, 1* (column_w + padding), 7* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Pitch");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.granular_speed;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->varMax = 5.0;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;


  widgetIndex = pages[PAGE].addWidget(CHORUS_LFORATE, 2* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Rate");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.chorus_lfoRate;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->varMax = 16.0;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;


  widgetIndex = pages[PAGE].addWidget(DELAY_TYPE, 3* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Type");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &audioParameters.delay_type;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;
  

  widgetIndex = pages[PAGE].addWidget(DELAY_TIME, 3* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Time");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.delay_time;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->varMax = 400;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(DELAY_FEEDBACK, 3* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Feed");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.delay_feedback;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;


  widgetIndex = pages[PAGE].addWidget(PHASER_MIX, 4* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Mix");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.phaser_mix;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(PHASER_LFORATE, 4* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Rate");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.phaser_lfoRate;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(PHASER_STAGES, 4* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Stgs");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &audioParameters.phaser_stages;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->varMax = 12.0;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(PHASER_FEEDBACK, 4* (column_w + padding), 4* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Feed");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.phaser_feedback;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(PHASER_DEPTH, 4* (column_w + padding), 5* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Dept");
  //pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = varOffsetY;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &audioParameters.phaser_depth;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustAudioParameter;
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_BOX_HBAR;
  pages[PAGE].widgetPointers[widgetIndex]->fontSize = 14;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 700, 430, 100, 50);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_patchName()
{
  uint8_t PAGE = PAGE_PATCHNAME;
  uint8_t widgetIndex = 0;
  uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;

  staticIndex = pages[PAGE].addStatic(0, 20, 20, 600, 420);
  pages[PAGE].staticPointers[staticIndex]->color1 = PATCHNAME_BG_COLOR;
  pages[PAGE].staticPointers[staticIndex]->color2 = PATCHNAME_BG_COLOR;
  pages[PAGE].staticPointers[staticIndex]->label("SAVE AS");
  pages[PAGE].staticPointers[staticIndex]->labelOffsetX = 2;
  pages[PAGE].staticPointers[staticIndex]->labelOffsetY = 14;

  widgetIndex = pages[PAGE].addWidget(0, 150, 30, 60, 40);
  pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &peekPatchNr;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &peekPatchNameWrapper;

  staticIndex = pages[PAGE].addStatic(0, 224, 30, 276, 40);
  pages[PAGE].staticPointers[staticIndex]->drawLabel = false;
  pages[PAGE].staticPointers[staticIndex]->drawVariable = true;
  pages[PAGE].staticPointers[staticIndex]->varOffsetX = 10;
  pages[PAGE].staticPointers[staticIndex]->varOffsetY = 4;
  pages[PAGE].staticPointers[staticIndex]->var_ptr_str = &peekPatchNameUI;

  const uint8_t nr_characters = PATCH_NAME_NR_CHARACTERS - 1; //last is null termination
  const uint8_t charWidth = 40;
  const uint8_t charHeight = 50;
  const uint8_t padding = 6;
  for (uint8_t charPos = 0; charPos < nr_characters; charPos++)
  {
    widgetIndex = pages[PAGE].addWidget(charPos, 40 + charPos * (charWidth + padding), 80, charWidth, charHeight);
    pages[PAGE].widgetPointers[widgetIndex]->drawLabel = false;
    pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
    pages[PAGE].widgetPointers[widgetIndex]->var_ptr_char = &patchInfo.name[charPos];
    pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustCharacter;
    pages[PAGE].widgetPointers[widgetIndex]->activateCb= &setCharPosition;
    pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 6;
    pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 10;
  }

  const uint8_t key_x0 = 40;
  const uint8_t digit_y0 = 150;
  const uint8_t ch_y0 = 210;
  const uint8_t key_w = 40;
  const uint8_t key_h = 50;
  const uint8_t key_pad = 2;

  for (uint8_t digit = 0; digit < 10; digit++)
  {
    widgetIndex = pages[PAGE].addWidget(digit + 49 , key_x0 + digit * (key_w + key_pad), digit_y0, key_w, key_h);
    if (digit == 9) pages[PAGE].widgetPointers[widgetIndex]->id = 48; // "0"
    pages[PAGE].widgetPointers[widgetIndex]->label( (char)(pages[PAGE].widgetPointers[widgetIndex]->id) );
    pages[PAGE].widgetPointers[widgetIndex]->fontSize = 1;
    pages[PAGE].widgetPointers[widgetIndex]->selectOnPress = false;
    pages[PAGE].widgetPointers[widgetIndex]->activateCb = &keyboardInput;
  }

  for (uint8_t ch = 0; ch < 26; ch++)
  {
    uint8_t row = ch / 10;
    uint8_t col = ch % 10;
    widgetIndex = pages[PAGE].addWidget(ch + 65 , key_x0 + col * (key_w + key_pad), ch_y0 + row * (key_h + key_pad), key_w, key_h);
    pages[PAGE].widgetPointers[widgetIndex]->label( (char)(pages[PAGE].widgetPointers[widgetIndex]->id) );
    pages[PAGE].widgetPointers[widgetIndex]->fontSize = 1;
    pages[PAGE].widgetPointers[widgetIndex]->selectOnPress = false;
    pages[PAGE].widgetPointers[widgetIndex]->activateCb = &keyboardInput;
  }

  widgetIndex = pages[PAGE].addWidget(0, 30, 380, 160, 50);
  pages[PAGE].widgetPointers[widgetIndex]->label("SAVE");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &savePatchWrapper;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 420, 380, 160, 50);
  pages[PAGE].widgetPointers[widgetIndex]->label("CANCEL");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;

}

FLASHMEM void configurePage_lfo()
{
  uint8_t PAGE = PAGE_LFO;
  uint8_t widgetIndex = 0;
  uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateEnvelope3;

  const uint16_t column_w = 130;
  const uint16_t row_h = 60;
  const uint8_t padding = 4;
  const uint16_t varOffsetX = 70;

  staticIndex = pages[PAGE].addStatic(0, 0* (column_w + padding), 0* (row_h + padding), column_w, 40); 
  pages[PAGE].staticPointers[staticIndex]->label("LFO1");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;

  staticIndex = pages[PAGE].addStatic(0, 1* (column_w + padding), 0* (row_h + padding), column_w, 40); 
  pages[PAGE].staticPointers[staticIndex]->label("LFO2");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;

  staticIndex = pages[PAGE].addStatic(0, 2* (column_w + padding), 0* (row_h + padding), SCREEN_XRES - 2* (column_w + padding), 40); 
  pages[PAGE].staticPointers[staticIndex]->label("ENV3");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;
  
  widgetIndex = pages[PAGE].addWidget(LFO1_WAVEFORM, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Wfm");
  pages[PAGE].widgetPointers[widgetIndex]->drawWaveform = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.lfo1_waveform;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(LFO1_FREQUENCY, 0* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Freq");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.lfo1Frequency;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(LFO1_AMPLITUDE, 0* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Ampl");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.lfo1Level;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  // widgetIndex = pages[PAGE].addWidget(LFO1_FILTER_MODE, 0* (column_w + padding), 4* (row_h + padding), column_w, row_h); 
  // pages[PAGE].widgetPointers[widgetIndex]->label("Flt");
  // pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  // pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  // pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.lfo1FilterMode;
  // pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;


  widgetIndex = pages[PAGE].addWidget(LFO2_WAVEFORM, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("LFO2");
  pages[PAGE].widgetPointers[widgetIndex]->drawWaveform = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.lfo2_waveform;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(LFO2_FREQUENCY, 1* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Freq");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.lfo2Frequency;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(LFO2_AMPLITUDE, 1* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("Ampl");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.lfo2Level;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  // widgetIndex = pages[PAGE].addWidget(LFO2_FILTER_MODE, 1* (column_w + padding), 4* (row_h + padding), column_w, row_h); 
  // pages[PAGE].widgetPointers[widgetIndex]->label("Filt");
  // pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  // pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  // pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.lfo2FilterMode;
  // pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;


  widgetIndex = pages[PAGE].addWidget(ENV3_ATTACK, 2* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Attack");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.envelope3_attack;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(ENV3_DECAY, 3* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Decay");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.envelope3_decay;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(ENV3_SUSTAIN, 4* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Sustain");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 1;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.envelope3_sustain;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(ENV3_RELEASE, 5* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Release");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.envelope3_release;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_wavetable()
{
  uint8_t PAGE = PAGE_WAVETABLE;
  uint8_t widgetIndex = 0;
  //uint8_t staticIndex = 0;

  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateWavetable;

  const uint16_t column_w = 140;
  const uint16_t row_h = 50;
  const uint8_t padding = 4;
  const uint16_t varOffsetX = 60;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_INDEX, 0* (column_w + padding), 0* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("wt#");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_index;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_MODE, 1* (column_w + padding), 0* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("mod");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_mode;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_START, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("strt");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_start;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_LENGTH, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("len");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_length;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;
  
  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_INTERVAL, 0* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("int");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_interval;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_STEPSIZE, 1* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("stp");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_stepSize;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;


 widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_INDEX, 0* (column_w + padding), 5* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("wt#");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc2_waveTable_index;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE2_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_MODE, 1* (column_w + padding), 5* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("mod");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc2_waveTable_mode;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE2_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_START, 0* (column_w + padding), 6* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("strt");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_start;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE2_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_LENGTH, 1* (column_w + padding), 6* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("len");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_length;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE2_IDLE;
  
  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_INTERVAL, 0* (column_w + padding), 7* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("int");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_interval;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE2_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_STEPSIZE, 1* (column_w + padding), 7* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgetPointers[widgetIndex]->label("stp");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_stepSize;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE2_IDLE;


  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_wavetable2()
{
  uint8_t PAGE = PAGE_WAVETABLE2;
  uint8_t widgetIndex = 0;
  //uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateWavetable2;

  const uint16_t rowHeight = 60;
  const uint16_t columnWidth = 160;
  const uint16_t padding = 4;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_INDEX, 0, 0 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgetPointers[widgetIndex]->label("wt#");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 100;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_index;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_MODE, 0, 1 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgetPointers[widgetIndex]->label("mod");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 100;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_mode;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_START, 0, 2 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgetPointers[widgetIndex]->label("strt");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_start;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_LENGTH, 0, 3 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgetPointers[widgetIndex]->label("len");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_length;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;
  
  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_INTERVAL, 0, 4 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgetPointers[widgetIndex]->label("int");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_interval;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_STEPSIZE, 0, 5 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgetPointers[widgetIndex]->label("stp");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_stepSize;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgetPointers[widgetIndex]->color2 = WAVETABLE1_IDLE;


  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_wavetableOsc1()
{
  uint8_t PAGE = PAGE_WAVETABLE_OSC1;
  uint8_t widgetIndex = 0;
  uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateWavetableOsc1;

  const uint16_t row_h = 50;
  const uint16_t column_w = 130;
  const uint16_t padding = 4;
  const uint16_t varOffsetX = 60;
  
  staticIndex = pages[PAGE].addStatic(0, 0* (column_w + padding), 0* (row_h + padding), column_w, 40); 
  pages[PAGE].staticPointers[staticIndex]->label("OSC1");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_INDEX, 0, 1 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("wt#");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_index;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_MODE, 0, 2 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("mod");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_mode;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_START, 0, 3 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("strt");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_start;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_LENGTH, 0, 4 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("len");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_length;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  
  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_INTERVAL, 0, 5 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("int");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_interval;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_STEPSIZE, 0, 6 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("stp");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_stepSize;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_MOVEMENT, 0, 7 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("mov");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_movement;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;


  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, SCREEN_XRES - column_w, SCREEN_YRES - 44, column_w, 44);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_wavetableOsc2()
{
  uint8_t PAGE = PAGE_WAVETABLE_OSC2;
  uint8_t widgetIndex = 0;
  uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateWavetableOsc2;

  const uint16_t row_h = 50;
  const uint16_t column_w = 130;
  const uint16_t padding = 4;
  const uint16_t varOffsetX = 60;
  
  staticIndex = pages[PAGE].addStatic(0, 0* (column_w + padding), 0* (row_h + padding), column_w, 40); 
  pages[PAGE].staticPointers[staticIndex]->label("OSC2");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_INDEX, 0, 1 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("wt#");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc2_waveTable_index;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_MODE, 0, 2 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("mod");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc2_waveTable_mode;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_START, 0, 3 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("strt");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_start;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_LENGTH, 0, 4 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("len");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_length;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;
  
  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_INTERVAL, 0, 5 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("int");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_interval;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_STEPSIZE, 0, 6 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("stp");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_stepSize;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_MOVEMENT, 0, 7 * (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("mov");
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &voiceBank1.patch.osc2_waveTable_movement;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;



  //widgetIndex = pages[PAGE].addWidget(0, 1 * (column_w + padding), SCREEN_YRES - 44, column_w, 44);
  //pages[PAGE].widgetPointers[widgetIndex]->label("cp1");
  //pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = varOffsetX;
  //pages[PAGE].widgetPointers[widgetIndex]->activateCb = &copyWavetableSettings;


  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, SCREEN_XRES - column_w, SCREEN_YRES - 44, column_w, 44);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_envelope()
{
  uint8_t PAGE = PAGE_ENVELOPE;
  uint8_t widgetIndex = 0;
  uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateAmpEnvelope;

  const uint16_t column_w = 140;
  const uint16_t row_h = 60;
  const uint8_t padding = 4;

  staticIndex = pages[PAGE].addStatic(0, 0* (column_w + padding), 0* (row_h + padding), 4* (column_w + padding), 40); 
  pages[PAGE].staticPointers[staticIndex]->label("AMP ENV");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;

  widgetIndex = pages[PAGE].addWidget(ENV_ATTACK, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Attack");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.ampEnvelope_attack;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(ENV_DECAY, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Decay");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.ampEnvelope_decay;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(ENV_SUSTAIN, 2* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Sustain");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 1;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.ampEnvelope_sustain;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(ENV_RELEASE, 3* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Release");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.ampEnvelope_release;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_filter()
{
  uint8_t PAGE = PAGE_FILTER;
  uint8_t widgetIndex = 0;
  uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateFilterEnvelope;

  const uint16_t column_w = 140;
  const uint16_t row_h = 60;
  const uint8_t padding = 4;

  staticIndex = pages[PAGE].addStatic(0, 0* (column_w + padding), 0* (row_h + padding), 5* (column_w + padding), 40); 
  pages[PAGE].staticPointers[staticIndex]->label("FLT ENV");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;

  staticIndex = pages[PAGE].addStatic(0, 3* (column_w + padding), 3* (row_h + padding), 2* (column_w + padding), 40); 
  pages[PAGE].staticPointers[staticIndex]->label("FLT PAR");
  pages[PAGE].staticPointers[staticIndex]->color2 = HEADER_COLOR;
  pages[PAGE].staticPointers[staticIndex]->textColor1 = HEADER_TEXT_COLOR;

  widgetIndex = pages[PAGE].addWidget(FILTER_ATTACK, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Attack");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.filterEnvelope_attack;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_DECAY, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Decay");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.filterEnvelope_decay;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_SUSTAIN, 2* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Sustain");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 1;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.filterEnvelope_sustain;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_RELEASE, 3* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Release");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.filterEnvelope_release;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTERENV_PWR, 4* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Ampl");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 1;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.envToFilter;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;


  widgetIndex = pages[PAGE].addWidget(FILTER_CUTOFF, 3* (column_w + padding), 4* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Freq");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 2;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.cutoff;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_RESONANCE, 4* (column_w + padding), 4* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Reso");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 70;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 1;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.resonance;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_controls()
{
  uint8_t PAGE = PAGE_CONTROLS;
  uint8_t widgetIndex = 0;
  //uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;

  const uint16_t column_w = 100;
  const uint16_t row_h = 400;
  const uint8_t padding = 4;

  widgetIndex = pages[PAGE].addWidget(FILTER_CUTOFF, 0* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("FRQ");
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_SLIDER_V;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.cutoff;
  pages[PAGE].widgetPointers[widgetIndex]->setF = &setVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_RESONANCE, 1* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("RES");
  pages[PAGE].widgetPointers[widgetIndex]->type = WIDGET_SLIDER_V;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_f = &voiceBank1.patch.resonance;
  pages[PAGE].widgetPointers[widgetIndex]->setF = &setVoiceBankWrapper;
  pages[PAGE].widgetPointers[widgetIndex]->color1 = WAVETABLE2_SELECTED;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

FLASHMEM void configurePage_system()
{
  uint8_t PAGE = PAGE_SYSTEM;
  uint8_t widgetIndex = 0;
  //uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateSystemPage;

  const uint16_t column_w = 140;
  const uint16_t row_h = 60;
  const uint8_t padding = 4;

  widgetIndex = pages[PAGE].addWidget(SYS_MIDICHANNEL, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Ch 1");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &midiSettings.channel;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustMidiParameter;

  widgetIndex = pages[PAGE].addWidget(SYS_MIDICHANNEL2, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Ch 2");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &midiSettings.channel2;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustMidiParameter;


  widgetIndex = pages[PAGE].addWidget(0, 3* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgetPointers[widgetIndex]->label("Bright");
  pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 80;
  pages[PAGE].widgetPointers[widgetIndex]->varOffsetY = 30;
  pages[PAGE].widgetPointers[widgetIndex]->floatPrecision = 0;
  pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &brightness;
  pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustBrightness;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgetPointers[widgetIndex]->label("<BACK");
  pages[PAGE].widgetPointers[widgetIndex]->activateCb = &setPage;
}

//void configurePageArpeggiator()
//{
    // widgetIndex = pages[PAGE].addWidget(0, 0, 0, 80, 20);
  // pages[PAGE].widgetPointers[widgetIndex]->label("MOD");
  // pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 45;
  // pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  // pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &midiSettings.arp_mode;
  // pages[PAGE].widgetPointers[widgetIndex]->setI8 = &setArpeggiatorMode;

  // widgetIndex = pages[PAGE].addWidget(0, 0, 22, 80, 20);
  // pages[PAGE].widgetPointers[widgetIndex]->label("BPM");
  // pages[PAGE].widgetPointers[widgetIndex]->varOffsetX = 45;
  // pages[PAGE].widgetPointers[widgetIndex]->drawVariable = true;
  // pages[PAGE].widgetPointers[widgetIndex]->var_ptr_u8 = &midiSettings.bpm;
  // pages[PAGE].widgetPointers[widgetIndex]->setI8 = &adjustBpm;
//}

FLASHMEM void updateVarPointers(uint8_t bankId)
{
  for(uint8_t pageId = 0; pageId < NR_PAGES; pageId++)
  {
    uint8_t widgetId = 0;
    while (pages[pageId].widgetPointers[widgetId] != nullptr)
    {
      switch(pages[pageId].widgetPointers[widgetId++]->id)
      {
        case OSC1_WAVEFORM:
          break;
        case OSC2_WAVEFORM:
          break;
        case TRANSPOSE:
          break;
        case DETUNE:
          break;
        case OSC1_LEVEL:
          break;
        case OSC2_LEVEL:
          break;
        case PULSE_LEVEL:
          break;
        case NOISE_LEVEL:
          break;
        case ENV_ATTACK:
          break;
        case ENV_DECAY:
          break;
        case ENV_SUSTAIN:
          break;
        case ENV_RELEASE:
          break;
        case FILTER_ATTACK:
          break;
        case FILTER_DECAY:
          break;
        case FILTER_SUSTAIN:
          break;
        case FILTER_RELEASE:
          break;
        case FILTER_CUTOFF:
          break;
        case FILTER_RESONANCE:
          break;
        case FILTERENV_PWR:
          break;
        case OSC1_SYNC:
          break;
        case OSC2_SYNC:
          break;
        case OSC1_WAVETABLE_INDEX:
          break;
        case OSC1_WAVETABLE_MODE:
          break;
        case OSC1_WAVETABLE_START:
          break;
        case OSC1_WAVETABLE_LENGTH:
          break;
        case OSC1_WAVETABLE_INTERVAL:
          break;
        case OSC1_WAVETABLE_STEPSIZE:
          break;
        case OSC1_WAVETABLE_MOVEMENT:
          break;
        case OSC2_WAVETABLE_INDEX:
          break;
        case OSC2_WAVETABLE_MODE:
          break;
        case OSC2_WAVETABLE_START:
          break;
        case OSC2_WAVETABLE_LENGTH:
          break;
        case OSC2_WAVETABLE_INTERVAL:
          break;
        case OSC2_WAVETABLE_STEPSIZE:
          break;
        case OSC2_WAVETABLE_MOVEMENT:
          break;

        case ENV3_ATTACK:
          break;
        case ENV3_DECAY:
          break;
        case ENV3_SUSTAIN:
          break;
        case ENV3_RELEASE:
          break;

        case MOD_ENV3_OSC1_PITCH:
          break;
        case MOD_ENV3_OSC2_PITCH:
          break;
        case MOD_ENV3_OSC1_PHASE:
          break;
        case MOD_ENV3_OSC2_PHASE:
          break;
        case MOD_ENV3_FILTER_CUTOFF:
          break;
        case MOD_ENV3_PWM:
          break;
        case MOD_ENV3_AM_PITCH:
          break;
        case MOD_ENV3_FM_PITCH:
          break;
        case MOD_ENV3_LFO1_AMPLITUDE:
          break;
        case MOD_ENV3_LFO2_AMPLITUDE:
          break;

        case MOD_LFO1_OSC1_PITCH:
          break;
        case MOD_LFO1_OSC2_PITCH:
          break;
        case MOD_LFO1_OSC1_PHASE:
          break;
        case MOD_LFO1_OSC2_PHASE:
          break;
        case MOD_LFO1_FILTER_CUTOFF:
          break;
        case MOD_LFO1_PWM:
          break;
        case MOD_LFO1_AM_PITCH:
          break;
        case MOD_LFO1_FM_PITCH:
          break;

        case MOD_LFO2_OSC1_PITCH:
          break;
        case MOD_LFO2_OSC2_PITCH:
          break;
        case MOD_LFO2_OSC1_PHASE:
          break;
        case MOD_LFO2_OSC2_PHASE:
          break;
        case MOD_LFO2_FILTER_CUTOFF:
          break;
        case MOD_LFO2_PWM:
          break;
        case MOD_LFO2_AM_PITCH:
          break;
        case MOD_LFO2_FM_PITCH:
          break;

        case MOD_VEL_OSC1_PITCH:
          break;
        case MOD_VEL_OSC2_PITCH:
          break;
        case MOD_VEL_OSC1_PHASE:
          break;
        case MOD_VEL_OSC2_PHASE:
          break;
        case MOD_VEL_FILTER_CUTOFF:
          break;
        case MOD_VEL_PWM:
          break;

        case MOD_WHL_OSC1_PITCH:
          break;
        case MOD_WHL_OSC2_PITCH:
          break;
        case MOD_WHL_OSC1_PHASE:
          break;
        case MOD_WHL_OSC2_PHASE:
          break;
        case MOD_WHL_FILTER_CUTOFF:
          break;
        case MOD_WHL_PWM:
          break;

        case MOD_OSC1_OSC2_PITCH:
          break;
        case MOD_OSC1_OSC2_PHASE:
          break;
        case MOD_OSC2_OSC1_PITCH:
          break;
        case MOD_OSC2_OSC1_PHASE:
          break;

        case AM_FREQ_MULTIPLIER:
          break;
        case AM_LEVEL:
          break;
        case AM_WAVEFORM:
          break;
        case AM_FIXEDFREQUENCY:
          break;

        case FM_FREQ_MULTIPLIER:
          break;
        case FM_LEVEL:
          break;
        case FM_WAVEFORM:
          break;
        case FM_OFFSET:
          break;

        case EFX_SEND_DRY:
          break;
        case EFX_SEND_REVERB:
          break;
        case EFX_SEND_CHORUS:
          break;
        case EFX_SEND_PHASER:
          break;
        case EFX_SEND_DELAY:
          break;
      }
    }
  }
}
    


// --- Widget callback functions ---

void setPage(uint8_t page) { currentPage = page; }

void changePatch(uint8_t callerId, int8_t delta)
{
  int8_t desiredPatchNr = currentPatchNr + delta;
  if (desiredPatchNr < 0) desiredPatchNr = NR_PATCHES - 1;
  if (desiredPatchNr > NR_PATCHES - 1) desiredPatchNr = 0;

  if (loadPatch(desiredPatchNr) == LOAD_OK)
  {
    currentPatchNr = desiredPatchNr;
    peekPatchNr = currentPatchNr;
    if (currentPage == PAGE_PATCH) pages[PAGE_PATCH].widgetPointers[0]->draw(false);
  }
  else Serial.println(F("Load failed"));
}

void adjustVoiceBankWrapper(uint8_t index, int8_t delta) { voiceBank1.adjustParameter(index, delta); }

void setVoiceBankWrapper(uint8_t index, float value) { voiceBank1.setParameter(index, value); }

FLASHMEM void setCharPosition(uint8_t index)
{
  currentCharPosition = index;
}

FLASHMEM void adjustCharacter(uint8_t charPos, int8_t delta)
{
  char temp = patchInfo.name[charPos] + delta;
  if (temp < 32) temp = 32;
  if (temp > 122) temp = 122;
  patchInfo.name[charPos] = temp;
  if (currentPage == PAGE_PATCHNAME) pages[currentPage].widgetPointers[charPos]->draw(true);
}

FLASHMEM void keyboardInput(uint8_t index)
{
  patchInfo.name[currentCharPosition] = index;
  if (currentPage == PAGE_PATCHNAME) pages[currentPage].widgetPointers[1 + currentCharPosition]->draw(false); // ugly and will break!
  if (currentCharPosition < (PATCH_NAME_NR_CHARACTERS - 1) ) currentCharPosition++;
}

FLASHMEM void savePatchWrapper(uint8_t index)
{
  savePatch(peekPatchNr);
  currentPatchNr = peekPatchNr;
  loadPatch(currentPatchNr);
  setPage(pages[currentPage].backPageId);
}

void peekPatchNameWrapper(uint8_t index, int8_t delta)
{
  //char buf[PATCH_NAME_NR_CHARACTERS];
  peekPatchNr = peekPatchNr + delta;
  peekPatchNr = constrain(peekPatchNr, 0, NR_PATCHES - 1);
  peekPatchName(peekPatchNr);
  pages[PAGE_PATCHNAME].staticPointers[1]->draw(false);
}

FLASHMEM void adjustBrightness(uint8_t index, int8_t delta)
{
  brightness = brightness + delta;
  tft.brightness(brightness);
}

FLASHMEM void copyWavetableSettings(uint8_t index)
{

}

FLASHMEM void setActiveVoiceBank(uint8_t index, int8_t delta)
{
  currentVoiceBank = currentVoiceBank + delta;
  currentVoiceBank = constrain(currentVoiceBank, 0, 1);
  //update page pointers
  // redraw current page
}

// --- Animation functions ---

void animateWavetable(bool firstCall)
{
  static elapsedMillis animateTimer = 0;
  const uint16_t x0 = 400;
  const uint16_t yMid1 = 100;
  const uint16_t yMid2 = 360;
  const uint16_t h = 60;

  int16_t * waveTable1 = &waveTable1_I16[voiceBank1.patch.osc1_waveTable_start];
  static int16_t oldWaveTable1[256];
  int16_t * waveTable2 = &waveTable2_I16[voiceBank1.patch.osc2_waveTable_start];
  static int16_t oldWaveTable2[256];

  static uint16_t oldStart1 = 0;
  static uint16_t oldStart2 = 0;

  static uint16_t oldLength1 = 0;
  static uint16_t oldLength2 = 0;

  if ( (animateTimer > 85) || firstCall)
  {
    animateTimer = 0;

    for (uint16_t index = 0; index < 256; index++)
    {
      int16_t sample1 = waveTable1[index];
      if ( (oldWaveTable1[index] != sample1) || firstCall)
      {
        oldWaveTable1[index] = sample1;
        tft.drawFastVLine(x0 + index, yMid1 - h, 2 * h, MAIN_BG_COLOR);
        uint16_t height = abs(sample1) >> 10;
        if (sample1 < 0) tft.drawFastVLine(x0 + index, yMid1, height, WAVETABLE1_SELECTED);
        else tft.drawFastVLine(x0 + index, yMid1 - height, height, WAVETABLE1_SELECTED);
      }

      int16_t sample2 = waveTable2[index];
      if ( (oldWaveTable2[index] != sample2) || firstCall)
      {
        oldWaveTable2[index] = sample2;
        tft.drawFastVLine(x0 + index, yMid2 - h, 2 * h, MAIN_BG_COLOR);
        uint16_t height = abs(sample2) >> 10;
        if (sample2 < 0) tft.drawFastVLine(x0 + index, yMid2, height, WAVETABLE2_SELECTED);
        else tft.drawFastVLine(x0 + index, yMid2 - height, height, WAVETABLE2_SELECTED);
      }
    }

    if ( (oldStart1 != voiceBank1.patch.osc1_waveTable_start) || (oldLength1 != voiceBank1.patch.osc1_waveTable_length) || firstCall )
    {
      oldStart1 = voiceBank1.patch.osc1_waveTable_start;
      oldLength1 = voiceBank1.patch.osc1_waveTable_length;
      uint16_t boxStart = voiceBank1.patch.osc1_waveTable_start >> 6;
      uint16_t boxWidth = voiceBank1.patch.osc1_waveTable_length >> 6;
      tft.fillRect(x0, yMid1 + h, 256, 5, WAVETABLE_BAR_BG);
      tft.fillRect(x0 + boxStart, yMid1 + h, boxWidth, 5, WAVETABLE1_SELECTED);
    }

    if ( (oldStart2 != voiceBank1.patch.osc2_waveTable_start) || (oldLength2 != voiceBank1.patch.osc2_waveTable_length) || firstCall )
    {
      oldStart2 = voiceBank1.patch.osc2_waveTable_start;
      oldLength2 = voiceBank1.patch.osc2_waveTable_length;
      uint16_t boxStart = voiceBank1.patch.osc2_waveTable_start >> 6;
      uint16_t boxWidth = voiceBank1.patch.osc2_waveTable_length >> 6;
      tft.fillRect(x0, yMid2 + h, 256, 5, WAVETABLE_BAR_BG);
      tft.fillRect(x0 + boxStart, yMid2 + h, boxWidth, 5, WAVETABLE2_SELECTED);
    }
  }
}

void animateWavetable2(bool firstCall)
{
  static elapsedMillis animateTimer = 0;
  int16_t * startWave = &waveTable1_I16[(voiceBank1.patch.osc1_waveTable_start / 256) * 256];
  static int16_t oldStartWave[256];
  int16_t * endWave = &waveTable1_I16[((voiceBank1.patch.osc1_waveTable_start + voiceBank1.patch.osc1_waveTable_length) / 256) * 256];
  static int16_t oldEndWave[256];
  int16_t * currentWave = voiceBank1.currentWaveform_I16;
  static int16_t oldCurrentWave[256];

  const uint16_t x0 = 250;
  const uint16_t yMidStart = 400;
  const uint16_t yMidCurrent = 250;
  const uint16_t yMidEnd = 100;
  const uint16_t xOffset = 120;
  const uint8_t scaler = 10;

  if (firstCall)
  {
    tft.drawLine(x0 - 5, yMidStart + 5, x0 - 5 + 2 * xOffset, yMidEnd, WAVETABLE_LANES);
    tft.drawLine(x0 + 256 + 5, yMidStart, x0 + 256 + 5 + 2 * xOffset, yMidEnd, WAVETABLE_LANES);
  }

  if (animateTimer > 50 || firstCall)
  {
    animateTimer = 0;
  
    for (uint16_t index = 0; index < 256; index++)
    {
      int16_t startSample = startWave[index];
      int16_t endSample = endWave[index];
      int16_t currentSample = currentWave[index];
      
      if ( (oldStartWave[index] != startSample) || firstCall)
      {
        tft.drawPixel(x0 + index, yMidStart + (oldStartWave[index] >> scaler), MAIN_BG_COLOR);
        oldStartWave[index] = startSample;
        tft.drawPixel(x0 + index, yMidStart + (startSample >> scaler), WAVETABLE1_SELECTED);
      }

      if ( (oldCurrentWave[index] != currentSample) || firstCall)
      {
        tft.drawPixel(x0 + xOffset + index, yMidCurrent + (oldCurrentWave[index] >> scaler), MAIN_BG_COLOR);
        oldCurrentWave[index] = currentSample;
        tft.drawPixel(x0 + xOffset + index, yMidCurrent + (currentSample >> scaler), WAVETABLE1_SELECTED);
      }

      if ( (oldEndWave[index] != endSample) || firstCall)
      {
        tft.drawPixel(x0 + 2 * xOffset + index, yMidEnd + (oldEndWave[index] >> scaler), MAIN_BG_COLOR);
        oldEndWave[index] = endSample;
        tft.drawPixel(x0 + 2 * xOffset + index, yMidEnd + (endSample >> scaler), WAVETABLE1_SELECTED);
      }
    }

  }
}

void animateWavetableOsc1(bool firstCall)
{
  animateWavetableMatrixView(firstCall, 1);
}

void animateWavetableOsc2(bool firstCall)
{
  animateWavetableMatrixView(firstCall, 2);
}

void animateWavetableMatrixView(bool firstCall, uint8_t oscillator)
{
  const uint16_t x0 = 150;
  const uint16_t y0 = 26;
  const uint16_t height = 22;
  const uint8_t paddingX = 6;
  const uint8_t paddingY = 6;
  const uint8_t thumbnailSamples = 75;

  static uint8_t oldWavetableIndex = 0;
  static uint16_t oldStart = 0;
  static uint16_t oldLength = 0;

  uint8_t newWavetableIndex = 0;
  uint16_t newStart = 0;
  uint16_t newLength = 0;

  static bool drawBoxes = true;
  static bool drawName = true;

  if (oscillator == 1)
  {
    newWavetableIndex = voiceBank1.patch.osc1_waveTable_index;
    newStart = voiceBank1.patch.osc1_waveTable_start;
    newLength = voiceBank1.patch.osc1_waveTable_length;
  }
  else
  {
    newWavetableIndex = voiceBank1.patch.osc2_waveTable_index;
    newStart = voiceBank1.patch.osc2_waveTable_start;
    newLength = voiceBank1.patch.osc2_waveTable_length;
  }

  if (firstCall || (oldWavetableIndex != newWavetableIndex) )
  {
    oldWavetableIndex = newWavetableIndex;
    tft.fillRect(x0, y0, SCREEN_XRES - x0, SCREEN_YRES - y0 - 60, MAIN_BG_COLOR);
    drawBoxes = true;
    drawName = true;

    for (uint8_t waveformIndex = 0; waveformIndex < 64; waveformIndex++)
    {
      uint8_t row = waveformIndex / 8;
      uint8_t column = waveformIndex % 8;
      uint16_t xStart = x0 + column * (thumbnailSamples + paddingX);
      uint16_t yStart = y0 + height + row * (2 * height + paddingY);
      uint16_t startIndex = row * 8 * 256 + column * 256;
        
      for (uint8_t plotIndex = 0; plotIndex < thumbnailSamples; plotIndex++)
      {
        int16_t sampleIndex = map( plotIndex, 0, thumbnailSamples - 1, 0, 255);
        sampleIndex = startIndex + sampleIndex;
        int16_t sample = 0;
        if (oscillator == 1) sample = height * waveTable1_I16[sampleIndex] / 32768.0;
        else sample = height * waveTable2_I16[sampleIndex] / 32768.0;
        tft.drawPixel(xStart + plotIndex, yStart - sample, WAVETABLE1_SELECTED);
      }
    }
  }

  if ( (oldStart != newStart) || (oldLength != newLength) || drawBoxes)
  {
    drawBoxes = false;
    oldStart = newStart;
    oldLength = newLength;
    uint8_t waveformIndexStart = oldStart / 256; // 0-63
    uint8_t waveformIndexEnd = min( (oldStart + oldLength) / 256, 63);

    for (uint8_t waveformIndex = 0; waveformIndex < 64; waveformIndex++)
    {
      uint8_t row = waveformIndex / 8;
      uint8_t column = waveformIndex % 8;
      uint16_t xStart = x0 + column * (thumbnailSamples + paddingX);
      uint16_t yStart = y0 + height + row * (2 * height + paddingY);

      if ( (waveformIndex >= waveformIndexStart) && (waveformIndex <= waveformIndexEnd) ) tft.drawRect(xStart - 1, yStart - height - 1, thumbnailSamples + 2, height * 2 + 2, WAVETABLE1_INRANGE);
      else tft.drawRect(xStart - 1, yStart - height - 1, thumbnailSamples + 2, height * 2 + 2, WAVETABLE1_OUTSIDE);
    }
  }

  if (drawName)
  {
    drawName = false;
    tft.fillRoundRect(x0, 0, SCREEN_XRES - x0, 20, 3, HEADER_COLOR);
    tft.setTextColor(HEADER_TEXT_COLOR);
    tft.setFont(Arial_12);
    tft.setCursor(x0 + 4, 4);
    tft.print(waveTableNames[newWavetableIndex]);
  }
}

void animateScreenSaver(bool firstCall)
{
  const uint8_t mode = 3;
  //static elapsedMillis animateTimer = 0;
  static uint16_t color = SELECTED_COLOR;
  static uint8_t noteStatusBuffer[128];
  static uint8_t stage = 0;

  const uint16_t column_w = 4;
  const uint16_t row_h = 4;
  const uint16_t offset_x = 4;
  
  if (mode == 3) // Fireworks
  {
    if (stage == 0)
    {
      tft.BTE_move(0, row_h, SCREEN_XRES, SCREEN_YRES - row_h, offset_x, 0, 0, 0, false);
      stage = 1;
    }

    if ( (stage == 1) && !tft.readStatus() ) stage = 2;

    if (stage == 2)
    {
      uint16_t clear_x = 0;
      uint16_t clear_w = 0;
      
      for (uint8_t note = 0; note < 128; note++)
      {
        if (noteStatus[note] > 0) noteStatusBuffer[note] = 100;
        else if (noteStatusBuffer[note] > 0) noteStatusBuffer[note] = noteStatusBuffer[note] - 10;
        
        if (noteStatusBuffer[note] == 0)
        {
          clear_w = clear_w + column_w;
        }

        if (noteStatusBuffer[note] > 0)
        {
          tft.fillRect(clear_x, SCREEN_YRES - row_h, note * column_w, row_h, MAIN_BG_COLOR); // Clear row between events
          
          float fadeOut = noteStatusBuffer[note] / 100.0;
          float noteFactor = note / 127.0;
          color = tft.Color565(255 * (1 - noteFactor) * fadeOut, 255 * fadeOut * noteFactor, 255* fadeOut * noteFactor);

          tft.fillRect(note * column_w, SCREEN_YRES - row_h, column_w, row_h, color);
          clear_w = 0;
          clear_x = (note + 1) * column_w;
        }
        if (note == 127) tft.fillRect(clear_x, SCREEN_YRES - row_h, SCREEN_XRES - clear_x, row_h, MAIN_BG_COLOR);
      }
      stage = 0;
    }
  }

  if (mode == 4) // moving FFT
  {
    if (firstCall)
    {
      tft.BTE_enable(true);
      tft.setBackgroundColor(MAIN_BG_COLOR);
      tft.setForegroundColor(MAIN_BG_COLOR);
      tft.setTransparentColor(MAIN_BG_COLOR);
      stage = 0;
      return;
    }

    const uint8_t binMaxHeight = 64;
    const uint16_t move_y = 10;
    if (stage == 0)
    {
      // scroll 3d section
      //tft.BTE_move(0, binMaxHeight, SCREEN_XRES, SCREEN_YRES - binMaxHeight, offset_x, 0, 0, 0, false);
      tft.BTE_move(100, move_y, SCREEN_XRES - 100, SCREEN_YRES - binMaxHeight - move_y, 100 + offset_x, 0, 0, 0, false);
      stage = 1;
      return;
    }

    if ( (stage == 1) && !tft.readStatus() ) stage = 2;

    if (stage == 2)
    {
      // copy latest fft with transparency
      //tft.BTE_move(0, binMaxHeight, SCREEN_XRES, SCREEN_YRES - binMaxHeight, offset_x, 0, 0, 0, false);
      tft.BTE_move(100, SCREEN_YRES - binMaxHeight, 300, binMaxHeight, 100, SCREEN_YRES - 2 * binMaxHeight, 0, 0, true, 224);
      stage = 3;
      return;
    }

    if ( (stage == 3) && !tft.readStatus() ) stage = 4;

    if ( (stage == 4) && fft.available()) stage = 5;

    if (stage == 5)
    {
      // draw here
      tft.fillRect(100, SCREEN_YRES - binMaxHeight, 300, binMaxHeight, MAIN_BG_COLOR); // Clear row between events

      for (uint8_t bin = 0; bin < 128; bin++)
      {
        uint8_t binValue = (uint8_t)(fft.read(bin) * binMaxHeight * pow(2, bin/127.0) );
        //tft.fillRect(100 + (bin << 2), SCREEN_YRES - binValue - 2, 2, 2, MIDIEVENT_ON);
        tft.drawPixel(100 + (bin << 1), SCREEN_YRES - binValue, MIDIEVENT_ON);
      }
      stage = 0; //
    }
  }
}

void animateAmpEnvelope(bool firstCall)
{
  const uint16_t x0 = 50;
  const uint16_t y0 = 300;
  const uint16_t w = 300;
  const uint16_t h = 150;
  animateEnvelope(x0, y0, w, h, voiceBank1.patch.ampEnvelope_attack, voiceBank1.patch.ampEnvelope_decay, voiceBank1.patch.ampEnvelope_sustain, voiceBank1.patch.ampEnvelope_release, SELECTED_COLOR, firstCall);
}

void animateFilterEnvelope(bool firstCall)
{
  const uint16_t x0 = 50;
  const uint16_t y0 = 300;
  const uint16_t w = 300;
  const uint16_t h = 150;
  animateEnvelope(x0, y0, w, h, voiceBank1.patch.filterEnvelope_attack, voiceBank1.patch.filterEnvelope_decay, voiceBank1.patch.filterEnvelope_sustain, voiceBank1.patch.filterEnvelope_release, SELECTED_COLOR, firstCall);
}

void animateEnvelope3(bool firstCall)
{
  const uint16_t x0 = 400;
  const uint16_t y0 = 300; // y0 is envelope = 0
  const uint16_t w = 300;
  const uint16_t h = 150;
  animateEnvelope(x0, y0, w, h, voiceBank1.patch.envelope3_attack  , voiceBank1.patch.envelope3_decay, voiceBank1.patch.envelope3_sustain, voiceBank1.patch.envelope3_release, SELECTED_COLOR, firstCall);
}

void animateEnvelope(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, float attack, float decay, float sustain, float release, uint16_t color, bool firstCall)
{
  const uint8_t MODE = 1;

  static float _attack, _decay, _sustain, _release;
  static elapsedMillis timer;

  uint16_t r = h >> 5;
  bool changed = ( (attack != _attack) || (decay != _decay) || (sustain != _sustain) || (release != _release));
  if (firstCall || (changed && (timer > 50)) )
  {
    timer = 0;
    _attack = attack;
    _decay = decay;
    _sustain = sustain;
    _release = release;
    uint16_t dx1 = attack / 20;
    uint16_t dx2 = decay / 20;
    uint16_t dx4 = release / 50;
    uint16_t dx3 = w - dx1 - dx2 - dx4; 
    uint16_t dy1 = h * sustain;

    // if (MODE == 0)
    // {
    //   tft.fillRect(x0, y0 - h, w + 1, h, MAIN_BG_COLOR);
    //   tft.fillTriangle(x0, y0, x0 + dx1, y0, x0 + dx1, y0 - h, color); // ATTACK
    //   tft.fillTriangle(x0 + dx1, y0 - h, x0 + dx1, y0 - dy1, x0 + dx1 + dx2, y0 - dy1, color); // DECAY
    //   tft.fillRect(x0 + dx1, y0 - dy1, dx2 + dx3, dy1, color); // SUSTAIN
    //   tft.fillTriangle(x0 + dx1 + dx2 + dx3, y0, x0 + dx1 + dx2 + dx3, y0 - dy1, x0 + dx1 + dx2 + dx3 + dx4, y0, color); // RELEASE
    // }

    if (MODE == 1)
    {
      tft.fillRect(x0 - 2 * r, y0 - h - r, w + 4 * r, h + 4 * r, MAIN_BG_COLOR);
      tft.drawLine(x0, y0, x0 + dx1, y0 - h, color); // ATTACK
      tft.drawLine(x0 + dx1, y0 - h, x0 + dx1 + dx2, y0 - dy1, color); // DECAY
      tft.drawFastHLine(x0 + dx1 + dx2, y0 - dy1, dx3, color); // SUSTAIN
      tft.drawLine(x0 + dx1 + dx2 + dx3, y0 - dy1, x0 + dx1 + dx2 + dx3 + dx4, y0, color); // RELEASE

      tft.fillCircle(x0, y0, r, color);
      tft.fillCircle(x0 + dx1, y0 -h, r, color);
      tft.fillCircle(x0 + dx1 + dx2, y0 - dy1, r, color);
      tft.fillCircle(x0 + dx1 + dx2 + dx3, y0 - dy1, r, color);
      tft.fillCircle(x0 + dx1 + dx2 + dx3 + dx4, y0, r, color);
    }
  }
}

void animateHomePage(bool firstCall)
{
  animateUsbPcStatus(firstCall);
  animateUsbDeviceStatus(firstCall);
  animateMidiInput();
  animateBpm(firstCall);
  animateMeter(firstCall);
}

void animateUsbPcStatus(bool firstCall)
{
  static bool oldUsbPcStatus = false;
  static elapsedMillis timer = 0;

  if (firstCall) oldUsbPcStatus = false;

  if ( (timer > 500) && !oldUsbPcStatus && usbPcStatus )
  {
    timer = 0;
    oldUsbPcStatus = usbPcStatus;
    tft.setTextColor(MIDIEVENT_ON);
    tft.setFont(Arial_14);
    tft.setCursor(SCREEN_XRES - 30 , 4);
    tft.print("pc");
  } 
  if ( (timer > 500) && oldUsbPcStatus && !usbPcStatus )
  {
    timer = 0;
    oldUsbPcStatus = usbPcStatus;
    tft.setTextColor(MAIN_BG_COLOR);
    tft.setFont(Arial_14);
    tft.setCursor(SCREEN_XRES - 30 , 4);
    tft.print("pc");
  }  
}

void animateUsbDeviceStatus(bool firstCall)
{
  static bool oldUsbDeviceStatus = false;
  static elapsedMillis timer = 0;

  if (firstCall) oldUsbDeviceStatus = false;

  if ( (timer > 500) && !oldUsbDeviceStatus && usbDeviceStatus )
  {
    timer = 0;
    oldUsbDeviceStatus = usbDeviceStatus;
    tft.setTextColor(MIDIEVENT_ON);
    tft.setFont(Arial_14);
    tft.setCursor(SCREEN_XRES - 50 , 4);
    tft.print("u");
  } 
  if ( (timer > 500) && oldUsbDeviceStatus && !usbDeviceStatus )
  {
    timer = 0;
    oldUsbDeviceStatus = usbDeviceStatus;
    tft.setTextColor(MAIN_BG_COLOR);
    tft.setFont(Arial_14);
    tft.setCursor(SCREEN_XRES - 50 , 4);
    tft.print("u");
  } 
}

void animateMidiInput()
{
  static uint8_t oldMidiActivity = 0;
  static elapsedMillis timer = 0;

  if ( (timer > 50) && (oldMidiActivity == 0) && (midiActivity == 1) )
  {
    timer = 0;
    oldMidiActivity = midiActivity;
    tft.setTextColor(MIDIEVENT_ON);
    tft.setFont(Arial_14);
    tft.setCursor(SCREEN_XRES - 80 , 4);
    tft.print("m");
  } 
  if ( (timer > 50) && (oldMidiActivity == 1) && (midiActivity == 0) )
  {
    timer = 0;
    oldMidiActivity = midiActivity;
    tft.setTextColor(MAIN_BG_COLOR);
    tft.setFont(Arial_14);
    tft.setCursor(SCREEN_XRES - 80 , 4);
    tft.print("m");
  } 
}

void animateBpm(bool firstCall)
{
  static uint8_t currentBpm = 120;
  if (firstCall || ( currentBpm != midiSettings.bpm))
  {
    tft.setFont(Arial_14);
    tft.setTextColor(MAIN_BG_COLOR);
    tft.setCursor(SCREEN_XRES - 130 , 4);
    tft.print(currentBpm);
    currentBpm = midiSettings.bpm;
    tft.setTextColor(MIDIEVENT_ON);
    tft.setCursor(SCREEN_XRES - 130 , 4);
    tft.print(currentBpm);
  }
}

void animateMeter(bool firstCall)
{
  const uint8_t meter_w = 80;
  const uint8_t meter_h = 10;
  const uint16_t x0 = 700;
  const uint16_t y0 = 30;
  const uint16_t meter_color = MIDIEVENT_ON;
  static elapsedMillis timer;

  if (timer > 50)
  {
    timer = 0;
    float barWidthL = peakLevels[0] * meter_w;
    float barWidthR = peakLevels[1] * meter_w;
    tft.fillRect(x0, y0, barWidthL, meter_h, meter_color);
    tft.fillRect(x0 + barWidthL, y0, meter_w - barWidthL, meter_h, MAIN_BG_COLOR);
    
    tft.fillRect(x0, y0 + meter_h + 2, barWidthR, meter_h, meter_color);
    tft.fillRect(x0 + barWidthR, y0 + meter_h + 2, meter_w - barWidthR, meter_h, MAIN_BG_COLOR);
  }
}

void animateSystemPage(bool firstCall)
{
  static float audioProcessorUsageMax = 0.0;
  static float audioMemoryUsageMax = 0.0;
  static elapsedMillis timer = 0;
  const uint16_t offset = 135;

  if (firstCall)
  {
    tft.setTextColor(MIDIEVENT_ON);
    tft.setCursor(0 , SCREEN_YRES - 90);
    tft.print("Build: ");
    tft.print(__DATE__);
    tft.print(" ");
    tft.print(__TIME__);
    tft.setCursor(0 , SCREEN_YRES - 60);
    tft.print("PRC_max: ");
    tft.setCursor(0 , SCREEN_YRES - 30);
    tft.print("MEM_max: ");
  }

  if (timer > 1000) 
  {
    timer = 0;
    tft.setCursor(offset , SCREEN_YRES - 60);
    tft.setTextColor(MAIN_BG_COLOR);
    tft.print(audioProcessorUsageMax, 0);
    
    audioProcessorUsageMax = AudioProcessorUsageMax();
    tft.setCursor(offset , SCREEN_YRES - 60);
    tft.setTextColor(MIDIEVENT_ON);
    tft.print(audioProcessorUsageMax, 0);

    tft.setCursor(offset , SCREEN_YRES - 30);
    tft.setTextColor(MAIN_BG_COLOR);
    tft.print(audioMemoryUsageMax, 0);

    audioMemoryUsageMax = AudioMemoryUsageMax();
    tft.setCursor(offset , SCREEN_YRES - 30);
    tft.setTextColor(MIDIEVENT_ON);
    tft.print(audioMemoryUsageMax, 0);
        
    AudioMemoryUsageMaxReset(); 
    AudioProcessorUsageMaxReset(); 
  }
}


// HW CONTROL FUNCTIONS
// --------------------

int16_t getEncoderDirection(uint8_t encoderNr)
{
  static elapsedMillis encoderTimer;
  static int8_t acceleration = 1;
  static bool isAccelerated = false;
  int16_t value = encoders[encoderNr].read();
  int16_t returnValue = 0;
  if (value > 3)
  {
    returnValue = 1;
    encoders[encoderNr].write(0);
  }
  if (value < -3)
  {
    returnValue = -1;
    encoders[encoderNr].write(0);
  }

  if (returnValue != 0)
  {
    if (encoderTimer < 100)
    {
      isAccelerated = true;
      acceleration++;
    }

    if (isAccelerated && (encoderTimer > 200))
    {
      isAccelerated = false;
      acceleration = 1;
    }

    returnValue = returnValue * acceleration;
    encoderTimer = 0;
  }
  //if (returnValue != 0) Serial.printf("Encoder %d: %d\n", encoderNr, returnValue);
  return returnValue;
}

// uint8_t updateButton(uint8_t buttonNr)
// {
//   bouncers[buttonNr].update();
//   if (bouncers[buttonNr].fallingEdge())
//   {
//     //Serial.print("Button ");
//     //Serial.println(buttonNr);
//     return 1;
//   }
//   else return 0;
// }