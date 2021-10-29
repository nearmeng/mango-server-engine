#pragma once
#include <cstddef>
#include <cstdint>

#ifdef _MSC_VER
using key_t = int32_t;
#else
#include <sys/types.h>
#endif  // _MSC_VER

enum class ShmStat : uint8_t {
  kInvalid,
  kCreate,
  kResume,
};
// std malloc only guarrenty result aligned to any scalar type.
// linux malloc is aligned to sizeof(std::size_t) *2
const static std::size_t ShmAlignSize = sizeof(std::size_t) *2;
ShmStat ShmCreate(key_t key, int ident_num, bool is_resume);
void ShmDestroy();

bool ShmGetBuffer(char** buffer, std::size_t* size);

void* ShmAlignedAlloc(std::size_t alignment, std::size_t size);
void* ShmMalloc(std::size_t size);
void ShmFree(void* ptr);
void ShmGetStat(size_t* out_maxfp, size_t* out_fp, size_t* out_used);