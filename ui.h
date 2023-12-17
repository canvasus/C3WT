#pragma once
//#include <Adafruit_GFX.h>
//#include <FT5206.h>
#include <SPI.h>
#include <RA8875.h>
#include "voice.h"
#include "audioFunc.h"
#include "midiFunc.h"
#include "sdMgr.h"
#include <Encoder.h>

#include <font_Arial.h>

#define SCREEN_YRES           480
#define SCREEN_XRES           800

#define RA8875_CS     10  // ER-TFTM070 pin 5
#define RA8875_RESET  9   // ER-TFTM070 pin 11
#define RA8875_MOSI   11  // ER-TFTM070 pin 7
#define RA8875_MISO   12  // ER-TFTM070 pin 6
#define RA8875_SCLK   13  // ER-TFTM070 pin 8
#define RA8875_INT    32  // FT5206 interrupt, ER-TFTM070 pin 

#define MAXTOUCHLIMIT   1 //1...5
#define CTP_FINGER_UP   1
#define CTP_FINGER_DOWN 2

//#define CTP_INT 32    // FT5206 touch controller interrupt

#define ENC1_A  38 //36
#define ENC1_B  37 //37
#define ENC1_SW 36 //38

#define SCREEN_SAVER_TIME 120 * 1000

#define MAIN_BG_COLOR         0x0000
#define SELECTED_COLOR        0x07e0 //0xfe01
#define IDLE_COLOR            0xe71c
#define BORDER_COLOR          0x9cf3 //0x8c71
#define HEADER_COLOR              
#define PATCHNAME_BG_COLOR    0xfbe8
#define PATCHNAME_MAINPAGE    0xfd80
#define MIDIEVENT_ON          0xbe1f
#define MIDIEVENT_OFF         0xd6da
#define MESSAGE_COLOR         0xfe01
#define WAVETABLE1_SELECTED   0x0726
#define WAVETABLE1_IDLE       0xb776
#define WAVETABLE2_SELECTED   0x055f
#define WAVETABLE2_IDLE       0xcebd
#define WAVETABLE_BAR_BG      0x8c71
#define WAVETABLE_LANES       0x7bef

#define PAGE_PATCH       0
#define PAGE_OSCILLATOR  1
#define PAGE_MODULATION  2
#define PAGE_MODULATION2 3
#define PAGE_EFFECTS     4
#define PAGE_PATCHNAME   5
#define PAGE_LFO         6
#define PAGE_WAVETABLE   7
#define PAGE_WAVETABLE2  8
#define PAGE_SCREENSAVER 9
#define PAGE_ENVELOPE    10
#define PAGE_FILTER      11
#define PAGE_CONTROLS    12
//#define PAGE_ARPEGGIATOR 

#define NR_PAGES 13

#define PAGE_MESSAGE  254
#define N_A           255

#define OK      0
#define CANCEL  1

#define MSG_IDLE      0
#define MSG_CUED      1
#define MSG_STR_CUED  2
#define MSG_SHOWN     3

#define WIDGET_BOX 0
#define WIDGET_SLIDER_H 1
#define WIDGET_SLIDER_V 2
#define WIDGET_POT 3

#define MAX_WIDGETS 32
#define MAX_STATICS 16

typedef void (*SetFunctionI8)(uint8_t index, int8_t value);
typedef void (*ActivateCb)(uint8_t index); 
typedef void (*AnimateFunction)(bool firstCall);

class Widget
{
  private:
    uint16_t _x = 0;
    uint16_t _y = 0;
    uint16_t _w = 0;
    uint16_t _h = 0;
    
