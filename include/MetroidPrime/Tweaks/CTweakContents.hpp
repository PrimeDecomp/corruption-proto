#ifndef _CTWEAKCONTENTS
#define _CTWEAKCONTENTS

#include "types.h"

#include "MetroidPrime/ScriptLoader/SLdrTweakAutoMapper.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakBall.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakCameraBob.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakGame.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakGui.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakGuiColors.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakParticle.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakPlayer.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakPlayerControls.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakPlayerGun.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakPlayerRes.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakSlideShow.hpp"
#include "MetroidPrime/ScriptLoader/SLdrTweakTargeting.hpp"

class CInputStream;

// Guessed name. Every tweak record read from Standard.NTWK (TweaksLoader.cpp
// 0x8023E754; "gpTweakLdrs %08x (size %d)\n" prints its size, 0x3EA8).
class CTweakContents {
public:
  CTweakContents();
  ~CTweakContents();

  SLdrTweakAutoMapper mAutoMapper;
  SLdrTweakBall mBall;
  SLdrTweakCameraBob mCameraBob;
  SLdrTweakGame mGame;
  SLdrTweakGui mGui;
  SLdrTweakGuiColors mGuiColors;
  SLdrTweakParticle mParticle;
  SLdrTweakPlayer mPlayer;
  SLdrTweakPlayerControls mPlayerControls;
  SLdrTweakPlayerGun mPlayerGun;
  SLdrTweakPlayerRes mPlayerRes;
  SLdrTweakSlideShow mSlideShow;
  SLdrTweakTargeting mTargeting;
};
CHECK_SIZEOF(CTweakContents, 0x3EA8)

extern CTweakContents* gpTweakLdrs;

// Echoes names (Tweaks.cpp REL_LoadTweaks, REL_CreateTweakGlobals, REL_FreeTweaks).
void DecodeAnyTweak(uint instanceId, CInputStream& input);
void LoadTweaks(CInputStream& input);
void CreateTweakGlobals();
void FreeTweaks();

#endif // _CTWEAKCONTENTS
