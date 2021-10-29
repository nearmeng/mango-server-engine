#include "shm.h"
#include "shm_test.h"
#include "utility_container.h"

const char* kCheckString = "::::::::::::ShmDeathTest::::::::::::";
constexpr int kTestDataSize = 1024;
static int g_ensure_not_fork_value;

TEST_F(ShmDeathTest, EnsureNotFork) {
  if (!IsDeathTestChildProcess()) {
    g_ensure_not_fork_value = kShmIdentNumber;
  }

  auto child = [this]() {
    if (g_ensure_not_fork_value == kShmIdentNumber) exit(kNormalExitCode - 1);
    exit(kNormalExitCode);
  };

  ASSERT_EXIT(child(), ::testing::ExitedWithCode(kNormalExitCode), "");
  ASSERT_EQ(g_ensure_not_fork_value, kShmIdentNumber);
}

TEST_F(ShmDeathTest, IdentNumDiff) {
  auto child = [this]() {
    ShmTestGuard guard(kShmKeyNumber, kShmIdentNumber, false);
    if (guard.stat_ != ShmStat::kCreate) exit(kNormalExitCode - 1);
    exit(kNormalExitCode);
  };

  ASSERT_EXIT(child(), ::testing::ExitedWithCode(kNormalExitCode), "");

  ShmTestGuard guard(kShmKeyNumber, kShmIdentNumber + 1, true);
  ASSERT_EQ(guard.stat_, ShmStat::kCreate);
}

TEST_F(ShmDeathTest, IdentNumSame) {
  auto child = [this]() {
    ShmTestGuard guard(kShmKeyNumber, kShmIdentNumber, false);
    if (guard.stat_ != ShmStat::kCreate) exit(kNormalExitCode - 1);
    exit(kNormalExitCode);
  };

  ASSERT_EXIT(child(), ::testing::ExitedWithCode(kNormalExitCode), "");

  ShmTestGuard guard(kShmKeyNumber, kShmIdentNumber, true);
  ASSERT_EQ(guard.stat_, ShmStat::kResume);
}

struct ShmTestBuffer {
  char data_[kTestDataSize];
  void InitTestData() { strncpy(data_, kCheckString, sizeof(data_)); }
  void CheckTestData() { ASSERT_STREQ(data_, kCheckString); }
};

struct ShmTestPtr {
  char* data_;
  void InitTestData() {
    data_ = static_cast<char*>(ShmMalloc(kTestDataSize));
    strncpy(data_, kCheckString, kTestDataSize);
  }
  void CheckTestData() {
    ASSERT_TRUE(data_);
    ASSERT_STREQ(data_, kCheckString);
    ShmFree(data_);
  }
};

struct ShmTestVector {
  shm::vector<size_t> data_;
  void InitTestData() {
    data_.clear();
    for (size_t i = 0; i < kTestDataSize; i++) {
      data_.push_back(i);
    }
  }
  void CheckTestData() {
    ASSERT_EQ(data_.size(), kTestDataSize);
    for (size_t i = 0; i < kTestDataSize; i++) {
      ASSERT_EQ(data_[i], i);
    }
  }
};

struct ShmTestList {
  shm::list<size_t> data_;
  void InitTestData() {
    data_.clear();
    for (size_t i = 0; i < kTestDataSize; i++) {
      data_.push_back(i);
    }
  }
  void CheckTestData() {
    ASSERT_EQ(data_.size(), kTestDataSize);
    size_t i = 0;
    for (auto v : data_) {
      ASSERT_EQ(v, i);
      i++;
    }
  }
};

using TestTypes = testing::Types<ShmTestBuffer, ShmTestPtr, ShmTestVector, ShmTestList>;
template <typename T>
using ShmTypedDeathTest_Shm = ShmTypedDeathTest<T>;
TYPED_TEST_SUITE(ShmTypedDeathTest_Shm, TestTypes);
TYPED_TEST(ShmTypedDeathTest_Shm, Run) { this->RunTestCase(); }
