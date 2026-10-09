#ifndef _CMAIN
#define _CMAIN

#include "types.h"

#include "Kyoto/Streams/CStreamPreloadedToken.hpp"
#include "Kyoto/TReservedAverage.hpp"
#include "Kyoto/TSignal.hpp"

#include "rstl/list.hpp"
#include "rstl/reserved_vector.hpp"
#include "rstl/single_ptr.hpp"
#include "rstl/vector.hpp"

class COsContext;
class CSaveRegion;
class CMemorySys;
class CDvdRequestSys;
class CGameGlobalObjects;
class CGameArchitectureSupport;
class CStopwatch;

// Six signals embedded in CMain at 0x70 with an out-of-line implicit constructor (0x8000EE28)
// and destructor (0x8000EAC4). The game architecture fires the first three around the tick
// loop (0x70 before, 0x88 once per 1/60 s tick with the tick time, 0xa0 after) and RsMain the
// last three around drawing and the architecture update (0xb8, 0xd0, 0xe8). Neither Prime nor
// Echoes has an equivalent. Guessed names.
struct SMainFrameCallbacks {
  TSignal0 x0_preTick;
  TSignal1< float > x18_tick;
  TSignal0 x30_postTick;
  TSignal0 x48_preDraw;
  TSignal0 x60_postDraw;
  TSignal0 x78_postUpdate;
};

class CMain {
public:
  enum ERestartMode {
    // Echoes values; the prototype constructor also defaults to 6.
    kRM_None,
    kRM_Credits1,
    kRM_Credits2,
    kRM_EndMovie1,
    kRM_EndAutoSave,
    kRM_EndMovie2,
    kRM_Default,
    kRM_StateSetter,
  };

  CMain(COsContext* context, CSaveRegion* saveRegion, CMemorySys* memorySys,
        CDvdRequestSys* dvdRequestSys);
  ~CMain();

  int RsMain(int argc, const char* const* argv);
  void InitializeSubsystems();
  void ShutdownSubsystems();
  bool CheckReset();
  void ResetGameState();
  void MemoryCardInitializePump();
  int GetLanguage() const;
  void AsyncIdle(uint time);
  bool CheckTerminate();
  void AddWorldPaks();
  void DrawDebugMetrics(double dt, CStopwatch& stopWatch);

  // Guessed name: the "screenshot" console command prints "Taking screenshot.\n" and calls
  // this, which only raises a flag consumed by the debug draw path.
  void TakeScreenshot();

  void SetRestartMode(ERestartMode mode) { mRestartMode = mode; }
  ERestartMode GetRestartMode() const { return mRestartMode; }

  void SetMaxSpeed(bool enabled);
  bool IsMaxSpeed();
  // Guessed names inherited from Echoes; the flag forces 30-FPS ticks and frame waits.
  void SetThirtyFps(bool enabled);
  bool GetThirtyFps() const { return mThirtyFps; }

  void SetGameExitReset(bool reset) { mGameExitReset = reset; }
  void SetManageCard(bool manage) { mManageCard = manage; }
  void SetGameFrameDrawn(bool drawn) { mGameFrameDrawn = drawn; }
  void SetGameFlowBuilt(bool built) { mMfGameBuilt = built; }

  void DecrementMaxSpeedDrawTimer(float dt) { mMaxSpeedDrawTimer -= dt; }
  bool GetFinished() const { return mFinished; }
  float GetAverageTickTime() const { return mAverageTickTime; }
  float GetAverageDrawTime() const { return mAverageDrawTime; }

  SMainFrameCallbacks& FrameCallbacks() { return x70_frameCallbacks; }

private:
  // Prototype-only leading vector; its element type is trivially destructible (the
  // destructor only frees the storage). Unknown use.
  rstl::vector< uint > x0_;
  // Echoes keeps these four pointers at 0x0; the prototype shifts them behind the vector.
  COsContext* mOsContext;
  CSaveRegion* mSaveRegion;
  CMemorySys* mMemorySys;
  CDvdRequestSys* mDvdRequestSys;
  double x20_;
  TReservedAverage< float, 4 > mTickTimes;
  TReservedAverage< float, 4 > mDrawTimes;
  float mAverageTickTime;
  float mAverageDrawTime;
  uint mFrameTimeMinimum;
  float mSoftResetHoldTime;
  float mResetInputDelay;
  CGameGlobalObjects* mGameGlobalObjects;
  ERestartMode mRestartMode;
  float mMaxSpeedDrawTimer; // Guessed name.
  SMainFrameCallbacks x70_frameCallbacks;
  rstl::reserved_vector< uint, 10 > mFrameTimes;
  int mFrameTimeIdx;
  // Released early by CMFGame once something else holds a reference (0x80009888).
  rstl::single_ptr< CStreamPreloadedToken > x130_streamToken;
  bool mFinished : 1;
  bool mMfGameBuilt : 1;
  bool mIsMaxSpeed : 1; // Guessed name: cinematic-skip fast-forward.
  bool mResetButtonHeld : 1;
  bool mManageCard : 1;
  bool mResetRequested : 1;
  bool mGameExitReset : 1;
  bool mGameFrameDrawn : 1;
  bool mThirtyFps : 1;
  CGameArchitectureSupport* mArchSupport;
};
CHECK_SIZEOF(CMain, 0x140)

extern CMain* gpMain;

#endif // _CMAIN
