#pragma once

#include <string>
#include "great_world_base/common/common.h"
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/memory_stream.h"

REALGHOST_NS_BEGIN

using SpaceId = uint64_t;
using EntityId = uint64_t;
using GameAppId = uint64_t;
using EntityTypeId = uint16_t;
using ComponentTypeId = uint16_t;

enum class MailBoxType : uint8_t { BASE = 0, CELL = 1 };

class EntityMailBox {
 public:
  explicit EntityMailBox(MailBoxType type = MailBoxType::BASE, GameAppId gapp_id = 0, EntityId entity_id = 0,
                         EntityTypeId type_id_ = 0, ComponentTypeId component_type_id = 0);
  MailBoxType type() const { return type_; }
  GameAppId gapp_id() const { return gapp_id_; }
  EntityId entity_id() const { return entity_id_; }
  EntityTypeId type_id() const { return type_id_; }
  ComponentTypeId component_type_id() const { return component_type_id_; }
  void set_component_type_id(ComponentTypeId type_id) { component_type_id_ = type_id; }

  void FromStream(IMemoryIStream& is);
  void IntoStream(IMemoryOStream& os);

 private:
  MailBoxType type_;
  GameAppId gapp_id_;
  EntityId entity_id_;
  EntityTypeId type_id_;
  ComponentTypeId component_type_id_;
};

REALGHOST_NS_END
