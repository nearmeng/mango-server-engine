#pragma once

#include <cstdint>
#include <cstring>
#include <map>
#include <type_traits>
#include <vector>

#include "great_world_base/common/string_slice.hpp"
#include "great_world_base/common/memory_stream.h"
#include "great_world_base/common/rect.h"
#include "great_world_base/common/shm/shm_stl.h"
#include "great_world_base/common/network/message.h"

#ifdef _MSC_VER
#include <intrin.h>

static inline int __builtin_ctz(unsigned x) {
  unsigned long ret;
  _BitScanForward(&ret, x);
  return (int)ret;
}

static inline int __builtin_ctzll(unsigned long long x) {
  unsigned long ret;
  _BitScanForward64(&ret, x);
  return (int)ret;
}

static inline int __builtin_ctzl(unsigned long x) {
  return sizeof(x) == 8 ? __builtin_ctzll(x) : __builtin_ctz((uint32_t)x);
}

static inline int __builtin_clz(unsigned x) {
  // unsigned long ret;
  //_BitScanReverse(&ret, x);
  // return (int)(31 ^ ret);
  return (int)__lzcnt(x);
}

static inline int __builtin_clzll(unsigned long long x) {
  // unsigned long ret;
  //_BitScanReverse64(&ret, x);
  // return (int)(63 ^ ret);
  return (int)__lzcnt64(x);
}

static inline int __builtin_clzl(unsigned long x) {
  return sizeof(x) == 8 ? __builtin_clzll(x) : __builtin_clz((uint32_t)x);
}

#ifdef __cplusplus
static inline int __builtin_ctzl(unsigned long long x) { return __builtin_ctzll(x); }

static inline int __builtin_clzl(unsigned long long x) { return __builtin_clzll(x); }
#endif
#endif

namespace gw {

class Bits {
 public:
  static uint32_t Log2FloorNonZero(uint32_t n) {
    return 31 ^ static_cast<uint32_t>(__builtin_clz(n));
  }

  static uint32_t Log2FloorNonZero64(uint64_t n) {
    return 63 ^ static_cast<uint32_t>(__builtin_clzll(n));
  }
};

template <typename To, typename From>
inline To bit_cast(const From& from) {
  static_assert(sizeof(From) == sizeof(To), "bit_cast_with_different_size");
  To dest;
  memcpy(&dest, &from, sizeof(dest));
  return dest;
}

#ifdef _WIN32
#define VARINT_STREAM_LIKELY(exp) exp
#define VARINT_STREAM_UNLIKELY(exp) exp
#else
#define VARINT_STREAM_LIKELY(exp) __builtin_expect(!!(exp), 1)
#define VARINT_STREAM_UNLIKELY(exp) __builtin_expect(!!(exp), 0)
#endif  // _WIN32




class WireFormat {
 public:
  static uint8_t* WriteUInt32ToArray(uint32_t value, uint8_t* target);
  static uint8_t* WriteUInt64ToArray(uint64_t value, uint8_t* target);
  static uint8_t* WriteSInt32ToArray(int32_t value, uint8_t* target);
  static uint8_t* WriteSInt64ToArray(int64_t value, uint8_t* target);
  static uint8_t* WriteFloatToArray(float value, uint8_t* target);
  static uint8_t* WriteDoubleToArray(double value, uint8_t* target);
  static uint8_t* WriteBoolToArray(bool value, uint8_t* target);
  static uint8_t* WriteStringWithSizeToArray(const GWString& str, uint8_t* target);
  static uint8_t* WriteStringToArray(const GWString& str, uint8_t* target);
  static uint8_t* WriteRawToArray(const void* data, int size, uint8_t* target);

  static std::pair<bool, uint8_t*> ReadUInt32FromArray(const uint8_t* buffer, uint32_t& value);
  static std::pair<bool, uint8_t*> ReadUInt64FromArray(const uint8_t* buffer, uint64_t& value);
  static std::pair<bool, uint8_t*> ReadSInt32FromArray(const uint8_t* buffer, int32_t& value);
  static std::pair<bool, uint8_t*> ReadSInt64FromArray(const uint8_t* buffer, int64_t& value);
  static std::pair<bool, uint8_t*> ReadFloatFromArray(const uint8_t* buffer, float& value);
  static std::pair<bool, uint8_t*> ReadDoubleFromArray(const uint8_t* buffer, double& value);
  static std::pair<bool, uint8_t*> ReadBoolFromArray(const uint8_t* buffer, bool& value);
  static std::pair<bool, uint8_t*> ReadStringFromArray(const uint8_t* buffer, GWString& value);
  static std::pair<bool, uint8_t*> ReadStringFromArray(const uint8_t* buffer, std::string& value);

