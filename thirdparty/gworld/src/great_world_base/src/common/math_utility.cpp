#include "great_world_base/common/math_utility.h"

#include <stdlib.h>
#include <time.h>

#include <algorithm>

#include "great_world_base/common/common_private.h"

static int Random() {
  static uint32_t seed = 0;
  if (UNLIKELY(seed == 0)) {
    seed = static_cast<uint32_t>(time(nullptr));
    srand(seed);
  }

  return rand();
}

namespace gw {

const float MathUtility::kFloatTolerance = 1.e-4f;
const double MathUtility::kDoubleTolerance = 1.e-6f;

int MathUtility::RandRange(int min, int max) {
  if (min == max) {
    return min;
  }

  if (min > max) {
    std::swap(min, max);
  }

  return Random() % (max - min + 1) + min;
}

}  // namespace gw