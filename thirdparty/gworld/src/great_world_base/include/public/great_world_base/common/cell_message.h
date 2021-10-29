// 头文件
#pragma once
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/rect.h"
#include "great_world_base/common/vector3.h"
#include "great_world_base/common/memory_stream.h"

namespace gw {

class AddCellMsg {
 private:
  // member
  uint64_t space_id_;
  GWString layout_data_;
  bool is_static_;
  GWString tag_;
   
 public:
  AddCellMsg() = default;
  AddCellMsg(const AddCellMsg&) = default;
  AddCellMsg& operator=(const AddCellMsg&) = default;
  // constructor
  AddCellMsg(uint64_t space_id, GWString layout_data, bool is_static, GWString tag) : space_id_(space_id), layout_data_(layout_data), is_static_(is_static), tag_(tag) { }
  // move constructor
  AddCellMsg(AddCellMsg&& other) : space_id_(std::move(other.space_id_)), layout_data_(std::move(other.layout_data_)), is_static_(std::move(other.is_static_)), tag_(std::move(other.tag_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << space_id_;
    *os << layout_data_;
    *os << is_static_;
    *os << tag_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> space_id_;
    *is >> layout_data_;
    *is >> is_static_;
    *is >> tag_;
    return true;
  }

  // getter and setter
  
  inline uint64_t space_id() const { return space_id_; }
  inline uint64_t& mutable_space_id() { return space_id_; }
  inline const GWString& layout_data() const { return layout_data_; }
  inline GWString& mutable_layout_data() { return layout_data_; }
  inline bool is_static() const { return is_static_; }
  inline bool& mutable_is_static() { return is_static_; }
  inline const GWString& tag() const { return tag_; }
  inline GWString& mutable_tag() { return tag_; }

  inline void set_space_id(const uint64_t& tmp) { space_id_ = tmp; }
  inline void set_space_id(uint64_t&& tmp) { space_id_ = std::move(tmp); }
  inline void set_layout_data(const GWString& tmp) { layout_data_ = tmp; }
  inline void set_layout_data(GWString&& tmp) { layout_data_ = std::move(tmp); }
  inline void set_is_static(const bool& tmp) { is_static_ = tmp; }
  inline void set_is_static(bool&& tmp) { is_static_ = std::move(tmp); }
  inline void set_tag(const GWString& tmp) { tag_ = tmp; }
  inline void set_tag(GWString&& tmp) { tag_ = std::move(tmp); }


};

class DestroySpaceNotice {
 private:
  // member
  uint64_t space_id_;
   
 public:
  DestroySpaceNotice() = default;
  DestroySpaceNotice(const DestroySpaceNotice&) = default;
  DestroySpaceNotice& operator=(const DestroySpaceNotice&) = default;
  // constructor
  DestroySpaceNotice(uint64_t space_id, GWString tag) : space_id_(space_id) { }
  // move constructor
  DestroySpaceNotice(DestroySpaceNotice&& other) : space_id_(std::move(other.space_id_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << space_id_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> space_id_;
    return true;
  }

  // getter and setter
  
  inline uint64_t space_id() const { return space_id_; }
  inline uint64_t& mutable_space_id() { return space_id_; }

  inline void set_space_id(const uint64_t& tmp) { space_id_ = tmp; }
  inline void set_space_id(uint64_t&& tmp) { space_id_ = std::move(tmp); }

};

class LayoutMsg {
 private:
  // member
  uint64_t space_id_;
  GWString layout_data_;
  bool is_static_;
  GWString tag_;
   
 public:
  LayoutMsg() = default;
  LayoutMsg(const LayoutMsg&) = default;
  LayoutMsg& operator=(const LayoutMsg&) = default;
  // constructor
  LayoutMsg(uint64_t space_id, GWString layout_data, bool is_static, GWString tag) : space_id_(space_id), layout_data_(layout_data), is_static_(is_static), tag_(tag) { }
  // move constructor
  LayoutMsg(LayoutMsg&& other) : space_id_(std::move(other.space_id_)), layout_data_(std::move(other.layout_data_)), is_static_(std::move(other.is_static_)), tag_(std::move(other.tag_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << space_id_;
    *os << layout_data_;
    *os << is_static_;
    *os << tag_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> space_id_;
    *is >> layout_data_;
    *is >> is_static_;
    *is >> tag_;
    return true;
  }

  // getter and setter
  
  inline uint64_t space_id() const { return space_id_; }
  inline uint64_t& mutable_space_id() { return space_id_; }
  inline const GWString& layout_data() const { return layout_data_; }
  inline GWString& mutable_layout_data() { return layout_data_; }
  inline bool is_static() const { return is_static_; }
  inline bool& mutable_is_static() { return is_static_; }
  inline const GWString& tag() const { return tag_; }
  inline GWString& mutable_tag() { return tag_; }

  inline void set_space_id(const uint64_t& tmp) { space_id_ = tmp; }
  inline void set_space_id(uint64_t&& tmp) { space_id_ = std::move(tmp); }
  inline void set_layout_data(const GWString& tmp) { layout_data_ = tmp; }
  inline void set_layout_data(GWString&& tmp) { layout_data_ = std::move(tmp); }
  inline void set_is_static(const bool& tmp) { is_static_ = tmp; }
  inline void set_is_static(bool&& tmp) { is_static_ = std::move(tmp); }
  inline void set_tag(const GWString& tmp) { tag_ = tmp; }
  inline void set_tag(GWString&& tmp) { tag_ = std::move(tmp); }


};

class SpaceLayoutMsg {
 private:
  // member
  GWVector<LayoutMsg> layouts_;
   
