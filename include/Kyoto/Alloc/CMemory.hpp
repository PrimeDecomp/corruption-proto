#ifndef KYOTO_ALLOC_CMEMORY_HPP
#define KYOTO_ALLOC_CMEMORY_HPP

class CMemory {
public:
  static void Free(const void* pointer);
};

void* operator new(unsigned long size, const char* source, const char* context);
void* operator new[](unsigned long size, const char* source, const char* context);

inline void operator delete(void* pointer) { CMemory::Free(pointer); }
inline void operator delete[](void* pointer) { CMemory::Free(pointer); }

#endif
