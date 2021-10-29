#pragma once

#include "great_world_base/common/common.h"
#include "great_world_base/common/rect.h"
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/string_slice.hpp"

namespace gw {

class IMemoryOStream {
 public:
  virtual ~IMemoryOStream() {}

  virtual IMemoryOStream& operator<<(const bool& val) = 0;
  virtual IMemoryOStream& operator<<(const StringSlice& val) = 0;
  virtual IMemoryOStream& operator<<(const GWString& val) = 0;
  virtual IMemoryOStream& operator<<(const std::string& val) = 0;
  virtual IMemoryOStream& operator<<(const gw::Rect& val) = 0;
  virtual IMemoryOStream& operator<<(const uint16_t& val) = 0;
  virtual IMemoryOStream& operator<<(const uint8_t& val) = 0;
  virtual IMemoryOStream& operator<<(const uint64_t& val) = 0;
  virtual IMemoryOStream& operator<<(const uint32_t& val) = 0;
  virtual IMemoryOStream& operator<<(const float& val) = 0;
  virtual IMemoryOStream& WriteData(const StringSlice& val) = 0;

  virtual GWString str() const = 0;
  virtual StringSlice view() = 0;
  virtual void reset() = 0;
  virtual size_t size() const = 0;
};

class IMemoryIStream {
 public:
  virtual ~IMemoryIStream() {}

  virtual IMemoryIStream& operator>>(bool& val) = 0;
  virtual IMemoryIStream& operator>>(gw::Rect& val) = 0;
  virtual IMemoryIStream& operator>>(uint16_t& val) = 0;
  virtual IMemoryIStream& operator>>(uint8_t& val) = 0;
  virtual IMemoryIStream& operator>>(uint32_t& val) = 0;
  virtual IMemoryIStream& operator>>(uint64_t& val) = 0;
  virtual IMemoryIStream& operator>>(GWString& val) = 0;
  virtual IMemoryIStream& operator>>(std::string& val) = 0;
  virtual IMemoryIStream& operator>>(float& val) = 0;

  virtual bool empty() = 0;
};

}