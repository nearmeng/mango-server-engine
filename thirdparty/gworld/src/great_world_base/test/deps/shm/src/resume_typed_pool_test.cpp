#include "resume_typed_pool.h"
#include "resume_typed_pool_test.h"
#include "shm_test.h"

TEST(ResumeTypedPool, New) {
  constexpr uint32_t kTestCount = 64 * 1024;
  ShmTestGuard guard(kShmKeyNumber, kShmIdentNumber, false);
  ResumeTypedPool<uint32_t> pool;

  for (uint32_t i = 1; i < kTestCount; i++) {
    auto* ptr = pool.New(i);
    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(*ptr, i);
    ASSERT_EQ(pool.GetCount(), (i - 1) / 2 + 1);

    if (i % 2 == 1) {
      pool.Delete(ptr);

      auto sn = pool.GetSerialNumber(ptr);
      ASSERT_EQ(sn, 0);
    }
  }
}

template <typename T>
void ResumeInit(T& pool) {
  ASSERT_TRUE(pool.Resume());
  ASSERT_TRUE(pool.PostResume());
}

struct VTable {
  ResumeTypedPool<DerivedComponent> pool_;
  int* vtable_;
  void InitTestData() {
    auto* component = pool_.New();
    ASSERT_EQ(component->value1, 1);
    ASSERT_EQ(component->value2, 10);
    ASSERT_EQ(component->Test(), 10);

    auto vptr_table_ptr = reinterpret_cast<int**>(component);
    vtable_ = *vptr_table_ptr;
    *vptr_table_ptr = nullptr;  // clear vtable
  }
  void CheckTestData() {
    ResumeInit(pool_);

    auto* component = pool_.Singleton();
    ASSERT_EQ(component->value1, 2);
    ASSERT_EQ(component->Test(), 100);

    auto vptr_table_ptr = reinterpret_cast<int**>(component);
    ASSERT_NE(*vptr_table_ptr, nullptr);
    ASSERT_EQ(*vptr_table_ptr, vtable_);
  }
};

struct MemberVTable {
  ResumeTypedPool<Derived2Component> pool_;
  int* vtable_;
  void InitTestData() {
    auto* component = pool_.New(5);
    ASSERT_EQ(component->value1, 1);
    ASSERT_EQ(component->value2, 5);
    ASSERT_EQ(component->GetValue3()->Test(), 10);

    auto vptr_table_ptr = reinterpret_cast<int**>(component->GetValue3());
    vtable_ = *vptr_table_ptr;
    *vptr_table_ptr = nullptr;  // clear vtable
  }
  void CheckTestData() {
    ResumeInit(pool_);

    auto* component = pool_.Singleton();
    ASSERT_EQ(component->value1, 2);
    ASSERT_EQ(component->value2, 5);
    ASSERT_EQ(component->GetValue3()->Test(), 100);

    auto vptr_table_ptr = reinterpret_cast<int**>(component->GetValue3());
    ASSERT_NE(*vptr_table_ptr, nullptr);
    ASSERT_EQ(*vptr_table_ptr, vtable_);
  }
};

using TestTypes = testing::Types<VTable, MemberVTable>;
template <typename T>
using ShmTypedDeathTest_ResumeTypedPool = ShmTypedDeathTest<T>;
TYPED_TEST_SUITE(ShmTypedDeathTest_ResumeTypedPool, TestTypes);
TYPED_TEST(ShmTypedDeathTest_ResumeTypedPool, Run) { this->RunTestCase(); }
