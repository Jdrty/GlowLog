#include <Arduino.h>

#pragma once

class CommitMapper {
public:
  // Brightness tiers: 0 (off), 63 (25%), 127 (50%), 255 (100%)
  static const int BRIGHTNESS_LEVELS[4];
  
  // Raw commit counts (for 32 days) to brightness values
  static void normalizeBrightness(uint8_t commits[32]);
};