#pragma once
#include <assert.h>
#include <emmintrin.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>

using ctrl_t = signed char;
using h2_t = uint8_t;
using h1_t = uint64_t;

// 数组长度是 2 的倍数

// #define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#ifndef LOG
#define LOG(fmt, ...) ;
#endif

enum Ctrl : ctrl_t {
  kEmpty = -128,   // 0b10000000
  kDeleted = -2,   // 0b11111110
  kSentinel = -1,  // 0b11111111
};

inline size_t HashSeed(const ctrl_t* ctrl) {
  return reinterpret_cast<uintptr_t>(ctrl) >> 12;
}
inline size_t H1(size_t hash, const ctrl_t* ctrl) {
  return (hash >> 7) ^ HashSeed(ctrl);
}
inline ctrl_t H2(size_t hash) { return hash & 0x7F; }

template <size_t Width>
class probe_seq {
 public:
  probe_seq(size_t hash, size_t size) {
    assert((size & (size - 1)) == 0);
    size_ = size;
    mask_ = size - 1;
    offset_ = hash & mask_;
  }
  inline size_t offset() const { return offset_; }
  inline size_t offset(size_t i) const { return (offset_ + i) & mask_; }

  void next() {
    offset_ += Width;
    if (offset_ > mask_) {
      index_ += (size_ + Width - offset_);
      offset_ = 0;
    } else {
      index_ += Width;
    }
  }
  // 0-based probe index. The i-th probe in the probe sequence.
  inline size_t index() const { return index_; }

 private:
  size_t size_;
  size_t offset_;
  size_t index_ = 0;
  size_t mask_;
};

// 每次读的一组数据
struct Group {
  static constexpr size_t kWidth = 16;
  Group(ctrl_t* ctrl) {
    ctrl_ = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ctrl));
  }
  __m128i ctrl_;

  // 低16位是匹配情况，高16位是0
  inline uint32_t Match(h2_t hash) {
    auto match = _mm_set1_epi8(hash);
    return _mm_movemask_epi8(_mm_cmpeq_epi8(match, ctrl_));
  }

  inline uint32_t MatchEmptyOrDeleted() {
    auto special = _mm_set1_epi8(kSentinel);
    return _mm_movemask_epi8(_mm_cmpgt_epi8(special, ctrl_));
  }

  inline uint32_t MatchFull() {
    auto special = _mm_set1_epi8(kSentinel);
    return _mm_movemask_epi8(_mm_cmpgt_epi8(ctrl_, special));
  }

  inline uint32_t MatchNotEmptyOrDeleted() {
    auto deleted = _mm_set1_epi8(kDeleted);
    return _mm_movemask_epi8(_mm_cmpgt_epi8(ctrl_, deleted));
  }
};

template <class K, class V, size_t SIZE = 2>
class SIMDHashTable {
 public:
  typedef std::pair<const K, V> Node;
  typedef std::pair<const K, V>& Reference;
  typedef std::pair<const K, V>* Pointer;

  SIMDHashTable() {
    capacity_ = SIZE;
    raw_data_ = (Pointer)malloc((capacity_ + 1) * sizeof(Node));  // 多一个哨兵
    ctrl_ = new ctrl_t[capacity_ + Group::kWidth];
    memset(ctrl_, Ctrl::kEmpty, capacity_);
    memset(ctrl_ + capacity_, Ctrl::kSentinel, Group::kWidth);  // 这里多出来的设置成哨兵
    valid_count_ = 0;
    reset_can_insert_size();
  }

  ~SIMDHashTable() {
    auto seq = probe_seq<Group::kWidth>(0, capacity_);
    while (true) {
      Group g{ctrl_ + seq.offset()};
      uint32_t match_ret = g.MatchFull();
      LOG("~match_ret:%d\n", match_ret);
      while (match_ret > 0) {
        auto offset = __builtin_ctz(match_ret);
        offset = seq.offset(offset);
        Pointer p = (raw_data_ + offset);
        p->first.~K();
        p->second.~V();
        match_ret &= (match_ret - 1);
      }
      seq.next();
      if (seq.index() >= capacity_) break;
    }
    delete[] ctrl_;
    free(raw_data_);
  }

 public:
  class iterator {
    friend class SIMDHashTable;

