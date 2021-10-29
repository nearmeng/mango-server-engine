#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <new>

namespace gw {
template <class TYPE>
class Singleton {
 public:
  static TYPE* GetInstance(void) {
    if (m_instance == NULL) {
      m_instance = new TYPE();
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

 protected:
  Singleton() {}
  ~Singleton() {}

 private:
  Singleton(const Singleton& src){};
  Singleton& operator=(const Singleton& src){};
  static TYPE* m_instance;
};

template <class TYPE>
TYPE* Singleton<TYPE>::m_instance = NULL;

template <class TYPE>
class SingletonRef {
 public:
  static TYPE& GetInstance(void) {
    if (m_instance == NULL) {
      m_instance = new TYPE();
    }
    return *m_instance;
  }
  static void Dispose() {
    if (!m_instance) {
      return;
    }
    delete m_instance;
    m_instance = nullptr;
  }

 protected:
  SingletonRef() {}
  ~SingletonRef() {}

 private:
  SingletonRef(const SingletonRef& src){};
  SingletonRef& operator=(const SingletonRef& src){};
  static TYPE* m_instance;
};

template <class TYPE>
TYPE* SingletonRef<TYPE>::m_instance = NULL;
}