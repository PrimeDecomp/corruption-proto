#ifndef _CENVFXMANAGER
#define _CENVFXMANAGER

#include "types.h"

#include "Kyoto/TToken.hpp"

#include "rstl/reserved_vector.hpp"
#include "rstl/vector.hpp"

class CAudioSoundEffect;
class CGenDescription;
class CTexture;

// Echoes' 8.8 fixed-point vector.
struct CVectorFixed8_8 {
  short mX;
  short mY;
  short mZ;
};

// Echoes' class, 0x4C bytes, with an implicit destructor (CEnvFxManager.cpp emits the grid
// vector's destructor, 0x80186148, and its element loop, 0x80186198). Only the three vectors at
// the end are modelled; the dirty flag, position, extent and block state come first in Echoes.
class CEnvFxManagerGrid {
private:
  uchar x0_[0x1C];
  // Echoes' names.
  rstl::vector< CVectorFixed8_8 > mParticles;
  rstl::vector< float > mParticleLifetimes;
  rstl::vector< int > mTrailFrames;
};

// Minimal view (CEnvFxManager.cpp). CStateManager news one (0x1468 bytes). The members that have
// destructors are placed from CStateManager.cpp's instance of the implicit destructor (0x802969AC)
// and named after the resources the constructor (0x80185A98) loads into them; they follow
// Echoes' order, with the rain sounds now held as CAUD tokens.
class CEnvFxManager {
public:
  CEnvFxManager();

  // Echoes' name (0x80184738); CStateManager's destructor calls it.
  void Cleanup();

private:
  uchar x0_[0x44];
  TLockedToken< CTexture > mTxtrEnvGradient; // Echoes' name
  rstl::reserved_vector< CEnvFxManagerGrid, 64 > mGrids;
  float mBaseSplashRate;
  TLockedToken< CGenDescription > mEnvRainSplash; // Echoes' name; "PART_EnvRainSplash"
  uchar x1364_[0x136C - 0x1364];
  TToken< CAudioSoundEffect > mRainLSound; // Guessed name; "CAUD_RainL"
  TToken< CAudioSoundEffect > mRainRSound; // Guessed name; "CAUD_RainR"
  uchar x137C_[0x1388 - 0x137C];
  TLockedToken< CTexture > mTxtrSnowFlake; // Echoes' name; "TXTR_SnowFlake"
  uchar x1394_[0x1458 - 0x1394];
  TLockedToken< CTexture > mUnderwaterFlake; // Echoes' name
  uchar x1464_[4];
};
CHECK_SIZEOF(CEnvFxManager, 0x1468)

#endif // _CENVFXMANAGER
