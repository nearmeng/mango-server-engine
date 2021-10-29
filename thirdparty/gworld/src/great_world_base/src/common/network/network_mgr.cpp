#include "great_world_base/common/network/network_mgr.h"
#include "great_world_base/common/memory_stream_private.h"

namespace gw {

void NetworkMgr::SendMessageToCell(uint64_t gapp_id, NetworkOStream& os) {
  sender_->SendMessageToCell(gapp_id, os.view());
}

void NetworkMgr::SendMessageToCellMgr(NetworkOStream& os) {
  sender_->SendMessageToCellMgr(os.view());
}

NetworkSender* NetworkMgr::sender_ = nullptr;

}  // namespace gw