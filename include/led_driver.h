// include/led_driver.h

#pragma once
#include <FastLED.h>

class LedDriver {
public:
  static const int NUM_LEDS = 32; 
  static const int DATA_PIN = D4;
  
  void initialize();
  void setBrightness(uint8_t brightness);
  void displayCommits(const uint8_t commits[]);
  void testPattern();
  void errorPattern();

private:
  CRGB leds[NUM_LEDS];
};
