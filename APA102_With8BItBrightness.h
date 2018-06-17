#include <FastLED.h>

#define FASTLED_SPI_BYTE_ONLY 

template <uint8_t DATA_PIN, uint8_t CLOCK_PIN, EOrder RGB_ORDER = BGR, uint8_t SPI_SPEED = DATA_RATE_MHZ(24)>
class APA102Controller_WithBrightness : public CPixelLEDController<RGB_ORDER> {
  typedef SPIOutput<DATA_PIN, CLOCK_PIN, SPI_SPEED> SPI;
  SPI mSPI;
  uint8_t bBaseValue;

  void startBoundary() { mSPI.writeWord(0); mSPI.writeWord(0); }
  void endBoundary(int nLeds) { int nDWords = (nLeds/32); do { mSPI.writeByte(0xFF); mSPI.writeByte(0x00); mSPI.writeByte(0x00); mSPI.writeByte(0x00); } while(nDWords--); }

  //inline void writeLed(uint8_t b0, uint8_t b1, uint8_t b2) __attribute__((always_inline)) {
  //  mSPI.writeByte(0xE0 | (mapBright[bBaseValue] & 0x1F)); mSPI.writeByte(uint8_t(b0*mapCol[bBaseValue])); mSPI.writeByte(uint8_t(b1*mapCol[bBaseValue])); mSPI.writeByte(uint8_t(b2*mapCol[bBaseValue]));
  // }

public:
  APA102Controller_WithBrightness() {bBaseValue = 0xFF;}

  virtual void init() {
    mSPI.init();
  }

  void setAPA102Brightness(uint8_t br) {
    bBaseValue = br;
  }

  uint8_t getAPA102Brightness() {
    return bBaseValue;
  }

protected:

  virtual void showPixels(PixelController<RGB_ORDER> & pixels) {
    mSPI.select();

    /*
    Serial.print(" Bright: ");
    Serial.print(bBaseValue);
    Serial.print(" -> ");
    Serial.print(0xE0 | (mapBright[bBaseValue] & 0x1F));
    Serial.print(" // Col: ");
    Serial.print(mapCol[bBaseValue]);
    Serial.print(" -> ");
    Serial.print("  R: ");
    Serial.print(map(pixels.loadAndScale0(), 0, 255, 0, mapCol[bBaseValue]));
    Serial.print(" / G: ");
    Serial.print(map(pixels.loadAndScale1(), 0, 255, 0, mapCol[bBaseValue]));
    Serial.print(" / B: ");
    Serial.println(map(pixels.loadAndScale2(), 0, 255, 0, mapCol[bBaseValue]));
    */

    startBoundary();
    while(pixels.has(1)) {
#ifdef FASTLED_SPI_BYTE_ONLY
      mSPI.writeByte(0xE0 | (mapBright[bBaseValue] & 0x1F));
      mSPI.writeByte(map(pixels.loadAndScale0(), 0, 255, 0, mapCol[bBaseValue]));
      mSPI.writeByte(map(pixels.loadAndScale1(), 0, 255, 0, mapCol[bBaseValue]));
      mSPI.writeByte(map(pixels.loadAndScale2(), 0, 255, 0, mapCol[bBaseValue]));
      
#else
      uint16_t b = (0xE0 | (mapBright[bBaseValue] & 0x1F) ) << 8;
              b |= uint16_t(map(pixels.loadAndScale0(), 0, 255, 0, mapCol[bBaseValue]));
      mSPI.writeWord(b);
      uint16_t w = uint16_t(map(pixels.loadAndScale1(), 0, 255, 0, mapCol[bBaseValue])) << 8;
              w |= uint16_t(map(pixels.loadAndScale2(), 0, 255, 0, mapCol[bBaseValue]));
      mSPI.writeWord(w);
#endif
      pixels.stepDithering();
      pixels.advanceData();
    }
    endBoundary(pixels.size());
    mSPI.waitFully();
    mSPI.release();
  }

  

private:
   int mapBright[256] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 31, 31, 31}; 
   int mapCol[256] = { 0, 3, 8, 12, 15, 19, 24, 28, 31, 35, 40, 43, 47, 52, 56, 61, 65, 68, 73, 77, 80, 84, 89, 93, 96, 102, 105, 110, 114, 117, 121, 126, 130, 133, 137, 142, 145, 151, 154, 158, 163, 167, 170, 175, 179, 182, 186, 191, 195, 200, 204, 207, 212, 216, 219, 223, 228, 232, 235, 239, 244, 249, 253, 128, 133, 137, 140, 144, 149, 153, 156, 161, 165, 170, 174, 177, 182, 186, 189, 193, 198, 202, 205, 209, 214, 219, 223, 226, 230, 235, 239, 242, 246, 251, 171, 174, 177, 182, 186, 191, 195, 198, 202, 207, 211, 214, 219, 223, 226, 230, 235, 239, 244, 247, 251, 191, 195, 200, 204, 207, 212, 216, 221, 225, 228, 232, 237, 240, 244, 249, 253, 205, 209, 214, 219, 223, 226, 230, 235, 239, 242, 246, 251, 213, 216, 219, 225, 228, 232, 237, 240, 244, 249, 253, 219, 223, 228, 232, 237, 240, 244, 249, 253, 225, 228, 232, 237, 240, 244, 249, 253, 228, 232, 237, 240, 244, 249, 253, 230, 235, 239, 242, 246, 251, 232, 235, 239, 244, 247, 251, 233, 237, 242, 246, 249, 236, 239, 242, 246, 251, 237, 240, 244, 247, 251, 239, 242, 247, 251, 239, 244, 247, 251, 240, 244, 249, 253, 242, 246, 251, 255, 244, 249, 253, 244, 247, 251, 242, 246, 251, 243, 247, 251, 244, 247, 251, 244, 249, 253, 246, 249, 253, 247, 253, 246, 251, 246, 249, 253, 247, 251, 246, 251, 247, 249, 255};

};