 private:
  static size_t VarintSize32(uint32_t value);
  static size_t VarintSize64(uint64_t value);
  static uint32_t EncodeFloat(float value);
  static float DecodeFloat(uint32_t value);
  static uint64_t EncodeDouble(double value);
  static double DecodeDouble(uint64_t value);
  static uint32_t ZigZagEncode32(int32_t n);
  static int32_t ZigZagDecode32(uint32_t n);
  static uint64_t ZigZagEncode64(int64_t n);
  static int64_t ZigZagDecode64(uint64_t n);

  static const uint8_t* ReadVarint32(const uint8_t* buffer, uint32_t& value);
  static const uint8_t* ReadVarint64(const uint8_t* buffer, uint64_t& value);

  template <typename T>
  static uint8_t* WriteVarint(T value, uint8_t* ptr) {
    static_assert(std::is_unsigned<T>::value, "Varint serialization must be unsigned");
    if (value < 0x80) {
      ptr[0] = static_cast<uint8_t>(value);
      return ptr + 1;
    }
    ptr[0] = static_cast<uint8_t>(value | 0x80);
    value >>= 7;
    if (value < 0x80) {
      ptr[1] = static_cast<uint8_t>(value);
      return ptr + 2;
    }
    ptr++;
    do {
      *ptr = static_cast<uint8_t>(value | 0x80);
      value >>= 7;
      ++ptr;
    } while (VARINT_STREAM_UNLIKELY(value >= 0x80));
    *ptr++ = static_cast<uint8_t>(value);
    return ptr;
  }

  static uint8_t* WriteSize(uint32_t value, uint8_t* ptr) {
    while (VARINT_STREAM_UNLIKELY(value >= 0x80)) {
      *ptr = static_cast<uint8_t>(value | 0x80);
      value >>= 7;
      ++ptr;
    }
    *ptr++ = static_cast<uint8_t>(value);
    return ptr;
  }

