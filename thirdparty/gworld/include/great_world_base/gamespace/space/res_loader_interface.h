#pragma once

#include "great_world_base/gamespace/space/space_resource_common.h"

GW_GAME_BEGIN

class IResLoader {
public:
    virtual ~IResLoader() {}
    virtual bool GetChunkParam(uint64_t space_id, float& unit_size, float& origin_x, float& origin_z) = 0;
    virtual void LoadResource(LoaderTask* task) = 0;
    virtual void UnloadResource(uint64_t space_id, RectRegion& region) = 0;
    virtual void OnLoadedResource(LoaderTask* task) = 0;
};
using ResLoaders = std::unordered_map<ResourceType, IResLoader*, ResourceTypeHash>;
using ResLoadersIter = ResLoaders::iterator;

class IResLoaderScheduler {
public:
    virtual ~IResLoaderScheduler() {}
    virtual int Init() = 0;
    virtual void UnInit() = 0;
    virtual bool GetChunkParam(ResourceType type, uint64_t space_id, float& unit_size, float& origin_x, float& origin_z) = 0;
    virtual bool LoadResource(ResourceType type, uint64_t space_id, RectRegion& region) = 0;
    virtual bool UnloadResource(ResourceType type, uint64_t space_id, RectRegion& region) = 0;
};

GW_GAME_END
