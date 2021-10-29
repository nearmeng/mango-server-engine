#pragma once
#include "great_world_base/common/reader/reader.h"

namespace gw {
class IConfig {
 public:
  virtual ~IConfig() {
    if (reader_) delete reader_;
  }

  bool Init(IReader* reader) {
    if (reader_) delete reader_;
    reader_ = reader;
    return reload();
  }

  void SetReader(IReader* reader) { reader_ = reader; };

  virtual bool reload() = 0;

 protected:
  IReader* reader_ = nullptr;
};

}  // namespace gw