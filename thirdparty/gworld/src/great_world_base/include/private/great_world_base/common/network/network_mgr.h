#pragma once
#include "great_world_base/common/singleton_template.h"
#include "great_world_base/common/network/message.h"
#include "great_world_base/common/memory_stream_private.h"

namespace gw {

class NetworkSender {
 public:
  virtual void SendMessageToCell(uint64_t gapp_id, const StringSlice& ss) = 0;
  virtual void SendMessageToCellMgr(const StringSlice& ss) = 0;
};

class NetworkMgr {
 public:
  static void SendMessageToCell(uint64_t gapp_id, NetworkOStream& os);
  static void SendMessageToCellMgr(NetworkOStream& os);
  static NetworkSender* sender_; 
};

#define SEND_MSG_TO_CELL(gapp_id, os) gw::NetworkMgr::SendMessageToCell(gapp_id, os)
#define SEND_MSG_TO_CELLMGR(os) gw::NetworkMgr::SendMessageToCellMgr(os)
#define DISPATCH_MESSAGE(msg_id, func) case msg_id: func(gapp_id, msg_id, is); break;

}