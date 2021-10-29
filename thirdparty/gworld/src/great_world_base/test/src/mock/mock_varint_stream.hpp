#pragma once

#include <cstdint>
#include <cstring>
#include <map>
#include <type_traits>
#include <vector>

#include "great_world_base/common/string_slice.hpp"
#include "great_world_base/common/memory_stream.h"
#include "great_world_base/common/rect.h"
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/network/message.h"
#include "great_world_base/common/varint_stream.hpp"

namespace gw {
namespace mock {

class MockMemoryOStream : public VarintMemoryOStream {
};

class MockMemoryIStream : public VarintMemoryIStream {
 public:
  ~MockMemoryIStream() {}
  MockMemoryIStream(const StringSlice& slice) : VarintMemoryIStream(slice) {
  }
};

class MockNetworkIStream : public MockMemoryIStream {
 public:
  MockNetworkIStream(const StringSlice& slice) : MockMemoryIStream(slice) {
    uint32_t msg_id_uint;
    varint_mem_is_ >> msg_id_uint;
    msg_id_ = static_cast<NetworkMessage>(msg_id_uint);
  }
  inline NetworkMessage msg_id() { return msg_id_; }
 private:
  NetworkMessage msg_id_;
};

}  // namespace gw
}