   public:
    iterator() {
    }
    iterator& operator++() {
      if (*ctrl_ == Ctrl::kSentinel) return *this;
      ++ctrl_;
      ++raw_data_;
      skip_empty_deleted();
      return *this;
    }
    iterator& operator++(int) {
      if (*ctrl_ == Ctrl::kSentinel) return *this;
      ++ctrl_;
      ++raw_data_;
      skip_empty_deleted();
      return *this;
    }
    inline Reference operator*() {
      return *raw_data_;
    }
    inline Pointer operator->() {
      return raw_data_;
    }
    inline bool operator==(iterator&& other) {
      return other.ctrl_ == ctrl_;
    }
    inline bool operator!=(iterator&& other) {
      return other.ctrl_ != ctrl_;
    }
    inline bool operator==(iterator& other) {
      return other.ctrl_ == ctrl_;
    }
    inline bool operator!=(iterator& other) {
      return other.ctrl_ != ctrl_;
    }

   private:
    iterator(Pointer raw_data, ctrl_t* ctrl) : raw_data_(raw_data), ctrl_(ctrl) {
      skip_empty_deleted();
    }

    iterator(Pointer raw_data, ctrl_t* ctrl, bool flag) : raw_data_(raw_data), ctrl_(ctrl) {
    }

    void skip_empty_deleted() {
      if (*ctrl_ == Ctrl::kSentinel) return;
      while (1) {
        Group g{ctrl_};
        auto match_ret = g.MatchNotEmptyOrDeleted();
        if (match_ret > 0) {
          auto offset = __builtin_ctz(match_ret);
          raw_data_ += offset;
          ctrl_ += offset;
          break;
        } else {
          raw_data_ += Group::kWidth;
          ctrl_ += Group::kWidth;
        }
      }
    }

   private:
    Pointer raw_data_;
    ctrl_t* ctrl_;
  };

  inline iterator begin() const {
    return iterator(raw_data_, ctrl_);
  }
  inline iterator end() const {
    return iterator(raw_data_ + capacity_, ctrl_ + capacity_, true);
  }

 public:
  inline std::pair<Pointer, bool> emplace(const K k, V& v) {
    return emplace_impl(k, v);
  }
  inline std::pair<Pointer, bool> emplace(const K k, V&& v) {
    return emplace_impl(k, v);
  }

  inline std::size_t erase(const K&& k) {
    return erase_impl(k);
  }
  inline std::size_t erase(const K& k) {
    return erase_impl(k);
  }

  iterator find(const K& k) {
    std::size_t hash = kHash(k);
    h1_t h1 = H1(hash, ctrl_);
    h2_t h2 = H2(hash);
    auto offset = find(k, h1, h2);
    return iterator(raw_data_ + offset, ctrl_ + offset, true);
  }

  inline iterator find(const K&& k) {
    return find(k);
  }

  inline std::size_t size() const {
    return valid_count_;
  }

  inline double valid_rate() const {
    return 1.0 * valid_count_ / capacity_;
  }

  inline float load_factor() const {
    return 1.0 * valid_count_ / capacity_;
  }

 private:
  std::size_t erase_impl(const K& k) {
    std::size_t hash = kHash(k);
    h1_t h1 = H1(hash, ctrl_);
    h2_t h2 = H2(hash);
    auto k_offset = find(k, h1, h2);
    if (k_offset == invalid_offset())
      return 0;
    else {
      ctrl_[k_offset] = Ctrl::kDeleted;
      raw_data_[k_offset].first.~K();
      raw_data_[k_offset].second.~V();
      --valid_count_;
      return 1;
    }
  }

  std::pair<Pointer, bool> emplace_impl(const K& k, V& v) {
    LOG("enter emplace_impl");
    std::size_t hash = kHash(k);
    h1_t h1 = H1(hash, ctrl_);
    h2_t h2 = H2(hash);
    auto k_offset = find(k, h1, h2);
    if (k_offset != invalid_offset()) {
      return {raw_data_ + k_offset, false};
    }
    std::size_t offset = find_first_non_full(hash);
    if (ctrl_[offset] != Ctrl::kDeleted && can_insert_size_ <= 0) {  // 如果要占用新的 empty，并且预留的数量已经没有了就尝试做一下 rehash
      rehash_and_grow_if_necessary();
      offset = find_first_non_full(hash);
    }
    can_insert_size_ -= (ctrl_[offset] != Ctrl::kDeleted);  // 只有这个数据是empty的时候才减一
    ctrl_[offset] = h2;
    new (raw_data_ + offset) Node(k, v);
    LOG("insert to %d\n", offset);
    ++valid_count_;
    return {raw_data_ + offset, true};
  }

