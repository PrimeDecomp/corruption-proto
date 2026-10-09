#ifndef _CMEMORYDRAWENUM
#define _CMEMORYDRAWENUM

#include "types.h"

#include "Kyoto/Alloc/IAllocator.hpp"

// Prime's class (CMemoryDrawEnum.cpp); the prototype also gives it a memory-map display that main's
// "Memory Metrics" option uses. Only what main touches is named.
class CMemoryDrawEnum {
public:
  // 0x800B4B3C: copies the allocator metrics and clears the layout and block statistics.
  explicit CMemoryDrawEnum(const IAllocator::SMetrics& metrics);

  // Guessed name. 0x800B43FC: lays the heap out in the given rectangle (x, y, width, height),
  // walking the allocations and filling in the block statistics below.
  void Draw(int x, int y, int width, int height);

  // Guessed names, after main's "MRAM Used:%d LAB:%d Free:%d LFB:%d" line.
  uint GetLargestFreeBlock() const { return x80_largestFreeBlock; }
  uint GetLargestAllocatedBlock() const { return x84_largestAllocatedBlock; }

  // Guessed name. 0x800B3E44: enumerates the heap's allocations and prints the block, free and
  // allocated counts, byte totals and average sizes (main's "Dump Memory Allocations").
  static void PrintBlockStatistics();

  // Prime's names. CGameArea and CMapArea count their memory here; main prints it as "WLD".
  static void AddWorldMemory(uint size) { sWorldMemory += size; }
  static void SubtractWorldMemory(uint size) { sWorldMemory -= size; }
  static uint GetWorldMemory() { return sWorldMemory; }

private:
  IAllocator::SMetrics mMetrics;
  int x5c_x;
  int x60_y;
  int x64_width;
  int x68_height;
  float x6c_;
  uint x70_;
  uint x74_;
  uint x78_;
  uint x7c_;
  uint x80_largestFreeBlock;
  uint x84_largestAllocatedBlock;

  static uint sWorldMemory;
};
CHECK_SIZEOF(CMemoryDrawEnum, 0x88)

#endif // _CMEMORYDRAWENUM