 public:
  SpaceLayoutMsg() = default;
  SpaceLayoutMsg(const SpaceLayoutMsg&) = default;
  SpaceLayoutMsg& operator=(const SpaceLayoutMsg&) = default;
  // constructor
  SpaceLayoutMsg(GWVector<LayoutMsg> layouts) : layouts_(layouts) { }
  // move constructor
  SpaceLayoutMsg(SpaceLayoutMsg&& other) : layouts_(std::move(other.layouts_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << layouts_.size(); for (auto& v:layouts_) { v.ToString(os); }
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    std::size_t layouts__i; *is >> layouts__i; for (std::size_t i = 0; i!= layouts__i ; i++) { LayoutMsg tmp; tmp.FromString(is);; layouts_.push_back(std::move(tmp)); }
    return true;
  }

  // getter and setter
  
  inline const GWVector<LayoutMsg>& layouts() const { return layouts_; }
  inline GWVector<LayoutMsg>& mutable_layouts() { return layouts_; }

  inline void set_layouts(const GWVector<LayoutMsg>& tmp) { layouts_ = tmp; }
  inline void set_layouts(GWVector<LayoutMsg>&& tmp) { layouts_ = std::move(tmp); }

  inline void add_layouts(LayoutMsg&& tmp) { layouts_.emplace_back(std::move(tmp)); }
  inline void add_layouts(const LayoutMsg& tmp) { layouts_.emplace_back(tmp); }
  inline std::size_t layouts_size() { return layouts_.size(); }

};

class AllCellCreatedMsg {
 private:
  // member
  uint64_t space_id_;
  GWString tag_;
  GWString meta_data_;
   
 public:
  AllCellCreatedMsg() = default;
  AllCellCreatedMsg(const AllCellCreatedMsg&) = default;
  AllCellCreatedMsg& operator=(const AllCellCreatedMsg&) = default;
  // constructor
  AllCellCreatedMsg(uint64_t space_id, GWString tag, GWString meta_data) : space_id_(space_id), tag_(tag), meta_data_(meta_data) { }
  // move constructor
  AllCellCreatedMsg(AllCellCreatedMsg&& other) : space_id_(std::move(other.space_id_)), tag_(std::move(other.tag_)), meta_data_(std::move(other.meta_data_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << space_id_;
    *os << tag_;
    *os << meta_data_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> space_id_;
    *is >> tag_;
    *is >> meta_data_;
    return true;
  }

  // getter and setter
  
  inline uint64_t space_id() const { return space_id_; }
  inline uint64_t& mutable_space_id() { return space_id_; }
  inline const GWString& tag() const { return tag_; }
  inline GWString& mutable_tag() { return tag_; }
  inline const GWString& meta_data() const { return meta_data_; }
  inline GWString& mutable_meta_data() { return meta_data_; }

  inline void set_space_id(const uint64_t& tmp) { space_id_ = tmp; }
  inline void set_space_id(uint64_t&& tmp) { space_id_ = std::move(tmp); }
  inline void set_tag(const GWString& tmp) { tag_ = tmp; }
  inline void set_tag(GWString&& tmp) { tag_ = std::move(tmp); }
  inline void set_meta_data(const GWString& tmp) { meta_data_ = tmp; }
  inline void set_meta_data(GWString&& tmp) { meta_data_ = std::move(tmp); }



};

class CellReportMsg {
 private:
  // member
  uint64_t space_id_;
  uint32_t num_entities_;
  uint32_t num_reals_;
  Rect entity_rect_;
  Rect tile_rect_;
  uint32_t num_avatars_;
   
