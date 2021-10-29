#include "great_world_base/cellmgr/spacelayout/space_layout_xml_reader.h"

#include "great_world_base/common/string_converter.h"
#include "great_world_base/common/memory_stream_private.h"

GW_CELL_MGR_BEGIN

GWCLASS_DECL_IMPLEMENT(SpaceLayoutXMLReader)

bool SpaceLayoutXMLReader::ParseEvenlyDivided(IMemoryOStream& os, gw_pugi::xml_node& node, Rect& rect) {
  uint16_t rows;
  uint16_t cols;
  TRY_STR_TO_UINT16(node.child_value("rows"), rows, false);
  TRY_STR_TO_UINT16(node.child_value("cols"), cols, false);
  os << rows << cols << static_cast<uint8_t>(0);

  std::unordered_map<uint16_t, std::unordered_map<uint16_t, gw_pugi::xml_node>> specialNodes;
  uint16_t rowId;
  uint16_t colId;
  for (auto subNode : node.children("Node")) {
    TRY_STR_TO_UINT16(subNode.child_value("rowid"), rowId, false);
    TRY_STR_TO_UINT16(subNode.child_value("colid"), colId, false);
    specialNodes[rowId][colId] = subNode;
  }

  float w = rect.width() / cols;
  float h = rect.height() / rows;
  for (uint16_t i = 0; i < rows; ++i) {
    for (uint16_t j = 0; j < cols; ++j) {
      Rect subRect(
          rect.minx() + j * w,
          rect.miny() + i * h,
          rect.minx() + j * w + w,
          rect.minx() + j * w + h);
      if (specialNodes.find(i) != specialNodes.end() && specialNodes[i].find(j) != specialNodes[i].end()) {
        Parse(os, specialNodes[i][j], subRect);
      } else {
        os << static_cast<uint8_t>(RegionNodeType::kCell);
      }
    }
  }
  return true;
}

bool SpaceLayoutXMLReader::ParseFixedDivided(IMemoryOStream& os, gw_pugi::xml_node& node, Rect& rect) {
  float width;
  float height;
  TRY_STR_TO_FLOAT(node.child_value("width"), width, -1);
  TRY_STR_TO_FLOAT(node.child_value("height"), height, -1);
  if (!MathUtility::IsEqual(width, rect.width()) || !MathUtility::IsEqual(height, rect.height())) {
    GW_LOG_ERROR("ParseFixedDivided faild(error area):%f,%f,%f,%f", width, height, rect.width(), rect.height());
    return false;
  }
  float minx;
  float miny;
  float maxx;
  float maxy;
  for (auto subNode : node.children("Node")) {
    TRY_STR_TO_FLOAT(subNode.child_value("minx"), minx, false);
    TRY_STR_TO_FLOAT(subNode.child_value("miny"), miny, false);
    TRY_STR_TO_FLOAT(subNode.child_value("maxx"), maxx, false);
    TRY_STR_TO_FLOAT(subNode.child_value("maxy"), maxy, false);
    Rect subRect(minx + rect.minx(), miny + rect.miny(), maxx + rect.minx(), maxy + rect.miny());
    os << subRect;
    GW_LOG_DEBUG("ParseFixedDivided:%f,%f,%f,%f", subRect.minx(), subRect.miny(), subRect.maxx(), subRect.maxy());
    if (!Parse(os, subNode, subRect)) {
      return false;
    }
  }
  return true;
}

bool SpaceLayoutXMLReader::ParseStripGroup(IMemoryOStream& os, gw_pugi::xml_node& node, Rect& rect) {
  uint16_t min_strips;
  uint16_t max_strips;
  uint16_t min_cells_per_strip;
  uint16_t max_cells_per_strip;

  TRY_STR_TO_UINT16(node.child_value("min_strips"), min_strips, false);
  TRY_STR_TO_UINT16(node.child_value("max_strips"), max_strips, false);
  TRY_STR_TO_UINT16(node.child_value("min_cells_per_strip"), min_cells_per_strip, false);
  TRY_STR_TO_UINT16(node.child_value("max_cells_per_strip"), max_cells_per_strip, false);

  os << min_strips << max_strips << min_cells_per_strip << max_cells_per_strip;
  return true;
}

bool SpaceLayoutXMLReader::ParseCell(IMemoryOStream& os, gw_pugi::xml_node& node, Rect& rect) {
  uint16_t min_parallel_num;

  if (strcmp(node.child_value("min_parallel_num"), "") != 0) {
    TRY_STR_TO_UINT16(node.child_value("min_parallel_num"), min_parallel_num, false);
  } else {
    min_parallel_num = 0;
  }

  os << min_parallel_num;
  return true;
}

// rect 是这个node的
bool SpaceLayoutXMLReader::Parse(IMemoryOStream& os, gw_pugi::xml_node& node, Rect& rect) {
  uint8_t type;
  TRY_STR_TO_UINT8(node.child_value("type"), type, false);
  os << type;
  RegionNodeType rnType = static_cast<RegionNodeType>(type);
  if (rnType == RegionNodeType::kCell) {
    return ParseCell(os, node, rect);
  } else if (rnType == RegionNodeType::kEvenlyDivided) {
    return ParseEvenlyDivided(os, node, rect);
  } else if (rnType == RegionNodeType::kFixedDivided) {
    return ParseFixedDivided(os, node, rect);
  } else if (rnType == RegionNodeType::kStripGroup) {
    return ParseStripGroup(os, node, rect);
  } else if (rnType == RegionNodeType::kFixedDividedByCell) {
    return ParseFixedDivided(os, node, rect);
  } else {
    return false;
  }
}

std::shared_ptr<SpaceLayoutData> SpaceLayoutXMLReader::LoadSpaceLayout(const GWString& name) {
  const GWString final_path = name;
  gw_pugi::xml_document cfg_doc;
  gw_pugi::xml_parse_result parse_result = cfg_doc.load_file(final_path.c_str());
  if (!parse_result) {
    GW_LOG_ERROR("SpaceLayoutXMLReader load_file(%s) fail: %s,%p", final_path.c_str(), parse_result.description(), this);
    return nullptr;
  }

  gw_pugi::xml_node spaceNode = cfg_doc.child("Space");
  float width;
  float height;
  TRY_STR_TO_FLOAT(spaceNode.child_value("width"), width, nullptr);
  TRY_STR_TO_FLOAT(spaceNode.child_value("height"), height, nullptr);
  Rect rootRect(0, 0, width, height);
  gw_pugi::xml_node node = spaceNode.child("Node");
  MemoryOStream os;
  if (!Parse(os, node, rootRect)) {
    return nullptr;
  } else {
    GWString s = os.str();
    auto ret = std::make_shared<SpaceLayoutData>();
    ret->data_ = std::make_shared<const std::vector<char>>(s.begin(), s.end());
    ret->width_ = width;
    ret->height_ = height;
    return ret;
  }
}

GW_CELL_MGR_END