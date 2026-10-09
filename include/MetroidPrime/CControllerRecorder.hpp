#ifndef _CCONTROLLERRECORDER
#define _CCONTROLLERRECORDER

#include "types.h"

// Minimal view of the prototype's demo/controller recorder (CControllerRecorder.cpp,
// 0x80120D74..0x8012478C), embedded in CInputGenerator at 0x10. Neither Echoes nor Prime has
// it. Only what the architecture tick reads is placed; the rest is padding. Guessed names: the
// recorder prints "Game Speed %.2f", and the tick runs exactly one normal frame when the step
// flag is set, then clears it.
class CControllerRecorder {
public:
  float GetGameSpeed() const { return mGameSpeed; }
  bool GetStepFrame() const { return mStepFrame; }
  void ClearStepFrame(); // 0x8012124C

private:
  uchar x0_[0x30];
  float mGameSpeed;
  bool mStepFrame : 1;
  uchar x35_[0xb];
};
CHECK_SIZEOF(CControllerRecorder, 0x40)

#endif // _CCONTROLLERRECORDER