 public:
  CellReportMsg() = default;
  CellReportMsg(const CellReportMsg&) = default;
  CellReportMsg& operator=(const CellReportMsg&) = default;
  // constructor
  CellReportMsg(uint64_t space_id, uint32_t num_entities, uint32_t num_reals, Rect entity_rect, Rect tile_rect, uint32_t num_avatars) : space_id_(space_id), num_entities_(num_entities), num_reals_(num_reals), entity_rect_(entity_rect), tile_rect_(tile_rect), num_avatars_(num_avatars) { }
  // move constructor
  CellReportMsg(CellReportMsg&& other) : space_id_(std::move(other.space_id_)), num_entities_(std::move(other.num_entities_)), num_reals_(std::move(other.num_reals_)), entity_rect_(std::move(other.entity_rect_)), tile_rect_(std::move(other.tile_rect_)), num_avatars_(std::move(other.num_avatars_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << space_id_;
    *os << num_entities_;
    *os << num_reals_;
    *os << entity_rect_;
    *os << tile_rect_;
    *os << num_avatars_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> space_id_;
    *is >> num_entities_;
    *is >> num_reals_;
    *is >> entity_rect_;
    *is >> tile_rect_;
    *is >> num_avatars_;
    return true;
  }

  // getter and setter
  
  inline uint64_t space_id() const { return space_id_; }
  inline uint64_t& mutable_space_id() { return space_id_; }
  inline uint32_t num_entities() const { return num_entities_; }
  inline uint32_t& mutable_num_entities() { return num_entities_; }
  inline uint32_t num_reals() const { return num_reals_; }
  inline uint32_t& mutable_num_reals() { return num_reals_; }
  inline const Rect& entity_rect() const { return entity_rect_; }
  inline Rect& mutable_entity_rect() { return entity_rect_; }
  inline const Rect& tile_rect() const { return tile_rect_; }
  inline Rect& mutable_tile_rect() { return tile_rect_; }
  inline uint32_t num_avatars() const { return num_avatars_; }
  inline uint32_t& mutable_num_avatars() { return num_avatars_; }

  inline void set_space_id(const uint64_t& tmp) { space_id_ = tmp; }
  inline void set_space_id(uint64_t&& tmp) { space_id_ = std::move(tmp); }
  inline void set_num_entities(const uint32_t& tmp) { num_entities_ = tmp; }
  inline void set_num_entities(uint32_t&& tmp) { num_entities_ = std::move(tmp); }
  inline void set_num_reals(const uint32_t& tmp) { num_reals_ = tmp; }
  inline void set_num_reals(uint32_t&& tmp) { num_reals_ = std::move(tmp); }
  inline void set_entity_rect(const Rect& tmp) { entity_rect_ = tmp; }
  inline void set_entity_rect(Rect&& tmp) { entity_rect_ = std::move(tmp); }
  inline void set_tile_rect(const Rect& tmp) { tile_rect_ = tmp; }
  inline void set_tile_rect(Rect&& tmp) { tile_rect_ = std::move(tmp); }
  inline void set_num_avatars(const uint32_t& tmp) { num_avatars_ = tmp; }
  inline void set_num_avatars(uint32_t&& tmp) { num_avatars_ = std::move(tmp); }


};

class ReportLoadMsg {
 private:
  // member
  float load_;
  uint32_t memsize_;
  uint32_t num_entities_;
  uint32_t num_reals_;
  GWVector<CellReportMsg> cell_report_;
  uint32_t num_avatars_;
   
