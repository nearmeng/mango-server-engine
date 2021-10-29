#include "shm_resume_tag_ctor_test.h"
#include "gtest/gtest.h"

TEST(ShmResumeTagCtor, resume) {
  ConsDefault object_0;
  ConsFirst object_1;
  ConsSecond object_2;
  object_0.int_value_ = 3;
  object_0.str_value_ = "hahah";

  ResumeConstruct<ConsDefault>(&object_0);
  ResumeConstruct<ConsFirst>(&object_1);
  ResumeConstruct<ConsSecond>(&object_2);
  ASSERT_EQ(object_0.int_value_, 1);
  ASSERT_EQ(object_0.str_value_, "");

  ASSERT_EQ(object_1.int_value_, 2);
  ASSERT_EQ(object_1.str_value_, "default_constructor_resume");

  ASSERT_EQ(object_2.int_value_, 2);
  ASSERT_EQ(object_2.str_value_, "default_constructor_resume");
  ASSERT_EQ(object_2.vec_value_.size(), 3);
  for (std::size_t i = 0; i < 3; i++) {
    ASSERT_EQ(object_2.vec_value_[i], i + 1);
  }
}