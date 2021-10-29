#pragma once
#include "great_world_base/common/shm/shm_mgr.h"
#include "great_world_base/common/shm/shm_singleton_template.h"
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi/aoi_manager.h"
#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_conf.h"
#include "great_world_base/gamespace/multi_cell_aoi/aoi_entity.h"

GW_GAME_BEGIN

#define AOI_WORLD gw::game::AoIWorld::GetInstance()

class AoIWorld : public ShmSingletonRef<AoIWorld> {
  GWCLASS_DECL(AoIWorld);

 public:
  ~AoIWorld();

  RetCode Init(GWString conf_path);
  void Uninit();

  static void OwnResume(void* aoi_world);

  RetCode CreateSpace(uint64_t space_id);
  RetCode DestroySpace(uint64_t space_id);
  AoIManager* GetAoIManager(uint64_t space_id);

  AoIEntity* Enter(uint64_t space_id, uint64_t eid, uint32_t radius, Vector3 pos);
  void Leave(uint64_t space_id, uint64_t eid);

  void OnBeginObserver(const GWVector<uint64_t>& self, uint64_t other);;
  void OnEndObserver(uint64_t self, uint64_t other);
  GWVector<AoIDiffProp>&& GetDiffProps(uint64_t eid, uint32_t labels);
  IMemoryOStream* NotifyOS();

  inline AoIConfig& AoIConf() { return aoi_conf_; }

 private:
  GWHashMap<uint64_t, AoIManager*> aoi_manager_map_;
  AoIConfig aoi_conf_;
};

GW_GAME_END