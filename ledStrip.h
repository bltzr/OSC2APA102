#include "APA102_WithGlobalBrightness.h"

class LedStrip {
  public:
  LedStrip (uint8_t dataPin, uint8_t clockPin, int numLEDs);
  CRGB leds[];

  private:
  uint8_t _dataPin;
  uint8_t _clockPin;
  int _numLEDs;
  
};

