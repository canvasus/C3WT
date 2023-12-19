#include "ui.h"

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);
Encoder encoders[1] = {Encoder(ENC1_A, ENC1_B)};

uint16_t coordinates[5][2];

Page pages[NR_PAGES];
//Header header;
uint8_t currentPage = PAGE_PATCH;
uint8_t backgroundPage = PAGE_PATCH;
bool forceReload = false;
String message = "";

void setupUI()
{
  Serial.println("UI SETUP");
  
  tft.begin(RA8875_800x480);
  tft.useCapINT(RA8875_INT);
  tft.fillWindow(MAIN_BG_COLOR);
  tft.enableCapISR(true);
  //tft.setFontScale(1);

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
  configurePage_screenSaver();
  configurePage_envelope();
  configurePage_filter();
  configurePage_controls();


  //changePatch(0, 0);
  //header.tft = &tft;
}

void showStartupScreen()
{
  // tft.setFont(Arial_24);
  // tft.setTextColor(ILI9341_BLUE);
  // tft.setCursor(20, 88);
  // tft.print(F("POLYMAST3R 2K"));
  // tft.setTextColor(ILI9341_WHITE);
  // tft.setCursor(22, 90);
  // tft.print(F("POLYMAST3R 2K"));
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

  if (uiTimer > uiInterval)
  {
    uiTimer = 0;
    static uint8_t oldPage = N_A;
    bool firstCall = (currentPage != oldPage) || forceReload;
    oldPage = currentPage;
    switch (currentPage)
    {
       default:
        updatePage(currentPage, firstCall);
        break;
    }
  }
}

