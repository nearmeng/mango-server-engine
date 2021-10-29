#pragma once

#include <cassert>
#include "resuming_guard.h"

struct BaseComponent {
  BaseComponent() {
    if (!ShmIsResuming()) {
      value1 = 1;
    } else {
      value1 = 2;
    }
  }

  virtual int Test() { return value1; }

  int value1;
};

struct DerivedComponent : BaseComponent {
  DerivedComponent() {
    if (!ShmIsResuming()) {
      value2 = 10;
    }
  }

  int Test() override { return value2; }

  virtual bool OnResume() {
    assert(ShmIsResuming() == true);
    value2 = 100;
    return true;
  }

  int value2;
};

struct Derived2Component : DerivedComponent {
  Derived2Component() = default;

  Derived2Component(int i) {
    if (!ShmIsResuming()) {
      value2 = i;
    }
  }

  bool OnResume() override {
    value3.OnResume();
    return true;
  }

  BaseComponent* GetValue3() { return &value3; }

  DerivedComponent value3;
};
