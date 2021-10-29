#include "great_world_base/gamespace/space/space_resource.h"

#include "great_world_base/gamespace/space/cell.h"
#include "great_world_base/gamespace/space/res_loader_interface.h"
#include "great_world_base/gamespace/space/space.h"
#include "great_world_base/gamespace/space/space_manager.h"
#include "great_world_base/gamespace/space/space_common.h"
// #include "g6/math/math_funcs.h"

GW_GAME_BEGIN

static const float kCheckGap = 5.f;
static const RectRegion kNoneRegion = {0, 0, -1, -1};

ResourceRegion::ResourceRegion(ResourceType type)
    : resource_type_(type)
    , loaded_region_(kNoneRegion)
    , loading_region_(kNoneRegion) {

}

ResourceRegion::~ResourceRegion() {
}

bool ResourceRegion::CheckRegionChange(Rect& new_rect, std::vector<RectRegion>& load, std::vector<RectRegion>& unload) {
    if (chunk_size_ <= 0.0f) {
        GW_LOG_ERROR("chunk size is 0, resource type: %d", static_cast<uint8_t>(resource_type_));
        return false;
    }

    RectRegion new_region;
    new_region.minx_ = floor((new_rect.minx() - origin_x_) / chunk_size_);
    new_region.miny_ = floor((new_rect.miny() - origin_z_) / chunk_size_);
    new_region.maxx_ = floor((new_rect.maxx() - origin_x_) / chunk_size_);
    new_region.maxy_ = floor((new_rect.maxy() - origin_z_) / chunk_size_);

    // 区域未变
    if (new_region == loading_region_) {
        return false;
    }

    // 初始化
    if (loading_region_.IsInvalid()) {
        load.push_back(new_region);
        loading_region_ = new_region;
        GW_LOG_DEBUG("check region change1 loading region:[%s], loaded region:[%s], new rect:[%s], resource type:[%d]", 
                     loading_region_.DebugString().c_str(), loaded_region_.DebugString().c_str(), 
                     new_rect.DebugString().c_str(), static_cast<uint8_t>(resource_type_));
        return true;
    }

    // 可能增、减同时多处发生，先算卸载，再算加载
    RectRegion tmp_region;
    if (new_region.minx_ > loading_region_.minx_) {
        tmp_region = loading_region_;
        tmp_region.minx_ = loading_region_.minx_;
        tmp_region.maxx_ = new_region.minx_ - 1;
        unload.push_back(tmp_region);
        loading_region_.minx_ = new_region.minx_;
    }
    if (new_region.maxx_ < loading_region_.maxx_) {
        tmp_region = loading_region_;
        tmp_region.minx_ = new_region.maxx_ + 1;
        tmp_region.maxx_ = loading_region_.maxx_;
        unload.push_back(tmp_region);
        loading_region_.maxx_ = new_region.maxx_;
    }
    if (new_region.miny_ > loading_region_.miny_) {
        tmp_region = loading_region_;
        tmp_region.miny_ = loading_region_.miny_;
        tmp_region.maxy_ = new_region.miny_ - 1;
        unload.push_back(tmp_region);
        loading_region_.miny_ = new_region.miny_;
    }
    if (new_region.maxy_ < loading_region_.maxy_) {
        tmp_region = loading_region_;
        tmp_region.miny_ = new_region.maxy_ + 1;
        tmp_region.maxy_ = loading_region_.maxy_;
        unload.push_back(tmp_region);
        loading_region_.maxy_ = new_region.maxy_;    
    }
    
    if (new_region.minx_ < loading_region_.minx_) {
        tmp_region = loading_region_;
        tmp_region.minx_ = new_region.minx_;
        tmp_region.maxx_ = loading_region_.minx_ - 1;
        load.push_back(tmp_region);
        loading_region_.minx_ = new_region.minx_;
    }
    if (new_region.maxx_ > loading_region_.maxx_) {
        tmp_region = loading_region_;
        tmp_region.minx_ = loading_region_.maxx_ + 1;
        tmp_region.maxx_ = new_region.maxx_;
        load.push_back(tmp_region);
        loading_region_.maxx_ = new_region.maxx_;
    }
    if (new_region.miny_ < loading_region_.miny_) {
        tmp_region = loading_region_;
        tmp_region.miny_ = new_region.miny_;
        tmp_region.maxy_ = loading_region_.miny_ - 1;
        load.push_back(tmp_region);
        loading_region_.miny_ = new_region.miny_;
    }
    if (new_region.maxy_ > loading_region_.maxy_) {
        tmp_region = loading_region_;
        tmp_region.miny_ = loading_region_.maxy_ + 1;
        tmp_region.maxy_ = new_region.maxy_;
        load.push_back(tmp_region);
        loading_region_.maxy_ = new_region.maxy_;
    }
    GW_LOG_DEBUG("check region change2 loading region:[%s], loaded region:[%s], new rect:[%s], resource type:[%s]", 
                 loading_region_.DebugString().c_str(), loaded_region_.DebugString().c_str(), 
                 new_rect.DebugString().c_str(), static_cast<uint8_t>(resource_type_));

    return true;
}

