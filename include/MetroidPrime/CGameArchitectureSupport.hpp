#ifndef _CGAMEARCHITECTURESUPPORT
#define _CGAMEARCHITECTURESUPPORT

#include "types.h"

#include "Kyoto/Basics/CStopwatch.hpp"
#include "Kyoto/TOneStatic.hpp"
#include "MetroidPrime/CArchitectureQueue.hpp"
#include "MetroidPrime/CIOWinManager.hpp"
#include "MetroidPrime/CInputGenerator.hpp"
#include "MetroidPrime/CScanTextDebugManager.hpp"

class COsContext;

// Layout from the constructor (0x8000BEFC) and destructor (0x8000BC04), both in main.cpp; it
// lives in TOneStatic storage (0xC8 bytes). Compared with Echoes the prototype has no audio
// system member and no infinite-loop alarm; it adds the scan-text debug manager at 0x98
// (published at 0x807990EC).
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
  CArchitectureQueue mArchQueue;
  CStopwatch mTickStopwatch;
  CStopwatch mDrawStopwatch;
  CInputGenerator mInputGenerator;
  CIOWinManager mIoWinMgr;
  CScanTextDebugManager mScanTextDebugManager;
  int mGameFrameCount;
  float mTickRemainder;
  float mPreviousTickRemainder2;
  float mPreviousTickRemainder;
};
CHECK_SIZEOF(CGameArchitectureSupport, 0xc8)

#endif // _CGAMEARCHITECTURESUPPORT
