#include "ledStrip.h"

LedStrip::LedStrip(uint8_t dataPin, uint8_t clockPin, int numLEDs) {
  _dataPin = dataPin;
  _clockPin = clockPin;
  _numLEDs = numLEDs;

  leds[_numLEDs] = {};
  APA102Controller_WithBrightness<dataPin, clockPin, BGR, DATA_RATE_MHZ(6)> ledController;
  FastLED.addLeds((CLEDController*) &ledController, leds, _numLEDs);
  
}

