#include "rstl/rmemory_allocator.hpp"
#include "Kyoto/Alloc/CMemory.hpp"

void* rstl::rmemory_allocator::allocate(int size) {
  return size == 0 ? 0 : new ("rstl_misc.cpp(36) : ", (const char*)0) unsigned char[size];
}
