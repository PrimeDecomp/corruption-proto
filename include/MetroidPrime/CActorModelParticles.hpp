#ifndef _CACTORMODELPARTICLES
#define _CACTORMODELPARTICLES

#include "types.h"

#include "Kyoto/TToken.hpp"

#include "rstl/list.hpp"
#include "rstl/reserved_vector.hpp"
#include "rstl/vector.hpp"

class CAudioSoundEffect;
class CElectricDescription;
class CGenDescription;

// Minimal view (CActorModelParticles.cpp). CStateManager news one (0x140 bytes). The layout is
// Echoes' with three CAUD tokens added after the ash texture: the members are placed from
// CStateManager.cpp's instance of the implicit destructor (0x80296AF4) and named after the
// resources the constructor (0x8016751C) loads into them.
class CActorModelParticles {
public:
  // Echoes' enum; the constructor loads the effects in this order from the name table.
  enum ESystemTypes {
    kST_OnFire,
    kST_Ice,
    kST_Ash,
    kST_FirePop,
    kST_Electric,
    kST_IcePop,
    kST_BlackHole, // Guessed name (Echoes'); Effect_Blackhole resource.
    kST_Imploder,  // Guessed name (Echoes'); Effect_Imploder resource.
  };

  // Echoes' struct, 0x18 bytes: the eight of them end where the item list starts.
  struct CSystem {
    rstl::vector< CToken > mTokens;
    int mRefCount;
    bool mLoaded;
  };

  // Echoes' class. Only its size is modelled: the list allocates 0x184-byte nodes (0x80165DF0).
  class CItem {
  public:
    ~CItem();

  private:
    uchar x0_[0x17C];
  };

  CActorModelParticles(); // 0x8016751C

private:
  // Echoes' name (0x801655B4): one CSystem per "<effect>_DGRP" dependency group.
  void InitializeSystemTypes();

  // Echoes' names, loaded from the "Effect_*" dependency-group names in this order.
  TToken< CGenDescription > mOnFire;
  TToken< CGenDescription > mAsh;
  TToken< CGenDescription > mIceBreak;
  TToken< CGenDescription > mFirePop;
  TToken< CGenDescription > mIcePop;
  TToken< CGenDescription > mBlackHole;
  TToken< CGenDescription > mImploder;
  TToken< CElectricDescription > mElectric;
  CToken mAshy; // "TXTR_Ashy"
  // Guessed names, after the resources.
  TToken< CAudioSoundEffect > mCreatureBurningSfx; // "CAUD_CreatureBurningLP"
  TToken< CAudioSoundEffect > mCreatureBurnedSfx;  // "CAUD_CreatureBurned"
  TToken< CAudioSoundEffect > mHitFireLoopSfx;     // "CAUD_HitFireLoop"
  rstl::reserved_vector< CSystem, 8 > mDgrps;      // Echoes' name
  rstl::list< CItem > mItems;                      // Echoes' name
  uchar x13C_[0x140 - 0x13C];
};
CHECK_SIZEOF(CActorModelParticles, 0x140)

#endif // _CACTORMODELPARTICLES
