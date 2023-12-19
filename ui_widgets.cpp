#include "ui.h"

void Widget::configure(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  _x = x;
  _y = y;
  _w = w;
  _h = h;
}

void Widget::label(String label) { label.toCharArray(_label, 8); }

void Widget::draw(bool selected)
{
  switch (type)
  {
    case WIDGET_BOX:
      _drawBox(selected);
      break;
    // case WIDGET_SLIDER_H:
    //   _drawSliderH(selected);
    //   break;
    case WIDGET_SLIDER_V:
      _drawBox(selected);
      _drawSliderV(selected);
      _previousSliderX = _x + (_w >> 1);
      _previousSliderY = _y + (_h >> 1);
      break;
    // case WIDGET_POT:
    //   _drawPot(selected);
    //   break;
  }
}

void Widget::_drawBox(bool selected)
{
  uint16_t colors[2] = {textColor1, textColor2};
  if(selected) tft->fillRect(_x, _y, _w, _h, color1);
  else tft->fillRect(_x, _y, _w, _h, color2);
  if (drawBorder && !selected) tft->drawRect(_x, _y, _w, _h, color2);

  tft->setTextColor(colors[selected]);
  switch (fontSize)
  {
    case 12:
      tft->setFont(Arial_12);
      break;
    case 14:
      tft->setFont(Arial_14);
      break;
    case 16:
      tft->setFont(Arial_16);
      break;
    case 18:
      tft->setFont(Arial_18);
      break;
    case 20:
      tft->setFont(Arial_20);
      break;
    case 24:
      tft->setFont(Arial_24);
      break;
    default:
      tft->setFont(Arial_10);
      break;
  }
   
  if (drawLabel)
  {
    tft->setCursor(_x + labelOffsetX , _y + labelOffsetY);
    tft->print(_label);
  }

  if (drawVariable)
  {
    tft->setCursor(_x + varOffsetX , _y + varOffsetY);
    if (var_ptr_str != nullptr) tft->print(*var_ptr_str);
    else if (var_ptr_u8 != nullptr) tft->print(*var_ptr_u8);
    else if (var_ptr_u16 != nullptr) tft->print(*var_ptr_u16);
    else if (var_ptr_f != nullptr) tft->print(*var_ptr_f, floatPrecision);
    else if (var_ptr_i8 != nullptr) tft->print(*var_ptr_i8);
    else if (var_ptr_char != nullptr) tft->print(*var_ptr_char);
  }

  if (drawWaveform) _drawWaveform(selected);
}

void Widget::_drawSliderV(bool selected)
{
  float value_f = 0.0;
  if (var_ptr_u8 != nullptr) value_f = (float)(*var_ptr_u8);
  else if (var_ptr_u16 != nullptr) value_f = (float)(*var_ptr_u16);
  else if (var_ptr_f != nullptr) value_f = *var_ptr_f;
  else if (var_ptr_i8 != nullptr) value_f = (float)(*var_ptr_i8);
  
  tft->fillCircle(_previousSliderX, _previousSliderY, 5, color1);
  tft->drawFastVLine(_x + (_w >> 1), _y, _h, SLIDER_LANE);
  float fractionalValue = (valueMax - valueMin) / value_f;
  uint16_t sliderLevel = _y + _h - fractionalValue * _h;
  tft->fillCircle(_x + (_w >> 1), sliderLevel, 5, SLIDER_HANDLE);
  _previousSliderX = _x;
  _previousSliderY = _y; 
}

void Widget::_drawWaveform(bool selected)
{
  // uint16_t colors[2] = {color1, color2};
  const String names[4] = {"SIN", "SAW", "PLS", "WTB"};
  uint16_t x0 = _x + varOffsetX;
  uint16_t y0 = _y + varOffsetY;
  uint8_t waveformIndex = 0;
  if (var_ptr_u8 != nullptr) waveformIndex = *var_ptr_u8;
  // //{"SINE", "SAW", "PULSE", "ARBT"};
  // if (waveformIndex < 3) tft->drawBitmap(x0, y0, epd_bitmap_allArray[waveformIndex], 32, 32, colors[selected]);
  // else 
  // {
  //  tft->drawBitmap(x0, y0, epd_bitmap_allArray[3], 32, 32, colors[selected]);
    tft->setCursor(x0 + 4, y0 + 4);
    tft->print(names[waveformIndex]);
    //tft->print(waveformIndex);
  //}
}

