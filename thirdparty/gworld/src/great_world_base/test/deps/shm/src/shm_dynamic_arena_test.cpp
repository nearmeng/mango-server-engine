#include "shm_dynamic_arena_test.h"
#include "gtest/gtest.h"
#include "shm_dynamic_singleton.h"
#include "shm_shared_ptr.h"
#include "shm_test.h"
#include "shm_unique_ptr.h"

#include <memory>

DYNCOMPONENT_IMPL(DynDerivedComponent1)

DYNCOMPONENT_IMPL(DynDerivedComponent2)

bool DynDerivedComponent1::ClassResumePostFunc() {
  value_2 += "_post";
  return true;
}

bool DynDerivedComponent2::ClassResumePostFunc() {
  if (!DynDerivedComponent1::ClassResumePostFunc()) {
    return false;
  }
  value_3.push_back(3);
  return true;
}

class ShmDynArenaTest : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    ShmConfig::SetEnable(true);
    auto shm_stat = ShmCreate(kShmKeyNumber, kShmIdentNumber, false);
    ASSERT_TRUE(shm_stat == ShmStat::kCreate);
    char* buffer;
    std::size_t buffer_size;
    auto ret = ShmGetBuffer(&buffer, &buffer_size);
    ASSERT_TRUE(ret);
    buffer = reinterpret_cast<char *>((reinterpret_cast<std::size_t>(buffer + 15) >> 4) << 4);
    auto singleton_data = reinterpret_cast<ShmDynamicSingletonData *>(buffer);
    ASSERT_TRUE(ShmSingletonArena::Init(singleton_data));
    arena_data = ShmSingletonArena::Get<ShmDynamicArenaData>();
    ASSERT_NE(arena_data, nullptr);
    ASSERT_TRUE(ShmDynamicArena::Init(arena_data));
  }

  static void TearDownTestSuite() {
    ShmDestroy();
  }
  static ShmDynamicArenaData *arena_data;
};
ShmDynamicArenaData *ShmDynArenaTest::arena_data = nullptr;

TEST_F(ShmDynArenaTest, Sample) {
  auto comp_0_idx_ptr = ShmDynamicArena::Create<DynBaseComponent>();
  DynBaseComponent *object_0 = comp_0_idx_ptr.Get();
  ASSERT_EQ(object_0, nullptr);
  auto comp_1_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent1>();
  DynDerivedComponent1 *object_1 = comp_1_idx_ptr.Get();
  ASSERT_NE(object_1, nullptr);
  ASSERT_EQ(object_1->value_2, "init");
  auto comp_2_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent2>();
  DynDerivedComponent2 *object_2 = comp_2_idx_ptr.Get();
  ASSERT_EQ(object_2->value_2, "init");
  std::vector<int> temp_vec;
  temp_vec.push_back(1);
  ASSERT_EQ(object_2->value_3, temp_vec);
  auto comp_1_idx_ptr_dup = comp_1_idx_ptr;
  auto comp_2_idx_ptr_dup = comp_2_idx_ptr;
  auto cur_stat = ShmDynamicArena::Stat();
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].first, 1);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].first, 1);
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].second, 1);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].second, 1);
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_1_idx_ptr));
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_2_idx_ptr));
  cur_stat = ShmDynamicArena::Stat();
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].first, 1);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].first, 1);
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].second, 0);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].second, 0);
  ASSERT_EQ(comp_1_idx_ptr_dup.Get(), nullptr);
  ASSERT_EQ(comp_2_idx_ptr_dup.Get(), nullptr);
}

