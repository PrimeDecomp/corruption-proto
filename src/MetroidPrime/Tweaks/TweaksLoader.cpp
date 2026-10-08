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

// Guessed name. 0x80796BF0: initialized to rs_debugger_printf.
extern void (*gpfnWarningPrintf)(const char* format, ...);

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
extern const uint kControllerTypeCube;  // 'CUBE'
extern const uint kControllerTypeRevn;  // 'REVN'
extern const uint kControllerTypeChak;  // 'CHAK'

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
    gpTweakLdrs = new ("TweaksLoader.cpp(3705) : ", nullptr) CTweakContents();
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
  gpTweakAutoMapper =
      new ("TweaksLoader.cpp(3764) : ", nullptr) CTweakAutoMapper(gpTweakLdrs->mAutoMapper);
  gpTweakBall = new ("TweaksLoader.cpp(3765) : ", nullptr) CTweakBall(gpTweakLdrs->mBall);
  gpTweakGame = new ("TweaksLoader.cpp(3766) : ", nullptr) CTweakGame(gpTweakLdrs->mGame);
  gpTweakGui = new ("TweaksLoader.cpp(3767) : ", nullptr) CTweakGui(gpTweakLdrs->mGui);
  gpTweakGuiColors =
      new ("TweaksLoader.cpp(3768) : ", nullptr) CTweakGuiColors(gpTweakLdrs->mGuiColors);
  gpTweakParticle =
      new ("TweaksLoader.cpp(3769) : ", nullptr) CTweakParticle(gpTweakLdrs->mParticle);
  gpTweakPlayer = new ("TweaksLoader.cpp(3770) : ", nullptr) CTweakPlayer(gpTweakLdrs->mPlayer);
  gpTweakPlayerGun =
      new ("TweaksLoader.cpp(3771) : ", nullptr) CTweakPlayerGun(gpTweakLdrs->mPlayerGun);
  gpTweakPlayerRes =
      new ("TweaksLoader.cpp(3772) : ", nullptr) CTweakPlayerRes(gpTweakLdrs->mPlayerRes);
  gpTweakSlideShow =
      new ("TweaksLoader.cpp(3773) : ", nullptr) CTweakSlideShow(gpTweakLdrs->mSlideShow);
  gpTweakTargeting =
      new ("TweaksLoader.cpp(3774) : ", nullptr) CTweakTargeting(gpTweakLdrs->mTargeting);

  if (kControllerTypeChak == gpController->GetControllerType() ||
      kControllerTypeRevn == gpController->GetControllerType()) {
    gpTweakPlayerControls = new ("TweaksLoader.cpp(3782) : ", nullptr)
        CTweakPlayerControls(gpTweakLdrs->mPlayerControls.revolutionControls);
    rs_debugger_printf("Using REVOLUTION control tweaks\n");
  } else if (kControllerTypeCube == gpController->GetControllerType()) {
    gpTweakPlayerControls = new ("TweaksLoader.cpp(3787) : ", nullptr)
        CTweakPlayerControls(gpTweakLdrs->mPlayerControls.gamecubeControls);
    rs_debugger_printf("Using GAMECUBE control tweaks\n");
  } else {
    CCallStack stack(0, "TweaksLoader.cpp(3791) : ", kUnknownType);
    rs_log_assert_failure(&stack, "TweaksLoader.cpp", 3791, "Verify", "false",
                          "Fatal error... unknown controller (check to make sure controller is "
                          "plugged in), can't determine player control tweaks!");
    rs_debugger_printf("Would have thrown exception: %s\n", "false");
    RAssert_TriggerIllegalInstruction();
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
