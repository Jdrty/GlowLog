#include "commit_mapper.h"

void CommitMapper::normalizeBrightness(uint8_t commits[30]) {
  for (int i = 0; i < 30; i++) {
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
