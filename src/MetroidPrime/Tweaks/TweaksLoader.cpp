#include "MetroidPrime/Tweaks/CTweakContents.hpp"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/SObjectTag.hpp"
#include "MetroidPrime/CMappableObject.hpp"
#include "MetroidPrime/Player/CPlayerCameraBob.hpp"
#include "MetroidPrime/Tweaks/CTweakAutoMapper.hpp"
#include "MetroidPrime/Tweaks/CTweakBall.hpp"
#include "MetroidPrime/Tweaks/CTweakGame.hpp"
#include "MetroidPrime/Tweaks/CTweakGui.hpp"
#include "MetroidPrime/Tweaks/CTweakGuiColors.hpp"
#include "MetroidPrime/Tweaks/CTweakParticle.hpp"
#include "MetroidPrime/Tweaks/CTweakPlayer.hpp"
#include "MetroidPrime/Tweaks/CTweakPlayerControls.hpp"
#include "MetroidPrime/Tweaks/CTweakPlayerGun.hpp"
#include "MetroidPrime/Tweaks/CTweakPlayerRes.hpp"
#include "MetroidPrime/Tweaks/CTweakSlideShow.hpp"
#include "MetroidPrime/Tweaks/CTweakTargeting.hpp"

#include "MetroidPrime/ScriptLoader/TweaksLoaderDefinitions.inc"

// Guessed name. The controller interface queried for its type (virtual slot 0x18).
class IControllerType {
public:
  virtual ~IControllerType();
  virtual void x8_();
  virtual void xc_();
  virtual void x10_();
  virtual uint GetControllerType() const = 0; // Guessed name
};
extern IControllerType* gpController; // Guessed name, 0x80797108

// Guessed names. Controller types in the input code's .sdata2 (0x807A26E4..0x807A26EC),
// beside 'UNKN' (CRevolutionController.cpp); loaded, not folded, by the comparisons.
extern const uint kControllerTypeCube; // 'CUBE'
extern const uint kControllerTypeRevn; // 'REVN'
extern const uint kControllerTypeChak; // 'CHAK'

// Only the string survives in the pool (lbl_80684260); no pointer is emitted.
static const char* gkTweakContainer = "Standard.NTWK";
CTweakContents* gpTweakLdrs;

CTweakContents::CTweakContents() {}

CTweakContents::~CTweakContents() {}

void DecodeAnyTweak(uint instanceId, CInputStream& input) {
  switch (instanceId) {
  case 'TWAM':
    LoadTypedefTweakAutoMapper(gpTweakLdrs->mAutoMapper, input);
    break;
  case 'TWBL':
    LoadTypedefTweakBall(gpTweakLdrs->mBall, input);
    break;
  case 'TWCB':
    LoadTypedefTweakCameraBob(gpTweakLdrs->mCameraBob, input);
    break;
  case 'TWGM':
    LoadTypedefTweakGame(gpTweakLdrs->mGame, input);
    break;
  case 'TWGU':
    LoadTypedefTweakGui(gpTweakLdrs->mGui, input);
    break;
  case 'TWGC':
    LoadTypedefTweakGuiColors(gpTweakLdrs->mGuiColors, input);
    break;
  case 'TWPA':
    LoadTypedefTweakParticle(gpTweakLdrs->mParticle, input);
    break;
  case 'TWPL':
    LoadTypedefTweakPlayer(gpTweakLdrs->mPlayer, input);
    break;
  case 'TWPC':
    LoadTypedefTweakPlayerControls(gpTweakLdrs->mPlayerControls, input);
    break;
  case 'TWPG':
    LoadTypedefTweakPlayerGun(gpTweakLdrs->mPlayerGun, input);
    break;
  case 'TWPR':
    LoadTypedefTweakPlayerRes(gpTweakLdrs->mPlayerRes, input);
    break;
  case 'TWSS':
    LoadTypedefTweakSlideShow(gpTweakLdrs->mSlideShow, input);
    break;
  case 'TWTG':
    LoadTypedefTweakTargeting(gpTweakLdrs->mTargeting, input);
    break;
  default:
    rs_debugger_printf("Unknown script type");
    break;
  }
}

