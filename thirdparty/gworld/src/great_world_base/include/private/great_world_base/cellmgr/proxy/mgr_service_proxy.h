#pragma once
#include "great_world_base/cellmgr/cellmgr_common.h"
#include "great_world_base/common/cell_message.h"
#include "great_world_base/common/network/network_mgr.h"

GW_CELL_MGR_BEGIN

class MgrServiceProxy {
 public:
  virtual ~MgrServiceProxy() {}

  /**
   * @brief space 创建的时候，根据space的划分去game创建cell
   * 
   * @param gapp_id 
   * @param msg 
   */
  void OnAddCell(uint64_t gapp_id, const AddCellMsg &msg) {
    NetworkOStream os(NetworkMessage::AddCell);
    msg.ToString(&os);
    SEND_MSG_TO_CELL(gapp_id, os);
  }

  /**
   * @brief space 销毁的时候把当前的cell都一起销毁
   * 
   * @param gapp_id 
   * @param msg 
   */
  void OnDestroyCell(uint64_t gapp_id, const DestroySpaceNotice &msg) {
    NetworkOStream os(NetworkMessage::DestroyCell);
    msg.ToString(&os);
    SEND_MSG_TO_CELL(gapp_id, os);
  }

  /**
   * @brief 更新space布局，可能是移动边界、更新分线数量、删除cell、添加cell
   * 
   * @param gapp_id 
   * @param msg 
   */
  void OnUpdateSpaceLayout(uint64_t gapp_id, const SpaceLayoutMsg &msg) {
    NetworkOStream os(NetworkMessage::UpdateSpaceLayout);
    msg.ToString(&os);
    SEND_MSG_TO_CELL(gapp_id, os);
  }

  /**
   * @brief space 创建的时候所有cell创建完成（不包括分线）
   * 
   * @param gapp_id 
   * @param msg 
   */
  virtual void OnAllCellCreated(uint64_t gapp_id, const AllCellCreatedMsg &msg) {
    NetworkOStream os(NetworkMessage::AllCellCreated);
    msg.ToString(&os);
    SEND_MSG_TO_CELL(gapp_id, os);
  }

  /**
   * @brief 给 base 同步布局信息，方便快速查找game
   * 
   * @param gapp_id 
   * @param msg 
   */
  virtual void OnSyncCell(uint64_t gapp_id, const AddCellMsg &msg) { }
};

GW_CELL_MGR_END