  template <size_t N>
  static const uint8_t* DecodeVarint64KnownSize(const uint8_t* buffer, uint64_t* value) {
    static_assert(N > 0, "N should > 0");
    uint64_t result = static_cast<uint64_t>(buffer[N - 1]) << (7 * (N - 1));
    for (int i = 0, offset = 0; i < N - 1; i++, offset += 7) {
      result += static_cast<uint64_t>(buffer[i] - 0x80) << offset;
    }
    *value = result;
    return buffer + N;
  }
};

// 参考：
// https://github.com/protocolbuffers/protobuf/blob/master/src/google/protobuf/io/coded_stream.h

inline size_t WireFormat::VarintSize32(uint32_t value) {
  uint32_t log2value = Bits::Log2FloorNonZero(value | 0x1);
  return static_cast<size_t>((log2value * 9 + 73) / 64);
}

inline size_t WireFormat::VarintSize64(uint64_t value) {
  uint32_t log2value = Bits::Log2FloorNonZero64(value | 0x1);
  return static_cast<size_t>((log2value * 9 + 73) / 64);
}

inline uint32_t WireFormat::EncodeFloat(float value) {
  return bit_cast<uint32_t>(value);
}

inline float WireFormat::DecodeFloat(uint32_t value) {
  return bit_cast<float>(value);
}

inline uint64_t WireFormat::EncodeDouble(double value) {
  return bit_cast<uint64_t>(value);
}

inline double WireFormat::DecodeDouble(uint64_t value) {
  return bit_cast<double>(value);
}

inline uint32_t WireFormat::ZigZagEncode32(int32_t n) {
  // Note:  the right-shift must be arithmetic
  // Note:  left shift must be unsigned because of overflow
  return (static_cast<uint32_t>(n) << 1) ^ static_cast<uint32_t>(n >> 31);
}

inline int32_t WireFormat::ZigZagDecode32(uint32_t n) {
  // Note:  Using unsigned types prevent undefined behavior
  return static_cast<int32_t>((n >> 1) ^ (~(n & 1) + 1));
}

inline uint64_t WireFormat::ZigZagEncode64(int64_t n) {
  // Note:  the right-shift must be arithmetic
  // Note:  left shift must be unsigned because of overflow
  return (static_cast<uint64_t>(n) << 1) ^ static_cast<uint64_t>(n >> 63);
}

inline int64_t WireFormat::ZigZagDecode64(uint64_t n) {
  // Note:  Using unsigned types prevent undefined behavior
  return static_cast<int64_t>((n >> 1) ^ (~(n & 1) + 1));
}

inline const uint8_t* WireFormat::ReadVarint32(const uint8_t* buffer, uint32_t& value) {
  if (*buffer < 0x80) {
    value = *buffer;
    buffer++;
    return buffer;
  }
  const uint8_t* ptr = buffer;
  uint32_t b;
  uint32_t result = *buffer - 0x80;
  ++ptr;
  b = *(ptr++);
  result += b << 7;
  if (!(b & 0x80)) goto done;
  result -= 0x80 << 7;
  b = *(ptr++);
  result += b << 14;
  if (!(b & 0x80)) goto done;
  result -= 0x80 << 14;
  b = *(ptr++);
  result += b << 21;
  if (!(b & 0x80)) goto done;
  result -= 0x80 << 21;
  b = *(ptr++);
  result += b << 28;
  if (!(b & 0x80)) goto done;

  value = 0;
  return nullptr;
done:
  value = result;
  return ptr;
}

inline const uint8_t* WireFormat::ReadVarint64(const uint8_t* buffer, uint64_t& value) {
  if (*buffer < 0x80) {
    value = *buffer;
    buffer++;
    return buffer;
  }
  const uint8_t* next = nullptr;
  uint64_t result;
  if (buffer[1] < 128) {
    next = DecodeVarint64KnownSize<2>(buffer, &result);
  } else if (buffer[2] < 128) {
    next = DecodeVarint64KnownSize<3>(buffer, &result);
  } else if (buffer[3] < 128) {
    next = DecodeVarint64KnownSize<4>(buffer, &result);
  } else if (buffer[4] < 128) {
    next = DecodeVarint64KnownSize<5>(buffer, &result);
  } else if (buffer[5] < 128) {
    next = DecodeVarint64KnownSize<6>(buffer, &result);
  } else if (buffer[6] < 128) {
    next = DecodeVarint64KnownSize<7>(buffer, &result);
  } else if (buffer[7] < 128) {
    next = DecodeVarint64KnownSize<8>(buffer, &result);
  } else if (buffer[8] < 128) {
    next = DecodeVarint64KnownSize<9>(buffer, &result);
  } else if (buffer[9] < 128) {
    next = DecodeVarint64KnownSize<10>(buffer, &result);
  } else {
    value = 0;
    return nullptr;  // 数据已经错乱。
  }
  value = result;
  return next;
}

inline uint8_t* WireFormat::WriteUInt32ToArray(uint32_t value, uint8_t* target) {
  return WriteVarint(value, target);
}

inline uint8_t* WireFormat::WriteUInt64ToArray(uint64_t value, uint8_t* target) {
  return WriteVarint(value, target);
}

inline uint8_t* WireFormat::WriteSInt32ToArray(int32_t value, uint8_t* target) {
  return WriteVarint(ZigZagEncode32(value), target);
}

inline uint8_t* WireFormat::WriteSInt64ToArray(int64_t value, uint8_t* target) {
  return WriteVarint(ZigZagEncode64(value), target);
}

inline uint8_t* WireFormat::WriteFloatToArray(float value, uint8_t* target) {
  uint32_t temp = EncodeFloat(value);
  memcpy(target, &temp, sizeof(value));  // 先忽略大小端处理。
  return target + sizeof(value);
}

inline uint8_t* WireFormat::WriteDoubleToArray(double value, uint8_t* target) {
  uint64_t temp = EncodeDouble(value);
  memcpy(target, &temp, sizeof(value));
  return target + sizeof(value);
}

inline uint8_t* WireFormat::WriteBoolToArray(bool value, uint8_t* target) {
  if(value)
    return WriteVarint(1u, target);
  else 
    return WriteVarint(0u, target);}

inline uint8_t* WireFormat::WriteStringWithSizeToArray(const GWString& str, uint8_t* target) {
  target = WriteVarint(static_cast<uint32_t>(str.size()), target);
  return WriteStringToArray(str, target);
}

inline uint8_t* WireFormat::WriteStringToArray(const GWString& str, uint8_t* target) {
  return WriteRawToArray(str.data(), static_cast<int>(str.size()), target);
}

inline uint8_t* WireFormat::WriteRawToArray(const void* data, int size, uint8_t* target) {
  memcpy(target, data, size);
  return target + size;
}

inline std::pair<bool, uint8_t*> WireFormat::ReadUInt32FromArray(const uint8_t* buffer, uint32_t& value) {
  buffer = ReadVarint32(buffer, value);
  if (!buffer) {
    return std::make_pair<bool, uint8_t*>(false, nullptr);
  }
  return std::make_pair(true, const_cast<uint8_t*>(buffer));
}

inline std::pair<bool, uint8_t*> WireFormat::ReadUInt64FromArray(const uint8_t* buffer, uint64_t& value) {
  buffer = ReadVarint64(buffer, value);
  if (!buffer) {
    return std::make_pair<bool, uint8_t*>(false, nullptr);
  }
  return std::make_pair(true, const_cast<uint8_t*>(buffer));
}

inline std::pair<bool, uint8_t*> WireFormat::ReadSInt32FromArray(const uint8_t* buffer, int32_t& value) {
  uint32_t temp;
  buffer = ReadVarint32(buffer, temp);
  if (!buffer) {
    return std::make_pair<bool, uint8_t*>(false, nullptr);
  }
  value = ZigZagDecode32(temp);
  return std::make_pair(true, const_cast<uint8_t*>(buffer));
}

inline std::pair<bool, uint8_t*> WireFormat::ReadSInt64FromArray(const uint8_t* buffer, int64_t& value) {
  uint64_t temp;
  buffer = ReadVarint64(buffer, temp);
  if (!buffer) {
    return std::make_pair<bool, uint8_t*>(false, nullptr);
  }
  value = ZigZagDecode64(temp);
  return std::make_pair(true, const_cast<uint8_t*>(buffer));
}

inline std::pair<bool, uint8_t*> WireFormat::ReadFloatFromArray(const uint8_t* buffer, float& value) {
  uint32_t temp;
  memcpy(&temp, buffer, sizeof(temp));
  buffer += sizeof(temp);
  value = DecodeFloat(temp);
  return std::make_pair(true, const_cast<uint8_t*>(buffer));
}

inline std::pair<bool, uint8_t*> WireFormat::ReadDoubleFromArray(const uint8_t* buffer, double& value) {
  uint64_t temp;
  memcpy(&temp, buffer, sizeof(temp));
  buffer += sizeof(temp);
  value = DecodeDouble(temp);
  return std::make_pair(true, const_cast<uint8_t*>(buffer));
}

inline std::pair<bool, uint8_t*> WireFormat::ReadBoolFromArray(const uint8_t* buffer, bool& value) {
  uint64_t temp;
  buffer = ReadVarint64(buffer, temp);
  if (!buffer) {
    value = false;
    return std::make_pair<bool, uint8_t*>(false, nullptr);
  }
  value = temp != 0;
  return std::make_pair(true, const_cast<uint8_t*>(buffer));
}

inline std::pair<bool, uint8_t*> WireFormat::ReadStringFromArray(const uint8_t* buffer, GWString& str) {
  uint32_t length;
  buffer = ReadVarint32(buffer, length);
  if (buffer == nullptr) {
    return std::make_pair<bool, uint8_t*>(false, nullptr);
  }
  str.resize(length);
  if (length > 0) {
    memcpy(&*str.begin(), buffer, length);
  }
  return std::make_pair<bool, uint8_t*>(true, const_cast<uint8_t*>(buffer + length));
}

inline std::pair<bool, uint8_t*> WireFormat::ReadStringFromArray(const uint8_t* buffer, std::string& str) {
  uint32_t length;
  buffer = ReadVarint32(buffer, length);
  if (buffer == nullptr) {
    return std::make_pair<bool, uint8_t*>(false, nullptr);
  }
  str.resize(length);
  if (length > 0) {
    memcpy(&*str.begin(), buffer, length);
  }
  return std::make_pair<bool, uint8_t*>(true, const_cast<uint8_t*>(buffer + length));
}

class VarintOStream {
 public:
  static const size_t kInitialSize = 1024;
  static const size_t kMaxUintSize = 10;

