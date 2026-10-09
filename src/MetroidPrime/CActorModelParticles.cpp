// NonMatching translation-unit scaffold.
// G2MEAB .text 0x801652A0..0x80169DE4 (end exclusive).
// Echoes' CActorModelParticles.cpp. Only the constructor is implemented.

#include "MetroidPrime/CActorModelParticles.hpp"

#include "Kyoto/CSimplePool.hpp"

static const char* const skParticleNames[] = {
    "Effect_OnFire",   "Effect_IceBreak", "Effect_Ash",       "Effect_FirePop",
    "Effect_Electric", "Effect_IcePop",   "Effect_Blackhole", "Effect_Imploder",
};

// Echoes' constructor with three sound tokens added.
CActorModelParticles::CActorModelParticles()
: mOnFire(gpSimplePool->GetObj(skParticleNames[kST_OnFire]))
, mAsh(gpSimplePool->GetObj(skParticleNames[kST_Ash]))
, mIceBreak(gpSimplePool->GetObj(skParticleNames[kST_Ice]))
, mFirePop(gpSimplePool->GetObj(skParticleNames[kST_FirePop]))
, mIcePop(gpSimplePool->GetObj(skParticleNames[kST_IcePop]))
, mBlackHole(gpSimplePool->GetObj(skParticleNames[kST_BlackHole]))
, mImploder(gpSimplePool->GetObj(skParticleNames[kST_Imploder]))
, mElectric(gpSimplePool->GetObj(skParticleNames[kST_Electric]))
, mAshy(gpSimplePool->GetObj("TXTR_Ashy"))
, mCreatureBurningSfx(gpSimplePool->GetObj("CAUD_CreatureBurningLP"))
, mCreatureBurnedSfx(gpSimplePool->GetObj("CAUD_CreatureBurned"))
, mHitFireLoopSfx(gpSimplePool->GetObj("CAUD_HitFireLoop")) {
  InitializeSystemTypes();
}
