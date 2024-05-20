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
        if (drawLabel || drawVariable) _drawText(selected);
        break;
      case WIDGET_BOX_HBAR:
        _drawBox(selected);
        _drawHbar(selected);
        if (drawLabel || drawVariable) _drawText(selected);
        break;
      case WIDGET_TOGGLE:
        break;
      case WIDGET_BOX_POT:
        _drawBox(selected);
        _drawPot(selected);
        if (drawLabel || drawVariable) _drawText(selected);
        break;
      case WIDGET_SLIDER_V:
        _drawBox(selected);
        if (drawLabel || drawVariable) _drawText(selected);
        _drawSliderV(selected);
        _previousSliderX = _x + (_w >> 1);
        _previousSliderY = _y + (_h >> 1);
        break;
    }
  }
}

void Widget::_drawBox(bool selected)
{
  //uint16_t colors[2] = {textColor1, textColor2};

  // --- Round rects ---
  const uint8_t r = 6;
  if(selected) tft->fillRoundRect(_x, _y, _w, _h, r, color1);
  else tft->fillRoundRect(_x, _y, _w, _h, r, color2);
  if (drawBorder && !selected) tft->drawRoundRect(_x, _y, _w, _h, r, color2);

  // --- Square rects ---
  //if(selected) tft->fillRect(_x, _y, _w, _h, color1);
  //else tft->fillRect(_x, _y, _w, _h, color2);
  //if (drawBorder && !selected) tft->drawRect(_x, _y, _w, _h, color2);

  // --- Square rects with border ---
  // if(selected)
  // {
  //   tft->fillRect(_x, _y, _w, _h, 0x8413);
  //   tft->fillRect(_x + 2, _y + 2, _w - 4, _h - 4, color1);
  // } 
  // else
  // {
  //   tft->fillRect(_x, _y, _w, _h, 0x8413);
  //   tft->fillRect(_x + 2, _y + 2, _w - 4, _h - 4, color2);
  // }

  //if (drawLabel || drawVariable) _configureText();
  //if (drawWaveform) _drawWaveform(selected);
}

