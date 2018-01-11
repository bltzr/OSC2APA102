#include "APA102_WithGlobalBrightness.h"
#include <OSCBundle.h>

template <uint8_t DATA_PIN, uint8_t CLOCK_PIN, int numLEDs>
class LedStrip : public LedStrip {

  CRGB leds[numLEDs];
  APA102Controller_WithBrightness<DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(6)> ledController;
  FastLED.addLeds((CLEDController*) &ledController, leds, numLEDs);

}

public:

  void LedStrip::LEDcontrol(OSCMessage &msg)
  {
    // When receiving an int, we set the APA102's global brightness (0 to 31)
    if (msg.isInt(0))
    {
      ledController.setAPA102Brightness(msg.getInt(0));
    }
    // When receiving a Blob, we assume it's a list of RGB values
    else if (msg.isBlob(0))
    {
      msg.getBlob(0, (unsigned char *)leds, _numLEDs);
    }
  }
  
  
};

