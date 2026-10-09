#ifndef _CFLUIDPLANEMANAGER
#define _CFLUIDPLANEMANAGER

#include "types.h"

#include "Kyoto/Math/CVector3f.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/reserved_vector.hpp"

// Minimal view (CFluidPlaneManager.cpp). CStateManager news one (0x11C bytes, Echoes' size); its
// destructor, inlined into CStateManager's, only walks the splash records, so the layout is
// Echoes'.
class CFluidPlaneManager {
public:
  // Echoes' class and names.
  class CFluidProfile {
  public:
    void Clear(); // 0x800F7AD8

  private:
    float x0_;
    float x4_;
    float x8_;
    float xc_;
    float x10_;
  };

  // Echoes' class: 8 bytes.
  class CSplashRecord {
  public:
    ~CSplashRecord() {}

    void SetTime(float time) { mTime = time; }

  private:
    float mTime;
    TUniqueId mId;
  };

  CFluidPlaneManager();  // 0x800F7A3C
  void Update(float dt); // Echoes' name; 0x800F79C8

  static CFluidProfile sProfile; // 0x8078178C

private:
  // Echoes' names.
  rstl::reserved_vector< CSplashRecord, 32 > mSplashes;
  CVector3f mLastSplashPosition;
  float mSplashCooldown;
  float mUvTime;
  bool x118_;
  bool mFrameActive;
};
CHECK_SIZEOF(CFluidPlaneManager, 0x11C)

#endif // _CFLUIDPLANEMANAGER
