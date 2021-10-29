#include "resume_layer.h"

bool ResumeLayerImpl::RegisterType(const char* type_name, std::size_t type_size,
                               const ResumeFunc& resume, const PostResumeFunc& post_resume) {
  if (data_array_) return false;

  ResumeType type;
  type.resume = resume;
  type.post_resume = post_resume;
  type.type_size = type_size;

  auto it = type_map_.emplace(type_name, type);
  if (!it.second) {
    assert(false);
    return false;
  }
  return true;
}

void ResumeLayerImpl::Clear() {
  type_map_.clear();
  data_array_ = nullptr;
}

bool ResumeLayerImpl::Init(ResumeLayerStorage* data_array) {
  if (data_array_) return false;
  data_array_ = data_array;

  data_array_->clear();
  data_array_->reserve(type_map_.size());

  for (auto& item : type_map_) {
    ResumePoolStorage data(item.first, item.second.type_size);
    data_array_->emplace_back(data);
    if (!item.second.pool.Bind(&data_array_->back())) return false;
  }
  return true;
}

bool ResumeLayerImpl::ResumeInit(ResumeLayerStorage* data_array) {
  if (data_array_) return false;
  data_array_ = data_array;

  if (data_array->size() != type_map_.size()) return false;

  for (auto& data : *data_array_) {
    auto it = type_map_.find(data.GetTypeName());
    if (it == type_map_.end()) return false;
    if (!data.IsValid(it->first.c_str(), it->second.type_size)) return false;

    if (!it->second.pool.Bind(&data)) return false;
  }

  for (auto& item : type_map_) {
    if (!item.second.pool.Resume(item.second.resume)) return false;
  }

  for (auto& item : type_map_) {
    if (!item.second.pool.PostResume(item.second.post_resume)) return false;
  }

  return true;
}

ResumePoolImpl* ResumeLayerImpl::Allocator(const char* type_name) {
  auto it = type_map_.find(type_name);
  if (it == type_map_.end()) return nullptr;
  return &it->second.pool;
}
