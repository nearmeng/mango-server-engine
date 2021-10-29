#pragma once
namespace gw {

enum RetCode {
  SUCCESS,
  FAIL,
  
  AOI_INIT_REPEAT,  // 重复初始化
  AOI_SAPCE_REPEAT, // 要创建的space已经存在
  AOI_SPACE_NOT_FOUND, // space 没找到 
  AOI_USE_DEFAULT_AOIMGR_DESTORY, // 传入的 callback 对象中的 destory 使用默认实现

  SPACE_ADD_CELL, // add cell 成功  
  SPACE_NOT_FOUND, // 找不到space
  SPACE_UPDATE_FAIL, // space 更新失败
  CELL_NODE_NOT_FOUND, // 找不到cellnode
  NODE_TYPE_ERROR, // region node 的类型和目标不符
  READER_NOT_FOUND, // reader 找不到或者初始化失败
  CONF_INIT_FAIL, // 配置文件初始化失败

  CELL_ENTITY_ADD_REPEAT  // 增加重复 
};

}