  explicit VarintOStream(size_t initial_size = kInitialSize) : buffer_(initial_size), writer_index_(0) {}

  GWString str() const {
    auto v = view();
    return GWString(v.data(), v.size());
  }

  StringSlice view() const {
    return StringSlice(&*buffer_.begin(), writer_index_);
  }

  void reset() {
    writer_index_ = 0;
  }

  size_t size() const {
    return writer_index_;
  }

  VarintOStream& operator<<(Rect val) {
    *this << val.minx() << val.maxx() << val.miny() << val.maxy();
    return *this;
  }

  VarintOStream& operator<<(float val) {
    EnsureSpace(sizeof(float));
    uint8_t* start = &buffer_[writer_index_];
    uint8_t* end = WireFormat::WriteFloatToArray(val, start);
    writer_index_ += end - start;
    return *this;
  }

  VarintOStream& operator<<(double val) {
    EnsureSpace(sizeof(double));
    uint8_t* start = &buffer_[writer_index_];
    uint8_t* end = WireFormat::WriteDoubleToArray(val, start);
    writer_index_ += end - start;
    return *this;
  }

  VarintOStream& operator<<(bool val) {
    EnsureSpace(sizeof(bool));
    uint8_t* start = &buffer_[writer_index_];
    uint8_t* end = WireFormat::WriteBoolToArray(val, start);
    writer_index_ += end - start;
    return *this;
  }

