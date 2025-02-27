#include <Arduino.h>
#include <FastLED.h>

// Configuration
#define DATA_PIN    D4     // ESP8266 pin connected to LED DIN
#define NUM_LEDS    32     // Total LEDs
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB    // Common for WS2812B

CRGB leds[NUM_LEDS];

// Testing lights
void theaterChase(CRGB color, int speed);

void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(50); // Start at 20% brightness
  FastLED.clear();
  FastLED.show();
}

void loop() {
  // Test 1
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
    FastLED.show();
    delay(50);
    leds[i] = CRGB::Black;
  }

  // Test 2
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
    FastLED.show();
    delay(50);
    leds[i] = CRGB::Black;
  }

  // Test 3
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
    FastLED.show();
    delay(50);
    leds[i] = CRGB::Black;
  }

  // Test 4
  theaterChase(CRGB::White, 50);
}

// Response and stability test
void theaterChase(CRGB color, int speed) {
  for (int j = 0; j < 10; j++) { // Repeat 10 cycles
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < NUM_LEDS; i += 3) {
        leds[i + q] = color; // Turn every third LED on
      }
      FastLED.show();
      delay(speed);
      for (int i = 0; i < NUM_LEDS; i += 3) {
        leds[i + q] = CRGB::Black; // Turn them off
      }
    }
  }
}