 public:
  ReportLoadMsg() = default;
  ReportLoadMsg(const ReportLoadMsg&) = default;
  ReportLoadMsg& operator=(const ReportLoadMsg&) = default;
  // constructor
  ReportLoadMsg(float load, uint32_t memsize, uint32_t num_entities, uint32_t num_reals, GWVector<CellReportMsg> cell_report, uint32_t num_avatars) : load_(load), memsize_(memsize), num_entities_(num_entities), num_reals_(num_reals), cell_report_(cell_report), num_avatars_(num_avatars) { }
  // move constructor
  ReportLoadMsg(ReportLoadMsg&& other) : load_(std::move(other.load_)), memsize_(std::move(other.memsize_)), num_entities_(std::move(other.num_entities_)), num_reals_(std::move(other.num_reals_)), cell_report_(std::move(other.cell_report_)), num_avatars_(std::move(other.num_avatars_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << load_;
    *os << memsize_;
    *os << num_entities_;
    *os << num_reals_;
    *os << cell_report_.size(); for (auto& v:cell_report_) { v.ToString(os); }
    *os << num_avatars_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> load_;
    *is >> memsize_;
    *is >> num_entities_;
    *is >> num_reals_;
    std::size_t cell_report__i; *is >> cell_report__i; for (std::size_t i = 0; i!= cell_report__i ; i++) { CellReportMsg tmp; tmp.FromString(is);; cell_report_.push_back(std::move(tmp)); }
    *is >> num_avatars_;
    return true;
  }

  // getter and setter
  
  inline float load() const { return load_; }
  inline float& mutable_load() { return load_; }
  inline uint32_t memsize() const { return memsize_; }
  inline uint32_t& mutable_memsize() { return memsize_; }
  inline uint32_t num_entities() const { return num_entities_; }
  inline uint32_t& mutable_num_entities() { return num_entities_; }
  inline uint32_t num_reals() const { return num_reals_; }
  inline uint32_t& mutable_num_reals() { return num_reals_; }
  inline const GWVector<CellReportMsg>& cell_report() const { return cell_report_; }
  inline GWVector<CellReportMsg>& mutable_cell_report() { return cell_report_; }
  inline uint32_t num_avatars() const { return num_avatars_; }
  inline uint32_t& mutable_num_avatars() { return num_avatars_; }

  inline void set_load(const float& tmp) { load_ = tmp; }
  inline void set_load(float&& tmp) { load_ = std::move(tmp); }
  inline void set_memsize(const uint32_t& tmp) { memsize_ = tmp; }
  inline void set_memsize(uint32_t&& tmp) { memsize_ = std::move(tmp); }
  inline void set_num_entities(const uint32_t& tmp) { num_entities_ = tmp; }
  inline void set_num_entities(uint32_t&& tmp) { num_entities_ = std::move(tmp); }
  inline void set_num_reals(const uint32_t& tmp) { num_reals_ = tmp; }
  inline void set_num_reals(uint32_t&& tmp) { num_reals_ = std::move(tmp); }
  inline void set_cell_report(const GWVector<CellReportMsg>& tmp) { cell_report_ = tmp; }
  inline void set_cell_report(GWVector<CellReportMsg>&& tmp) { cell_report_ = std::move(tmp); }
  inline void set_num_avatars(const uint32_t& tmp) { num_avatars_ = tmp; }
  inline void set_num_avatars(uint32_t&& tmp) { num_avatars_ = std::move(tmp); }

  inline void add_cell_report(CellReportMsg&& tmp) { cell_report_.emplace_back(std::move(tmp)); }
  inline void add_cell_report(const CellReportMsg& tmp) { cell_report_.emplace_back(tmp); }
  inline std::size_t cell_report_size() { return cell_report_.size(); }

};

class CreateSpaceMsg {
 private:
  // member
  GWString name_;
  GWString tag_;
  uint32_t min_cell_num_;
  uint32_t max_cell_num_;
  Rect rect_;
  GWString layout_data_;
  GWString map_name_;
  bool is_static_;
  GWString meta_data_;
  GWVector<uint64_t> app_ids_;
   