  std::size_t find_first_non_full(std::size_t hash) const {
    h1_t h1 = H1(hash, ctrl_);
    auto seq = probe_seq<Group::kWidth>(h1, capacity_);
    while (true) {
      Group g{ctrl_ + seq.offset()};
      LOG("find_first_non_full:%llu,%d\n", h1, seq.offset());
      uint32_t match_ret = g.MatchEmptyOrDeleted();
      if (match_ret > 0) {
        return seq.offset(__builtin_ctz(match_ret));
      } else {
        seq.next();
        if (seq.index() > capacity_ + Group::kWidth - 1) break;
      }
    }
    assert(false);
    return 0;
  }

  std::size_t find(const K& k, h1_t h1, h2_t h2) const {
    auto seq = probe_seq<Group::kWidth>(h1, capacity_);
    while (true) {
      Group g{ctrl_ + seq.offset()};
      uint32_t match_ret = g.Match(h2);
      LOG("match_ret:%d,%llu\n", match_ret, k);
      while (match_ret > 0) {
        auto offset = seq.offset(__builtin_ctz(match_ret));
        Pointer p = (raw_data_ + offset);
        if (p->first == k) {
          LOG("return:%p\n", p);
          return offset;
        }
        match_ret &= (match_ret - 1);
      }
      if (g.Match(static_cast<h2_t>(Ctrl::kEmpty)) > 0) {
        break;
      }
      seq.next();
      if (seq.index() > capacity_) break;
    }
    return invalid_offset();
  }

  inline void reset_can_insert_size() {
    can_insert_size_ = capacity_ - std::max(capacity_ / 7, static_cast<std::size_t>(1)) - valid_count_;
  }

  void rehash_and_grow_if_necessary() {
    if (valid_count_ < (capacity_ - capacity_ / 7) / 2) {
      drop_deletes_without_resize();
    } else {
      rehash(capacity_ << 1);
    }
  }

  inline void drop_deletes_without_resize() {
    // 先写个简单的，不过费点内存
    rehash(capacity_);
  }

  void rehash(std::size_t new_size) {
    LOG("rehash:%d\n", new_size);
    assert((new_size & (new_size - 1)) == 0);
    ctrl_t* new_ctrl = new ctrl_t[new_size + Group::kWidth];
    memset(new_ctrl, Ctrl::kEmpty, new_size);
    memset(new_ctrl + new_size, Ctrl::kSentinel, Group::kWidth);
    Pointer new_raw_data = (Pointer)malloc((new_size + 1) * sizeof(Node));
    for (std::size_t i = 0; i < capacity_; i++) {
      if (ctrl_[i] < 0) continue;
      Pointer data = raw_data_ + i;
      std::size_t hash = kHash(data->first);
      h1_t h1 = H1(hash, new_ctrl);
      h2_t h2 = H2(hash);
      auto seq = probe_seq<Group::kWidth>(h1, new_size);
      while (true) {
        Group g{new_ctrl + seq.offset()};
        uint32_t match_ret = g.Match(Ctrl::kEmpty);
        if (match_ret > 0) {
          auto offset = __builtin_ctz(match_ret);
          offset = seq.offset(offset);
          new_ctrl[offset] = h2;
          new (new_raw_data + offset) Node(*data);
          LOG("insert to %d\n", offset);
          break;
        } else {
          seq.next();
          assert(seq.index() <= new_size);
        }
      }
    }
    delete[] ctrl_;
    free(raw_data_);
    ctrl_ = new_ctrl;
    raw_data_ = new_raw_data;
    capacity_ = new_size;
    reset_can_insert_size();
  }

  inline std::size_t invalid_offset() const { return capacity_; }

  inline std::size_t kHash(const K&& k) {
    return kHash(k);
  }

  inline std::size_t kHash(const K& k) {
    std::size_t m = (std::hash<K>{}(k) + 13) * kMul;
    return static_cast<uint64_t>(m ^ (m >> (sizeof(m) * 8 / 2)));
  }

 private:
  // node 数据，考虑到pairs是pod类型的，把里面换成裸的数据，然后使用 inplacement new 来设置数据
  // 这里数据使用裸的数据和使用指针数据，经过实际测量，插入效率裸的是指针的4倍多，查找效率是指针的1倍
  Pointer raw_data_;
  ctrl_t* ctrl_;             // meta data
  std::size_t capacity_;     // 数组的大小
  std::size_t valid_count_;  // 有效的数据数量
  int32_t can_insert_size_;  // 还能插入多少数据

  static constexpr uint64_t kMul =
      sizeof(size_t) == 4 ? uint64_t{0xcc9e2d51}
                          : uint64_t{0x9ddfea08eb382d69};
};