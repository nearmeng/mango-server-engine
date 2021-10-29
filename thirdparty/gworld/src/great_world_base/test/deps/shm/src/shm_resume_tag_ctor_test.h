#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "shm_pool_tag_ctor_handler.h"

struct ConsDefault {
  int int_value_ = 1;
  std::string str_value_;
};
struct ConsFirst {
  int int_value_;
  std::string str_value_;
  ConsFirst() {
    int_value_ = 0;
    str_value_ = "default_constructor";
  }
  ConsFirst(ConsFirst& other, const ResumeConstructorFlag& flag)
      : str_value_(std::move(other.str_value_)) {
    int_value_ = 2;
    str_value_ += "_resume";
  }
};

struct ConsSecond : public ConsFirst {
  std::vector<int> vec_value_;
  ConsSecond() : ConsFirst() {
    vec_value_.push_back(1);
    vec_value_.push_back(2);
  }

  ConsSecond(ConsSecond& other, const ResumeConstructorFlag& flag)
      : ConsFirst(other, flag), vec_value_(std::move(other.vec_value_)) {
    vec_value_.push_back(3);
  }
};