 public:
  CreateSpaceMsg() = default;
  CreateSpaceMsg(const CreateSpaceMsg&) = default;
  CreateSpaceMsg& operator=(const CreateSpaceMsg&) = default;
  // constructor
  CreateSpaceMsg(GWString name, GWString tag, uint32_t min_cell_num, uint32_t max_cell_num, Rect rect, GWString layout_data, GWString map_name, bool is_static, GWString meta_data, GWVector<uint64_t> app_ids) : name_(name), tag_(tag), min_cell_num_(min_cell_num), max_cell_num_(max_cell_num), rect_(rect), layout_data_(layout_data), map_name_(map_name), is_static_(is_static), meta_data_(meta_data), app_ids_(app_ids) { }
  // move constructor
  CreateSpaceMsg(CreateSpaceMsg&& other) : name_(std::move(other.name_)), tag_(std::move(other.tag_)), min_cell_num_(std::move(other.min_cell_num_)), max_cell_num_(std::move(other.max_cell_num_)), rect_(std::move(other.rect_)), layout_data_(std::move(other.layout_data_)), map_name_(std::move(other.map_name_)), is_static_(std::move(other.is_static_)), meta_data_(std::move(other.meta_data_)), app_ids_(std::move(other.app_ids_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << name_;
    *os << tag_;
    *os << min_cell_num_;
    *os << max_cell_num_;
    *os << rect_;
    *os << layout_data_;
    *os << map_name_;
    *os << is_static_;
    *os << meta_data_;
    *os << app_ids_.size(); for (auto& v : app_ids_) { *os << v; }
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> name_;
    *is >> tag_;
    *is >> min_cell_num_;
    *is >> max_cell_num_;
    *is >> rect_;
    *is >> layout_data_;
    *is >> map_name_;
    *is >> is_static_;
    *is >> meta_data_;
    std::size_t app_ids__i; *is >> app_ids__i; for (std::size_t i = 0; i != app_ids__i; i++) { uint64_t tmp; *is >> tmp; app_ids_.push_back(std::move(tmp)); }
    return true;
  }

  // getter and setter
  
  inline const GWString& name() const { return name_; }
  inline GWString& mutable_name() { return name_; }
  inline const GWString& tag() const { return tag_; }
  inline GWString& mutable_tag() { return tag_; }
  inline uint32_t min_cell_num() const { return min_cell_num_; }
  inline uint32_t& mutable_min_cell_num() { return min_cell_num_; }
  inline uint32_t max_cell_num() const { return max_cell_num_; }
  inline uint32_t& mutable_max_cell_num() { return max_cell_num_; }
  inline const Rect& rect() const { return rect_; }
  inline Rect& mutable_rect() { return rect_; }
  inline const GWString& layout_data() const { return layout_data_; }
  inline GWString& mutable_layout_data() { return layout_data_; }
  inline const GWString& map_name() const { return map_name_; }
  inline GWString& mutable_map_name() { return map_name_; }
  inline bool is_static() const { return is_static_; }
  inline bool& mutable_is_static() { return is_static_; }
  inline const GWString& meta_data() const { return meta_data_; }
  inline GWString& mutable_meta_data() { return meta_data_; }
  inline const GWVector<uint64_t>& app_ids() const { return app_ids_; }
  inline GWVector<uint64_t>& mutable_app_ids() { return app_ids_; }

  inline void set_name(const GWString& tmp) { name_ = tmp; }
  inline void set_name(GWString&& tmp) { name_ = std::move(tmp); }
  inline void set_tag(const GWString& tmp) { tag_ = tmp; }
  inline void set_tag(GWString&& tmp) { tag_ = std::move(tmp); }
  inline void set_min_cell_num(const uint32_t& tmp) { min_cell_num_ = tmp; }
  inline void set_min_cell_num(uint32_t&& tmp) { min_cell_num_ = std::move(tmp); }
  inline void set_max_cell_num(const uint32_t& tmp) { max_cell_num_ = tmp; }
  inline void set_max_cell_num(uint32_t&& tmp) { max_cell_num_ = std::move(tmp); }
  inline void set_rect(const Rect& tmp) { rect_ = tmp; }
  inline void set_rect(Rect&& tmp) { rect_ = std::move(tmp); }
  inline void set_layout_data(const GWString& tmp) { layout_data_ = tmp; }
  inline void set_layout_data(GWString&& tmp) { layout_data_ = std::move(tmp); }
  inline void set_map_name(const GWString& tmp) { map_name_ = tmp; }
  inline void set_map_name(GWString&& tmp) { map_name_ = std::move(tmp); }
  inline void set_is_static(const bool& tmp) { is_static_ = tmp; }
  inline void set_is_static(bool&& tmp) { is_static_ = std::move(tmp); }
  inline void set_meta_data(const GWString& tmp) { meta_data_ = tmp; }
  inline void set_meta_data(GWString&& tmp) { meta_data_ = std::move(tmp); }
  inline void add_app_ids(uint64_t&& tmp) { app_ids_.emplace_back(std::move(tmp)); }
  inline void add_app_ids(const uint64_t& tmp) { app_ids_.emplace_back(tmp); }


};

class CellStateChangeInfo {
 private:
  // member
  uint64_t space_id_;
  uint64_t gapp_id_;
  uint8_t old_state_;
  uint8_t new_state_;
  uint32_t index_;
   
 public:
  CellStateChangeInfo() = default;
  CellStateChangeInfo(const CellStateChangeInfo&) = default;
  CellStateChangeInfo& operator=(const CellStateChangeInfo&) = default;
  // constructor
  CellStateChangeInfo(uint64_t space_id, uint64_t gapp_id, uint8_t old_state, uint8_t new_state, uint32_t index) : space_id_(space_id), gapp_id_(gapp_id), old_state_(old_state), new_state_(new_state), index_(index) { }
  // move constructor
  CellStateChangeInfo(CellStateChangeInfo&& other) : space_id_(std::move(other.space_id_)), gapp_id_(std::move(other.gapp_id_)), old_state_(std::move(other.old_state_)), new_state_(std::move(other.new_state_)), index_(std::move(other.index_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << space_id_;
    *os << gapp_id_;
    *os << old_state_;
    *os << new_state_;
    *os << index_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> space_id_;
    *is >> gapp_id_;
    *is >> old_state_;
    *is >> new_state_;
    *is >> index_;
    return true;
  }

  // getter and setter
  
  inline uint64_t space_id() const { return space_id_; }
  inline uint64_t& mutable_space_id() { return space_id_; }
  inline uint64_t gapp_id() const { return gapp_id_; }
  inline uint64_t& mutable_gapp_id() { return gapp_id_; }
  inline uint8_t old_state() const { return old_state_; }
  inline uint8_t& mutable_old_state() { return old_state_; }
  inline uint8_t new_state() const { return new_state_; }
  inline uint8_t& mutable_new_state() { return new_state_; }
  inline uint32_t index() const { return index_; }
  inline uint32_t& mutable_index() { return index_; }

  inline void set_space_id(const uint64_t& tmp) { space_id_ = tmp; }
  inline void set_space_id(uint64_t&& tmp) { space_id_ = std::move(tmp); }
  inline void set_gapp_id(const uint64_t& tmp) { gapp_id_ = tmp; }
  inline void set_gapp_id(uint64_t&& tmp) { gapp_id_ = std::move(tmp); }
  inline void set_old_state(const uint8_t& tmp) { old_state_ = tmp; }
  inline void set_old_state(uint8_t&& tmp) { old_state_ = std::move(tmp); }
  inline void set_new_state(const uint8_t& tmp) { new_state_ = tmp; }
  inline void set_new_state(uint8_t&& tmp) { new_state_ = std::move(tmp); }
  inline void set_index(const uint32_t& tmp) { index_ = tmp; }
  inline void set_index(uint32_t&& tmp) { index_ = std::move(tmp); }


};

class CellStateChangeRsp {
 private:
  // member
  uint32_t state_;
   
