#pragma once
#include <list>
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "great_world_base/common/shm/shm_allocator.h"
#ifndef _WIN32
#include "great_world_base/common/shm/shm_basic_string.h"
#endif

namespace gw {

template <typename T>
using GWVector = std::vector<T, ShmAllocator<T>>;

template <typename T>
using GWList = std::list<T, ShmAllocator<T>>;

template <typename K, typename V, typename L = std::less<K>>
using GWMap = std::map<K, V, L, ShmAllocator<std::pair<const K, V>>>;

template <typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>>
using GWHashMap = std::unordered_map<K, V, H, E, ShmAllocator<std::pair<const K, V>>>;

template <typename K, typename L = std::less<K>>
using GWSet = std::set<K, L, ShmAllocator<K>>;

template <typename K, typename H = std::hash<K>, typename E = std::equal_to<K>>
using GWHashSet = std::unordered_set<K, H, E, ShmAllocator<K>>;

template <typename K>
using GWDeque = std::deque<K, ShmAllocator<K>>;

template <typename K, typename COMP>
using ShmPriorityQueue = std::priority_queue<K, GWVector<K>, COMP>;

template <typename K>
using GWQueue = std::queue<K, GWDeque<K>>;

#if _WIN32 || __GNUC__ >= 5
using GWString = std::basic_string<char, std::char_traits<char>, ShmAllocator<char>>;
#else
// codes from gcc 5.2.0 libstdc++ basic_string
// previous gcc cow string cannot support share memory
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=16612
using GWString = gw::ShmBasicString<char>;
#endif

#define GW_TOSHMSTR(orig_str) gw::GWString(orig_str.data(), orig_str.size())
#define GW_TOSTDSTR(shm_str) std::string(shm_str.data(), shm_str.size())
}

// 和g6的冲突，只能这么搞一下
#ifdef GW_LOCAL
namespace std {
template <>
struct hash<gw::GWString> {
  std::size_t operator()(const gw::GWString& str) const {
    std::size_t hash = 5381;
    for (auto one_char : str) {
      hash = ((hash << 5) + hash) + std::size_t(one_char); /* hash * 33 + c */
    }
    return hash;
  }
};
}  // namespace std
#endif