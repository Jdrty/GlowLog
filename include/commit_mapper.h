#include <Arduino.h>

#pragma once

class CommitMapper {
public:
  static const int BRIGHTNESS_LEVELS[4];
  static void normalizeBrightness(uint8_t commits[30]);
};