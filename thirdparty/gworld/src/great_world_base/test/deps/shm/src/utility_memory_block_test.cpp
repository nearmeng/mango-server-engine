#include "utility_memory_block.h"
#include "gtest/gtest.h"

TEST(MemoryBlock, Size) {
  MemoryBlock<10>* p1 = nullptr;
  ASSERT_EQ(sizeof(*p1), 10);

  MemoryBlock<kArraySizeMax + 10>* p2 = nullptr;
  ASSERT_EQ(sizeof(*p2), kArraySizeMax + 10);

  MemoryBlock<kArraySizeMax* 2>* p3 = nullptr;
  ASSERT_EQ(sizeof(*p3), kArraySizeMax * 2);
}

struct Test1 {
  int v;
  int v1;
};

struct alignas(16) Test2 {
  int v;
  int v1;
};
struct Test3 {
  alignas(32) int v;
  alignas(16) int v1;
};

TEST(MemoryBlock, Align) {
  int a1 = alignof(Test1);
  int a2 = alignof(Test2);
  int a3 = alignof(Test3);

  int s1 = sizeof(Test1);
  int s2 = sizeof(Test2);
  int s3 = sizeof(Test3);

  char sz[512];
  auto p1 = new (sz + 1) Test1;
  auto p2 = new (sz + 1) Test2;
  auto p3 = new (sz + 1) Test3;

  auto p30 = &p3->v;
  auto p31 = &p3->v1;
  auto p33 = p31 - p30;

  auto o30 = offsetof(Test3, v);
  auto o31 = offsetof(Test3, v1);

  Test1 t1;
  Test2 t2;
  Test3 t3;

  auto pt1 = &t1;
  auto pt2 = &t2;
  auto pt3 = &t3;


  ASSERT_EQ(a2, 16);
}