  template <typename T, typename = typename std::enable_if<
                            std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
                            std::is_same<T, uint32_t>::value || std::is_same<T, int8_t>::value ||
                            std::is_same<T, int16_t>::value || std::is_same<T, int32_t>::value>::type>
  VarintOStream& operator<<(T val) {
    WriteImp(val, std::is_signed<T>());
    return *this;
  }

  VarintOStream& operator<<(uint64_t val) {
    EnsureSpace(kMaxUintSize);  // 空间超过实际可能使用，不影响。
    uint8_t* start = &buffer_[writer_index_];
    uint8_t* end = WireFormat::WriteUInt64ToArray(val, start);
    writer_index_ += end - start;
    return *this;
  }

  VarintOStream& operator<<(int64_t val) {
    EnsureSpace(kMaxUintSize);  // 空间超过实际可能使用，不影响。
    uint8_t* start = &buffer_[writer_index_];
    uint8_t* end = WireFormat::WriteSInt64ToArray(val, start);
    writer_index_ += end - start;
    return *this;
  }

  VarintOStream& operator<<(const GWString& val) {
    EnsureSpace(kMaxUintSize + val.size());
    uint8_t* start = &buffer_[writer_index_];
    uint8_t* end = WireFormat::WriteStringWithSizeToArray(val, start);
    writer_index_ += end - start;
    return *this;
  }

  VarintOStream& operator<<(const StringSlice& val) {
    EnsureSpace(kMaxUintSize + val.size());
    uint8_t* start = &buffer_[writer_index_];
    uint8_t* next = WireFormat::WriteUInt32ToArray(static_cast<uint32_t>(val.size()), start);
    next = WireFormat::WriteRawToArray(val.data(), val.size(), next);
    writer_index_ += next - start;
    return *this;
  }

  template <typename T>
  VarintOStream& operator<<(const std::vector<T>& vi) {
    *this << static_cast<uint32_t>(vi.size());
    for (auto& elt : vi) {
      *this << elt;
    }
    return *this;
  }

  template <typename T, typename U>
  VarintOStream& operator<<(const std::map<T, U>& mp) {
    *this << static_cast<uint32_t>(mp.size());
    for (auto& elt : mp) {
      *this << elt.first;
      *this << elt.second;
    }
    return *this;
  }

  VarintOStream& WriteData(const StringSlice& val) {
      EnsureSpace(val.size());
      uint8_t* start = &buffer_[writer_index_];
      uint8_t* next = WireFormat::WriteRawToArray(val.data(), val.size(), start);
      writer_index_ += next - start;
      return *this;
  }

