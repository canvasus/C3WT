#include "ui.h"

void Widget::configure(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  _x = x;
  _y = y;
  _w = w;
  _h = h;

  slider_header_h = _h * 0.1;
  slider_footer_h = _h * 0.1;
  sliderHandleRadius = _w * 0.2;
}

void Widget::label(String label) { label.toCharArray(_label, 8); }

void Widget::draw(bool selected)
{
  if(!hidden)
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
}

void Widget::_drawBox(bool selected)
{
  uint16_t colors[2] = {textColor1, textColor2};
  const uint8_t r = 6;

  if(selected) tft->fillRoundRect(_x, _y, _w, _h, r, color1);
  else tft->fillRoundRect(_x, _y, _w, _h, r, color2);
  if (drawBorder && !selected) tft->drawRoundRect(_x, _y, _w, _h, r, color2);

  //if(selected) tft->fillRect(_x, _y, _w, _h, color1);
  //else tft->fillRect(_x, _y, _w, _h, color2);
  //if (drawBorder && !selected) tft->drawRect(_x, _y, _w, _h, color2);

  tft->setTextColor(colors[selected]);

  //if (fontSize > 16)  tft->setFontScale(1);
  //else tft->setFontScale(0);

  switch (fontSize)
  {
    case 0:
      tft->setFont();
      tft->setFontScale(0);
      break;
    case 1:
      tft->setFont();
      tft->setFontScale(1);
      break;
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
  uint16_t slider_mid_h = _h - slider_footer_h - slider_header_h;

  if (var_ptr_u8 != nullptr) value_f = (float)(*var_ptr_u8);
  else if (var_ptr_u16 != nullptr) value_f = (float)(*var_ptr_u16);
  else if (var_ptr_f != nullptr) value_f = *var_ptr_f;
  else if (var_ptr_i8 != nullptr) value_f = (float)(*var_ptr_i8);
  
  if ( (_previousSliderX != 0) && (_previousSliderY != 0) ) tft->fillCircle(_previousSliderX, _previousSliderY, sliderHandleRadius, color1); // clear old point
  tft->drawFastVLine(_x + (_w >> 1), _y + slider_header_h, slider_mid_h, SLIDER_LANE); // draw lane
  uint16_t slider_y = _y + slider_header_h + slider_mid_h - value_f * slider_mid_h;
  tft->fillCircle(_x + (_w >> 1), slider_y, sliderHandleRadius, SLIDER_HANDLE);
  _previousSliderX = _x + (_w >> 1);
  _previousSliderY = slider_y; 
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
  const uint16_t xMargin = 1;
  const uint16_t yMargin = 1;
  
  if (xPos < _x + xMargin) return false;      // outside left
  if (xPos > _x + _w - xMargin) return false; // outside right
  if (yPos < _y + yMargin) return false;      // outside above
  if (yPos > _y + _h - yMargin) return false; // outside below

  switch (type)
  {
    case WIDGET_BOX:
      if (eventType == CTP_FINGER_UP)
      {
        if (activateCb) activateCb(id);
      }
      return true;
    case WIDGET_SLIDER_V:
      {
        if (yPos < _y + slider_header_h) return true; // on header
        if (yPos > _y + _h - slider_footer_h) return true; // on footer

        float sliderLevelFractional = 1.0 - (yPos - slider_header_h) / (1.0 * (_h - slider_header_h - slider_footer_h));
        if (setF) setF(id, sliderLevelFractional);
        Serial.println(sliderLevelFractional);
        //draw(true);
        _drawSliderV(true);
        return true;
      }
  }
  return false;
}

int Page::checkTouch(uint16_t xPos, uint16_t yPos, uint8_t eventType)
{
  for(uint8_t widgetId = 0; widgetId < nrWidgets; widgetId++)
  {
    if (widgets[widgetId].checkTouch(xPos, yPos, eventType) )
    {
      if (widgets[widgetId].selectOnPress) selectedId = widgetId;
      return selectedId;
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

uint8_t Page::draw(bool firstCall)
{
  // draws one element per pass
  // returns 1 when all done

  if (firstCall)
  {
    //tft->backlight(false); // seems to mess with BTE function?
    _drawStaticIndex = 0;
    _drawWidgetIndex = 0;
    _initAnimation = 0;
    if (clearOnFirstCall) tft->fillWindow(MAIN_BG_COLOR);
    return 0;
  }

  if (_drawStaticIndex < nrStatics )
  {
    statics[_drawStaticIndex++].draw(false);
    return 0;
  }
  
  if (_drawWidgetIndex < nrWidgets )
  {
    widgets[_drawWidgetIndex].draw(_drawWidgetIndex == selectedId);
    _drawWidgetIndex++;
    return 0;
  }

  if (_initAnimation == 0)
  {
    animate(true);
    _initAnimation = 1;

  }
  
  //tft->backlight(true);

  return 1; // all done
}

void Page::animate(bool firstCall)
{
  if (animateFunction != nullptr) animateFunction(firstCall);
}