#include "dlmalloc/mspace_malloc.h"
#include <vector>
#include "gtest/gtest.h"
#include "shm.h"
#include "shm_test.h"

struct Malloc {
  void* ptr_;
  size_t maxfp_;
  size_t fp_;
  size_t used_;

  void GetStats(size_t* out_maxfp, size_t* out_fp, size_t* out_used) {
    char* buffer;
    size_t size;
    auto ret = ShmGetBuffer(&buffer, &size);
    ASSERT_TRUE(ret);
    auto mspace = base_to_mspace(buffer + size);
    mspace_get_stats(mspace, out_maxfp, out_fp, out_used);
  }

  void InitTestData() {
    for (auto i = 1; i < 16; ++i) {
      auto* ptr = ShmMalloc(16 * i);
      if (i == 8) {
        ptr_ = ptr;
      }
      if (i % 3 == 0) {
        ShmFree(ptr);
      }
    }
    GetStats(&maxfp_, &fp_, &used_);
  }

  void CheckTestData() {
    size_t maxfp;
    size_t fp;
    size_t used;
    GetStats(&maxfp, &fp, &used);

    ASSERT_EQ(maxfp, maxfp_);
    ASSERT_EQ(fp, fp_);
    ASSERT_EQ(used, used_);

    ShmFree(ptr_);
  }
};

using TestTypes = testing::Types<Malloc>;
template <typename T>
using ShmTypedDeathTest_Malloc = ShmTypedDeathTest<T>;
TYPED_TEST_SUITE(ShmTypedDeathTest_Malloc, TestTypes);
TYPED_TEST(ShmTypedDeathTest_Malloc, Run) { this->RunTestCase(); }