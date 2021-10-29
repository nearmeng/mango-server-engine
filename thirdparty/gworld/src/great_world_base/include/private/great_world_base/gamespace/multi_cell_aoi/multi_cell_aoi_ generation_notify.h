// todo
// 分代同步：
// 之前同步频率高的对象之后的一段时间内同步频率还是高
// 我周围的人的同步都希望尽快同步到我

// 原始算法：
// 每100ms遍历一次周围的玩家(n)
// 单个玩家 10s 内会做 10n*10->100n次同步判断
// 这个时候只能采用限制单帧同步数量，或者降低频率，可以预见的导致体验不好

// 分代算法：
// 分2段，分别的 
// 频率 [100,500,1000,2000,5000]
// 第一级预留，用来做高强度的更新，适合少量的玩家 pk 情况
#pragma once
#include "great_world_base/gamespace/multi_cell_aoi/multi_cell_aoi_observer.h"

GW_GAME_BEGIN

class Generation {


private:
  uint16_t notify_frequency_;
};

class MultiCellAoiGenerationNotify : public MultiCellAoIObserver {

};

GW_GAME_END