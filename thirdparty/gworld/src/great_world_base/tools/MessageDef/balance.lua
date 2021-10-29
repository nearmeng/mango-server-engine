-- 生成类包含的头文件
__Header = [[
#pragma once
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/rect.h"
#include "great_world_base/common/vector3.h"
#include "great_world_base/common/memory_stream.h"
]]

__Namespace_Begin = [==[namespace gw {
namespace game {]==]
__Namespace_End  = [==[}
}]==]

-- 输出文件
__OutputFileName = private_space_include_path .. "/balance_message.h"

-- 所有类的定义
__Classes = {
  {
    name = "MigrateAvatarNum",
    members = {
      {name = "space_id", type = "uint64_t"},
      {name = "num", type = "uint32_t"},
    }
  },
}
