#ifndef RSTL_RMEMORY_ALLOCATOR_HPP
#define RSTL_RMEMORY_ALLOCATOR_HPP

#include "types.h"
#include "Kyoto/Alloc/CMemory.hpp"

namespace rstl {

struct rmemory_allocator {
  rmemory_allocator() {}
  rmemory_allocator(const rmemory_allocator&) {}
  static void* allocate(int size);

  template < typename T >
  static void allocate(T*& output, int size) {
    output = reinterpret_cast< T* >(allocate(size * sizeof(T)));
  }
  template < typename T >
  static T* allocate2(int count) {
    int size = count * sizeof(T);
    return size == 0 ? 0 : reinterpret_cast< T* >(new uchar[size]);
  }
  template < typename T >
  static void deallocate(T* ptr) { delete[] reinterpret_cast< uchar* >(ptr); }
};

struct aligned_allocator {
  aligned_allocator() {}
  aligned_allocator(const aligned_allocator&) {}
  template < typename T >
  static void allocate(T*& output, int count) {
    const int size = count * sizeof(T);
    output = size == 0 ? 0 : static_cast< T* >(CMemory::Alloc(size, IAllocator::kHI_RoundUpLen));
  }
  template < typename T >
  static void deallocate(T* ptr) { delete[] reinterpret_cast< uchar* >(ptr); }
};

} // namespace rstl

#endif