TEST_F(ShmDynArenaTest, PointerOperator) {
  auto comp_0_idx_ptr = ShmDynamicArena::Create<DynBaseComponent>();
  DynBaseComponent *object_0 = comp_0_idx_ptr.Get();
  ASSERT_EQ(object_0, nullptr);
  auto comp_1_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent1>();
  DynDerivedComponent1 *object_1 = comp_1_idx_ptr.Get();
  ASSERT_NE(object_1, nullptr);
  ASSERT_EQ(object_1->value_2, "init");
  auto comp_2_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent2>();
  DynDerivedComponent2 *object_2 = comp_2_idx_ptr.Get();
  ASSERT_EQ(object_2->value_2, "init");
  std::vector<int> temp_vec;
  temp_vec.push_back(1);
  ASSERT_EQ(object_2->value_3, temp_vec);

  ASSERT_EQ(comp_1_idx_ptr->ClassName(), "DynDerivedComponent1");
  ASSERT_EQ(comp_2_idx_ptr->ClassName(), "DynDerivedComponent2");

  ASSERT_EQ(object_1->ClassName(), "DynDerivedComponent1");
  ASSERT_EQ(object_2->ClassName(), "DynDerivedComponent2");

  ASSERT_EQ((*comp_1_idx_ptr).ClassName(), "DynDerivedComponent1");
  ASSERT_EQ((*comp_2_idx_ptr).ClassName(), "DynDerivedComponent2");

  auto comp_1_idx_ptr_dup = comp_1_idx_ptr;
  auto comp_2_idx_ptr_dup = comp_2_idx_ptr;
  comp_1_idx_ptr_dup = nullptr;
  comp_2_idx_ptr_dup = nullptr;
  ASSERT_EQ(comp_1_idx_ptr_dup.Get(), nullptr);
  ASSERT_EQ(comp_2_idx_ptr_dup.Get(), nullptr);
  auto cur_stat = ShmDynamicArena::Stat();
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].first, 2);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].first, 2);
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].second, 1);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].second, 1);
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_1_idx_ptr));
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_2_idx_ptr));
  ASSERT_FALSE(ShmDynamicArena::Destroy(comp_1_idx_ptr));
  ASSERT_FALSE(ShmDynamicArena::Destroy(comp_2_idx_ptr));
  ASSERT_EQ(comp_1_idx_ptr.Get(), nullptr);
  ASSERT_EQ(comp_2_idx_ptr.Get(), nullptr);

  ASSERT_FALSE(ShmDynamicArena::Destroy(comp_1_idx_ptr_dup));
  ASSERT_FALSE(ShmDynamicArena::Destroy(comp_2_idx_ptr_dup));
}

TEST_F(ShmDynArenaTest, UniquePtr) {
  {
    auto comp_0_idx_ptr = ShmMakeUnique<DynBaseComponent>();
    DynBaseComponent *object_0 = comp_0_idx_ptr.get();
    ASSERT_EQ(object_0, nullptr);
    auto comp_1_idx_ptr = ShmMakeUnique<DynDerivedComponent1>();
    DynDerivedComponent1 *object_1 = comp_1_idx_ptr.get();
    ASSERT_NE(object_1, nullptr);
    ASSERT_EQ(object_1->value_2, "init");
    auto comp_2_idx_ptr = ShmMakeUnique<DynDerivedComponent2>();
    DynDerivedComponent2 *object_2 = comp_2_idx_ptr.get();
    ASSERT_EQ(object_2->value_2, "init");
    std::vector<int> temp_vec;
    temp_vec.push_back(1);
    ASSERT_EQ(object_2->value_3, temp_vec);

    ASSERT_EQ(comp_1_idx_ptr->ClassName(), "DynDerivedComponent1");
    ASSERT_EQ(comp_2_idx_ptr->ClassName(), "DynDerivedComponent2");

    ASSERT_EQ(object_1->ClassName(), "DynDerivedComponent1");
    ASSERT_EQ(object_2->ClassName(), "DynDerivedComponent2");

    ASSERT_EQ((*comp_1_idx_ptr).ClassName(), "DynDerivedComponent1");
    ASSERT_EQ((*comp_2_idx_ptr).ClassName(), "DynDerivedComponent2");

    ShmUniquePtr<DynBaseComponent> comp_1_idx_ptr_dup = std::move(comp_1_idx_ptr);
    ShmUniquePtr<DynBaseComponent> comp_2_idx_ptr_dup = std::move(comp_2_idx_ptr);
    auto cur_stat = ShmDynamicArena::Stat();
    ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].first, 3);
    ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].first, 3);
    ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].second, 1);
    ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].second, 1);
    comp_1_idx_ptr = nullptr;
    comp_2_idx_ptr = nullptr;
    ASSERT_EQ(comp_1_idx_ptr_dup->ClassName(), "DynDerivedComponent1");
    ASSERT_EQ(comp_2_idx_ptr_dup->ClassName(), "DynDerivedComponent2");
  }
  auto cur_stat = ShmDynamicArena::Stat();
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].first, 3);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].first, 3);
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].second, 0);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].second, 0);
  auto comp_1_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent1>();
  DynDerivedComponent1 *object_1 = comp_1_idx_ptr.Get();
  ASSERT_NE(object_1, nullptr);
  ASSERT_EQ(object_1->value_2, "init");
  ASSERT_EQ(comp_1_idx_ptr.offset_idx, 0);
  auto comp_2_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent2>();
  DynDerivedComponent2 *object_2 = comp_2_idx_ptr.Get();
  ASSERT_EQ(object_2->value_2, "init");
  ASSERT_EQ(comp_2_idx_ptr.offset_idx, 0);
  cur_stat = ShmDynamicArena::Stat();
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].first, 4);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].first, 4);
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].second, 1);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].second, 1);
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_1_idx_ptr));
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_2_idx_ptr));
}

