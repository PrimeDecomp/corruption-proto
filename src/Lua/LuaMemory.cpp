#include "LuaMemory.h"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Basics/CBasics.hpp"

void* luaHelper_ReallocFunction(void* ptr, int oldsize, int size, void* data,
                              const char* allocName, unsigned int allocFlags) {
  if (oldsize == 0) {
    CMemory::Free(ptr);
    ptr = NULL;
  }

  if (ptr == NULL) {
    return CMemory::Alloc(size, IAllocator::kHI_None, IAllocator::kSC_Unk1,
                          IAllocator::kTP_Heap, CCallStack(-1, allocName, kUnknownType));
  }

  void* newPtr = CMemory::Alloc(size, IAllocator::kHI_None, IAllocator::kSC_Unk1,
                              IAllocator::kTP_Heap, CCallStack(-1, allocName, kUnknownType));
  CBasics::CopyMemory(newPtr, ptr, size < oldsize ? size : oldsize);
  CMemory::Free(ptr);
  return newPtr;
}

void luaHelper_FreeFunction(void* ptr, int oldsize, void* data) {
  CMemory::Free(ptr);
}
