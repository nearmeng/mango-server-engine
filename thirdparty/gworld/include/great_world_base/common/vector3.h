#pragma once
#include "great_world_base/common/memory_stream.h"
typedef float real_t;

namespace gw {
struct Vector3 {

  Vector3() {}
  Vector3(real_t x, real_t y, real_t z) {
    coord[0] = x;
    coord[1] = y;
    coord[2] = z;
  }

  union {
    struct {
      real_t x;  // The vector's X component. Also accessible by using the index position [0].
      real_t y;  // The vector's Y component. Also accessible by using the index position [1].
      real_t z;  // The vector's Z component. Also accessible by using the index position [2].
    };

    real_t coord[3];
  };

  bool ToString(IMemoryOStream* os) const {
    *os << x << y << z;
    return true;
  }

  bool FromString(IMemoryIStream* is) {
    *is >> x >> y >> z;
    return true;
  }
};
}  // namespace gw