 private:
  template <typename T>
  void WriteImp(T val, std::true_type) {
    EnsureSpace(kMaxUintSize);  // 空间超过实际可能使用，不影响。
    uint8_t* start = &buffer_[writer_index_];
    uint8_t* end = WireFormat::WriteSInt32ToArray(val, start);
    writer_index_ += end - start;
  }

  template <typename T>
  void WriteImp(T val, std::false_type) {
    EnsureSpace(kMaxUintSize);  // 空间超过实际可能使用，不影响。
    uint8_t* start = &buffer_[writer_index_];
    uint8_t* end = WireFormat::WriteUInt32ToArray(val, start);
    writer_index_ += end - start;
  }

  void EnsureSpace(size_t len) {
    if (writer_index_ + len >= buffer_.size()) {
      buffer_.resize(writer_index_ + len + kInitialSize);
    }
  }

  std::vector<uint8_t> buffer_;
  size_t writer_index_;
};

class VarintIStream {
 public:
  explicit VarintIStream(const StringSlice& slice)
      : slice_(slice), start_(reinterpret_cast<const uint8_t*>(slice_.data())), ptr_(start_) {}

  ~VarintIStream() {
    assert(empty());
  }

  bool empty() const {
    return static_cast<uint32_t>(ptr_ - start_) >= slice_.size();
  }

  void MoveLeftTo(GWVector<char>& left) {
    left.clear();
    if(empty()) 
      return;
    left = std::move(GWVector<char>(reinterpret_cast<const char*>(ptr_), slice_.data() + slice_.size()));
    ptr_ = start_ + slice_.size();
  }

  inline std::size_t LeftSize() const { 
    if(empty()) 
      return 0;
    return start_ + slice_.size() - ptr_;
  }

  VarintIStream& operator>>(Rect& rect) {
    float minx, maxx, miny, maxy;
    *this >> minx >> maxx >> miny >> maxy;
    rect.set_minx(minx);
    rect.set_maxx(maxx);
    rect.set_miny(miny);
    rect.set_maxy(maxy);
    return *this;
  }

  VarintIStream& operator>>(bool& val) {
    auto p = WireFormat::ReadBoolFromArray(ptr_, val);
    ptr_ = p.second;
    return *this;
  }

  VarintIStream& operator>>(float& val) {
    auto p = WireFormat::ReadFloatFromArray(ptr_, val);
    ptr_ = p.second;
    return *this;
  }

  VarintIStream& operator>>(double& val) {
    auto p = WireFormat::ReadDoubleFromArray(ptr_, val);
    ptr_ = p.second;
    return *this;
  }

  template <typename T, typename = typename std::enable_if<
                            std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
                            std::is_same<T, uint32_t>::value || std::is_same<T, int8_t>::value ||
                            std::is_same<T, int16_t>::value || std::is_same<T, int32_t>::value>::type>
  VarintIStream& operator>>(T& val) {
    ReadImp(val, std::is_signed<T>());
    return *this;
  }

  VarintIStream& operator>>(uint64_t& val) {
    auto p = WireFormat::ReadUInt64FromArray(ptr_, val);
    ptr_ = p.second;
    return *this;
  }

  VarintIStream& operator>>(int64_t& val) {
    auto p = WireFormat::ReadSInt64FromArray(ptr_, val);
    ptr_ = p.second;
    return *this;
  }

  VarintIStream& operator>>(GWString& str) {
    auto p = WireFormat::ReadStringFromArray(ptr_, str);
    ptr_ = p.second;
    return *this;
  }

  VarintIStream& operator>>(std::string& str) {
    auto p = WireFormat::ReadStringFromArray(ptr_, str);
    ptr_ = p.second;
    return *this;
  }

  template <typename T>
  VarintIStream& operator>>(std::vector<T>& v) {
    uint32_t sz;
    *this >> sz;
    T elt;
    for (uint32_t i = 0; i < sz; i++) {
      *this >> elt;
      v.push_back(elt);
    }
    return *this;
  }