void ResourceRegion::CheckResourceChange(RectRegion& region, bool is_load) {
    if (loaded_region_.IsInvalid()) {
        loaded_region_ = region;
        GW_LOG_DEBUG("check resource change1 loading region:[%s], loaded region:[%s], resource type:[%d]", 
                     loading_region_.DebugString().c_str(), loaded_region_.DebugString().c_str(), 
                     static_cast<uint8_t>(resource_type_));
        return;
    }
    if (is_load) {
        loaded_region_.minx_ = std::min(loaded_region_.minx_, region.minx_);
        loaded_region_.miny_ = std::min(loaded_region_.miny_, region.miny_);
        loaded_region_.maxx_ = std::max(loaded_region_.maxx_, region.maxx_);
        loaded_region_.maxy_ = std::max(loaded_region_.maxy_, region.maxy_);
    }
    else {
        loaded_region_.minx_ = std::max(loaded_region_.minx_, region.minx_);
        loaded_region_.miny_ = std::max(loaded_region_.miny_, region.miny_);
        loaded_region_.maxx_ = std::min(loaded_region_.maxx_, region.maxx_);
        loaded_region_.maxy_ = std::min(loaded_region_.maxy_, region.maxy_);
    }
    GW_LOG_DEBUG("check resource change2 loading region:[%s], loaded region:[%s], resource type:[%d]", 
                 loading_region_.DebugString().c_str(), loaded_region_.DebugString().c_str(), 
                 static_cast<uint8_t>(resource_type_));
}

bool ResourceRegion::GetLoadedRect(Rect& rect) {
    if (loaded_region_.IsInvalid()) {
        return false;
    }
    rect.set_minx(loaded_region_.minx_ * chunk_size_ + origin_x_);
    rect.set_miny(loaded_region_.miny_ * chunk_size_ + origin_z_);
    rect.set_maxx((loaded_region_.maxx_ + 1) * chunk_size_ + origin_x_);
    rect.set_maxy((loaded_region_.maxy_ + 1) * chunk_size_ + origin_z_);
    return true;
}

void ResourceRegion::SetChunkParam(float chunk_size, float origin_x, float origin_z) {
    chunk_size_ = chunk_size;
    origin_x_ = origin_x;
    origin_z_ = origin_z;
    GW_LOG_DEBUG("SetChunkParam, chunk size:%f, origin x:%f, origin z:%f, resource type: %d",
                    chunk_size_, origin_x_, origin_z_, static_cast<uint8_t>(resource_type_)); 
}

SpaceResource::SpaceResource(Space* space): space_(space) {

}

SpaceResource::~SpaceResource() {
    UnInit();
}

