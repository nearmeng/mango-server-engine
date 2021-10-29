#pragma once
#include "great_world_base/common/cell_message.h"
#include "great_world_base/common/timer_mgr.h"

GW_CELL_MGR_BEGIN

class ICellMgrAdapter {
 public:
  virtual void SendMessageToCell(uint64_t gapp_id, const StringSlice& ss) = 0;
  virtual GWString SpaceConfPath() = 0;
  virtual GWString CellMgrConfPath() = 0;
  virtual uint8_t* MetaData() = 0;
  virtual ITimeMgr* CreateTimerMgr() = 0;
  virtual void DestoryTimerMgr(ITimeMgr* mgr) = 0;
  virtual ILogger* GetLogger() = 0;
  virtual uint64_t GetTimeMs() = 0;
  virtual void OnDestroySpace(uint64_t space_id, const GWString& tag) = 0;
  virtual void AllCellCreated(const gw::AllCellCreatedMsg& msg) = 0;
};

class CellMgrApi {
 public:

  /**
   * @brief 初始化模块
   * 
   * @param proxy cellmgr的功能回调
   * @param cfg_path cellmgr中配置文件的路径，例如 "./"
   * @param buffer 一段内存，如果需要使用共享内存外部需要把这个buffer分配在共享内存上，并且维护这个变量，resume的时候传回来
   * @return true 
   * @return false 
   */
  static bool Init(ICellMgrAdapter* adapter);

  /**
   * @brief 
   * 
   * @param is_resume 
   */
  static void UnInit(bool is_resume);

  /**
   * @brief 初始化结束，cellmgr 内部开始进行
   * 
   */
  static void StartUp();

  /**
   * @brief 
   * 
   * @return true 
   * @return false 
   */
  static bool Resume(ICellMgrAdapter* adapter);

  /**
   * @brief reload cell cfg
   * 
   * @return true 
   * @return false 
   */
  static bool ReloadCfg();

  /**
   * @brief 添加一个可供分配cell的game实例
   * 
   * @param gapp_id 
   * @return true 注册成功
   * @return false 
   */
  static bool RegisterGameApp(uint64_t gapp_id, bool is_base = false);
  
  /**
   * @brief 删除一个可供分配cell的game实例
   * 
   * @param gapp_id 
   * @return true 注册成功
   * @return false 
   */
  static bool UnregisterGameApp(uint64_t gapp_id, bool is_base = false);

  /**
   * @brief 返回一个 gappid 的set
   * 
   * @return std::unordered_set<std::vector<uint64_t>> 
   */
  static std::unordered_set<uint64_t> GetGameAppIds(bool is_base = false);

  /**
   * @brief Get the Game App Ids object
   * 
   * @param space_id 
   * @return std::unordered_map<uint64_t, gw::CellState> 
   */
  static std::unordered_map<uint64_t, gw::CellState> GetGameAppIds(uint64_t space_id);

  /**
   * @brief Get the Space Ids object
   * 
   * @return std::unordered_set<uint64_t> 
   */
  static std::unordered_set<uint64_t> GetSpaceIds();
  
  /**
   * @brief Create a Space object
   * 
   * @return uint64_t spaceid，0代表创建失败
   */
  static uint64_t CreateSpace(CreateSpaceMsg& req);

  /**
   * @brief 销毁space
   * 
   * @param id space_id
   * @return true success
   * @return false fail
   */
  static bool DestroySpace(uint64_t id);

  /**
   * @brief 根据坐标选择一个 gapp。返回 0 表示找不到合适的 gapp
   * 
   * @param space_id 
   * @param x 
   * @param y 
   * @return uint64_t 
   */
  static uint64_t ChooseFreeCellGame(uint64_t space_id, float x, float y);

  /**
   * @brief 
   * 
   * @return uint64_t 
   */
  static uint64_t ChooseFreeBaseGame();

  /**
   * @brief 输出spce/cell/布局信息
   * 
   */
  static GWString DebugString();

  /**
   * @brief 更新某个 cell node 的最少分线数量
   * 
   * @param space_id 
   * @param cell_node_id 
   * @param value 数值
   * @return true 
   * @return false 
   */
  static RetCode UpdateMinParallelNum(uint64_t space_id, uint64_t cell_node_id, uint32_t value);

  /**
   * @brief 
   * 
   * @param gapp_id 
   * @param ss 
   */
  static void DispatchMessage(uint64_t gapp_id, StringSlice ss);

};

GW_CELL_MGR_END

/**
 * 需要的配置文件 
 * 1. cell config 布局之类的
 * 2. CellMgrConfig
 */
