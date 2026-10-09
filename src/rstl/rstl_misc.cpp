#include "Kyoto/Alloc/CMemory.hpp"
#include "rstl/rmemory_allocator.hpp"

void* rstl::rmemory_allocator::allocate(int size) {
  return size == 0 ? 0 : rs_new(36) unsigned char[size];
}
