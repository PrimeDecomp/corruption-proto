#ifndef RSTL_AUTO_PTR_HPP
#define RSTL_AUTO_PTR_HPP

#include "Kyoto/Alloc/CMemory.hpp"

namespace rstl {

template < typename T >
class auto_ptr {
  mutable bool mOwns;
  T* mPointer;

public:
  auto_ptr(T* pointer = 0) : mOwns(pointer != 0), mPointer(pointer) {}
  auto_ptr(const auto_ptr& other) : mOwns(other.mOwns), mPointer(other.mPointer) {
    other.mOwns = false;
  }
  ~auto_ptr() {
    if (mOwns) {
      delete mPointer;
    }
  }

  auto_ptr& operator=(const auto_ptr& other) {
    if (&other != this) {
      if (mOwns) {
        delete mPointer;
      }
      mOwns = other.mOwns;
      mPointer = other.mPointer;
      other.mOwns = false;
    }
    return *this;
  }

  T* get() const { return mPointer; }
  T& operator*() const { return *mPointer; }
  T* release() const {
    mOwns = false;
    return mPointer;
  }
  bool null() const { return mPointer == 0; }
  void reset() {
    mOwns = false;
    mPointer = 0;
  }
  T* operator->() const { return mPointer; }
  bool owner() const { return mOwns; }
};

} // namespace rstl

#endif
