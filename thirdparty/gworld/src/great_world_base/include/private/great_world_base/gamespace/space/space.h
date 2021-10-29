#pragma once

#include "great_world_base/common/timer_mgr_private.h"
#include "great_world_base/gamespace/space/cell_node.h"
#include "great_world_base/gamespace/space/region_node.h"
#include "great_world_base/gamespace/space/space_common.h"
#include "great_world_base/gamespace/gamespace_api.h"
#include "great_world_base/gamespace/cell_entity/cell_entity.h"

GW_GAME_BEGIN

class Cell;
class SpaceResource;

class Space : public ISpace {
  GWCLASS_DECL(Space)
 public:
  enum class SpaceState : uint8_t {
    // 初始化
    kInit,
    // 迁移中
    kRemoving,
    // 预销毁
    kDestroying,
    // 已销毁
    kDestroyed,
  };

  struct CellStateRsp {
    uint32_t target_num_ = 0;
    uint32_t back_num_ = 0;
    uint32_t index_ = 0;  // todo evan 用全局唯一的 uuid 更好
    uint64_t check_timerid_ = 0;
    Space* space_;

    CellStateRsp(Space* space) { space_ = space; }
    ~CellStateRsp() { 
      if ( check_timerid_ != 0 )
      {
        TimerManager::Get()->DeleteTimer(check_timerid_);
        check_timerid_ = 0;
      }
    }

    void update(uint32_t target_num) {
      target_num_ = target_num;
      back_num_ = 0;
      index_++;
      auto temp_index = index_;
      Space* space = space_;
      check_timerid_ = TimerManager::Get()->CreateTimer([temp_index, space](uint64_t) {
        if (space) {
          space->OnChangeCellStateTimeOutFromGame();
        }
      }, 10000, false);
    }
  };

  Space(uint64_t id);
  Space() : cell_state_rsp_(this) {}
  virtual ~Space();
  void PrepareRemove();
  void PrepareDestroy();
  void OnDestroy();

  // 不管是real 还是 ghost都会走到
  RetCode AddCellEntity(CellEntity* entity);
  void DelCellEntity(uint64_t id);

  // 用户调用的，用于 real 初次进入 space
  std::pair<ICellEntity*, RetCode> Enter(uint64_t eid, uint64_t base_eid, uint64_t base_gapp_id, Vector3 pos,
                                         ICellEntityAdapter* apapter) override;

  std::pair<ICellEntity*, RetCode> Enter(CellEntity*, const Vector3& pos, bool is_real = false);
  
  void Leave(ICellEntity*) override;

  void Leave(CellEntity*, LeaveSpaceReason);

  bool LeaveWithoutDel(CellEntity*, LeaveSpaceReason);

  void OnAllCellCreated();
  void DebugOutput() override;

  CellNode* FindCellNode(uint64_t gapp_id);
  CellNode* FindCellNode(float x, float z);
  RegionNode* FindNonCellNode(uint64_t node_id);
  bool AddNonCellNode(RegionNode* node);
  bool UpdateLayout(IMemoryIStream& in);
  bool AddCellNode(uint64_t gapp_id, CellNode* node);
  bool RemoveCellNode(uint64_t gapp_id);
  bool RemoveNonCellNode(uint64_t node_id);

  bool HasLayout() const { return root_ != nullptr; }
  RegionNode* mutable_root() { return root_; }

  uint64_t id() const { return id_; }
  const Rect& rect() const { return rect_; }
  //const GameAppEntries& gapps() const { return gapps_; }

  float ghost_range() const { return ghost_range_; }
  bool set_ghost_range(float range) override;
  bool is_static() const { return is_static_; }
  void set_is_static(bool is_static) { is_static_ = is_static; }

  Cell* cell() const;
  void set_cell(Cell* cell);
  CellNode* curr_cell_node() const;
  RegionNode* root_region_node() { return root_; }
  bool wait_remove() { return state_ == SpaceState::kRemoving; }
  bool InitSpaceResource();
  SpaceResource* space_resource() override { return space_resource_; }

  void ChangeCellState(CellState new_state);
  bool ChangeOtherCellState(CellState new_state, uint64_t gapp_id);
  void OnChangeCellStateSuccessFromGame(uint32_t index, uint64_t other_gapp_id, CellState other_state);
  void OnChangeCellStateTimeOutFromGame();

  CellState GetCellNodeState(uint64_t gapp_id = 0);

  void set_space_id(uint64_t id) { id_ = id; }

  uint32_t num_entities() const { return num_entities_; }
  void set_num_entities(uint32_t num) { num_entities_ = num; }
  uint32_t num_entity_avatars() const { return num_entity_avatars_; }
  void set_num_entity_avatars(uint32_t num) { num_entity_avatars_ = num; }
  uint32_t num_reals() const { return num_reals_; }
  void set_num_reals(uint32_t num) { num_reals_ = num; }
  uint32_t num_real_avatars() const { return num_real_avatars_; }
  void set_num_real_avatars(uint32_t num) { num_real_avatars_ = num; }

  void IncreaseNumEntities(CellEntity* entity);
  void DecreaseNumEntities(CellEntity* entity);
  void IncreaseNumReals(CellEntity* entity);
  void DecreaseNumReals(CellEntity* entity);

  void set_resource_mask(uint32_t resource_mask) override { resource_mask_ = resource_mask; }

  CellEntities& cell_entities() { return cell_entities_; }

  GWString tag() const { return tag_; }
  void set_tag(GWString tag) { tag_ = tag; }

 private:
  uint64_t id_;
  //GameAppEntries gapps_;
  Rect rect_;
  RegionNode* root_ = nullptr;
  Cell* cell_ = nullptr;  // cell的生命周期由Space管理
  GWHashMap<uint64_t, CellNode*> cell_nodes_;
  GWHashMap<uint64_t, RegionNode*> non_cell_nodes_;
  SpaceState state_;
  CellStateRsp cell_state_rsp_;
  float ghost_range_;
  SpaceResource* space_resource_ = nullptr;
  bool is_static_ = false;

  uint32_t num_entities_ = 0;
  uint32_t num_reals_ = 0;
  uint32_t num_real_avatars_ = 0;
  uint32_t num_entity_avatars_ = 0;

  uint32_t resource_mask_ = 0;

  CellEntities cell_entities_;
  GWString tag_;
};

GW_GAME_END
