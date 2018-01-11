/*
  OSC2APA102

  listen on USB Serial for slip encoded OSC packets
  to control two APA102 LED strips 
  with some extra DMX output

  Copyright Antoine Villeret / Pascal Baltazar - 2015/2017

*/

#include "ledStrip.h"

/////////////////////////////////////////////////////////////////////
// THIS IS THE PART THAT NEEDS TO BE CONFIGURED BASED ON YOUR NEED //
/////////////////////////////////////////////////////////////////////

// Change values marked with  <-- according to your needs

#define nStrips 3          // <-- How many strips do you want to use ?

#define DMX 1              // <-- set to 1 to use DMX, to 0 not to use it

// How many leds in each of your strips?
#if nStrips > 0
numLEDs[0] = 320;         // <-- # of LEDs in strip 1
#elif nStrips > 1
numLEDs[1] = 320;         // <-- # of LEDs in strip 2
#elif nStrips > 2
numLEDs[2] = 320;         // <-- # of LEDs in strip 3
#endif

#if DMX
// How many DMX channels at Max?
#define NUM_DMX 64         // <-- # of DMX Channels
#endif

// APA102 LED strips are SPI based 
// (four wires - data, clock, ground, and power),
// so we have to define DATA_PIN and CLOCK_PIN:
#if nStrips > 0                  // - for LED strip 1:
dataPin[0] = 11;          // <-- pin number for DATA (MOSI, green wire)
clockPin[0] = 27;         // <-- pin number for CLOCK (SCK, yellow wire) - NB: use 27 for teensy >= 3.5 / for teensy <3.5, use pin 13 (which causes the LED to stay lit)
#elif nStrips > 1               // - for LED strip 2
dataPin[1] = 7;           // <-- pin number for DATA (MOSI, green wire)
clockPin[1] = 14;         // <-- pin number for CLOCK (SCK, yellow wire)
#elif nStrips > 2              // - for LED strip 3 // only for teensy >= 3.5
dataPin[2] = 21;          // <-- pin number for DATA (MOSI, green wire)
clockPin[2] = 20;         // <-- pin number for DATA (MOSI, green wire)
#endif


/////////////////////////////////////////////////////////////////////
//                    HERE FOLLOWS THE CODE                        //
/////////////////////////////////////////////////////////////////////


#include <OSCBundle.h>
#include <PacketSerial.h>
#include <TeensyDmx.h>

// Use the serial device with PacketSerial
PacketSerial_<SLIP, SLIP::END, 8192> serial;

// Here we create the LED controllers for FastLED (see .h file as 2d tab)
LedStrip *ledStrips[nStrips];
uint8_t numLEDs[nStrips], dataPin[nStrips], clockPin[nStrips];


#if DMX
// Here we create a DMX output on Serial 1
TeensyDmx Dmx(Serial1);
// Array containing the DMX Values
uint8_t DMXvalues[NUM_DMX];
#endif


/////////////////////////////////////////////////////////////////////
void setup() {
  // We must specify a packet handler method
  serial.setPacketHandler(&onPacket);
  serial.begin(12000000); // baudrate is ignored, is always run at 12Mbps


  for (int i=0;i<nStrips;i++){
    ledStrips[i] = new LedStrip(dataPin[i], clockPin[i], numLEDs[i]) ;
    ledStrips[i]->ledController.setAPA102Brightness(1);
  }

  #if DMX
  // Now set DMX mode
  Dmx.setMode(TeensyDmx::DMX_OUT);
  #endif
  
    // Turn off all LEDs 
  FastLED.show(CRGB::Black);
}

/////////////////////////////////////////////////////////////////////
// This one is FastLED "global brightness", aka software dithering
void setGlobalBrightness(OSCMessage &msg)
{
  if (msg.isInt(0))
  {
    FastLED.setBrightness(msg.getInt(0));
  }
}



/////////////////////////////////////////////////////////////////////
// This part actually sends the DMX
#if DMX
void setDMX(OSCMessage &msg)
{
  if (msg.isBlob(0))
  {
    Dmx.setChannels(0, DMXvalues, msg.getBlob(0, (unsigned char *)DMXvalues));
  }
}
#endif


/////////////////////////////////////////////////////////////////////
// This is where the OSC address parsing happens:
void onPacket(const uint8_t* buffer, size_t size) {
  OSCBundle bundleIN;

  for (size_t i = 0; i < size; i++) {
    bundleIN.fill(buffer[i]);
  }

  if (!bundleIN.hasError()) {
    bundleIN.dispatch("/1", ledStrips[0]->LEDcontrol);
    bundleIN.dispatch("/2", ledStrips[1]->LEDcontrol);
    bundleIN.dispatch("/3", ledStrips[2]->LEDcontrol);
    bundleIN.dispatch("/b", setGlobalBrightness);
    bundleIN.dispatch("/DMX", setDMX);
  }
}


/////////////////////////////////////////////////////////////////////
void loop() {
  serial.update();
  FastLED.show();
}



