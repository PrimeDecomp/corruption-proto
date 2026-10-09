#ifndef _CMEMORY
#define _CMEMORY

#include "Kyoto/Alloc/CCallStack.hpp"
#include "Kyoto/Alloc/IAllocator.hpp"
#include "types.h"

class COsContext;
class CMemory {
  static IAllocator* mpAllocator;
  static bool mInitialized;

public:
  static void Startup(COsContext& ctx);
  static void Shutdown();
  static void SetAllocator(COsContext& ctx, IAllocator& allocator);
  static IAllocator* GetAllocator();
  static void* Alloc(size_t len, IAllocator::EHint hint = IAllocator::kHI_None,
                     IAllocator::EScope scope = IAllocator::kSC_Unk1,
                     IAllocator::EType type = IAllocator::kTP_Heap,
                     const CCallStack& callstack = CCallStack(-1, "??(??)"));
  static void Free(const void* ptr);
  static void SetOutOfMemoryCallback(IAllocator::FOutOfMemoryCb callback, const void* context);
  static IAllocator::SMetrics GetMetrics(bool unk1, bool unk2);
  static void OffsetFakeStatics(int);
};

#if defined(__MWERKS__) || defined(CLANGD)
void* operator new(size_t sz, const char*, const char*);
void* operator new[](size_t sz, const char*, const char*);
// TODO remove

inline void* operator new(size_t sz) { return operator new(sz, "??(??)", nullptr); }
inline void* operator new[](size_t sz) { return operator new[](sz, "??(??)", nullptr); }
#else
/*__attribute__((weak)) void* operator new(size_t sz) { return operator new(sz, "??(??)", nullptr);
}
__attribute__((weak)) void* operator new[](size_t sz) {
  return operator new[](sz, "??(??)", nullptr);
}*/
#endif

// placement new
inline void* operator new(size_t n, void* ptr) { return ptr; };

#ifdef __MWERKS__
inline void operator delete(void* ptr) { CMemory::Free(ptr); }
inline void operator delete[](void* ptr) { CMemory::Free(ptr); }
// Allocations without a source position ("??(??)", escaped so it is not read as trigraphs).
#define NEW new ("\?\?(\?\?)", nullptr)
// Allocations that record their source position, as Prime's and Echoes' rs_new does with
// __FILE__ and __LINE__. The line is explicit here so the original line numbers survive in the
// strings.
#define RS_NEW_STRINGIZE_IMPL(x) #x
#define RS_NEW_STRINGIZE(x) RS_NEW_STRINGIZE_IMPL(x)
#define rs_new(line) rs_new_in(__FILE__, line)
// Same, for sources whose original file name differs from ours (Main.cpp, DolphinCFont.cpp).
#define rs_new_in(file, line) new (file "(" RS_NEW_STRINGIZE(line) ") : ", nullptr)
#else
__attribute__((weak)) void operator delete(void* ptr) { CMemory::Free(ptr); }
__attribute__((weak)) void operator delete[](void* ptr) { CMemory::Free(ptr); }
#define NEW new
#define rs_new(line) new
#define rs_new_in(file, line) new
#endif

#endif // _CMEMORY
