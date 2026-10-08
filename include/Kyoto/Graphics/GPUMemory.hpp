#ifndef _GPUMEMORY
#define _GPUMEMORY

#include "types.h"

// Interface name follows Corruption's DolphinGPUMemory.cpp diagnostics.
// Method names describe the native operations; no original Echoes export is known.
class GPUMemory {
public:
  static void SetBuffer(void* buffer, uint size);
  static void* EnsureAllocation(int size);
  static void ReleaseAllocation();
  static void TickAllocations();
};

#endif // _GPUMEMORY