bool SpaceResource::Init(uint32_t resource_mask) {
    uint32_t bit_size = sizeof(resource_mask) * 8;
    for (int i = 0; i < bit_size && resource_mask > 0; ++i) {
        uint32_t mask = 0x1 << i;
        uint32_t mask_val = resource_mask & mask;
        if (mask_val) {
            ResourceType curr_type = static_cast<ResourceType>(mask_val);
            res_regions_.emplace(curr_type, new ResourceRegion(curr_type));
            GW_LOG_DEBUG("add space resource, space id: %ld, resource type: %d", space_->id(), mask_val);            
        }
        resource_mask &= ~mask;
    }
    
    auto scheduler = SPACE_MGR.res_loader_scheduler();
    if (!scheduler) {
        GW_LOG_ERROR("res loader scheduler is null, space id: %ld", space_->id());
        return false;
    }
    for (auto& item : res_regions_) {
        float chunk_size = 0.0f;
        float origin_x = 0.0f;
        float origin_z = 0.0f;
        if (!scheduler->GetChunkParam(item.first, space_->id(), chunk_size, origin_x, origin_z)) {
            GW_LOG_ERROR("get chunk param fail, type: %d, space id: %ld", static_cast<uint8_t>(item.first), space_->id());
            return false;
        }
        item.second->SetChunkParam(chunk_size, origin_x, origin_z);
    }
    CheckRegionChange();
    return true;
}

void SpaceResource::UnInit() {
    for (auto& item : res_regions_) {
        delete item.second;
    }
    res_regions_.clear();
}

void SpaceResource::CheckRegionChange() {
    if (nullptr == space_) {
        GW_LOG_ERROR("space is null");
        return;
    }
    Cell* cell = space_->cell();
    if (nullptr == cell) {
        GW_LOG_ERROR("cell is null");
        return;
    }
    CellNode* cell_node = cell->node();
    if (nullptr == cell_node) {
        GW_LOG_ERROR("cell node is null");
        return;
    }
    const Rect& cell_rect = cell_node->rect();
    float ghost_range = space_->ghost_range();
    Rect new_rect;
    new_rect.set_minx(cell_rect.minx() - ghost_range - kCheckGap);
    new_rect.set_miny(cell_rect.miny() - ghost_range - kCheckGap);
    new_rect.set_maxx(cell_rect.maxx() + ghost_range + kCheckGap);
    new_rect.set_maxy(cell_rect.maxy() + ghost_range + kCheckGap);
    // ue -> recast
    Rect fix_rect(-new_rect.maxx(), -new_rect.maxy(), -new_rect.minx(), -new_rect.miny());

    auto scheduler = SPACE_MGR.res_loader_scheduler();
    for (auto& item : res_regions_) {
        std::vector<RectRegion> load_region;
        std::vector<RectRegion> unload_region;
        if (!item.second->CheckRegionChange(fix_rect, load_region, unload_region)) {
            GW_LOG_ERROR("check region change fail, resource type: %d", static_cast<uint8_t>(item.first));
            return;
        }
        if (!unload_region.empty()) {
            for (auto& region : unload_region) {
                scheduler->UnloadResource(item.first, space_->id(), region);
            }
        }
        if (!load_region.empty()) {
            for (auto region : load_region) {
                scheduler->LoadResource(item.first, space_->id(), region);
            }
        }
    }
}

void SpaceResource::CheckResourceChange(ResourceType type, RectRegion& region, bool is_load) {
    auto iter = res_regions_.find(type);
    if (iter == res_regions_.end()) {
        GW_LOG_ERROR("resource region is null, resource type: %d", static_cast<uint8_t>(type));
        return;
    }
    iter->second->CheckResourceChange(region, is_load);
    UpdateTileRect();
}

void SpaceResource::UpdateTileRect() {
    if (!space_) {
        GW_LOG_ERROR("space is null");
        return;
    }
    Rect tile_rect;
    for (auto& item : res_regions_) {
        Rect temp_rect;
        if (!item.second->GetLoadedRect(temp_rect)) {
            return;
        }
        if (!tile_rect.HasArea()) {
            tile_rect = temp_rect;
        }
        else {
            tile_rect.IntersectsRect(temp_rect);
        }
        // GW_LOG_DEBUG("temp_rect:[%s], tile_rect:[%s]", temp_rect.DebugString().c_str(), tile_rect.DebugString().c_str());
    }
    // recast -> ue
    Rect fix_rect(-tile_rect.maxx(), -tile_rect.maxy(), -tile_rect.minx(), -tile_rect.miny());
    space_->cell()->mutable_tile_rect().reset(&fix_rect);
    GW_LOG_DEBUG("fix_rect:[%s]", fix_rect.DebugString().c_str());
}

GW_GAME_END