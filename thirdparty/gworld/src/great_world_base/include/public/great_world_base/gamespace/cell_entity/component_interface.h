#pragma once
#include "great_world_base/common/common.h"
#include "great_world_base/common/memory_stream.h"

REALGHOST_NS_BEGIN

class IComponent {
 public:
 
  virtual bool RequiredByGhost() const { return false; }

  virtual bool WriteToMemoryStreamGhost(IMemoryOStream& os) { return true; }

  virtual bool ReadFromMemoryStreamGhost(IMemoryIStream& is) { return true; }

  virtual bool WriteToMemoryStreamReal(IMemoryOStream& os) { return true; }

  virtual bool ReadFromMemoryStreamReal(IMemoryIStream& is) { return true; }

};

REALGHOST_NS_END