void updatePage(uint8_t pageId, bool firstCall)
{
  uint8_t selectedId = pages[pageId].selectedId;
  uint8_t nrWidgets = pages[pageId].nrWidgets;
  uint8_t nrStatics = pages[pageId].nrStatics;
  int newSelectedId = pages[pageId].selectedId;
  static elapsedMillis screenSaverTimer = 0;

  if (firstCall)
  {
    screenSaverTimer = 0;
    if (pages[pageId].clearOnFirstCall) tft.fillWindow(MAIN_BG_COLOR);
    for (uint8_t staticId = 0; staticId < nrStatics; staticId++) pages[pageId].statics[staticId].draw(false);
    for (uint8_t widgetId = 0; widgetId < nrWidgets; widgetId++) pages[pageId].widgets[widgetId].draw(widgetId == selectedId);
    forceReload = false;
  }

  if (pages[pageId].animateFunction != nullptr) pages[pageId].animateFunction(firstCall);

  if (tft.touched())
  {
    screenSaverTimer = 0;
    tft.updateTS();
    uint8_t touchState = tft.getTouchState();
    if (touchState == CTP_FINGER_DOWN || touchState == CTP_FINGER_UP)
    {
      tft.getTScoordinates(coordinates);
      //Serial.printf("X: %d, Y: %d\n", coordinates[0][0], coordinates[0][1]);
      newSelectedId = pages[pageId].checkTouch(coordinates[0][0], coordinates[0][1], touchState);
    }
    tft.enableCapISR(); // needed?
  }

  if ( (newSelectedId > -1) && (newSelectedId != selectedId) )
  {
    pages[pageId].widgets[selectedId].draw(false);
    if (newSelectedId >= nrWidgets) newSelectedId = 0;
    selectedId = newSelectedId;
    pages[pageId].selectedId = selectedId;
    pages[pageId].widgets[selectedId].draw(true);
  } 
  
  int valueChange = getEncoderDirection(0);
  if (valueChange != 0 && (pages[pageId].widgets[selectedId].setI8 != nullptr))
  {
    screenSaverTimer = 0;
    pages[pageId].widgets[selectedId].setI8(pages[pageId].widgets[selectedId].id, valueChange);
    pages[pageId].widgets[selectedId].draw(true);
  }

  //if (updateButton(0) &&  (pages[pageId].widgets[selectedId].activateCb != nullptr) ) pages[pageId].widgets[selectedId].activateCb(selectedId);
  //if (updateButton(1)) currentPage = pages[pageId].backPageId;

  if ( (screenSaverTimer > SCREEN_SAVER_TIME) && (currentPage != PAGE_SCREENSAVER) )
  {
    pages[PAGE_SCREENSAVER].widgets[0].id = currentPage;
    currentPage = PAGE_SCREENSAVER;
  }
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
  pages[PAGE].widgets[widgetIndex].color1 = MAIN_BG_COLOR;
  pages[PAGE].widgets[widgetIndex].color2 = MAIN_BG_COLOR;
  pages[PAGE].widgets[widgetIndex].drawLabel = false;
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
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

  const uint16_t column_w = 140;
  const uint16_t row_h = 60;
  const uint8_t padding = 4;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCHNAME, 0, 0, 400, 60);
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_str = &patchNameUI;
  pages[PAGE].widgets[widgetIndex].setI8 = &changePatch;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCHNAME, 420, 0, 100, 60);
  pages[PAGE].widgets[widgetIndex].label("SAVE");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;


  // ----------------------------------
  widgetIndex = pages[PAGE].addWidget(PAGE_OSCILLATOR, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("OSC");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_ENVELOPE, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("ENV");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_FILTER, 2* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("FILT");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_MODULATION, 3* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("MOD");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_LFO, 4* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("LFO");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_WAVETABLE, 0* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("WTB");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_WAVETABLE2, 1* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("WTB2");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_EFFECTS, 2* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("EFX");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_CONTROLS, 3* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("CTRL");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;


  widgetIndex = pages[PAGE].addWidget(POLY_MODE, 0* (column_w + padding), 4* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Uni:");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 45;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 4;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.polyMode;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
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
  const uint16_t row_h = 68;
  const uint8_t padding = 4;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVEFORM, 0, 0, column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("OSC1");
  pages[PAGE].widgets[widgetIndex].drawWaveform = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.osc1_waveform;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVEFORM, 0, 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("OSC2");
  pages[PAGE].widgets[widgetIndex].drawWaveform = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.osc2_waveform;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  staticIndex = pages[PAGE].addStatic(0, 0, 2* (row_h + padding), column_w, row_h);
  pages[PAGE].statics[staticIndex].label("PULSE");

  staticIndex = pages[PAGE].addStatic(0, 0, 3* (row_h + padding), column_w, row_h);
  pages[PAGE].statics[staticIndex].label("NOISE");

  widgetIndex = pages[PAGE].addWidget(TRANSPOSE, 0, 20 + 4* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("TRP");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_i8 = &voiceBank1.patch.transpose;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(DETUNE, 0, 20 + 5* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("DET");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.detune;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  // --- COLUMN 2 - MIX LEVELS ---

  widgetIndex = pages[PAGE].addWidget(OSC1_LEVEL, 1* (column_w + padding), 0, column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("LVL");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.osc1_level;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(OSC2_LEVEL, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("LVL");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.osc2_level;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(PULSE_LEVEL, 1* (column_w + padding), 2* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("LVL");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.pulse_level;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(NOISE_LEVEL, 1* (column_w + padding), 3* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("LVL");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.noise_level;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;


  widgetIndex = pages[PAGE].addWidget(AM_WAVEFORM, 2* (column_w + padding), 0, column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("AM w");
  pages[PAGE].widgets[widgetIndex].drawWaveform = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.osc_am_waveform;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(AM_FREQ_MULTIPLIER, 2* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("AM xF");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.am_frequency_multiplier;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(AM_LEVEL, 2* (column_w + padding), 2* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("AM LVL");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.am_level;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;


  widgetIndex = pages[PAGE].addWidget(FM_WAVEFORM, 3* (column_w + padding), 0, column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("FM w");
  pages[PAGE].widgets[widgetIndex].drawWaveform = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.osc_fm_waveform;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FM_FREQ_MULTIPLIER, 3* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("FM xF");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.fm_frequency_multiplier;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FM_LEVEL, 3* (column_w + padding), 2* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("FM LVL");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.fm_level;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FM_OFFSET, 3* (column_w + padding), 3* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("FM ofs");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.fm_offset;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;


  widgetIndex = pages[PAGE].addWidget(EFX_SEND_DRY, 4* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("Dry");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.dryLevel;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(EFX_SEND_REVERB, 4* (column_w + padding), 1* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("Rev");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.reverbSend;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(EFX_SEND_CHORUS, 4* (column_w + padding), 2* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("Cho");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.chorusSend;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(EFX_SEND_PHASER, 4* (column_w + padding), 3* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("Pha");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.phaserSend;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(EFX_SEND_DELAY, 4* (column_w + padding), 4* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("Del");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 60;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.delaySend;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;


  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgets[widgetIndex].label("<BACK");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
 
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
  const uint8_t rowHeight = 50;
  const uint8_t padding = 2;
  const uint8_t columnWidth = 120;

  const String rowNames[nrRows] = {"O1P", "O2P", "ENV3", "LFO1", "LFO2", "VEL", "WHL"};

  for (uint8_t row = 0; row < nrRows; row++)
  {
    staticIndex = pages[PAGE].addStatic(0, 0, (1 + row) * (rowHeight + 2), columnWidth, rowHeight);
    pages[PAGE].statics[staticIndex].label(rowNames[row]);
  }

  // draw column headers
  
  const String columnNames[nrRows] = {"O1P", "O2P", "O1PH", "O2PH", "FILT"};
  for (uint8_t column = 0; column < nrColumns; column++)
  {
    staticIndex = pages[PAGE].addStatic(0, (1 + column) * (columnWidth + padding), 0, columnWidth, rowHeight);
    pages[PAGE].statics[staticIndex].label(columnNames[column]);
  }

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
        pages[PAGE].widgets[widgetIndex].drawLabel = false;
        pages[PAGE].widgets[widgetIndex].drawVariable = true;
        pages[PAGE].widgets[widgetIndex].var_ptr_f = varPointers[row][column];
        pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
      }
    }
  }

  widgetIndex = pages[PAGE].addWidget(PAGE_MODULATION2, 700, 430, 100, 50);
  pages[PAGE].widgets[widgetIndex].label("PAGE2>");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 0, 430, 100, 50);
  pages[PAGE].widgets[widgetIndex].label("<BACK");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
  
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
  const uint8_t rowHeight = 50;
  const uint8_t padding = 2;
  const uint8_t columnWidth = 120;

  const String rowNames[nrRows] = {"O1P", "O2P", "ENV3", "LFO1", "LFO2", "VEL", "WHL"};

  for (uint8_t row = 0; row < nrRows; row++)
  {
    staticIndex = pages[PAGE].addStatic(0, 0, (1 + row) * (rowHeight + 2), columnWidth, rowHeight);
    pages[PAGE].statics[staticIndex].label(rowNames[row]);
  }

  // draw column headers
  const String columnNames[nrColumns] = {"PWM", "AMp", "FMp", "L1a", "L2a"};
  for (uint8_t column = 0; column < nrColumns; column++)
  {
    staticIndex = pages[PAGE].addStatic(0, (1 + column) * (columnWidth + padding), 0, columnWidth, rowHeight);
    pages[PAGE].statics[staticIndex].label(columnNames[column]);
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
        pages[PAGE].widgets[widgetIndex].drawLabel = false;
        pages[PAGE].widgets[widgetIndex].drawVariable = true;
        pages[PAGE].widgets[widgetIndex].var_ptr_f = varPointers[row][column];
        pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
      }
    }
  }

  widgetIndex = pages[PAGE].addWidget(PAGE_MODULATION, 700, 430, 100, 50);
  pages[PAGE].widgets[widgetIndex].label("PAGE1>");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 0, 430, 100, 50);
  pages[PAGE].widgets[widgetIndex].label("<BACK");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
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

  const uint16_t column_w = 200;
  const uint16_t row_h = 50;
  const uint8_t padding = 2;
  const uint16_t varOffsetX = 80;
   
  staticIndex = pages[PAGE].addStatic(0, 0* (column_w + padding), 0* (row_h + padding), column_w, row_h); 
  pages[PAGE].statics[staticIndex].label("REVERB");
  
  staticIndex = pages[PAGE].addStatic(0, 1* (column_w + padding), 0* (row_h + padding), column_w, row_h); 
  pages[PAGE].statics[staticIndex].label("CHORUS");

  staticIndex = pages[PAGE].addStatic(0, 2* (column_w + padding), 0* (row_h + padding), column_w, row_h); 
  pages[PAGE].statics[staticIndex].label("DELAY");

  staticIndex = pages[PAGE].addStatic(0, 3* (column_w + padding), 0* (row_h + padding), column_w, row_h); 
  pages[PAGE].statics[staticIndex].label("PHASER");


  widgetIndex = pages[PAGE].addWidget(REVERB_SIZE, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Size");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.reverb_size;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(REVERB_HIDAMP, 0* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("HiD");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.reverb_hidamp;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(REVERB_LODAMP, 0* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("LoD");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.reverb_lodamp;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(REVERB_LOWPASS, 0* (column_w + padding), 4* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("LoP");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.reverb_lowpass;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(REVERB_DIFFUSION, 0* (column_w + padding), 5* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Diff");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.reverb_diffusion;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(REVERB_FEEDBACK, 0* (column_w + padding), 6* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Shimr");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.reverb_feedback;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(GRANULAR_SPEED, 0* (column_w + padding), 7* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Pitch");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.granular_speed;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(GRANULAR_LENGTH, 0* (column_w + padding), 8* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Length");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX + 5;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &audioParameters.granular_length;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;


  widgetIndex = pages[PAGE].addWidget(CHORUS_LFORATE, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Rate");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.chorus_lfoRate;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(DELAY_TYPE, 2* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Type");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &audioParameters.delay_type;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(DELAY_TIME, 2* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Time");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 0;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.delay_time;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(DELAY_FEEDBACK, 2* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Feed");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.delay_feedback;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;


  widgetIndex = pages[PAGE].addWidget(PHASER_MIX, 3* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Mix");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.phaser_mix;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(PHASER_LFORATE, 3* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Rate");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.phaser_lfoRate;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(PHASER_STAGES, 3* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Stgs");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &audioParameters.phaser_stages;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(PHASER_FEEDBACK, 3* (column_w + padding), 4* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Feed");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.phaser_feedback;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(PHASER_DEPTH, 3* (column_w + padding), 5* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Dept");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &audioParameters.phaser_depth;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustAudioParameter;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 700, 430, 100, 50);
  pages[PAGE].widgets[widgetIndex].label("<BACK");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
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
  //pages[PAGE].fontSize = 14;

  staticIndex = pages[PAGE].addStatic(0, 20, 20, 500, 200);
  pages[PAGE].statics[staticIndex].color1 = PATCHNAME_BG_COLOR;
  pages[PAGE].statics[staticIndex].color2 = PATCHNAME_BG_COLOR;
  pages[PAGE].statics[staticIndex].label("SAVE AS");
  pages[PAGE].statics[staticIndex].labelOffsetX = 2;
  pages[PAGE].statics[staticIndex].labelOffsetY = 2;

  const uint8_t nr_characters = PATCH_NAME_NR_CHARACTERS - 1; //last is null termination
  const uint8_t charWidth = 40;
  const uint8_t charHeight = 60;
  const uint8_t padding = 6;
  for (uint8_t charPos = 0; charPos < nr_characters; charPos++)
  {
    widgetIndex = pages[PAGE].addWidget(charPos, 40 + charPos * (charWidth + padding), 80, charWidth, charHeight);
    pages[PAGE].widgets[widgetIndex].drawVariable = true;
    pages[PAGE].widgets[widgetIndex].var_ptr_char = &patchInfo.name[charPos];
    pages[PAGE].widgets[widgetIndex].setI8 = &adjustCharacter;
  }

  widgetIndex = pages[PAGE].addWidget(0, 200, 30, 60, 40);
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &peekPatchNr;

  widgetIndex = pages[PAGE].addWidget(0, 30, 160, 160, 50);
  pages[PAGE].widgets[widgetIndex].label("SAVE");
  //pages[PAGE].widgets[widgetIndex].setI8 = &savePatchWrapper;
  pages[PAGE].widgets[widgetIndex].activateCb = &savePatchWrapper;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 300, 160, 160, 50);
  pages[PAGE].widgets[widgetIndex].label("CANCEL");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;

}

FLASHMEM void configurePage_lfo()
{
  uint8_t PAGE = PAGE_LFO;
  uint8_t widgetIndex = 0;
  //uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;

  const uint16_t column_w = 140;
  const uint16_t row_h = 50;
  const uint8_t padding = 4;
  const uint16_t varOffsetX = 70;

  widgetIndex = pages[PAGE].addWidget(LFO1_WAVEFORM, 0* (column_w + padding), 0* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("LFO1");
  pages[PAGE].widgets[widgetIndex].drawWaveform = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.lfo1_waveform;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(LFO1_FREQUENCY, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Freq");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.lfo1Frequency;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(LFO1_AMPLITUDE, 0* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Ampl");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.lfo1Level;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(LFO1_FILTER_MODE, 0* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Filt");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.lfo1FilterMode;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;


  widgetIndex = pages[PAGE].addWidget(LFO2_WAVEFORM, 1* (column_w + padding), 0* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("LFO2");
  pages[PAGE].widgets[widgetIndex].drawWaveform = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.lfo2_waveform;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(LFO2_FREQUENCY, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Freq");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.lfo2Frequency;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(LFO2_AMPLITUDE, 1* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Ampl");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.lfo2Level;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(LFO2_FILTER_MODE, 1* (column_w + padding), 3* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("Filt");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.lfo2FilterMode;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;


  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgets[widgetIndex].label("<BACK");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
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
  pages[PAGE].widgets[widgetIndex].label("wt#");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_index;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_MODE, 1* (column_w + padding), 0* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("mod");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_mode;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_START, 0* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("strt");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_start;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_LENGTH, 1* (column_w + padding), 1* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("len");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_length;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;
  
  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_INTERVAL, 0* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("int");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_interval;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_STEPSIZE, 1* (column_w + padding), 2* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("stp");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_stepSize;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;


 widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_INDEX, 0* (column_w + padding), 5* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("wt#");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.osc2_waveTable_index;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE2_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_MODE, 1* (column_w + padding), 5* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("mod");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.osc2_waveTable_mode;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE2_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_START, 0* (column_w + padding), 6* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("strt");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_start;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE2_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_LENGTH, 1* (column_w + padding), 6* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("len");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_length;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE2_IDLE;
  
  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_INTERVAL, 0* (column_w + padding), 7* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("int");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_interval;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE2_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC2_WAVETABLE_STEPSIZE, 1* (column_w + padding), 7* (row_h + padding), column_w, row_h); 
  pages[PAGE].widgets[widgetIndex].label("stp");
  pages[PAGE].widgets[widgetIndex].varOffsetX = varOffsetX;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc2_waveTable_stepSize;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE2_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE2_IDLE;


  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgets[widgetIndex].label("<BACK");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
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
  const uint16_t padding = 2;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_INDEX, 0, 0 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgets[widgetIndex].label("wt#");
  pages[PAGE].widgets[widgetIndex].varOffsetX = 100;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_index;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_MODE, 0, 1 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgets[widgetIndex].label("mod");
  pages[PAGE].widgets[widgetIndex].varOffsetX = 100;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &voiceBank1.patch.osc1_waveTable_mode;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_START, 0, 2 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgets[widgetIndex].label("strt");
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_start;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_LENGTH, 0, 3 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgets[widgetIndex].label("len");
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_length;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;
  
  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_INTERVAL, 0, 4 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgets[widgetIndex].label("int");
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_interval;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;

  widgetIndex = pages[PAGE].addWidget(OSC1_WAVETABLE_STEPSIZE, 0, 5 * (rowHeight + padding), columnWidth, rowHeight);
  pages[PAGE].widgets[widgetIndex].label("stp");
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].var_ptr_u16 = &voiceBank1.patch.osc1_waveTable_stepSize;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE1_SELECTED;
  pages[PAGE].widgets[widgetIndex].color2 = WAVETABLE1_IDLE;


  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgets[widgetIndex].label("<BACK");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
}

FLASHMEM void configurePage_envelope()
{
  uint8_t PAGE = PAGE_ENVELOPE;
  uint8_t widgetIndex = 0;
  //uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateAmpEnvelope;

  const uint16_t column_w = 140;
  const uint16_t row_h = 70;
  const uint8_t padding = 4;

  widgetIndex = pages[PAGE].addWidget(ENV_ATTACK, 0* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("ATTACK");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 80;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 0;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.ampEnvelope_attack;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(ENV_DECAY, 1* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("DECAY");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 80;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 0;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.ampEnvelope_decay;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(ENV_SUSTAIN, 2* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("SUSTAIN");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 80;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 1;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.ampEnvelope_sustain;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(ENV_RELEASE, 3* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("RELEASE");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 80;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 0;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.ampEnvelope_release;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgets[widgetIndex].label("<BACK");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
}

FLASHMEM void configurePage_filter()
{
  uint8_t PAGE = PAGE_FILTER;
  uint8_t widgetIndex = 0;
  //uint8_t staticIndex = 0;
  pages[PAGE].tft = &tft;
  pages[PAGE].backPageId = PAGE_PATCH;
  pages[PAGE].color1 = SELECTED_COLOR;
  pages[PAGE].color2 = IDLE_COLOR;
  pages[PAGE].animateFunction = &animateFilterEnvelope;

  const uint16_t column_w = 140;
  const uint16_t row_h = 70;
  const uint8_t padding = 4;

  widgetIndex = pages[PAGE].addWidget(FILTER_ATTACK, 0* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("ATTACK");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 80;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 0;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.filterEnvelope_attack;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_DECAY, 1* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("DECAY");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 80;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 0;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.filterEnvelope_decay;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_SUSTAIN, 2* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("SUSTAIN");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 80;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 1;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.filterEnvelope_sustain;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_RELEASE, 3* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("RELEASE");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 80;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 0;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.filterEnvelope_release;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTERENV_PWR, 4* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("ENV");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 80;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 1;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.envToFilter;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_CUTOFF, 4* (column_w + padding), 1* (row_h + padding), column_w + 20, row_h);
  pages[PAGE].widgets[widgetIndex].label("FREQ");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 2;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.cutoff;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_RESONANCE, 4* (column_w + padding), 2* (row_h + padding), column_w + 20, row_h);
  pages[PAGE].widgets[widgetIndex].label("RESO");
  pages[PAGE].widgets[widgetIndex].drawVariable = true;
  pages[PAGE].widgets[widgetIndex].varOffsetX = 70;
  pages[PAGE].widgets[widgetIndex].varOffsetY = 30;
  pages[PAGE].widgets[widgetIndex].floatPrecision = 1;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.resonance;
  pages[PAGE].widgets[widgetIndex].setI8 = &adjustVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgets[widgetIndex].label("<BACK");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
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
  pages[PAGE].widgets[widgetIndex].label("FRQ");
  pages[PAGE].widgets[widgetIndex].type = WIDGET_SLIDER_V;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.cutoff;
  pages[PAGE].widgets[widgetIndex].setF = &setVoiceBankWrapper;

  widgetIndex = pages[PAGE].addWidget(FILTER_RESONANCE, 1* (column_w + padding), 0* (row_h + padding), column_w, row_h);
  pages[PAGE].widgets[widgetIndex].label("RES");
  pages[PAGE].widgets[widgetIndex].type = WIDGET_SLIDER_V;
  pages[PAGE].widgets[widgetIndex].var_ptr_f = &voiceBank1.patch.resonance;
  pages[PAGE].widgets[widgetIndex].setF = &setVoiceBankWrapper;
  pages[PAGE].widgets[widgetIndex].color1 = WAVETABLE2_SELECTED;

  widgetIndex = pages[PAGE].addWidget(PAGE_PATCH, 680, 410, 120, 60);
  pages[PAGE].widgets[widgetIndex].label("<BACK");
  pages[PAGE].widgets[widgetIndex].activateCb = &setPage;
}

//void configurePageArpeggiator()
//{
    // widgetIndex = pages[PAGE].addWidget(0, 0, 0, 80, 20);
  // pages[PAGE].widgets[widgetIndex].label("MOD");
  // pages[PAGE].widgets[widgetIndex].varOffsetX = 45;
  // pages[PAGE].widgets[widgetIndex].drawVariable = true;
  // pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &midiSettings.arp_mode;
  // pages[PAGE].widgets[widgetIndex].setI8 = &setArpeggiatorMode;

  // widgetIndex = pages[PAGE].addWidget(0, 0, 22, 80, 20);
  // pages[PAGE].widgets[widgetIndex].label("BPM");
  // pages[PAGE].widgets[widgetIndex].varOffsetX = 45;
  // pages[PAGE].widgets[widgetIndex].drawVariable = true;
  // pages[PAGE].widgets[widgetIndex].var_ptr_u8 = &midiSettings.bpm;
  // pages[PAGE].widgets[widgetIndex].setI8 = &adjustBpm;
//}

void setPage(uint8_t page) { currentPage = page; }

void changePatch(uint8_t callerId, int8_t delta)
{
  int8_t desiredPatchNr = currentPatchNr + delta;
  if (desiredPatchNr < 0) desiredPatchNr = NR_PATCHES - 1;
  if (desiredPatchNr > NR_PATCHES - 1) desiredPatchNr = 0;

  if (loadPatch(desiredPatchNr) == LOAD_OK)
  {
    currentPatchNr = desiredPatchNr;
    if (currentPage == PAGE_PATCH) pages[PAGE_PATCH].widgets[0].draw(false);
  }
  else Serial.println(F("Load failed"));
}

void adjustVoiceBankWrapper(uint8_t index, int8_t delta) { voiceBank1.adjustParameter(index, delta); }

void setVoiceBankWrapper(uint8_t index, float value) { voiceBank1.setParameter(index, value); }

void adjustCharacter(uint8_t charPos, int8_t delta)
{
  patchInfo.name[charPos] = patchInfo.name[charPos] + delta;
  if (currentPage == PAGE_PATCHNAME) pages[currentPage].widgets[charPos].draw(true);
}

void savePatchWrapper(uint8_t index)
{
  Serial.println("save patch wrapper");
  savePatch(currentPatchNr);
  //setPage(pages[currentPage].backPageId, 0);
  setPage(pages[currentPage].backPageId);
}

void peekPatchNameWrapper(uint8_t index, int8_t delta)
{
  // char buf[PATCH_NAME_NR_CHARACTERS];
  // peekPatchNr = currentPatchNr + delta;
  // peekPatchNr = constrain(peekPatchNr, 0, NR_PATCHES - 1);
}

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

  if (animateTimer > 85)
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

  if (animateTimer > 50)
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

void animateScreenSaver(bool firstCall)
{
  static elapsedMillis animateTimer = 0;
  static uint16_t column = 0;
  static uint16_t color = SELECTED_COLOR;
  if (animateTimer > 50)
  {
    for (uint8_t row = 0; row < 128; row++)
    {
      int8_t rowDelta = row - 64; // -64 --> +64
      uint16_t rowHeight = 10 - (abs(rowDelta) >> 3); // 10 --> 2
      uint16_t yMid = (SCREEN_YRES >> 1);
      if (noteStatus[row] > 0) tft.fillRect(column * 10, yMid + rowDelta * rowHeight, 10, rowHeight, color * (128 / 1.0 * noteStatus[row]));
    }
    column++;
    if (column > SCREEN_XRES / 10)
    {
      column = 0;
      color = tft.Color565(random(255), random(255), random(255));
    }
    
  }
}

void animateAmpEnvelope(bool firstCall)
{
  animateEnvelope(voiceBank1.patch.ampEnvelope_attack, voiceBank1.patch.ampEnvelope_decay, voiceBank1.patch.ampEnvelope_sustain, voiceBank1.patch.ampEnvelope_release, SELECTED_COLOR, firstCall);
}

void animateFilterEnvelope(bool firstCall)
{
  animateEnvelope(voiceBank1.patch.filterEnvelope_attack, voiceBank1.patch.filterEnvelope_decay, voiceBank1.patch.filterEnvelope_sustain, voiceBank1.patch.filterEnvelope_release, SELECTED_COLOR, firstCall);
}

void animateEnvelope(float attack, float decay, float sustain, float release, uint16_t color, bool firstCall)
{
  const uint8_t MODE = 1;

  static float _attack, _decay, _sustain, _release;
  static elapsedMillis timer;
  uint16_t x0 = 50;
  uint16_t y0 = 300;
  const uint16_t w = 300;
  const uint16_t h = 150;
  const uint16_t r = 5;
  bool changed = ( (attack != _attack) || (decay != _decay) || (sustain != _sustain) || (release != _release));
  if (firstCall || (changed && (timer > 50)) )
  {
    timer = 0;
    _attack = attack;
    _decay = decay;
    _sustain = sustain;
    _release = release;
    uint16_t dx1 = attack / 10;
    uint16_t dx2 = decay / 10;
    uint16_t dx4 = release / 50;
    uint16_t dx3 = w - dx1 - dx2 - dx4; 
    uint16_t dy1 = h * sustain;
    if (MODE == 0)
    {
      tft.fillRect(x0, y0 - h, w + 1, h, MAIN_BG_COLOR);
      tft.fillTriangle(x0, y0, x0 + dx1, y0, x0 + dx1, y0 - h, color); // ATTACK
      tft.fillTriangle(x0 + dx1, y0 - h, x0 + dx1, y0 - dy1, x0 + dx1 + dx2, y0 - dy1, color); // DECAY
      tft.fillRect(x0 + dx1, y0 - dy1, dx2 + dx3, dy1, color); // SUSTAIN
      tft.fillTriangle(x0 + dx1 + dx2 + dx3, y0, x0 + dx1 + dx2 + dx3, y0 - dy1, x0 + dx1 + dx2 + dx3 + dx4, y0, color); // RELEASE
    }

    if (MODE == 1)
    {
      tft.fillRect(x0 - r, y0 - h - r, w + 2 * r, h + 2 * r, MAIN_BG_COLOR);
      tft.drawLine(x0, y0, x0 + dx1, y0 - h, color); // ATTACK
      tft.drawLine(x0 + dx1, y0 - h, x0 + dx1 + dx2, y0 - dy1, color); // DECAY
      tft.drawFastHLine(x0 + dx1 + dx2, y0 - dy1, dx3, color); // SUSTAIN
      tft.drawLine(x0 + dx1 + dx2 + dx3, y0 - dy1, x0 + dx1 + dx2 + dx3 + dx4, y0, color); // RELEASE

      tft.fillCircle(x0, y0, 5, color);
      tft.fillCircle(x0 + dx1, y0 -h, 5, color);
      tft.fillCircle(x0 + dx1 + dx2, y0 - dy1, 5, color);
      tft.fillCircle(x0 + dx1 + dx2 + dx3, y0 - dy1, 5, color);
      tft.fillCircle(x0 + dx1 + dx2 + dx3 + dx4, y0, 5, color);
    }
  }
}

// HW CONTROL FUNCTIONS
// --------------------

int16_t getEncoderDirection(uint8_t encoderNr)
{
  static elapsedMillis encoderTimer;
  static int8_t acceleration = 1;
  bool isAccelerated = false;
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
    if (encoderTimer < 25)
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