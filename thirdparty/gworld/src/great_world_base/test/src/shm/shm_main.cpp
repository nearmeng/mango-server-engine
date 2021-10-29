#include <iostream>
#include <memory>

#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/ilog.h"
#include "great_world_base/common/shm/shm_mgr.h"
#include "mock/log.h"

int main() {
  SHM_MGR.set_free_func(std::free);
  SHM_MGR.set_malloc_func(std::malloc);

  gw::Logger::InitLogger(new gw::MockLogger());

  gw::GWHashMap<uint64_t, uint64_t> shm_map;
  shm_map.emplace(1, 1);
  auto it = shm_map.find(1);

  std::cout << (it == shm_map.end()) << " 11111111" << std::endl;
  return 0;
}