    char _label[8];
    void _drawBox(bool selected);
    void _drawSliderV(bool selected);
    //void _drawSliderH(bool selected);
    //void _drawPot(bool selected);
    void _drawWaveform(bool selected);
    void _drawArray(bool selected);
  public:
    RA8875 * tft = nullptr;
    void configure(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void label(String label);
    void draw(bool selected);
    bool checkTouch(uint16_t xPos, uint16_t yPos);
    
    uint8_t id = 0;
    uint8_t type = WIDGET_BOX;
    bool drawBorder = true;
    bool drawLabel = true;
    bool drawVariable = false;
    bool drawWaveform = false;
    bool drawArray = false;
    
    uint16_t color1 = 0;
    uint16_t color2 = 0;
    uint16_t textColor1 = 0;
    uint16_t textColor2 = 0;
    uint8_t labelOffsetX = 2;
    uint8_t labelOffsetY = 4;
    uint8_t varOffsetX = 2;
    uint8_t varOffsetY = 4;
    uint8_t valueMax = 127;
    uint8_t valueMin = 0;
    uint8_t valueScaler = 1;
    uint8_t fontSize = 16;
    uint8_t floatPrecision = 2;
    
    SetFunctionI8 setI8 = nullptr;
    ActivateCb activateCb = nullptr;
    uint8_t * var_ptr_u8 = nullptr;
    uint16_t * var_ptr_u16 = nullptr;
    int8_t * var_ptr_i8 =  nullptr;
    float * var_ptr_f = nullptr;
    String * var_ptr_str = nullptr;
    char * var_ptr_char = nullptr;
};

class Page
{
 public:
    //ILI9341_t3 * tft =  nullptr;
    RA8875 * tft = nullptr;
    AnimateFunction animateFunction = nullptr;
    uint8_t addWidget(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    uint8_t addStatic(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void setAllWidgetIds(uint8_t id);
    Widget widgets[MAX_WIDGETS];
    Widget statics[MAX_STATICS];
    uint8_t nrWidgets = 0;
    uint8_t nrStatics = 0;
    uint8_t backPageId = 0;
    uint8_t selectedId = 0;
    uint16_t color1 = 0;
    uint16_t color2 = 0;
    uint8_t fontSize = 18;
    bool clearOnFirstCall = true;
    int checkTouch(uint16_t xPos, uint16_t yPos);
};

class Header
{
  private:
    elapsedMillis _messageTimer;
    elapsedMillis _midiTimer;
    uint8_t _variableState = MSG_IDLE;
    uint8_t _midiState = MSG_IDLE;
    float _value = 0.00;
    uint8_t _precision = 2;
    char _message[10];
  public:
    //ILI9341_t3 * tft = nullptr;
    RA8875 * tft = nullptr;
    void update();
    void eventMidi();
    void eventVariable(String name, float value, uint8_t precision);
    void eventString(String name);
};

void setupUI();
void showStartupScreen();

void updateUI();
//void updateUiBoardControls();
void updatePage(uint8_t pageId, bool firstCall);
//void updateFooter(String variableName, float value, uint8_t precision);

void configurePage_patch();
void configurePage_oscillator();
void configurePage_modulation();
void configurePage_modulation2();
void configurePage_effects();
void configurePage_patchName();
void configurePage_lfo();
void configurePage_wavetable();
void configurePage_wavetable2();
void configurePage_screenSaver();
void configurePage_envelope();
void configurePage_filter();
void configurePage_controls();

void setPage(uint8_t page);
void changePatch(uint8_t callerId, int8_t delta);
void adjustVoiceBankWrapper(uint8_t index, int8_t delta);
void adjustCharacter(uint8_t charPos, int8_t delta);
void savePatchWrapper(uint8_t index);
void peekPatchNameWrapper(uint8_t index, int8_t delta);

void animateWavetable(bool firstCall);
void animateWavetable2(bool firstCall);
void animateScreenSaver(bool firstCall);
void animateAmpEnvelope(bool firstCall);
void animateFilterEnvelope(bool firstCall);
void animateEnvelope(float attack, float decay, float sustain, float release, uint16_t color, bool firstCall);

int16_t getEncoderDirection(uint8_t encoderNr);
uint8_t updateButton(uint8_t buttonNr);