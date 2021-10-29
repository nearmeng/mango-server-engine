#include "resume_typed_pool_test.h"
#include "shm_test.h"

struct Sample {
  ResumeTypeWrapper<ResumeLayerStorage> wrapper_;

  Sample() {
    if (!IsDeathTestChildProcess()) ResumeLayer::Impl().Clear();

    ResumeLayer::RegisterType<BaseComponent>();
    ResumeLayer::RegisterType<DerivedComponent>();
    ResumeLayer::RegisterType<Derived2Component>();
  }

  void InitTestData() {
    if (!ResumeLayer::Init(wrapper_.Ptr())) return exit(kNormalExitCode + 1);

    auto* allocator = ResumeLayer::Allocator<BaseComponent>();
    if (!allocator) exit(kNormalExitCode + 2);
    auto* ptr = allocator->Singleton();
    if (ptr) exit(kNormalExitCode + 3);
    ptr = allocator->New();
    if (ptr->Test() != 1) return exit(kNormalExitCode + 4);
  }
  void CheckTestData() {
    auto ret = ResumeLayer::ResumeInit(wrapper_.Ptr());
    ASSERT_TRUE(ret);

    auto* allocator = ResumeLayer::Allocator<BaseComponent>();
    ASSERT_TRUE(allocator);

    auto* ptr = allocator->Singleton();
    ASSERT_TRUE(ptr);
    ASSERT_EQ(ptr->Test(), 2);

    allocator->Delete(ptr);
  }
};

struct TypeNotExist {
  ResumeTypeWrapper<ResumeLayerStorage> wrapper_;

  TypeNotExist() {
    if (!IsDeathTestChildProcess()) ResumeLayer::Impl().Clear();

    ResumeLayer::RegisterType<DerivedComponent>();
    ResumeLayer::RegisterType<Derived2Component>();
  }

  void InitTestData() {
    if (!ResumeLayer::Init(wrapper_.Ptr())) return exit(kNormalExitCode + 1);

    auto* allocator = ResumeLayer::Allocator<BaseComponent>();
    if (allocator) return exit(kNormalExitCode + 2);
  }
  void CheckTestData() {
    auto ret = ResumeLayer::ResumeInit(wrapper_.Ptr());
    ASSERT_TRUE(ret);

    auto* allocator = ResumeLayer::Allocator<BaseComponent>();
    ASSERT_FALSE(allocator);
  }
};

struct RegisterTypeChange {
  ResumeTypeWrapper<ResumeLayerStorage> wrapper_;

  RegisterTypeChange() {
    if (!IsDeathTestChildProcess()) ResumeLayer::Impl().Clear();
    if (!ShmIsResuming()) ResumeLayer::RegisterType<BaseComponent>();

    ResumeLayer::RegisterType<DerivedComponent>();
    ResumeLayer::RegisterType<Derived2Component>();
  }

  void InitTestData() {
    if (!ResumeLayer::Init(wrapper_.Ptr())) return exit(kNormalExitCode + 1);

    auto* allocator = ResumeLayer::Allocator<BaseComponent>();
    if (!allocator) exit(kNormalExitCode + 2);
  }
  void CheckTestData() {
    auto ret = ResumeLayer::ResumeInit(wrapper_.Ptr());
    ASSERT_FALSE(ret);
  }
};

using TestTypes = testing::Types<Sample, TypeNotExist, RegisterTypeChange>;
template <typename T>
using ShmResumeTypedDeathTest_ResumeLayer = ShmResumeTypedDeathTest<T>;
TYPED_TEST_SUITE(ShmResumeTypedDeathTest_ResumeLayer, TestTypes);
TYPED_TEST(ShmResumeTypedDeathTest_ResumeLayer, Run) { this->RunTestCase(); }

struct MyEnv {
  MyEnv() {
    ResumeLayer::RegisterType<BaseComponent>();
    ResumeLayer::RegisterType<DerivedComponent>();
    ResumeLayer::RegisterType<Derived2Component>();
  }
};

using ResumeLayerTestMyEnv1 = ResumeLayerTestEnv<MyEnv>;
using ResumeLayerTestMyEnv2 = ResumeLayerTestEnv<MyEnv>;

TEST_F(ResumeLayerTestMyEnv1, Sample) {
  auto* allocator = ResumeLayer::Allocator<BaseComponent>();
  ASSERT_TRUE(allocator);
}

TEST_F(ResumeLayerTestMyEnv2, Sample) {
  auto* allocator = ResumeLayer::Allocator<BaseComponent>();
  ASSERT_TRUE(allocator);
}
