#ifndef _LEDRING_H_
#define _LEDRING_H_

#include <Adafruit_NeoPixel.h>
#include "LDR.h"

//Adafruit NeoPixel compatible LED Ring
class LEDRing {
    Adafruit_NeoPixel _strip; //https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html
    byte _activeLed = 254; //Used to not overwrite identical active LED avoiding pulsing
                           //用于不覆盖相同的活动LED，避免产生脉冲
    word _bgHue; //Background LED hue背景LED色相

  public:

    LEDRing(byte ledCount, byte pin, word backgroundHue) {
      _strip = Adafruit_NeoPixel(ledCount, pin, NEO_GRB + NEO_KHZ800);
      _strip.begin();
      _strip.clear();
      _bgHue = backgroundHue;
    }

    //Empirical brightness values
    //经验亮度值
    void setBrightness(DayPeriod period) {
      switch (period) {
        case (DayPeriod::NIGHT): _strip.setBrightness(100); break;
        case (DayPeriod::EVENING): _strip.setBrightness(100); break;
        case (DayPeriod::DAY): _strip.setBrightness(100);
      }
    }

    

    //Set LED with Hue, Saturation, Value (more intuitive than RGB).
    //Hue is allowed to overflow so it just begins all over
    //See HSV section https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
    //使用色相，饱和度，值（比RGB更直观）设置LED。
    //允许色调溢出，所以它从头开始
    //请参阅HSV部分https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
      void setLed(char ledNum, word  h, byte s, byte  v) {
      _strip.setPixelColor(ledNum, _strip.ColorHSV(h, s, v));
      _strip.show();
    }

    //Auto map led position to hue color (cyan/green-yellow-red-purple)
    //自动将LED位置映射到色相（青色/绿色-黄色-红色-紫色）
    void setLed(int ledPos) {
      _activeLed = ledPos;
      //HSV mapping https://learn.adafruit.com/assets/74094
      //Using HSV Colors: Cyan= 65536/2; Green=65536/3; Yellow=65536/6; Red=0; Purple=-65536/6
      //使用HSV颜色：青色= 65536/2；绿色= 65536/3；黄色= 65536/6;红色= 0；紫色= -65536 / 6
      uint16_t hue = map(ledPos, 0, _strip.numPixels(), 25000, -65536 / 6);
      setLed(ledPos, hue, 255, 255); //Active LED (one LED of ring)
      resetBackground();   //Background LED (rest of the ring) 背景LED（其余部分）
    }

  private:

    void resetBackground() {
      for (char i = 0; i < _strip.numPixels(); i++) {
        if (i != _activeLed) setLed(i, _bgHue, 255, 10);
      }
    }

};



#endif //_LEDRING_H_
