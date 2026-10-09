#ifndef _CIOWINMANAGER
#define _CIOWINMANAGER

#include "types.h"

// Minimal view of the IOWin manager (CIOWinManager.cpp, 0x80033B7C..0x80034A5C) for main.cpp.
// Names and the two list roots follow Echoes; RsMain tests both roots for emptiness inline and
// draws through 0x80033E48. The rest of the 0x20 bytes is not modelled.
class CIOWinManager {
public:
  void Draw() const;
  bool IsEmpty() const { return mPumpRoot == nullptr && mDrawRoot == nullptr; }

private:
  void* mDrawRoot;
  void* mPumpRoot;
  uchar x8_[0x18];
};

#endif // _CIOWINMANAGER
