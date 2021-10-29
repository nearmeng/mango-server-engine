#pragma once
#include "great_world_base/cellmgr/balance/region_node.h"
#include "great_world_base/common/xml/pugixml.hpp"
#include "space_layout_reader.h"

/** xml 结构
 * space 根节点
 *      -weight space的宽
 *      -height space的高
 *      Node    space中包含的节点
 *          -Node 递归的node
 *              ...
 */

GW_CELL_MGR_BEGIN

class SpaceLayoutXMLReader : public ISpaceLayoutReader {
  GWCLASS_DECL(SpaceLayoutXMLReader)
 private:

  bool ParseEvenlyDivided(IMemoryOStream& os, gw_pugi::xml_node& node, Rect& rootRect);
  bool ParseFixedDivided(IMemoryOStream& os, gw_pugi::xml_node& node, Rect& rootRect);
  bool ParseStripGroup(IMemoryOStream& os, gw_pugi::xml_node& node, Rect& rootRect);
  bool ParseCell(IMemoryOStream& os, gw_pugi::xml_node& node, Rect& rootRect);

  bool Parse(IMemoryOStream& os, gw_pugi::xml_node& node, Rect& rootRect);

 public:
  std::shared_ptr<SpaceLayoutData> LoadSpaceLayout(const GWString& name) override;
};

GW_CELL_MGR_END