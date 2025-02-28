// src/led_driver.cpp

#include "led_driver.h"

// Initialize the WS2812B LED strip
void LedDriver::initialize() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
}

// Set global brightness (0-255)
void LedDriver::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(brightness);
  FastLED.show();
}

// Display commit data by mapping each day
void LedDriver::displayCommits(const uint8_t commits[]) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (commits[i] == 0) {
      leds[i] = CRGB::Black;
    } else {
      leds[i] = CRGB(commits[i], commits[i], commits[i]);
    }
  }
  FastLED.show();
}

// Test pattern
void LedDriver::testPattern() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
    FastLED.show();
    delay(50);
    leds[i] = CRGB::Black;
  }
}

// Error pattern
void LedDriver::errorPattern() {
  for (int i = 0; i < 3; i++) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
    delay(200);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(200);
  }
}