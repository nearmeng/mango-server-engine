#pragma once
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "shm_allocator.h"

namespace shm {
template <typename T>
using basic_string = std::basic_string<T, std::char_traits<T>, ShmAllocator<T>>;
using string = basic_string<char>;
using wstring = basic_string<wchar_t>;

template <typename T>
using vector = std::vector<T, ShmAllocator<T>>;

template <typename T>
using deque = std::deque<T, ShmAllocator<T>>;

template <typename T>
using forward_list = std::forward_list<T, ShmAllocator<T>>;

template <typename T>
using list = std::list<T, ShmAllocator<T>>;

template <typename Key, typename Compare = std::less<Key>>
using set = std::set<Key, Compare, ShmAllocator<Key>>;

template <typename Key, typename Compare = std::less<Key>>
using multiset = std::multiset<Key, Compare, ShmAllocator<Key>>;

template <typename Key, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
using unordered_set = std::unordered_set<Key, Hash, KeyEqual, ShmAllocator<Key>>;

template <typename Key, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
using unordered_multiset = std::unordered_multiset<Key, Hash, KeyEqual, ShmAllocator<Key>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using map = std::map<Key, T, Compare, ShmAllocator<std::pair<const Key, T>>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using multimap = std::multimap<Key, T, Compare, ShmAllocator<std::pair<const Key, T>>>;

template <typename Key, typename T, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>>
using unordered_map =
    std::unordered_map<Key, T, Hash, KeyEqual, ShmAllocator<std::pair<const Key, T>>>;

template <typename Key, typename T, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>>
using unordered_multimap =
    std::unordered_multimap<Key, T, Hash, KeyEqual, ShmAllocator<std::pair<const Key, T>>>;
}  // namespace shm

namespace std {
template <>
struct hash<shm::string> {
  std::size_t operator()(shm::string const& s) const noexcept {
    return hash<std::string>()(std::string(s.c_str(), s.size()));
  }
};
template <>
struct hash<shm::wstring> {
  std::size_t operator()(shm::wstring const& s) const noexcept {
    return hash<std::wstring>()(std::wstring(s.c_str(), s.size()));
  }
};
};  // namespace std
