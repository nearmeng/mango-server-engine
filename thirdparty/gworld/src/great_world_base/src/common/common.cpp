#include "great_world_base/common/common_private.h"

namespace gw {

namespace {
std::function<uint64_t()> get_time_func = nullptr;
}

uint64_t GetWalltimeMs() {
  if(get_time_func) {
    return get_time_func();
  }
  return 0;
}

void SetWalltimeMs(std::function<uint64_t()> func) {
  get_time_func = func;
}

}  // namespace gw