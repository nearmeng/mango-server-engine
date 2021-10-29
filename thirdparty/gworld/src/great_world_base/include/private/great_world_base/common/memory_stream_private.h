#pragma once
#include <functional>

#include "great_world_base/common/common.h"
#include "great_world_base/common/rect.h"
#include "great_world_base/common/string_slice.hpp"
#include "great_world_base/common/varint_stream.hpp"

namespace gw {

using MemoryOStream = VarintMemoryOStream;
using MemoryIStream = VarintMemoryIStream;

class UnarchiveError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class ArchiveError : public std::runtime_error {
};

class NetworkOStream : public VarintMemoryOStream {
 public:
  NetworkOStream(NetworkMessage msg_id) { varint_mem_os_ << static_cast<uint32_t>(msg_id); }
};

class NetworkIStream : public VarintMemoryIStream {
 public:
  NetworkIStream(const StringSlice& slice) : VarintMemoryIStream(slice) {
    uint32_t msg_id_uint;
    varint_mem_is_ >> msg_id_uint;
    msg_id_ = static_cast<NetworkMessage>(msg_id_uint);
  }
  inline NetworkMessage msg_id() { return msg_id_; }
 private:
  NetworkMessage msg_id_;
};

}  // namespace gw