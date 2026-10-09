// NonMatching translation-unit scaffold.
// G2MEAB .text 0x80180944..0x801863B4 (end exclusive).
// Echoes' CEnvFxManager.cpp. Only the constructor and the grid constructor are implemented.

#include "MetroidPrime/CEnvFxManager.hpp"

#include "Kyoto/CRandom16.hpp"
#include "Kyoto/CSimplePool.hpp"

static const float skMaximumBlockingHeight = 3.402823466e+38F;

CEnvFxManagerGrid::CEnvFxManagerGrid(const CVector2i& position, const CVector2i& extent,
                                     const rstl::vector< CVectorFixed8_8 >& initialParticles,
                                     int reserve)
: mBlockDirty(true)
, mPosition(position)
, mExtent(extent)
, mBlock(false, skMaximumBlockingHeight)
, mParticles(initialParticles) {
  mParticles.reserve(reserve);
}

// Unlike Echoes, the constructor also calls an empty function with no arguments (0x8018623C)
// before seeding its random generator, and sets the single rain splash id twice.
CEnvFxManager::CEnvFxManager()
: mParticleBounds(CVector3f(-63.5f, -63.5f, -63.5f), CVector3f(63.5f, 63.5f, 63.5f))
, mFocusCellPosition(CVector3f::Zero())
, mEnableSplash(false)
, mFirstSnowForce(0.f)
, mLastBlockedGridIdx(-1)
, mFxDensity(0.f)
, mTargetFxDensity(0.f)
, mMaxDensityDeltaSpeed(0.f)
, mRainSoundFade(1.f)
, mSnowflakeTextureMipBlanked(false)
, mTxtrEnvGradient(gpSimplePool->GetObj("TXTR_EnvGradient"))
, mEnvRainSplash(gpSimplePool->GetObj("PART_EnvRainSplash"))
, mEnvRainSplashId(kInvalidUniqueId)
, mRainSoundActive(false)
, mRainLSound(gpSimplePool->GetObj("CAUD_RainL"))
, mRainRSound(gpSimplePool->GetObj("CAUD_RainR"))
, mRainSoundsStopped(false)
, mTxtrSnowFlake(gpSimplePool->GetObj("TXTR_SnowFlake"))
, mUnderwaterFlake(gpSimplePool->GetObj("TXTR_UnderwaterFlake"))
, mPreviousFxType(kEFX_None) {
  CRandom16 random(0);
  mEnvRainSplashId = kInvalidUniqueId;

  for (int row = 0; row < 8; ++row) {
    for (int column = 0; column < 8; ++column) {
      mGrids.push_back(CEnvFxManagerGrid(CVector2i(column * 0x800, row * 0x800),
                                         CVector2i(0x800, 0x800), rstl::vector< CVectorFixed8_8 >(),
                                         0xab));
    }
  }

  for (int i = 15; i >= 0; --i) {
    mSnowZDeltas.push_back(CVector3f(0.f, 0.f, random.Range(-2.f, -4.f)));
  }
}
