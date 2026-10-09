#ifndef _CGAMEARCHITECTURESUPPORT
#define _CGAMEARCHITECTURESUPPORT

#include "types.h"

#include "Kyoto/Basics/CStopwatch.hpp"
#include "Kyoto/TOneStatic.hpp"
#include "MetroidPrime/CIOWinManager.hpp"

class COsContext;

// Layout from the constructor (0x8000BEFC) and destructor (0x8000BC04), both in main.cpp; it
// lives in TOneStatic storage (0xC8 bytes). Compared with Echoes the prototype has no audio
// system member and no infinite-loop alarm; it adds an object at 0x98 (constructed by
// 0x8020C338 and published at 0x807990EC). Unmodelled members are padding.
class CGameArchitectureSupport : public TOneStatic< CGameArchitectureSupport > {
public:
  CGameArchitectureSupport(COsContext& context);
  ~CGameArchitectureSupport();

  bool UpdateTicks();
  void Update();

  CStopwatch& GetStopwatch1() { return mTickStopwatch; }
  CStopwatch& GetStopwatch2() { return mDrawStopwatch; }
  CIOWinManager& GetIOWinManager() { return mIoWinMgr; }
  int& GetFramesDrawn() { return mGameFrameCount; }

private:
  uchar x0_archQueue[0x18]; // CArchitectureQueue; destroyed by 0x8000FC48.
  CStopwatch mTickStopwatch;
  CStopwatch mDrawStopwatch;
  uchar x28_inputGenerator[0x50]; // CInputGenerator.
  CIOWinManager mIoWinMgr;
  uchar x98_[0x20];
  int mGameFrameCount;
  float mTickRemainder;
  float mPreviousTickRemainder2;
  float mPreviousTickRemainder;
};
CHECK_SIZEOF(CGameArchitectureSupport, 0xc8)

#endif // _CGAMEARCHITECTURESUPPORT