 public:
  CellStateChangeRsp() = default;
  CellStateChangeRsp(const CellStateChangeRsp&) = default;
  CellStateChangeRsp& operator=(const CellStateChangeRsp&) = default;
  // constructor
  CellStateChangeRsp(uint32_t state) : state_(state) { }
  // move constructor
  CellStateChangeRsp(CellStateChangeRsp&& other) : state_(std::move(other.state_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << state_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> state_;
    return true;
  }

  // getter and setter
  
  inline uint32_t state() const { return state_; }
  inline uint32_t& mutable_state() { return state_; }

  inline void set_state(const uint32_t& tmp) { state_ = tmp; }
  inline void set_state(uint32_t&& tmp) { state_ = std::move(tmp); }


};

class ClientInfo {
 private:
  // member
  uint64_t session_id_;
   
 public:
  ClientInfo() = default;
  ClientInfo(const ClientInfo&) = default;
  ClientInfo& operator=(const ClientInfo&) = default;
  // constructor
  ClientInfo(uint64_t session_id) : session_id_(session_id) { }
  // move constructor
  ClientInfo(ClientInfo&& other) : session_id_(std::move(other.session_id_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << session_id_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> session_id_;
    return true;
  }

  // getter and setter
  
  inline uint64_t session_id() const { return session_id_; }
  inline uint64_t& mutable_session_id() { return session_id_; }

  inline void set_session_id(const uint64_t& tmp) { session_id_ = tmp; }
  inline void set_session_id(uint64_t&& tmp) { session_id_ = std::move(tmp); }


};

class EntityInfo {
 private:
  // member
  uint64_t eid_;
  GWString entity_name_;
  uint64_t custom_id_;
  GWString content_;
  ClientInfo client_info_;
  uint64_t base_app_id_;
  uint64_t base_eid_;
   
 public:
  EntityInfo() = default;
  EntityInfo(const EntityInfo&) = default;
  EntityInfo& operator=(const EntityInfo&) = default;
  // constructor
  EntityInfo(uint64_t eid, GWString entity_name, uint64_t custom_id, GWString content, ClientInfo client_info, uint64_t base_app_id, uint64_t base_eid) : eid_(eid), entity_name_(entity_name), custom_id_(custom_id), content_(content), client_info_(client_info), base_app_id_(base_app_id), base_eid_(base_eid) { }
  // move constructor
  EntityInfo(EntityInfo&& other) : eid_(std::move(other.eid_)), entity_name_(std::move(other.entity_name_)), custom_id_(std::move(other.custom_id_)), content_(std::move(other.content_)), client_info_(std::move(other.client_info_)), base_app_id_(std::move(other.base_app_id_)), base_eid_(std::move(other.base_eid_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << eid_;
    *os << entity_name_;
    *os << custom_id_;
    *os << content_;
    client_info_.ToString(os);
    *os << base_app_id_;
    *os << base_eid_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> eid_;
    *is >> entity_name_;
    *is >> custom_id_;
    *is >> content_;
    client_info_.FromString(is);
    *is >> base_app_id_;
    *is >> base_eid_;
    return true;
  }

  // getter and setter
  
  inline uint64_t eid() const { return eid_; }
  inline uint64_t& mutable_eid() { return eid_; }
  inline const GWString& entity_name() const { return entity_name_; }
  inline GWString& mutable_entity_name() { return entity_name_; }
  inline uint64_t custom_id() const { return custom_id_; }
  inline uint64_t& mutable_custom_id() { return custom_id_; }
  inline const GWString& content() const { return content_; }
  inline GWString& mutable_content() { return content_; }
  inline const ClientInfo& client_info() const { return client_info_; }
  inline ClientInfo& mutable_client_info() { return client_info_; }
  inline uint64_t base_app_id() const { return base_app_id_; }
  inline uint64_t& mutable_base_app_id() { return base_app_id_; }
  inline uint64_t base_eid() const { return base_eid_; }
  inline uint64_t& mutable_base_eid() { return base_eid_; }

  inline void set_eid(const uint64_t& tmp) { eid_ = tmp; }
  inline void set_eid(uint64_t&& tmp) { eid_ = std::move(tmp); }
  inline void set_entity_name(const GWString& tmp) { entity_name_ = tmp; }
  inline void set_entity_name(GWString&& tmp) { entity_name_ = std::move(tmp); }
  inline void set_custom_id(const uint64_t& tmp) { custom_id_ = tmp; }
  inline void set_custom_id(uint64_t&& tmp) { custom_id_ = std::move(tmp); }
  inline void set_content(const GWString& tmp) { content_ = tmp; }
  inline void set_content(GWString&& tmp) { content_ = std::move(tmp); }
  inline void set_client_info(const ClientInfo& tmp) { client_info_ = tmp; }
  inline void set_client_info(ClientInfo&& tmp) { client_info_ = std::move(tmp); }
  inline void set_base_app_id(const uint64_t& tmp) { base_app_id_ = tmp; }
  inline void set_base_app_id(uint64_t&& tmp) { base_app_id_ = std::move(tmp); }
  inline void set_base_eid(const uint64_t& tmp) { base_eid_ = tmp; }
  inline void set_base_eid(uint64_t&& tmp) { base_eid_ = std::move(tmp); }


};

class CallEntityMessage {
 private:
  // member
  uint64_t eid_;
  GWString method_name_;
  GWString content_;
   
