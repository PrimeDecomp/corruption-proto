#ifndef _CENVFXMANAGER
#define _CENVFXMANAGER

#include "types.h"

#include "Kyoto/Audio/CAudioHandle.hpp"
#include "Kyoto/Math/CAABox.hpp"
#include "Kyoto/Math/CVector2i.hpp"
#include "Kyoto/Math/CVector3f.hpp"
#include "Kyoto/TToken.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/pair.hpp"
#include "rstl/reserved_vector.hpp"
#include "rstl/vector.hpp"

class CAudioSoundEffect;
class CStateManager;
class CGenDescription;
class CTexture;

// Echoes' 8.8 fixed-point vector.
struct CVectorFixed8_8 {
  short mX;
  short mY;
  short mZ;
};

// Echoes' class and names, 0x4C bytes, with an implicit destructor (CEnvFxManager.cpp emits the
// grid vector's destructor, 0x80186148, and its element loop, 0x80186198).
class CEnvFxManagerGrid {
public:
  CEnvFxManagerGrid(const CVector2i& position, const CVector2i& extent,
                    const rstl::vector< CVectorFixed8_8 >& initialParticles,
                    int reserve); // 0x8018609C

private:
  bool mBlockDirty : 1;
  CVector2i mPosition;              // 8.8 fixed point
  CVector2i mExtent;                // 8.8 fixed point
  rstl::pair< bool, float > mBlock; // Visibility and world-space blocking height
  rstl::vector< CVectorFixed8_8 > mParticles;
  rstl::vector< float > mParticleLifetimes; // Guessed name (Echoes')
  rstl::vector< int > mTrailFrames;         // Guessed name (Echoes')
};
CHECK_SIZEOF(CEnvFxManagerGrid, 0x4C)

// Echoes' enum; only the value the constructor uses is listed.
enum EEnvFxType {
  kEFX_None,
};

// CEnvFxManager.cpp. CStateManager news one (0x1468 bytes). The layout is Echoes' with the changes
// the constructor (0x80185A98) shows: the optional_object wrappers around the locked tokens are
// gone, a single rain splash id replaces Echoes' four, the rain sounds are held as CAUD tokens
// beside their audio handles, and Echoes' dark world particle texture is gone.
class CEnvFxManager {
public:
  CEnvFxManager();

  static void Initialize(); // Echoes' name; 0x80184790, called from CGameGlobalObjects

  void Update(float dt, CStateManager& mgr); // Echoes' name; 0x80183F40

  // Echoes' name (0x80184738); CStateManager's destructor calls it.
  void Cleanup();

private:
  // Echoes' names, except where noted.
  CAABox mParticleBounds;
  CVector3f mFocusCellPosition;
  bool mEnableSplash;
  float mFirstSnowForce;
  int mLastBlockedGridIdx;
  float mFxDensity;
  float mTargetFxDensity;
  float mMaxDensityDeltaSpeed;
  float mRainSoundFade; // Guessed name (Echoes')
  bool mSnowflakeTextureMipBlanked;
  TLockedToken< CTexture > mTxtrEnvGradient;
  rstl::reserved_vector< CEnvFxManagerGrid, 64 > mGrids;
  float mBaseSplashRate;
  TLockedToken< CGenDescription > mEnvRainSplash;
  TUniqueId mEnvRainSplashId; // Guessed name, after Echoes' mEnvRainSplashIds
  bool mRainSoundActive;
  TToken< CAudioSoundEffect > mRainLSound; // Guessed name; "CAUD_RainL"
  TToken< CAudioSoundEffect > mRainRSound; // Guessed name; "CAUD_RainR"
  CAudioHandle mLeftRainSound;
  CAudioHandle mRightRainSound;
  bool mRainSoundsStopped; // Guessed name (Echoes')
  TLockedToken< CTexture > mTxtrSnowFlake;
  rstl::reserved_vector< CVector3f, 16 > mSnowZDeltas;
  TLockedToken< CTexture > mUnderwaterFlake;
  EEnvFxType mPreviousFxType; // Guessed name (Echoes')
};
CHECK_SIZEOF(CEnvFxManager, 0x1468)

#endif // _CENVFXMANAGER
