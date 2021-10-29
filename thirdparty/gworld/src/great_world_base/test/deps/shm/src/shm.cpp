#include "shm.h"
#include <cstdlib>
#include <cassert>
#include <ctime>
#include "shm_config.h"

//#include "log/log.h"
#include "dlmalloc/mspace_malloc.h"
#ifndef _MSC_VER
#include "shm_mapping_linux.h"
#else
#include "shm_mapping_windows.h"
#endif



constexpr std::size_t kPageSize = 4096;
constexpr std::size_t kGbSize = 1024 * 1024 * 1024;
constexpr std::size_t kShmSize = kGbSize * 3;
constexpr std::size_t kShmHeaderMaxSize = kGbSize * 1;
static struct ShmHeader* g_shm_header = nullptr;

struct ShmHeader {
  key_t key;           // 4
  int ident_num;       // 4
  time_t create_time;  // 8

  uint32_t header_hash;  // 4
  int shm_id;            // 4
  void* mspace;          // 8
};

inline uint32_t hash(ShmHeader* header) {
  uint64_t value64 = header->ident_num ? header->ident_num : 1;
  value64 *= header->key;
  if (header->ident_num > 0) value64 *= header->create_time;
  return static_cast<uint32_t>(value64 & 0xFFFFFFFF);
}

inline bool CanResume(ShmHeader* header, key_t key, int ident_num, void* mspace) {
  if(!ShmConfig::IsEnabled()) {
    SHMLOG_WARN("[can_resume]failed. shm not enabled");
    return false;
  }
  if (key != header->key) {
    SHMLOG_WARN("[can_resume]failed. key=%d, header=%d", key, header->key);
    return false;
  }
  if (ident_num != header->ident_num) {
    SHMLOG_WARN("[can_resume]failed. ident_num=%d, header=%d", ident_num, header->ident_num);
    return false;
  }

  if (header->mspace != mspace) {
    SHMLOG_WARN("[can_resume]failed. mspace=%p, header=%p", mspace, header->mspace);
    return false;
  }
  auto header_hash = hash(header);
  if (header_hash != header->header_hash) {
    SHMLOG_WARN("[can_resume]failed. header_hash=%d, header=%d", header_hash, header->header_hash);
    return false;
  }
  return true;
}

ShmStat ShmCreate(key_t key, int ident_num, bool is_resume) {
  if(!ShmConfig::IsEnabled()) {
    if (is_resume) return ShmStat::kInvalid;
    g_shm_header = reinterpret_cast<ShmHeader*>(std::malloc(kShmHeaderMaxSize));
    return ShmStat::kCreate;
  }
  

  assert(!g_shm_header);
  SHMLOG_INFO("ShmCreate with key %d ident_num %d is_resume %d", key, ident_num, is_resume);
  if (g_shm_header) return ShmStat::kInvalid;

  int shm_id;
  if (!ShmMapping::Open(key, kShmSize, is_resume, &shm_id)) return ShmStat::kInvalid;

  auto* ptr = ShmMapping::Mapping(shm_id, kShmSize);
  if (!ptr || ptr != kShmAddress) return ShmStat::kInvalid;

  auto* header = static_cast<ShmHeader*>(ptr);
  if (!header) return ShmStat::kInvalid;

  auto* mspace = base_to_mspace(static_cast<char*>(kShmAddress) + kShmHeaderMaxSize);
  if (is_resume && CanResume(header, key, ident_num, mspace)) {
    header->shm_id = shm_id;

    g_shm_header = header;
    return ShmStat::kResume;
  }

  time_t now;
  time(&now);

  header->key = key;
  header->ident_num = ident_num;
  header->create_time = now;
  header->header_hash = hash(header);
  header->shm_id = shm_id;

  auto* allocator = static_cast<char*>(ptr) + kShmHeaderMaxSize;
  header->mspace = create_mspace_with_base(allocator, kShmSize - kShmHeaderMaxSize, 0);

  if (!header->mspace || header->mspace != mspace) {
    ShmMapping::Close(header->shm_id, header);
    return ShmStat::kInvalid;
  }

  g_shm_header = header;
  return ShmStat::kCreate;
}

void ShmDestroy() {
  if (!g_shm_header) return;
  if(!ShmConfig::IsEnabled()) {
    std::free(g_shm_header);
    g_shm_header = nullptr;
  }
  else {
    destroy_mspace(g_shm_header->mspace);
    g_shm_header->mspace = nullptr;

    ShmMapping::Close(g_shm_header->shm_id, g_shm_header);
    g_shm_header = nullptr;
  }
}

bool ShmGetBuffer(char** buffer, std::size_t* size) {
  if (!g_shm_header) return false;
  *buffer = reinterpret_cast<char*>(g_shm_header) + sizeof(*g_shm_header);
  *size = kShmHeaderMaxSize - sizeof(*g_shm_header);
  return true;
}

void* ShmMalloc(std::size_t size) {
  if (!g_shm_header) {
    assert(g_shm_header);
    return nullptr;
  }

  if(ShmConfig::IsEnabled()) {
    auto* mspace = base_to_mspace(static_cast<char*>(kShmAddress) + kShmHeaderMaxSize);
    auto* ptr = mspace_malloc(mspace, size);
    if (!ptr) {
      SHMLOG_WARN("[ShmMalloc] failed. size=%zu", size);
      assert(false);
    }
    return ptr;
  }
  else {
    return std::malloc(size);
  }
}

void* ShmAlignedAlloc(std::size_t alignment, std::size_t size) {
  assert((alignment & (alignment - 1)) == 0);

  assert(alignment <= ShmAlignSize);
  auto result = ShmMalloc(size);
  assert(std::size_t(result) % alignment == 0);
  return result;
}

void ShmFree(void* ptr) {
  if (!g_shm_header) {
    assert(g_shm_header);
    return;
  }
  if(ShmConfig::IsEnabled()) {
    auto* mspace = base_to_mspace(static_cast<char*>(kShmAddress) + kShmHeaderMaxSize);
    mspace_free(mspace, ptr);
  }
  else {    
      std::free(ptr);
  }
}

void ShmGetStat(size_t* out_maxfp, size_t* out_fp, size_t* out_used) {
  if (!g_shm_header) {
    assert(g_shm_header);
    return;
  }
  if(ShmConfig::IsEnabled()) {
    auto* mspace = base_to_mspace(static_cast<char*>(kShmAddress) + kShmHeaderMaxSize);
    mspace_get_stats(mspace, out_maxfp, out_fp, out_used);
    return;
  }
  else {
    *out_maxfp = 0;
    *out_fp = 0;
    *out_used = 0;
    return;
  }
}