bool Widget::checkTouch(uint16_t xPos, uint16_t yPos, uint8_t eventType)
{
  const uint16_t xMargin = 2;
  const uint16_t yMargin = 2;
  
  //if ( (xPos < _x + xMargin) || (xPos > _x + _w - xMargin) || (yPos < _y + yMargin) || (yPos > _y + _h - yMargin) ) return false;
  if (xPos < _x + xMargin) return false;
  if (xPos > _x + _w - xMargin) return false;
  if (yPos < _y + yMargin) return false;
  if (yPos > _y + _h - yMargin) return false;
  switch (type)
  {
    case WIDGET_BOX:
      if (eventType == CTP_FINGER_DOWN)
      {
        if (activateCb) activateCb(id);
       }
      return true;
    case WIDGET_SLIDER_V:
      {
        float sliderLevelFractional = 1.0 - (yPos - _y) / (1.0 * _h);
        if (setF) setF(id, sliderLevelFractional);
        Serial.println(sliderLevelFractional);
        //draw(true);
        _drawSliderV(true);
        break;
      }
  }
  return false;
}

int Page::checkTouch(uint16_t xPos, uint16_t yPos, uint8_t eventType)
{
  for(uint8_t widgetId = 0; widgetId < nrWidgets; widgetId++)
  {
    if (widgets[widgetId].checkTouch(xPos, yPos, eventType))
    {
      selectedId = widgetId;
      return widgetId;
    }
  }
  return -1;
}

uint8_t Page::addWidget(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  nrWidgets++;
  if (nrWidgets > MAX_WIDGETS) nrWidgets = MAX_WIDGETS;
  widgets[nrWidgets - 1].configure(x, y, w, h);
  widgets[nrWidgets - 1].id = id;
  widgets[nrWidgets - 1].color1 = color1;
  widgets[nrWidgets - 1].color2 = color2;
  widgets[nrWidgets - 1].fontSize = fontSize;
  widgets[nrWidgets - 1].tft = tft;
  return (nrWidgets - 1);
}

uint8_t Page::addStatic(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  nrStatics++;
  if (nrStatics > MAX_STATICS) nrStatics = MAX_STATICS;
  statics[nrStatics - 1].configure(x, y, w, h);
  statics[nrStatics - 1].id = id;
  statics[nrStatics - 1].color1 = color1;
  statics[nrStatics - 1].color2 = color2;
  statics[nrStatics - 1].fontSize = fontSize;
  statics[nrStatics - 1].tft = tft;
  return (nrStatics - 1);
}

void Header::eventVariable(String name, float value, uint8_t precision)
{
  _messageTimer = 0;
  _variableState = MSG_CUED;
  name.toCharArray(_message, 10);
  _value = value;
  _precision = precision;
}

void Header::eventString(String name)
{
  _messageTimer = 0;
  _variableState = MSG_STR_CUED;
  name.toCharArray(_message, 10);
}

void Header::eventMidi()
{
  _midiTimer = 0;
  _midiState = MSG_CUED;
}

void Header::update()
{
  if ( (_variableState == MSG_CUED) || (_variableState == MSG_STR_CUED))
  {
    tft->fillRect(0, 220, 160, 20, IDLE_COLOR);
    tft->setTextColor(MAIN_BG_COLOR);
    //tft->setFont(Arial_10);
    tft->setCursor(4 , 225);
    tft->print(_message);
    tft->setCursor(10 + 70 , 225);
    if (_variableState == MSG_CUED) tft->print(_value, _precision);
    _variableState = MSG_SHOWN;
    return;
  }

  if((_messageTimer > 1000) && (_variableState == MSG_SHOWN))
  {
    _variableState = MSG_IDLE;
    tft->fillRect(0, 220, 160, 20, MAIN_BG_COLOR);
    return;
  }
}
