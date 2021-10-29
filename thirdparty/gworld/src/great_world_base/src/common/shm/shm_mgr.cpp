#include "great_world_base/common/shm/shm_mgr.h"

namespace gw {

void* ShmMalloc(std::size_t size) {
  if (size == 0) {
    size = 1;
  }
  return SHM_MGR.malloc(size);
}

void ShmFree(void* p) {
  if (p == NULL) {
    return;
  }
  SHM_MGR.free(p);
}

}