void Widget::_configureText(bool selected)
{
  uint16_t colors[2] = {textColor1, textColor2};
  tft->setTextColor(colors[selected]);

  switch (fontSize)
  {
    case 0:
      tft->setFontDefault();
      tft->setFontScale(0);
      break;
    case 1:
      tft->setFontDefault();
      tft->setFontScale(1);
      break;
    case 2:
      tft->setFontDefault();
      tft->setFontScale(2);
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
}

void Widget::_drawText(bool selected)
{
  _configureText(selected);
  if (drawLabel)
  {
    tft->setCursor(_x + labelOffsetX , _y + labelOffsetY);
    tft->print(_label);
  }

  if (drawVariable)
  {
    tft->setCursor(_x + varOffsetX , _y + varOffsetY);
    if (var_ptr_str != nullptr) tft->print(*var_ptr_str);
    else if ( (var_ptr_charArray != nullptr) && (var_ptr_u8 != nullptr) ) tft->print(var_ptr_charArray[*var_ptr_u8]);
    else if (var_ptr_u8 != nullptr) tft->print(*var_ptr_u8);
    else if (var_ptr_u16 != nullptr) tft->print(*var_ptr_u16);
    else if (var_ptr_f != nullptr) tft->print(*var_ptr_f, floatPrecision);
    else if (var_ptr_i8 != nullptr) tft->print(*var_ptr_i8);
    else if (var_ptr_char != nullptr) tft->print(*var_ptr_char);
  }

  if (drawWaveform) _drawWaveform(selected);
}

void Widget::_drawPot(bool selected)
{
  // uint16_t center_x = _x + _w / 2;
  // uint16_t center_y = _y + _h / 2;
  // uint16_t radius = (min(_w, _h) << 1) - 2;

  // float value = 0.0;

  // if (var_ptr_u8 != nullptr) value = (float)(*var_ptr_u8);
  // else if (var_ptr_u16 != nullptr) value = (float)(*var_ptr_u16);
  // else if (var_ptr_f != nullptr) value = (float)(*var_ptr_f);
  // else if (var_ptr_i8 != nullptr) value = (float)(*var_ptr_i8);

  // float angleFractional = (value - varMin) / (varMax - varMin);

  // tft->drawCircle(center_x, center_y, radius + 2, color2);
  // tft->fillCircle(center_x, center_y, radius, color1);
  
}

void Widget::_drawHbar(bool selected)
{
  uint16_t x0 = _x + varOffsetX;
  uint16_t y0 = _y + varOffsetY;
  uint16_t w = 2 * _w / 3 - 8;
  uint16_t h = _h / 3;
  
  uint16_t text_x0 = x0 + w + 2;

  float value = 0.0;

  if (var_ptr_u8 != nullptr) value = (float)(*var_ptr_u8);
  else if (var_ptr_u16 != nullptr) value = (float)(*var_ptr_u16);
  else if (var_ptr_f != nullptr) value = (float)(*var_ptr_f);
  else if (var_ptr_i8 != nullptr) value = (float)(*var_ptr_i8);

  float valueFractional = (value - varMin) / (varMax - varMin);
  float barWidth = valueFractional * w;

  tft->drawRect(x0 - 1, y0 - 1, w + 2, h + 2, HBAR_BORDER_COLOR);

  if (selected)
  {
    tft->fillRect(x0, y0, barWidth, h, HBAR_ACTIVE_SELECTED_COLOR);
    tft->fillRect(x0 + barWidth, y0, w - barWidth, h, HBAR_BG_SELECTED_COLOR);
  }
  else
  {
    tft->fillRect(x0, y0, barWidth, h, HBAR_ACTIVE_IDLE_COLOR);
    tft->fillRect(x0 + barWidth, y0, w - barWidth, h, HBAR_BG_IDLE_COLOR);
  }

  tft->setFontDefault();
  tft->setTextColor(0x0000);
  tft->setFontScale(0);
  tft->setCursor(text_x0 , y0);
  tft->print(value, floatPrecision);
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
  //const String names[NR_WAVEFORMS] = {"SIN", "SAW", "PLS", "WTB"};
  uint16_t x0 = _x + varOffsetX;
  uint16_t y0 = _y + varOffsetY;
  uint8_t waveformIndex = 0;
  if (var_ptr_u8 != nullptr) waveformIndex = *var_ptr_u8;
  // //{"SINE", "SAW", "PULSE", "ARBT"};
  //if (waveformIndex < 4) tft->drawBitmap(x0, y0, epd_waveforms[waveformIndex], 32, 32, colors[selected]);
  //if (waveformIndex < 4) tft->writeRect(x0, y0, 32, 32, epd_waveforms[waveformIndex]);
  // else 
  // {
  //  tft->drawBitmap(x0, y0, epd_bitmap_allArray[3], 32, 32, colors[selected]);
  tft->setCursor(x0 + 0, y0 + 4);
  tft->setFont(Arial_14);
  //tft->print(names[waveformIndex]);
  tft->print(waveformNames[waveformIndex]);
  //tft->writeRect(x0 + 4, y0 + 4, 32, 32, epd_waveforms[waveformIndex]);
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
    case WIDGET_BOX_HBAR:
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

Page::Page()
{
  for (uint8_t widgetId = 0; widgetId < MAX_WIDGETS; widgetId++) widgetPointers[widgetId] = nullptr;
  for (uint8_t staticId = 0; staticId < MAX_STATICS; staticId++) staticPointers[staticId] = nullptr;
}

int Page::checkTouch(uint16_t xPos, uint16_t yPos, uint8_t eventType)
{
  for(uint8_t widgetId = 0; widgetId < nrWidgets; widgetId++)
  {
    if (widgetPointers[widgetId]->checkTouch(xPos, yPos, eventType) )
    {
      if (widgetPointers[widgetId]->selectOnPress) selectedId = widgetId;
      return selectedId;
    }
  }
  return -1;
}

uint8_t Page::addWidget(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  nrWidgets++;
  if (nrWidgets > MAX_WIDGETS) nrWidgets = MAX_WIDGETS;

  widgetPointers[nrWidgets - 1] = new(Widget);
  widgetPointers[nrWidgets - 1]->configure(x, y, w, h);
  widgetPointers[nrWidgets - 1]->id = id;
  widgetPointers[nrWidgets - 1]->color1 = color1;
  widgetPointers[nrWidgets - 1]->color2 = color2;
  widgetPointers[nrWidgets - 1]->fontSize = fontSize;
  widgetPointers[nrWidgets - 1]->tft = tft;

  return (nrWidgets - 1);
}

uint8_t Page::addStatic(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  nrStatics++;
  if (nrStatics > MAX_STATICS) nrStatics = MAX_STATICS;
  
  staticPointers[nrStatics - 1] = new(Widget);
  staticPointers[nrStatics - 1]->configure(x, y, w, h);
  staticPointers[nrStatics - 1]->id = id;
  staticPointers[nrStatics - 1]->color1 = color1;
  staticPointers[nrStatics - 1]->color2 = color2;
  staticPointers[nrStatics - 1]->fontSize = fontSize;
  staticPointers[nrStatics - 1]->tft = tft;
  return (nrStatics - 1);
}

void Page::clear()
{
  for (uint8_t staticId = 0; staticId < nrStatics; staticId++) delete staticPointers[staticId];
  for (uint8_t widgetId = 0; widgetId < nrWidgets; widgetId++) delete widgetPointers[widgetId];
  nrWidgets = 0;
  nrStatics = 0;
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
    staticPointers[_drawStaticIndex++]->draw(false);
    return 0;
  }
  
  if (_drawWidgetIndex < nrWidgets )
  {
    widgetPointers[_drawWidgetIndex]->draw(_drawWidgetIndex == selectedId);
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