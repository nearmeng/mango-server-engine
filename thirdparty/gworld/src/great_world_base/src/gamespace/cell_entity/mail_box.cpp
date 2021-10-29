#include "great_world_base/gamespace/cell_entity/mail_box.h"
#include "great_world_base/common/memory_stream_private.h"
REALGHOST_NS_BEGIN

EntityMailBox::EntityMailBox(MailBoxType type, GameAppId gapp_id, EntityId entity_id, EntityTypeId type_id,
                             ComponentTypeId component_type_id)
    : type_(type), gapp_id_(gapp_id), entity_id_(entity_id), type_id_(type_id), component_type_id_(component_type_id) {}

void EntityMailBox::FromStream(IMemoryIStream& is) {
  uint8_t type;
  is >> type;
  type_ = static_cast<MailBoxType>(type);
  is >> gapp_id_;
  is >> entity_id_;
  is >> type_id_;
  is >> component_type_id_;
}

void EntityMailBox::IntoStream(IMemoryOStream& os) {
  os << static_cast<uint8_t>(type_);
  os << gapp_id_;
  os << entity_id_;
  os << type_id_;
  os << component_type_id_;
}

REALGHOST_NS_END
