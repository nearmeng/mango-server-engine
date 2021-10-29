#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <new>
#include "great_world_base/common/shm/shm_mgr.h"

namespace gw {

template <class TYPE>
class ShmSingleton {
 public:
  static TYPE* GetInstance(void) {
    if (m_instance == NULL) {
      m_instance = ShmNew<TYPE>();
    }
    return m_instance;
  }
  static void Dispose() {
    if (!m_instance) {
      return;
    }
    delete m_instance;
    m_instance = nullptr;
  }
  static void InstanceResume(TYPE* p) {
    m_instance = p;
  }

 protected:
  ShmSingleton() {}
  ~ShmSingleton() {}
  ShmSingleton(const ShmSingleton& src){};
  ShmSingleton& operator=(const ShmSingleton& src){};

 private:
  static TYPE* m_instance;
};

template <class TYPE>
TYPE* ShmSingleton<TYPE>::m_instance = NULL;

template <class TYPE>
class ShmSingletonRef {
 public:
  static TYPE& GetInstance(void) {
    if (m_instance == NULL) {
      m_instance = ShmNew<TYPE>();
    }
    return *m_instance;
  }
  static void Dispose() {
    if (!m_instance) {
      return;
    }
    ShmDelete<TYPE>(m_instance);
    m_instance = nullptr;
  }
  static void InstanceResume(TYPE* p) {
    m_instance = p;
  }

 protected:
  ShmSingletonRef() {}
  ~ShmSingletonRef() {}
  ShmSingletonRef(const ShmSingletonRef& src){};
  ShmSingletonRef& operator=(const ShmSingletonRef& src){};

 private:
  static TYPE* m_instance;
};

template <class TYPE>
TYPE* ShmSingletonRef<TYPE>::m_instance = NULL;
}