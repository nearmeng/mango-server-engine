#pragma once

#include "great_world_base/common/common.h"
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/ilog.h"
#include "great_world_base/common/rect.h"
#include "great_world_base/common/memory_stream.h"
#include "great_world_base/common/shm/shm_allocator.h"
#include "great_world_base/common/shm/shm_mgr.h"

GW_GAME_BEGIN

enum class ResourceType : uint32_t {
    None = 0x00,
    Mesh = 0x01,
    Span = 0x02,
};

struct ResourceTypeHash {
    template<typename T>
    uint32_t operator()(T t) const {
        return static_cast<uint32_t>(t);
    }
};

struct RectRegion {
    int32_t minx_;
    int32_t miny_;
    int32_t maxx_;
    int32_t maxy_;
    RectRegion(int32_t minx = 0, int32_t miny = 0, int32_t maxx = 0, int32_t maxy = 0) 
    : minx_(minx), miny_(miny), maxx_(maxx), maxy_(maxy) {

    }
    bool operator == (const RectRegion& r) const {
        return minx_ == r.minx_ && miny_ == r.miny_ && maxx_ == r.maxx_ && maxy_ == r.maxy_;
    }
    bool IsInvalid() const {
        return minx_ > maxx_ || miny_ > maxy_;
    }
    GWString DebugString() const {
        std::stringstream ss;
        ss << "minx:" << minx_ << ","
           << "maxx:" << maxx_ << ","
           << "miny:" << miny_ << ","
           << "maxy:" << maxy_;
        return GWString(ss.str().c_str(), ss.str().size());
    }
};

struct LoaderTask {
    uint64_t space_id_;
    RectRegion process_region_;
    ResourceType type_;
    void* data_;
    
    LoaderTask(uint64_t id, RectRegion region, ResourceType type) : 
        space_id_(id), 
        process_region_(region), 
        type_(type),
        data_(nullptr) {
    }
};
using LoaderTasks = std::unordered_map<uint64_t, LoaderTask*>;
using LoaderTasksIter = LoaderTasks::iterator;

GW_GAME_END
