#pragma once

#include "great_world_base/gamespace/space/space_resource_common.h"

GW_GAME_BEGIN

class Space;
class ResourceRegion {
public:
    ResourceRegion(ResourceType type);
    ~ResourceRegion();

    bool CheckRegionChange(Rect& new_rect, std::vector<RectRegion>& load, std::vector<RectRegion>& unload);
    void CheckResourceChange(RectRegion& region, bool is_load);
    bool GetLoadedRect(Rect& rect);
    void SetChunkParam(float chunk_size, float origin_x, float origin_z);

private:
    ResourceType resource_type_;
    RectRegion loaded_region_;
    RectRegion loading_region_;
    float chunk_size_ = 0.0f;
    float origin_x_ = 0.0f;
    float origin_z_ = 0.0f;
};

using ResourceRegions = std::unordered_map<ResourceType, ResourceRegion*, ResourceTypeHash>;
using ResourceRegionsIter = ResourceRegions::iterator;

class SpaceResource {
public:
    SpaceResource(Space* space);
    ~SpaceResource();

    bool Init(uint32_t resource_mask);
    void UnInit();

    void CheckRegionChange();
    void CheckResourceChange(ResourceType type, RectRegion& region, bool is_load);

private:
    void UpdateTileRect();

private:
    Space* space_ = nullptr;
    ResourceRegions res_regions_;
};

GW_GAME_END
