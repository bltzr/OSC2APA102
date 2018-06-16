#include <FastLED.h>

template <uint8_t DATA_PIN, uint8_t CLOCK_PIN, EOrder RGB_ORDER = BGR, uint8_t SPI_SPEED = DATA_RATE_MHZ(24)>
class APA102Controller_WithBrightness : public CLEDController {
  typedef SPIOutput<DATA_PIN, CLOCK_PIN, SPI_SPEED> SPI;
  SPI mSPI;
  uint8_t bBaseValue;

  void startBoundary() { mSPI.writeWord(0); mSPI.writeWord(0); }
  void endBoundary(int nLeds) { int nBytes = (nLeds/32); do { mSPI.writeWord(0xFF00); mSPI.writeWord(0x0000); } while(nBytes--); }

  inline void writeLed(uint8_t b0, uint8_t b1, uint8_t b2) __attribute__((always_inline)) {
    mSPI.writeByte(bBaseValue); mSPI.writeByte(b0); mSPI.writeByte(b1); mSPI.writeByte(b2);
  }

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
  virtual void clearLeds(int nLeds) {
    showColor(CRGB(0,0,0), nLeds, CRGB(0,0,0));
  }

protected:

  virtual void showColor(const struct CRGB & data, int nLeds, CRGB scale) {
    PixelController<RGB_ORDER> pixels(data, nLeds, scale, getDither());

    mSPI.select();

    startBoundary();
    for(int i = 0; i < nLeds; i++) {
      uint8_t b = uint8_t(pixels.loadAndScale0()*mapCol[bBaseValue]);
      mSPI.writeWord((0xE0 | (mapBright[bBaseValue] & 0x1F) ) << 8 | b);
      uint16_t w = uint8_t(pixels.loadAndScale1()*mapCol[bBaseValue]) << 8;
      w |= uint8_t(pixels.loadAndScale2()*mapCol[bBaseValue]);
      mSPI.writeWord(w);
      pixels.stepDithering();
    }
    endBoundary(nLeds);

    mSPI.waitFully();
    mSPI.release();
  }

  virtual void show(const struct CRGB *data, int nLeds, CRGB scale) {
    PixelController<RGB_ORDER> pixels(data, nLeds, scale, getDither());

    mSPI.select();

    startBoundary();
    for(int i = 0; i < nLeds; i++) {
      uint16_t b =  0xE0 | (mapBright[bBaseValue] & 0x1F) << 8; 
      b |= uint16_t(uint8_t(pixels.loadAndScale0()*mapCol[bBaseValue]));
      mSPI.writeWord(b);
      uint16_t w = uint16_t(uint8_t(pixels.loadAndScale1()*mapCol[bBaseValue])) << 8;
      w |= uint16_t(uint8_t(pixels.loadAndScale2()*mapCol[bBaseValue]));
      mSPI.writeWord(w);
      pixels.advanceData();
      pixels.stepDithering();
    }
    endBoundary(nLeds);
    mSPI.waitFully();
    mSPI.release();
  }

private:
   int mapBright[256] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 21, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 30, 30, 30, 31, 31}; 
   float mapCol[256] = { 0., 0.015686, 0.031373, 0.047059, 0.062745, 0.078431, 0.094118, 0.109804, 0.12549, 0.141176, 0.156863, 0.172549, 0.188235, 0.207843, 0.223529, 0.239216, 0.254902, 0.270588, 0.286275, 0.301961, 0.317647, 0.333333, 0.34902, 0.364706, 0.380392, 0.4, 0.415686, 0.431373, 0.447059, 0.462745, 0.478431, 0.494118, 0.509804, 0.52549, 0.541176, 0.556863, 0.572549, 0.592157, 0.607843, 0.623529, 0.639216, 0.654902, 0.670588, 0.686275, 0.701961, 0.717647, 0.733333, 0.74902, 0.764706, 0.784314, 0.8, 0.815686, 0.831373, 0.847059, 0.862745, 0.878431, 0.894118, 0.909804, 0.92549, 0.941176, 0.956863, 0.976471, 0.992157, 0.505882, 0.521569, 0.537255, 0.552941, 0.568627, 0.584314, 0.6, 0.615686, 0.631373, 0.647059, 0.666667, 0.682353, 0.698039, 0.713726, 0.729412, 0.745098, 0.760784, 0.776471, 0.792157, 0.807843, 0.823529, 0.839216, 0.858824, 0.87451, 0.890196, 0.905882, 0.921569, 0.937255, 0.952941, 0.968627, 0.984314, 1., 0.682353, 0.698039, 0.713726, 0.733333, 0.74902, 0.764706, 0.780392, 0.796078, 0.811765, 0.827451, 0.843137, 0.858824, 0.87451, 0.890196, 0.905882, 0.92549, 0.941176, 0.956863, 0.972549, 0.988235, 0.752941, 0.768627, 0.784314, 0.8, 0.815686, 0.831373, 0.847059, 0.866667, 0.882353, 0.898039, 0.913725, 0.929412, 0.945098, 0.960784, 0.976471, 0.992157, 0.807843, 0.823529, 0.839216, 0.858824, 0.87451, 0.890196, 0.905882, 0.921569, 0.937255, 0.952941, 0.968627, 0.984314, 1., 0.847059, 0.862745, 0.882353, 0.898039, 0.913725, 0.929412, 0.945098, 0.960784, 0.976471, 0.992157, 0.862745, 0.878431, 0.894118, 0.909804, 0.929412, 0.945098, 0.960784, 0.976471, 0.992157, 0.882353, 0.898039, 0.913725, 0.929412, 0.945098, 0.960784, 0.976471, 0.996078, 0.898039, 0.913725, 0.929412, 0.945098, 0.960784, 0.976471, 0.992157, 0.905882, 0.921569, 0.937255, 0.952941, 0.968627, 0.988235, 0.909804, 0.92549, 0.941176, 0.956863, 0.972549, 0.988235, 0.917647, 0.933333, 0.94902, 0.964706, 0.980392, 1., 0.937255, 0.952941, 0.968627, 0.984314, 1., 0.941176, 0.956863, 0.972549, 0.988235, 0.937255, 0.952941, 0.972549, 0.988235, 0.941176, 0.956863, 0.972549, 0.988235, 0.945098, 0.960784, 0.976471, 0.992157, 0.94902, 0.964706, 0.984314, 1., 0.960784, 0.976471, 0.992157, 0.956863, 0.972549, 0.988235, 0.952941, 0.968627, 0.984314, 1., 0.972549, 0.988235, 0.956863, 0.972549, 0.988235, 0.960784, 0.976471, 0.992157, 0.964706, 0.980392, 0.996078, 0.972549, 0.992157, 0.968627, 0.984314, 1., 0.976471, 0.992157, 0.972549, 0.988235, 0.968627, 0.984314, 1., 0.980392, 1.};

};
