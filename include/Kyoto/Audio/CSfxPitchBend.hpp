#ifndef _CSFXPITCHBEND
#define _CSFXPITCHBEND

#include "Kyoto/Audio/CAudioHandle.hpp"

// Guessed name: a timed pitch transition owned by CAudioManager.
class CSfxPitchBend {
public:
  CSfxPitchBend(const CAudioHandle& handle, ushort start, ushort target, float duration);
  // Guessed method names, supported by the manager's update/apply/retire sequence.
  void Update(float dt);
  bool IsFinished() const;
  const CAudioHandle& GetHandle() const { return mHandle; }
  ushort GetPitch() const { return mPitch; }

private:
  CAudioHandle mHandle;
  ushort mPitch;
  ushort mTargetPitch;
  float mTimeRemaining;
};
CHECK_SIZEOF(CSfxPitchBend, 0xc)

#endif // _CSFXPITCHBEND