 public:
  CallEntityMessage() = default;
  CallEntityMessage(const CallEntityMessage&) = default;
  CallEntityMessage& operator=(const CallEntityMessage&) = default;
  // constructor
  CallEntityMessage(uint64_t eid, GWString method_name, GWString content) : eid_(eid), method_name_(method_name), content_(content) { }
  // move constructor
  CallEntityMessage(CallEntityMessage&& other) : eid_(std::move(other.eid_)), method_name_(std::move(other.method_name_)), content_(std::move(other.content_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << eid_;
    *os << method_name_;
    *os << content_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> eid_;
    *is >> method_name_;
    *is >> content_;
    return true;
  }

  // getter and setter
  
  inline uint64_t eid() const { return eid_; }
  inline uint64_t& mutable_eid() { return eid_; }
  inline const GWString& method_name() const { return method_name_; }
  inline GWString& mutable_method_name() { return method_name_; }
  inline const GWString& content() const { return content_; }
  inline GWString& mutable_content() { return content_; }

  inline void set_eid(const uint64_t& tmp) { eid_ = tmp; }
  inline void set_eid(uint64_t&& tmp) { eid_ = std::move(tmp); }
  inline void set_method_name(const GWString& tmp) { method_name_ = tmp; }
  inline void set_method_name(GWString&& tmp) { method_name_ = std::move(tmp); }
  inline void set_content(const GWString& tmp) { content_ = tmp; }
  inline void set_content(GWString&& tmp) { content_ = std::move(tmp); }


};

class CreateEntityRequest {
 private:
  // member
  uint64_t space_id_;
  Vector3 position_;
  bool is_lua_;
  EntityInfo info_;
   
 public:
  CreateEntityRequest() = default;
  CreateEntityRequest(const CreateEntityRequest&) = default;
  CreateEntityRequest& operator=(const CreateEntityRequest&) = default;
  // constructor
  CreateEntityRequest(uint64_t space_id, Vector3 position, bool is_lua, EntityInfo info) : space_id_(space_id), position_(position), is_lua_(is_lua), info_(info) { }
  // move constructor
  CreateEntityRequest(CreateEntityRequest&& other) : space_id_(std::move(other.space_id_)), position_(std::move(other.position_)), is_lua_(std::move(other.is_lua_)), info_(std::move(other.info_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << space_id_;
    position_.ToString(os);
    *os << is_lua_;
    info_.ToString(os);
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> space_id_;
    position_.FromString(is);
    *is >> is_lua_;
    info_.FromString(is);
    return true;
  }

  // getter and setter
  
  inline uint64_t space_id() const { return space_id_; }
  inline uint64_t& mutable_space_id() { return space_id_; }
  inline const Vector3& position() const { return position_; }
  inline Vector3& mutable_position() { return position_; }
  inline bool is_lua() const { return is_lua_; }
  inline bool& mutable_is_lua() { return is_lua_; }
  inline const EntityInfo& info() const { return info_; }
  inline EntityInfo& mutable_info() { return info_; }

  inline void set_space_id(const uint64_t& tmp) { space_id_ = tmp; }
  inline void set_space_id(uint64_t&& tmp) { space_id_ = std::move(tmp); }
  inline void set_position(const Vector3& tmp) { position_ = tmp; }
  inline void set_position(Vector3&& tmp) { position_ = std::move(tmp); }
  inline void set_is_lua(const bool& tmp) { is_lua_ = tmp; }
  inline void set_is_lua(bool&& tmp) { is_lua_ = std::move(tmp); }
  inline void set_info(const EntityInfo& tmp) { info_ = tmp; }
  inline void set_info(EntityInfo&& tmp) { info_ = std::move(tmp); }


};

class CreateBaseEntityRequest {
 private:
  // member
  uint64_t dbid_;
  bool is_lua_;
  EntityInfo info_;
   
