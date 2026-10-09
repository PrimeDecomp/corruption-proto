// NonMatching translation-unit scaffold.
// G2MEAB .text 0x800F7350..0x800F7B24 (end exclusive).
// Echoes' CFluidPlaneManager.cpp. Listed below are the functions not implemented yet.
// 0x800F7350 +0x534
// 0x800F7884 +0x4C
// 0x800F78D0 +0xF8
// 0x800F79C8 +0x74

#include "MetroidPrime/CFluidPlaneManager.hpp"

CFluidPlaneManager::CFluidProfile CFluidPlaneManager::sProfile;

void CFluidPlaneManager::CFluidProfile::Clear() {
  x10_ = 0.f;
  xc_ = 0.f;
  x8_ = 0.f;
  x4_ = 0.f;
  x0_ = 0.f;
}

// Unlike Echoes, mFrameActive is left uninitialized.
CFluidPlaneManager::CFluidPlaneManager()
: mLastSplashPosition(CVector3f::Zero()), mSplashCooldown(0.f), mUvTime(0.f), x118_(false) {
  sProfile.Clear();
  for (CSplashRecord* it = mSplashes.begin(); it != mSplashes.end(); ++it) {
    it->SetTime(9999.f);
  }
}
