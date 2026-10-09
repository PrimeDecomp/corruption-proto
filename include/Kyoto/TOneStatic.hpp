#ifndef _TONESTATIC
#define _TONESTATIC

#include "stdio.h"
#include "types.h"

#include "Kyoto/Alloc/Assert.hpp"

// A class that is only ever allocated once, into static storage. The prototype keeps the
// TOneStatic.h checks (lines 51/52 and 81/82); see the main.cpp instantiations at 0x8000F2CC
// (operator new), 0x8000F3E8 (operator delete), 0x8000F50C and 0x8000F518.
template < typename T >
class TOneStatic {
public:
  void* operator new(size_t size, const char*, const char*);
  void* operator new(size_t sz) { return operator new(sz, "??(??)", nullptr); }
  void operator delete(void* ptr);

private:
  static void* GetAllocSpace();
  static uint& ReferenceCount();
};

template < typename T >
void* TOneStatic< T >::GetAllocSpace() {
  static uchar sAllocSpace[sizeof(T)];
  return &sAllocSpace;
}

template < typename T >
uint& TOneStatic< T >::ReferenceCount() {
  static uint sReferenceCount = 0;
  return sReferenceCount;
}

template < typename T >
void* TOneStatic< T >::operator new(size_t size, const char*, const char*) {
  uint& refCount = ReferenceCount();
  if (refCount != 0) {
    RS_VERIFY_FAILURE_IN("TOneStatic.h", 51, "refCount == 0", "false",
                         "Bad reference count for static new'd class");
  }
  if (size > sizeof(T)) {
    RS_VERIFY_FAILURE_IN("TOneStatic.h", 52, "size <= sizeof ( T )", "false",
                         "Bad size passed for operator new");
  }
  ++refCount;
  return GetAllocSpace();
}

template < typename T >
void TOneStatic< T >::operator delete(void* ptr) {
  uint& refCount = ReferenceCount();
  if (refCount != 1) {
    RS_VERIFY_FAILURE_IN("TOneStatic.h", 81, "refCount == 1", "false",
                         "Bad reference count for static delete'd class");
  }
  if (!(ptr == GetAllocSpace())) {
    RS_VERIFY_FAILURE_IN("TOneStatic.h", 82, "p == GetAllocSpace ()", "false",
                         "ERROR - Trying to delete the wrong pointer!");
  }
  --refCount;
}

#endif // _TONESTATIC
