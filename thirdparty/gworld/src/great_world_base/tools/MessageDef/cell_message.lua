-- 生成类包含的头文件
__Header = [[
#pragma once
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/rect.h"
#include "great_world_base/common/vector3.h"
#include "great_world_base/common/memory_stream.h"
]]

__Namespace_Begin = [==[namespace gw {]==]
__Namespace_End = [==[}]==]

-- 输出文件
__OutputFileName = public_common_include_path .. "/cell_message.h"

-- 所有类的定义
__Classes = {
  {
    name = "AddCellMsg",
    members = {
      {name = "space_id", type = "uint64_t"},
      {name = "layout_data", type = "GWString"},
      {name = "is_static", type = "bool"},
      {name = "tag", type = "GWString"}
    }
  },
  {
    name = "DestroySpaceNotice",
    members = {
      {name = "space_id", type = "uint64_t"}
    }
  },
  {
    name = "LayoutMsg",
    members = {
      {name = "space_id", type = "uint64_t"},
      {name = "layout_data", type = "GWString"},
      {name = "is_static", type = "bool"},
      {name = "tag", type = "GWString"}
    }
  },
  {
    name = "SpaceLayoutMsg",
    members = {
      {name = "layouts", type = "GWVector<LayoutMsg>"}
    }
  },
  {
    name = "AllCellCreatedMsg",
    members = {
      {name = "space_id", type = "uint64_t"},
      {name = "tag", type = "GWString"}
      {name = "meta_data", type = "GWString"}
    }
  },
  {
    name = "CellReportMsg",
    members = {
      {name = "space_id", type = "uint64_t"},
      {name = "num_entities", type = "uint32_t"},
      {name = "num_reals", type = "uint32_t"},
      {name = "entity_rect", type = "Rect"},
      {name = "tile_rect", type = "Rect"},
      {name = "num_avatars", type = "uint32_t"}
    }
  },
  {
    name = "ReportLoadMsg",
    members = {
      {name = "load", type = "float"},
      {name = "memsize", type = "uint32_t"},
      {name = "num_entities", type = "uint32_t"},
      {name = "num_reals", type = "uint32_t"},
      {name = "cell_report", type = "GWVector<CellReportMsg>"},
      {name = "num_avatars", type = "uint32_t"}
    }
  },
  {
    name = "CreateSpaceMsg",
    members = {
      {name = "name", type = "GWString"},
      {name = "tag", type = "GWString"},
      {name = "min_cell_num", type = "uint32_t"},
      {name = "max_cell_num", type = "uint32_t"},
      {name = "rect", type = "Rect"},
      {name = "layout_data", type = "GWString"},
      {name = "map_name", type = "GWString"},
      {name = "is_static", type = "bool"},
      {name = "meta_data", type = "GWString"}
    }
  },
  {
    name = "CellStateChangeInfo",
    members = {
      {name = "space_id", type = "uint64_t"},
      {name = "gapp_id", type = "uint64_t"},
      {name = "old_state", type = "uint8_t"},
      {name = "new_state", type = "uint8_t"},
      {name = "index", type = "uint32_t"},
    }
  },
  {
    name = "CellStateChangeRsp",
    members = {
      {name = "state", type = "uint32_t"}
    }
  },
  {
    name = "ClientInfo",
    members = {
      {name = "session_id", type = "uint64_t"}
    }
  },
  {
    name = "EntityInfo",
    members = {
      {name = "eid", type = "uint64_t"},
      {name = "entity_name", type = "GWString"},
      {name = "custom_id", type = "uint64_t"},
      {name = "content", type = "GWString"},
      {name = "client_info", type = "ClientInfo"},
      {name = "base_app_id", type = "uint64_t"},
      {name = "base_eid", type = "uint64_t"}
    }
  },
  {
    name = "CallEntityMessage",
    members = {
      {name = "eid", type = "uint64_t"},
      {name = "method_name", type = "GWString"},
      {name = "content", type = "GWString"}
    }
  },
  {
    name = "CreateEntityRequest",
    members = {
      {name = "space_id", type = "uint64_t"},
      {name = "position", type = "Vector3"},
      {name = "is_lua", type = "bool"},
      {name = "info", type = "EntityInfo"}
    }
  },
  {
    name = "CreateBaseEntityRequest",
    members = {
      {name = "dbid", type = "uint64_t"},
      {name = "is_lua", type = "bool"},
      {name = "info", type = "EntityInfo"}
    }
  },
  {
    name = "CreateEntityMgrReply",
    members = {
      {name = "gapp_id", type = "uint64_t"},
      {name = "is_success", type = "bool"}
    }
  },
  {
    name = "CreateEntityClientReply",
    members = {
      {name = "is_success", type = "bool"}
    }
  }
}
