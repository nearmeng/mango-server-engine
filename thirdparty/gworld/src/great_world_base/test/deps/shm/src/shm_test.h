#pragma once
#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"
#include "resume_typed_layer.h"
#include "resume_typed_pool.h"
#include "resume_layer.h"
#include "shm.h"
#include "shm_config.h"
#ifndef _MSC_VER
#include "shm_mapping_linux.h"
#else
#include "shm_mapping_windows.h"
#endif

const int kNormalExitCode = 18;
const int kShmKeyNumber = 43210;
const int kShmIdentNumber = 98765;

inline bool IsDeathTestChildProcess() {
  return testing::internal::GTEST_FLAG(internal_run_death_test).length() > 0;
}

class ShmDeathTest : public ::testing::Test {
 protected:
  void SetUp() override { ::testing::FLAGS_gtest_death_test_style = "threadsafe"; }
};

struct ShmTestGuard {
  ShmTestGuard() {}

  ShmTestGuard(key_t key, int ident_num, bool is_resume) { Init(key, ident_num, is_resume); }

  void Init(key_t key, int ident_num, bool is_resume) {
    ShmConfig::SetEnable(true);
    stat_ = ShmCreate(key, ident_num, is_resume);
    if (stat_ != ShmStat::kInvalid) ShmGetBuffer(&buffer_, &buffer_size_);
  }
  ~ShmTestGuard() {
    if (stat_ != ShmStat::kInvalid) ShmDestroy();
  }
  ShmStat stat_ = ShmStat::kInvalid;
  char* buffer_ = nullptr;
  size_t buffer_size_ = 0;
};

template <typename T>
struct ShmTypedDeathTest : ShmDeathTest {
  static void InitTestData() {
    ShmTestGuard guard(kShmKeyNumber, kShmIdentNumber, false);
    if (guard.stat_ != ShmStat::kCreate) exit(kNormalExitCode - 1);

    assert(guard.buffer_size_ >= sizeof(T));
    auto* ptr = new (guard.buffer_) T;
    if (!ptr) exit(kNormalExitCode - 2);

    ptr->InitTestData();
    exit(kNormalExitCode);
  }

  static void CheckTestData() {
    ShmTestGuard guard(kShmKeyNumber, kShmIdentNumber, true);
    ASSERT_EQ(guard.stat_, ShmStat::kResume);

    assert(guard.buffer_size_ >= sizeof(T));
    auto* ptr = reinterpret_cast<T*>(guard.buffer_);
    ASSERT_TRUE(ptr);

    ptr->CheckTestData();
    ptr->~T();
  }

  void RunTestCase() {
    ASSERT_EXIT(InitTestData(), ::testing::ExitedWithCode(kNormalExitCode), "");
    CheckTestData();
  }
};

template <typename T, template <typename> class ResumeStrategy = ResumeDefaultStrategy>
struct ShmResumeTypedDeathTest : ShmDeathTest {
  using TestResumeTypedPool = ResumeTypedPool<T, ResumeStrategy>;
  using TestResumeTypedLayer = ResumeTypedLayer<TestResumeTypedPool>;

  static void InitTestData() {
    ShmTestGuard guard(kShmKeyNumber, sizeof(typename TestResumeTypedLayer::ResumeTuple), false);
    if (guard.stat_ != ShmStat::kCreate) exit(kNormalExitCode - 2);

    auto layer = TestResumeTypedLayer::Init(guard.buffer_, guard.buffer_size_);
    if (!layer) exit(kNormalExitCode - 3);

    auto* allocator = TestResumeTypedLayer::template Allocator<T>(*layer);
    auto* ptr = allocator->New();
    if (!ptr) exit(kNormalExitCode - 4);

    ptr->InitTestData();
    exit(kNormalExitCode);
  }

  static void CheckTestData() {
    ShmTestGuard guard(kShmKeyNumber, sizeof(typename TestResumeTypedLayer::ResumeTuple), true);
    ASSERT_EQ(guard.stat_, ShmStat::kResume);

    auto* layer = TestResumeTypedLayer::ResumeInit(guard.buffer_, guard.buffer_size_);
    ASSERT_TRUE(layer);

    auto* allocator = TestResumeTypedLayer::template Allocator<T>(*layer);
    auto ptr = allocator->Singleton();
    ASSERT_TRUE(ptr);

    ptr->CheckTestData();
  }

  void RunTestCase() {
    ASSERT_EXIT(InitTestData(), ::testing::ExitedWithCode(kNormalExitCode), "");
    CheckTestData();
  }
};

template <typename T>
class ResumeLayerTestEnv : public ShmDeathTest {
  using ShmTestEnvLayer = ResumeTypedLayer<ResumeTypedPool<ResumeLayerStorageWrapper>>;

  template <typename U>
  struct InnerEnv {
    U custom_env;
    ShmTestGuard guard;
  };

  static InnerEnv<T>* env_;

 public:
  static void SetUpTestSuite() {
    ASSERT_TRUE(!env_);

    ResumeLayer::Impl().Clear();
    env_ = new InnerEnv<T>;

    auto is_resume = false;
    env_->guard.Init(kShmKeyNumber, kShmIdentNumber, is_resume);

    auto* tuple =
        ResumeLayerInit<ShmTestEnvLayer>(is_resume, env_->guard.buffer_, env_->guard.buffer_size_);
    ASSERT_TRUE(tuple);

    auto* storage_allocator = ShmTestEnvLayer::Allocator<ResumeLayerStorageWrapper>(*tuple);
    ASSERT_TRUE(storage_allocator);

    if (!is_resume) {
      storage_allocator->New();
    }

    auto* wrapper = storage_allocator->Singleton();
    ASSERT_TRUE(wrapper);

    auto ret = ResumeLayerInit<ResumeLayer>(is_resume, wrapper->Ptr());
    ASSERT_TRUE(wrapper);
  }

  static void TearDownTestSuite() {
    delete env_;
    env_ = nullptr;
  }
};
template <typename T>
ResumeLayerTestEnv<T>::InnerEnv<T>* ResumeLayerTestEnv<T>::env_ = nullptr;