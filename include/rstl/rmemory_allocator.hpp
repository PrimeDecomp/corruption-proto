#ifndef RSTL_RMEMORY_ALLOCATOR_HPP
#define RSTL_RMEMORY_ALLOCATOR_HPP

#include "types.h"

namespace rstl {

struct rmemory_allocator {
  rmemory_allocator() {}
  rmemory_allocator(const rmemory_allocator&) {}
  static void* allocate(int size);

  template < typename T >
  static void allocate(T*& output, int size) {
    output = reinterpret_cast< T* >(allocate(size));
  }
};

} // namespace rstl

#endif