TEST_F(ShmDynArenaTest, SharedPtr) {
  {
    auto comp_0_idx_ptr = ShmMakeShared<DynBaseComponent>();
    DynBaseComponent *object_0 = comp_0_idx_ptr.get();
    ASSERT_EQ(object_0, nullptr);
    auto comp_1_idx_ptr = ShmMakeShared<DynDerivedComponent1>();
    DynDerivedComponent1 *object_1 = comp_1_idx_ptr.get();
    ASSERT_NE(object_1, nullptr);
    ASSERT_EQ(object_1->value_2, "init");
    auto comp_2_idx_ptr = ShmMakeShared<DynDerivedComponent2>();
    DynDerivedComponent2 *object_2 = comp_2_idx_ptr.get();
    ASSERT_EQ(object_2->value_2, "init");
    std::vector<int> temp_vec;
    temp_vec.push_back(1);
    ASSERT_EQ(object_2->value_3, temp_vec);

    ASSERT_EQ(comp_1_idx_ptr->ClassName(), "DynDerivedComponent1");
    ASSERT_EQ(comp_2_idx_ptr->ClassName(), "DynDerivedComponent2");

    ASSERT_EQ(object_1->ClassName(), "DynDerivedComponent1");
    ASSERT_EQ(object_2->ClassName(), "DynDerivedComponent2");

    ASSERT_EQ((*comp_1_idx_ptr).ClassName(), "DynDerivedComponent1");
    ASSERT_EQ((*comp_2_idx_ptr).ClassName(), "DynDerivedComponent2");

    auto comp_1_idx_ptr_dup = std::move(comp_1_idx_ptr);
    auto comp_2_idx_ptr_dup = std::move(comp_2_idx_ptr);
    ShmSharedPtr<DynBaseComponent> comp_1_idx_ptr_dup_2 = comp_1_idx_ptr_dup;
    ShmSharedPtr<DynBaseComponent> comp_2_idx_ptr_dup_2 = comp_2_idx_ptr_dup;
    ShmSharedPtr<DynDerivedComponent1> comp_1_idx_ptr_dup_3 =
        ShmDynamicPointerCast<DynDerivedComponent1>(comp_1_idx_ptr_dup_2);
    ASSERT_TRUE(comp_1_idx_ptr_dup_3);
     ShmSharedPtr<DynDerivedComponent2> comp_2_idx_ptr_dup_3 =
        ShmDynamicPointerCast<DynDerivedComponent2>(comp_2_idx_ptr_dup_2);
    ASSERT_TRUE(comp_2_idx_ptr_dup_3);
    ASSERT_EQ(comp_1_idx_ptr_dup_3->ClassName(), "DynDerivedComponent1");
    ASSERT_EQ(comp_2_idx_ptr_dup_3->ClassName(), "DynDerivedComponent2");
    comp_1_idx_ptr_dup = nullptr;
    comp_2_idx_ptr_dup = nullptr;
    auto cur_stat = ShmDynamicArena::Stat();
    ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].first, 5);
    ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].first, 5);
    ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].second, 1);
    ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].second, 1);
  }
  auto cur_stat = ShmDynamicArena::Stat();
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].first, 5);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].first, 5);
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].second, 0);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].second, 0);
  auto comp_1_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent1>();
  DynDerivedComponent1 *object_1 = comp_1_idx_ptr.Get();
  ASSERT_NE(object_1, nullptr);
  ASSERT_EQ(object_1->value_2, "init");
  ASSERT_EQ(comp_1_idx_ptr.offset_idx, 0);
  auto comp_2_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent2>();
  DynDerivedComponent2 *object_2 = comp_2_idx_ptr.Get();
  ASSERT_EQ(object_2->value_2, "init");
  ASSERT_EQ(comp_2_idx_ptr.offset_idx, 0);
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_1_idx_ptr));
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_2_idx_ptr));
}