  template <typename T, typename U>
  VarintIStream& operator>>(std::map<T, U>& mp) {
    uint32_t sz;
    *this >> sz;
    T first;
    U second;
    for (uint32_t i = 0; i < sz; i++) {
      *this >> first >> second;
      mp[first] = second;
    }
    return *this;
  }

 private:
  template <typename T>
  void ReadImp(T& val, std::true_type) {
    int32_t temp = 0;
    auto p = WireFormat::ReadSInt32FromArray(ptr_, temp);
    ptr_ = p.second;
    val = static_cast<T>(temp);
  }

  template <typename T>
  void ReadImp(T& val, std::false_type) {
    uint32_t temp = 0;
    auto p = WireFormat::ReadUInt32FromArray(ptr_, temp);
    ptr_ = p.second;
    val = static_cast<T>(temp);
  }
  StringSlice slice_;
  const uint8_t* start_;
  const uint8_t* ptr_;
};

// todo 防止类型强制转换转出奇怪的东西，比如char* 被认为是bool类型
class VarintMemoryOStream : public IMemoryOStream {
 public:
  ~VarintMemoryOStream() {}

  inline IMemoryOStream& operator<<(const bool& val) override { varint_mem_os_ << val; return *this; }
  inline IMemoryOStream& operator<<(const StringSlice& val) override { varint_mem_os_ << val; return *this; }
  inline IMemoryOStream& operator<<(const GWString& val) override { varint_mem_os_ << val; return *this; }
  inline IMemoryOStream& operator<<(const std::string& val) override { 
    StringSlice ss(val.c_str(), val.size());   
    varint_mem_os_ << ss; 
    return *this;
  }
  inline IMemoryOStream& operator<<(const gw::Rect& val) override { varint_mem_os_ << val; return *this; }
  inline IMemoryOStream& operator<<(const uint16_t& val) override { varint_mem_os_ << val; return *this; }
  inline IMemoryOStream& operator<<(const uint8_t& val) override { varint_mem_os_ << val; return *this; }
  inline IMemoryOStream& operator<<(const uint64_t& val) override { varint_mem_os_ << val; return *this; }
  inline IMemoryOStream& operator<<(const uint32_t& val) override { varint_mem_os_ << val; return *this; }
  inline IMemoryOStream& operator<<(const float& val) override { varint_mem_os_ << val; return *this; }
  inline IMemoryOStream& WriteData(const StringSlice& val) override { varint_mem_os_.WriteData(val); return *this; }

  inline GWString str() const override { return varint_mem_os_.str(); }
  inline StringSlice view() override { return varint_mem_os_.view(); }
  inline void reset() override { varint_mem_os_.reset(); }
  inline size_t size() const override { return varint_mem_os_.size(); }
 protected:
  VarintOStream varint_mem_os_;
};

class VarintMemoryIStream : public IMemoryIStream {
 public:
  ~VarintMemoryIStream() {}
  VarintMemoryIStream(const StringSlice& slice) : varint_mem_is_(slice) {
  }

  inline IMemoryIStream& operator>>(bool& val) override { varint_mem_is_ >> val; return *this; }
  inline IMemoryIStream& operator>>(Rect& val) override { varint_mem_is_ >> val; return *this; }
  inline IMemoryIStream& operator>>(uint16_t& val) override { varint_mem_is_ >> val; return *this; }
  inline IMemoryIStream& operator>>(uint8_t& val) override { varint_mem_is_ >> val; return *this; }
  inline IMemoryIStream& operator>>(uint32_t& val) override { varint_mem_is_ >> val; return *this; }
  inline IMemoryIStream& operator>>(uint64_t& val) override { varint_mem_is_ >> val; return *this; }
  inline IMemoryIStream& operator>>(GWString& val) override { 
    varint_mem_is_ >> val; 
    return *this; 
  }
  inline IMemoryIStream& operator>>(std::string& val) override {
    varint_mem_is_ >> val;
    return *this; 
  }
  inline IMemoryIStream& operator>>(float& val) override { varint_mem_is_ >> val; return *this; }

  inline bool empty() { return varint_mem_is_.empty(); }

  inline void MoveLeftTo(GWVector<char>& left) { varint_mem_is_.MoveLeftTo(left); }
  
  inline std::size_t LeftSize() const { return varint_mem_is_.LeftSize(); }

 protected:
  VarintIStream varint_mem_is_;
};

}  // namespace gw
