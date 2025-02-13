#include "commit_mapper.h"

// Define brightness levels (not currently used directly, but kept for reference)
const int CommitMapper::BRIGHTNESS_LEVELS[4] = {0, 63, 127, 255};

void CommitMapper::normalizeBrightness(uint8_t commits[32]) {
  for (int i = 0; i < 32; i++) {
    if (commits[i] == 0) {
      commits[i] = 0;         // Off
    } else if (commits[i] <= 1) {
      commits[i] = 63;        // 25%
    } else if (commits[i] <= 3) {
      commits[i] = 127;       // 50%
    } else {
      commits[i] = 255;       // 100%
    }
  }
}