TEST_F(ShmDynArenaTest, Resume) {
  auto comp_0_idx_ptr = ShmDynamicArena::Create<DynBaseComponent>();
  DynBaseComponent *object_0 = comp_0_idx_ptr.Get();
  ASSERT_EQ(object_0, nullptr);
  auto comp_1_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent1>();
  DynDerivedComponent1 *object_1 = comp_1_idx_ptr.Get();
  ASSERT_NE(object_1, nullptr);
  ASSERT_EQ(object_1->value_2, "init");
  auto comp_2_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent2>();
  DynDerivedComponent2 *object_2 = comp_2_idx_ptr.Get();
  ASSERT_EQ(object_2->value_2, "init");
  std::vector<int> temp_vec;
  temp_vec.push_back(1);
  ASSERT_EQ(object_2->value_3, temp_vec);

  ShmDynamicArena::Resume(arena_data);

  ASSERT_EQ(comp_1_idx_ptr.Get(), object_1);

  ASSERT_EQ(comp_2_idx_ptr.Get(), object_2);

  ASSERT_EQ(object_1->value_2, "init_resume");
  ASSERT_EQ(object_1->ClassName(), "DynDerivedComponent1");

  temp_vec.push_back(2);
  ASSERT_EQ(object_2->value_2, "init_resume");
  ASSERT_EQ(object_2->value_3, temp_vec);

  ASSERT_EQ(object_2->ClassName(), "DynDerivedComponent2");
  auto cur_stat = ShmDynamicArena::Stat();
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].first, 7);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].first, 7);
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].second, 1);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].second, 1);
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_1_idx_ptr));
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_2_idx_ptr));
  ASSERT_EQ(comp_1_idx_ptr.Get(), nullptr);
  ASSERT_EQ(comp_2_idx_ptr.Get(), nullptr);
}

TEST_F(ShmDynArenaTest, PostResume) {
  auto comp_0_idx_ptr = ShmDynamicArena::Create<DynBaseComponent>();
  DynBaseComponent *object_0 = comp_0_idx_ptr.Get();
  ASSERT_EQ(object_0, nullptr);
  auto comp_1_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent1>();
  DynDerivedComponent1 *object_1 = comp_1_idx_ptr.Get();
  ASSERT_NE(object_1, nullptr);
  ASSERT_EQ(object_1->value_2, "init");
  auto comp_2_idx_ptr = ShmDynamicArena::Create<DynDerivedComponent2>();
  DynDerivedComponent2 *object_2 = comp_2_idx_ptr.Get();
  ASSERT_EQ(object_2->value_2, "init");
  std::vector<int> temp_vec;
  temp_vec.push_back(1);
  ASSERT_EQ(object_2->value_3, temp_vec);

  ShmDynamicArena::Resume(arena_data);
  ShmDynamicArena::PostResume();
  auto cur_stat = ShmDynamicArena::Stat();
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].first, 8);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].first, 8);
  ASSERT_EQ(cur_stat[DynDerivedComponent1::StaticClassName()].second, 1);
  ASSERT_EQ(cur_stat[DynDerivedComponent2::StaticClassName()].second, 1);

  ASSERT_EQ(comp_1_idx_ptr.Get(), object_1);

  ASSERT_EQ(comp_2_idx_ptr.Get(), object_2);

  ASSERT_EQ(object_1->value_2, "init_resume_post");
  ASSERT_EQ(object_1->ClassName(), "DynDerivedComponent1");

  temp_vec.push_back(2);
  temp_vec.push_back(3);
  ASSERT_EQ(object_2->value_2, "init_resume_post");
  ASSERT_EQ(object_2->value_3, temp_vec);

  ASSERT_EQ(object_2->ClassName(), "DynDerivedComponent2");

  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_1_idx_ptr));
  ASSERT_TRUE(ShmDynamicArena::Destroy(comp_2_idx_ptr));
  ASSERT_EQ(comp_1_idx_ptr.Get(), nullptr);
  ASSERT_EQ(comp_2_idx_ptr.Get(), nullptr);
}

TEST_F(ShmDynArenaTest, SingletonLifeTest) {
  auto signleton_ptr_1_1 = ShmSingletonArena::Get<SingletonTest1>();
  auto signleton_ptr_1_2 = ShmSingletonArena::Get<SingletonTest1>();
  ASSERT_NE(signleton_ptr_1_1, nullptr);
  ASSERT_EQ(signleton_ptr_1_1, signleton_ptr_1_2);
  ASSERT_EQ(signleton_ptr_1_1->value_1, 3456);
  ShmSingletonArena::Dispose<SingletonTest1>();

  auto signleton_ptr_2_1 = ShmSingletonArena::Get<SingletonTest2>();
  auto signleton_ptr_2_2 = ShmSingletonArena::Get<SingletonTest2>();
  ASSERT_NE(signleton_ptr_2_1, nullptr);
  ASSERT_EQ(signleton_ptr_2_1, signleton_ptr_2_2);
  ASSERT_EQ(signleton_ptr_2_1->value_2, 7890);
  ShmSingletonArena::Dispose<SingletonTest2>();
}