 public:
  CreateBaseEntityRequest() = default;
  CreateBaseEntityRequest(const CreateBaseEntityRequest&) = default;
  CreateBaseEntityRequest& operator=(const CreateBaseEntityRequest&) = default;
  // constructor
  CreateBaseEntityRequest(uint64_t dbid, bool is_lua, EntityInfo info) : dbid_(dbid), is_lua_(is_lua), info_(info) { }
  // move constructor
  CreateBaseEntityRequest(CreateBaseEntityRequest&& other) : dbid_(std::move(other.dbid_)), is_lua_(std::move(other.is_lua_)), info_(std::move(other.info_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << dbid_;
    *os << is_lua_;
    info_.ToString(os);
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> dbid_;
    *is >> is_lua_;
    info_.FromString(is);
    return true;
  }

  // getter and setter
  
  inline uint64_t dbid() const { return dbid_; }
  inline uint64_t& mutable_dbid() { return dbid_; }
  inline bool is_lua() const { return is_lua_; }
  inline bool& mutable_is_lua() { return is_lua_; }
  inline const EntityInfo& info() const { return info_; }
  inline EntityInfo& mutable_info() { return info_; }

  inline void set_dbid(const uint64_t& tmp) { dbid_ = tmp; }
  inline void set_dbid(uint64_t&& tmp) { dbid_ = std::move(tmp); }
  inline void set_is_lua(const bool& tmp) { is_lua_ = tmp; }
  inline void set_is_lua(bool&& tmp) { is_lua_ = std::move(tmp); }
  inline void set_info(const EntityInfo& tmp) { info_ = tmp; }
  inline void set_info(EntityInfo&& tmp) { info_ = std::move(tmp); }


};

class CreateEntityMgrReply {
 private:
  // member
  uint64_t gapp_id_;
  bool is_success_;
   
 public:
  CreateEntityMgrReply() = default;
  CreateEntityMgrReply(const CreateEntityMgrReply&) = default;
  CreateEntityMgrReply& operator=(const CreateEntityMgrReply&) = default;
  // constructor
  CreateEntityMgrReply(uint64_t gapp_id, bool is_success) : gapp_id_(gapp_id), is_success_(is_success) { }
  // move constructor
  CreateEntityMgrReply(CreateEntityMgrReply&& other) : gapp_id_(std::move(other.gapp_id_)), is_success_(std::move(other.is_success_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << gapp_id_;
    *os << is_success_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> gapp_id_;
    *is >> is_success_;
    return true;
  }

  // getter and setter
  
  inline uint64_t gapp_id() const { return gapp_id_; }
  inline uint64_t& mutable_gapp_id() { return gapp_id_; }
  inline bool is_success() const { return is_success_; }
  inline bool& mutable_is_success() { return is_success_; }

  inline void set_gapp_id(const uint64_t& tmp) { gapp_id_ = tmp; }
  inline void set_gapp_id(uint64_t&& tmp) { gapp_id_ = std::move(tmp); }
  inline void set_is_success(const bool& tmp) { is_success_ = tmp; }
  inline void set_is_success(bool&& tmp) { is_success_ = std::move(tmp); }


};

class CreateEntityClientReply {
 private:
  // member
  bool is_success_;
   
 public:
  CreateEntityClientReply() = default;
  CreateEntityClientReply(const CreateEntityClientReply&) = default;
  CreateEntityClientReply& operator=(const CreateEntityClientReply&) = default;
  // constructor
  CreateEntityClientReply(bool is_success) : is_success_(is_success) { }
  // move constructor
  CreateEntityClientReply(CreateEntityClientReply&& other) : is_success_(std::move(other.is_success_)) { }
  // serialize
  bool ToString(IMemoryOStream* os) const {
    *os << is_success_;
    return true;
  }
  bool FromString(IMemoryIStream* is) {
    *is >> is_success_;
    return true;
  }

  // getter and setter
  
  inline bool is_success() const { return is_success_; }
  inline bool& mutable_is_success() { return is_success_; }

  inline void set_is_success(const bool& tmp) { is_success_ = tmp; }
  inline void set_is_success(bool&& tmp) { is_success_ = std::move(tmp); }


};


}
