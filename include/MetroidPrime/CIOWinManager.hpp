#ifndef _CIOWINMANAGER
#define _CIOWINMANAGER

#include "types.h"

#include "MetroidPrime/CArchitectureQueue.hpp"

// Minimal view of the IOWin manager (CIOWinManager.cpp, 0x80033B7C..0x80034A5C) for main.cpp.
// Names and layout follow Echoes: the draw and pump list roots and a local message queue.
// RsMain tests both roots for emptiness inline and draws through 0x80033E48.
class CIOWinManager {
public:
  CIOWinManager();  // 0x800349EC
  ~CIOWinManager(); // 0x80034988

  void Draw() const;
  void PumpMessages(CArchitectureQueue& queue); // 0x800342A4
  void RemoveAllIOWins();                       // 0x8003461C
  bool IsEmpty() const { return mPumpRoot == nullptr && mDrawRoot == nullptr; }

private:
  void* mDrawRoot;
  void* mPumpRoot;
  CArchitectureQueue mLocalQueue;
};
CHECK_SIZEOF(CIOWinManager, 0x20)

// 0x80797104; CGameArchitectureSupport publishes its manager here.
extern CIOWinManager* gpIOWinManager;

#endif // _CIOWINMANAGER