void LoadTweaks(CInputStream& input) {
  if (static_cast< uint >(input.ReadInt32()) == 'NTWK' && input.ReadUint8() == 1) {
    gpTweakLdrs = RS_NEW(3705) CTweakContents();
    int instanceCount = input.ReadInt32();
    while (instanceCount--) {
      const uint instanceType = input.ReadInt32();
      const u16 serializedSize = input.ReadUint16();
      input.ReadInt32(); // Instance ID.
      uint instanceSize = serializedSize - 6;

      ushort connectionCount = input.ReadUint16();
      while (connectionCount--) {
        instanceSize -= 12;
        input.ReadInt32();
        input.ReadInt32();
        input.ReadInt32();
      }

      const uint position = input.GetReadPosition();
      input.ReadInt32(); // Root property ID and size precede its field count.
      input.ReadUint16();
      DecodeAnyTweak(instanceType, input);
      instanceSize -= input.GetReadPosition() - position;
      if (instanceSize != 0) {
        rs_debugger_printf(
            "BUG: LoadTweaks: Skipping end of object data for script object type %s\n",
            SObjectTag::Type2Text(instanceType));
        gpfnWarningPrintf(
            "BUG: LoadTweaks: Skipping end of object data for script object type %s\n",
            SObjectTag::Type2Text(instanceType));
        uint remaining = instanceSize;
        while (remaining--) {
          input.ReadUint8();
        }
      }
    }
    rs_debugger_printf("*** Tweaks loaded.\n");
  }
}

void CreateTweakGlobals() {
  gpTweakAutoMapper = RS_NEW(3764) CTweakAutoMapper(gpTweakLdrs->mAutoMapper);
  gpTweakBall = RS_NEW(3765) CTweakBall(gpTweakLdrs->mBall);
  gpTweakGame = RS_NEW(3766) CTweakGame(gpTweakLdrs->mGame);
  gpTweakGui = RS_NEW(3767) CTweakGui(gpTweakLdrs->mGui);
  gpTweakGuiColors = RS_NEW(3768) CTweakGuiColors(gpTweakLdrs->mGuiColors);
  gpTweakParticle = RS_NEW(3769) CTweakParticle(gpTweakLdrs->mParticle);
  gpTweakPlayer = RS_NEW(3770) CTweakPlayer(gpTweakLdrs->mPlayer);
  gpTweakPlayerGun = RS_NEW(3771) CTweakPlayerGun(gpTweakLdrs->mPlayerGun);
  gpTweakPlayerRes = RS_NEW(3772) CTweakPlayerRes(gpTweakLdrs->mPlayerRes);
  gpTweakSlideShow = RS_NEW(3773) CTweakSlideShow(gpTweakLdrs->mSlideShow);
  gpTweakTargeting = RS_NEW(3774) CTweakTargeting(gpTweakLdrs->mTargeting);

  if (kControllerTypeChak == gpController->GetControllerType() ||
      kControllerTypeRevn == gpController->GetControllerType()) {
    gpTweakPlayerControls =
        RS_NEW(3782) CTweakPlayerControls(gpTweakLdrs->mPlayerControls.revolutionControls);
    rs_debugger_printf("Using REVOLUTION control tweaks\n");
  } else if (kControllerTypeCube == gpController->GetControllerType()) {
    gpTweakPlayerControls =
        RS_NEW(3787) CTweakPlayerControls(gpTweakLdrs->mPlayerControls.gamecubeControls);
    rs_debugger_printf("Using GAMECUBE control tweaks\n");
  } else {
    RS_VERIFY_THROW(3791, false, false,
                    "Fatal error... unknown controller (check to make sure controller is "
                    "plugged in), can't determine player control tweaks!");
  }

  CPlayerCameraBob::BindTweaks(gpTweakLdrs->mCameraBob);
  CMappableObject::ReadAutomapperTweaks();
}

void FreeTweaks() {
  rs_debugger_printf("gpTweakLdrs %08x (size %d)\n", gpTweakLdrs, sizeof(CTweakContents));
  delete gpTweakLdrs;
  gpTweakLdrs = nullptr;
  gpTweakAutoMapper = nullptr;
  gpTweakBall = nullptr;
  gpTweakGame = nullptr;
  gpTweakGui = nullptr;
  gpTweakGuiColors = nullptr;
  gpTweakParticle = nullptr;
  gpTweakPlayer = nullptr;
  gpTweakPlayerControls = nullptr;
  gpTweakPlayerGun = nullptr;
  gpTweakPlayerRes = nullptr;
  gpTweakSlideShow = nullptr;
  gpTweakTargeting = nullptr;
}
