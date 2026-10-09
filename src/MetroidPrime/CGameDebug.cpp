// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x8003B898..0x800492C8 (93 native functions).
// Source identity: asserted target basename; absent from both retail source inventories.
// Complete native/helper/callback inventory retained; no speculative declarations.
// Remaining (not implemented) functions:
// 0x8003C9B0 +0xD4: debug movie-capture name sanitization (SetMovieCaptureName; strips
//   ":*?\"<>|\\/\n\t%" with a find_first_of(const char*) that rstl::string lacks)
// 0x8003CA84 +0xD8: emitted string find helper used by movie name sanitization
// 0x8003CB5C +0x80: emitted string iterator search helper
// 0x800451AC +0x880: debug menu input and selected option handling (blocked on unnamed
//   CGameState/CPlayerState/CGameOptions/language helpers and the menu builder below)
// 0x800466E4 +0xD58: debug menu construction from category/options (OpenMenu(page, controller));
//   vector.h assertion482; blocked on the unnamed demo/save-slot helpers and the
//   optional_object<CDebugMenu> assign helpers 0x8004743C/0x80047484/0x8004775C
// 0x8004743C +0x48: retained native/emitted helper; exact historical name/type unresolved
// 0x80047484 +0xA8: retained native/emitted helper; exact historical name/type unresolved
// 0x8004752C +0x18C: retained native/emitted helper; exact historical name/type unresolved
// 0x800476B8 +0xA4: retained native/emitted helper; exact historical name/type unresolved
// 0x8004775C +0x20: retained native/emitted helper; exact historical name/type unresolved
// 0x8004777C +0x28: retained native/emitted helper; exact historical name/type unresolved
// 0x800477A4 +0xA4: retained native/emitted helper; exact historical name/type unresolved
// 0x80047848 +0x174: retained native/emitted helper; exact historical name/type unresolved
// 0x800479BC +0x8C: retained native/emitted helper; exact historical name/type unresolved
// 0x80047A48 +0x68: retained native/emitted helper; exact historical name/type unresolved
// 0x80047AB0 +0x20: retained native/emitted helper; exact historical name/type unresolved
// 0x80047AD0 +0x28: retained native/emitted helper; exact historical name/type unresolved
// 0x80047AF8 +0xBC: retained native/emitted helper; exact historical name/type unresolved
// 0x80047BB4 +0x84: retained native/emitted helper; exact historical name/type unresolved
// 0x80047C38 +0x38: retained native/emitted helper; exact historical name/type unresolved
// 0x80047C70 +0x50: retained native/emitted helper; exact historical name/type unresolved
// 0x80047CC0 +0x20: retained native/emitted helper; exact historical name/type unresolved
// 0x80047CE0 +0x24: retained native/emitted helper; exact historical name/type unresolved
// 0x80047D04 +0x9C: retained native/emitted helper; exact historical name/type unresolved
// 0x80047DA0 +0xF0: emitted vector push-back helper; vector.h assertion482
// 0x80047E90 +0x98: debug option-name/value text building
// 0x80047F28 +0x2C4: debug option value formatting
// 0x800481EC +0xA4: debug powerup values from player state
// 0x80048A94 +0x18C: debug unlock-music/map rewards operation ("UnlockMusic%d", "UnlockMap%d")
// 0x80049298 +0x30: registered CGameDebug static initializer; raw native and .ctors8065B508
//   (stores -1, -1, -1, 0, 1, 2, -1 into 0x807973C0..0x807973D8, unused by this unit)

#include "MetroidPrime/CGameDebug.hpp"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Audio/CStreamAudioManager.hpp"
#include "Kyoto/Basics/CBasics.hpp"
#include "Kyoto/Basics/COsContext.hpp"
#include "Kyoto/CDvdFile.hpp"
#include "Kyoto/Graphics/CGraphics.hpp"
#include "Kyoto/Network/CBBASupport.hpp"
#include "Kyoto/Text/CFont.hpp"
#include "MetaRender/CCubeRenderer.hpp"
#include "MetroidPrime/CConsoleOutputWindow.hpp"
#include "MetroidPrime/CMain.hpp"
#include "MetroidPrime/Player/CGameState.hpp"
#include "MetroidPrime/Player/CPlayerState.hpp"
#include "MetroidPrime/Tweaks/CTweakAutoMapper.hpp"
#include "MetroidPrime/Tweaks/CTweakGame.hpp"
#include "MetroidPrime/Tweaks/CTweakGui.hpp"
#include "MetroidPrime/Tweaks/CTweakPlayer.hpp"
#include "MetroidPrime/Tweaks/CTweakTargeting.hpp"

#include "rstl/math.hpp"

#include <string.h>

// Guessed names. The tags a debug message line may embed (see DrawTaggedText).
const char* gDebugBarTag = "[BAR ";
const char* gDebugBarColorTag = "[BC ";

// Guessed names. The debug font arrow glyphs pointing to the other page of the category list.
const char kNextPageArrow[] = "\x8D";
const char kPreviousPageArrow[] = "\x88";

int CGameDebug::GetPlayerItemForOption(int index) {
  switch (index) {
  case kDO_PowerBeam:
    return 0;
  case kDO_PlasmaBeam:
    return 1;
  case kDO_NovaBeam:
    return 2;
  case kDO_ChargeUpgrade:
    return 3;
  case kDO_Missile:
    return 4;
  case kDO_IceMissile:
    return 5;
  case kDO_SeekerMissile:
    return 6;
  case kDO_GrappleBeam:
    return 7;
  case kDO_GrappleBeamVoltage:
    return 8;
  case kDO_Bomb:
    return 9;
  case kDO_HyperShot:
    return 28;
  case kDO_CombatVisor:
    return 10;
  case kDO_ScanVisor:
    return 11;
  case kDO_CommandVisor:
    return 12;
  case kDO_XRayVisor:
    return 13;
  case kDO_VariaSuit:
    return 16;
  case kDO_DoubleJump:
    return 14;
  case kDO_ScrewAttack:
    return 15;
  case kDO_MorphBall:
    return 20;
  case kDO_BoostBall:
    return 21;
  case kDO_SpiderBall:
    return 22;
  case kDO_PhazonBall:
    return 31;
  case kDO_IceBall:
    return 32;
  case kDO_FireBallFlammable:
    return 33;
  case kDO_FireBall:
    return 34;
  case kDO_CannonBall:
    return 35;
  case kDO_ActivateMorphballBoost:
    return 36;
  case kDO_Energy:
    return 17;
  case kDO_EnergyTank:
    return 18;
  case kDO_ItemPercentage:
    return 19;
  default:
    return -1;
  }
}

CGameDebug::CGameDebug()
: mOptions(kDO_Count, rstl::optional_object< CDebugOption >())
, mOptionConnections(kDO_Count, rstl::auto_ptr< IConnection >())
, mOptionSignals(0)
, mCategorySelection(kC_Count, 0)
, x9FE8_menuPage(-1)
, xA154_currentCategory(0)
, xA158_(0)
, xA15C_backItemId(0)
, xA160_exitItemId(0)
, xA164_controller(0)
, xA168_(false)
, xA169_(false)
, xA16A_(false)
, xA16B_(false)
, mMovieCaptureNameChosen(false)
, xA180_(0)
, mMovieCaptureTime(0.f)
, xA188_(0)
, mLogSize(0)
, mLogFileCreated(false) {}

// The first registration of an index wins. The option reports its changes to OnOptionChanged.
void CGameDebug::InstallOption(const CDebugOption& option) {
  rstl::optional_object< CDebugOption >& slot = mOptions[option.GetIndex()];
  if (!slot) {
    slot = option;
    mOptionConnections[option.GetIndex()] =
        mOptions[option.GetIndex()]->Connect(TFunctor1FromMethod< CGameDebug, CDebugOption* >::Make(
            *this, &CGameDebug::OnOptionChanged));
  }
}

void CGameDebug::AddOptionChoice(int index, const char* name, float value) {
  mOptions[index]->AddChoice(rstl::string_l(name), value);
}

// Guessed names. Engine switches that the debug options start from; they live in other units.
extern int gPVSMode;
extern int gFaceReflectMode;
extern bool gWidescreenBallMode;
extern float gMinimumShakeAmplitude;
extern bool gDisablePlayerLockon;
extern bool gAutoAim;
extern bool gAutoAimAtOrbitedObject;
extern bool gShowReflection;
extern float gDamageForcesBallTransition;
extern float gDamageBreaksOrbit;
extern bool gDoubleJumpBreaksOrbit;
extern bool gDoubleDashBreaksOrbit;
extern int gMorphballBreaksLockon;
extern float gPowerupSuckDistance;
extern bool gWaterEnable;
extern bool gWaterProfile;
extern bool gWaterFogOverWater;
extern bool gBoostBreaksOrbit;
extern bool gFreeLookPreventsOrbitMovement;

// Two int constants whose larger value bounds "DrawTime Info"; only CGameDebug reads them.
extern const int lbl_8079BA40;
extern const int lbl_8079BA44;

extern "C" {
void RAssert_SetDiagnosticPrintCallback(void (*callback)(const char* format, ...));
void RAssert_DiscardDiagnosticCallback(const char* format, ...);
// Guessed type: the debugger print callback returns whether it consumed the message.
bool RAssert_ConsumeDebuggerPrintCallback(const char* message);
void RAssert_SetDebuggerPrintCallback(bool (*callback)(const char* message));
// Guessed name (RVL SDK): stores the sensor bar distance used by the pointer.
void KPADSetObjInterval(float interval);
}

inline float GetPowerUpMax(int option) {
  return CPlayerState::GetPowerUpMaxValue(
      static_cast< CPlayerState::EItemType >(CGameDebug::GetPlayerItemForOption(option)));
}

// Guessed names. More engine switches the options read and write.
extern bool gLoadRELFilesOverBBA;
extern int gFrozenJostleCount;
// Set when the options are first applied; its owner is not split yet.
extern bool lbl_80796EC8;
// CElementGen.cpp's flag that stops particles from rendering.
extern bool lbl_8079B3D9;

extern "C" void SelectMaterialTevHandler(int mode);

// The earlier of two positions in a string, as a reference like CMath::Min.
inline const char* const& EarlierOf(const char* const& a, const char* const& b) {
  return a < b ? a : b;
}

// Guessed name. Reads the space separated decimal values of a tag up to its closing ']' into
// values (at most maxCount of them) and moves the cursor past the tag. Returns the number of
// values read, or 0 for a malformed tag.
int CGameDebug::ParseTagValues(const char** cursor, int* values, int maxCount) {
  int count = 0;
  const char* text = *cursor;
  while (*text != ']') {
    int value = 0;
    while (*text != ' ' && *text != ']') {
      char c = *text;
      if (c >= '0' && c <= '9') {
        value = value * 10 + (c - '0');
      } else {
        return 0;
      }
      ++text;
    }
    if (count == maxCount) {
      return 0;
    }
    *values++ = value;
    ++count;
    if (*text == ' ') {
      ++text;
    }
  }
  *cursor = text + 1;
  return count;
}

// Guessed name. Parses "[BAR <fill> <size>]"; a single value is both the fill and the size.
// Returns the text after the tag, or null for a malformed tag.
const char* CGameDebug::ParseBarTag(const char* text, int* fill, int* size) {
  const char* cursor = text + strlen(gDebugBarTag);
  int values[2] = {0, 0};
  int count = ParseTagValues(&cursor, values, 2);
  if (count == 1) {
    *fill = values[0];
    *size = values[0];
  } else if (count == 2) {
    *fill = values[0];
    *size = values[1];
  } else {
    return nullptr;
  }
  return cursor;
}

// Guessed name. Parses "[BC <r> <g> <b> [<a>]]" (alpha defaults to 192) into the colour of the
// bars that follow. Returns the text after the tag, or null for a malformed tag.
const char* CGameDebug::ParseColorTag(const char* text, CColor* color) {
  const char* cursor = text + strlen(gDebugBarColorTag);
  int values[4] = {0, 0, 0, 192};
  int count = ParseTagValues(&cursor, values, 4);
  if (count == 3 || count == 4) {
    uchar r = CMath::Clamp(0, values[0], 255);
    uchar g = CMath::Clamp(0, values[1], 255);
    uchar b = CMath::Clamp(0, values[2], 255);
    uchar a = CMath::Clamp(0, values[3], 255);
    *color = CColor(r, g, b, a);
    return cursor;
  }
  return nullptr;
}

// Guessed name. Draws a debug message line right aligned. The line may embed "[BAR ...]" bars,
// drawn as a filled and a dimmed part, and "[BC ...]" colour changes for the bars that follow.
void CGameDebug::DrawTaggedText(const CFont& font, const char* text, int y, CColor color) {
  const int right = CGraphics::GetViewport().mWidth - 18;
  if (strstr(text, "[") == nullptr) {
    font.DrawString(text, right - font.StringWidth(text), y, color);
    return;
  }

  rstl::reserved_vector< STextSegment, kMaxTextSegments > segments;
  const char* cursor = text;
  while (*cursor != '\0' && segments.size() < kMaxTextSegments) {
    if (strncmp(cursor, gDebugBarTag, strlen(gDebugBarTag)) == 0) {
      int fill = 0;
      int size = 0;
      const char* next = ParseBarTag(cursor, &fill, &size);
      if (next != nullptr) {
        cursor = next;
        STextSegment segment;
        segment.mType = STextSegment::kT_Bar;
        segment.mWidth = size;
        segment.mFill = rstl::min_val(fill, size);
        segment.mColor = CColor::Black().GetColor_u32();
        segments.push_back(segment);
        continue;
      }
    } else if (strncmp(cursor, gDebugBarColorTag, strlen(gDebugBarColorTag)) == 0) {
      CColor barColor = CColor::Black();
      const char* next = ParseColorTag(cursor, &barColor);
      if (next != nullptr) {
        cursor = next;
        STextSegment segment;
        segment.mType = STextSegment::kT_BarColor;
        segment.mWidth = 0;
        segment.mColor = barColor.GetColor_u32();
        segments.push_back(segment);
        continue;
      }
    }

    // Plain text up to the next tag.
    const char* tag = *cursor != '\0' ? strstr(cursor + 1, "[") : nullptr;
    const char* end = EarlierOf(cursor + STextSegment::kTextSize - 1,
                                tag == nullptr ? cursor + strlen(cursor) : tag);
    STextSegment segment;
    segment.mType = STextSegment::kT_Text;
    segment.mFill = 0;
    segment.mColor = CColor::Black().GetColor_u32();
    int length = end - cursor;
    strncpy(segment.mText, cursor, length);
    segment.mText[length] = '\0';
    segment.mWidth = font.StringWidth(segment.mText);
    segments.push_back(segment);
    cursor = end;
  }

  int width = 0;
  for (int i = 0; i < segments.size(); ++i) {
    width += segments[i].mWidth;
  }

  int x = right - width;
  CColor barColor = color.WithAlphaOf(0.75f);
  int barY = y + 2;
  for (int i = 0; i < segments.size(); ++i) {
    const STextSegment& segment = segments[i];
    switch (segment.mType) {
    case STextSegment::kT_Text:
      font.DrawString(segment.mText, x, y, color);
      break;
    case STextSegment::kT_Bar:
      CGraphics::Render2D(nullptr, x, barY, segment.mFill, font.GetFontSize() - 4, barColor);
      CGraphics::Render2D(nullptr, x + segment.mFill, barY, segment.mWidth - segment.mFill,
                          font.GetFontSize() - 4, barColor.WithAlphaModulatedBy(0.5f));
      break;
    case STextSegment::kT_BarColor:
      barColor = segment.mColor;
      break;
    }
    x += segment.mWidth;
  }
}

// Guessed name. Draws the debug messages the options gathered this frame from the bottom of the
// screen up (top down when xA16A_ is set), logging them with the frame number in "Log" mode,
// then the debug menu and, on the category list, the arrow to the other page.
void CGameDebug::Draw() {
  gpRender->SetDepthReadWrite(false, false);
  CGraphics::SetDepthRange(0.f, 1.f);
  const int& screenHeight = CGraphics::GetViewport().mHeight;
  int y = screenHeight;
  bool firstLine = true;

  int messageMode = GetOptionInt(kDO_DebugMessagesEnabled);
  if (gpGameDebug->IsOptionSet(kDO_AITraceViewMode)) {
    messageMode = 0;
  }
  if (messageMode > 0) {
    CColor color = gpGameDebug->GetDebugMessageColor();
    bool frameLogged = false;
    for (int i = 0; i < mOptions.size(); ++i) {
      if (!mOptions[i]) {
        continue;
      }
      const CDebugOption& option = mOptions[i].data();
      if (option.GetIndex() == kDO_GenericMsgs && option.GetValue() == 0.f) {
        continue;
      }
      CFont font(option.GetMessageScale());
      if (firstLine) {
        y -= font.GetFontSize();
        firstLine = false;
      }
      const rstl::vector< rstl::string >& messages = option.GetMessages();
      if (messageMode > 1 && messages.size() != 0 && !frameLogged) {
        frameLogged = true;
        AppendToLog(CBasics::Stringize("@Frame %06d\r\n", xA188_));
      }
      for (rstl::vector< rstl::string >::const_iterator it = messages.begin(); it != messages.end();
           ++it) {
        y -= font.GetFontSize();
        DrawTaggedText(font, it->data(), xA16A_ ? screenHeight - y - font.GetFontSize() : y, color);
        if (messageMode > 1) {
          AppendToLog(it->data());
          AppendToLog("\r\n");
        }
      }
    }
  }

  if (mMenu) {
    mMenu.data().Draw();
  }
  if (mMenu.valid() == true && xA154_currentCategory == 0) {
    CFont font(2.f);
    int height = screenHeight;
    switch (x9FE8_menuPage) {
    case 0:
      font.DrawString(kNextPageArrow, mMenu.data().GetRightEdge() + 30, height / 2,
                      CColor::White());
      break;
    case 1:
      font.DrawString(kPreviousPageArrow, mMenu.data().GetLeftEdge() - 50, height / 2,
                      CColor::White());
      break;
    }
  }
  gpRender->SetDepthReadWrite(true, true);
}

// Guessed name. Steps the open debug menu; "Save Debug Message Log" closes the menu and dumps the
// log once. The callers (CMFGame and CFrontEndUIDevelopment) pass the result on as their IOWin
// message return, and it is always kMR_Normal.
int CGameDebug::UpdateMenu(float dt) {
  if (!mMenu) {
    return 0;
  }
  mMenu.data().Update(dt);
  if (IsOptionSet(kDO_SaveDebugMessageLog)) {
    if (mMenu) {
      CloseMenu();
    }
    SetOptionValue(kDO_SaveDebugMessageLog, 0.f);
    DumpLog();
  }
  return 0;
}

// Guessed name. Reads the tweaks and engine switches back into their options.
void CGameDebug::ReadEngineState() {
  // Orbit and player switches
  SetOptionValue(kDO_AutoAim, gAutoAim);
  SetOptionValue(kDO_AutoAimAtOrbitedObject, gAutoAimAtOrbitedObject);
  SetOptionValue(kDO_FreeLookPreventsOrbitMovement, gFreeLookPreventsOrbitMovement);
  SetOptionValue(kDO_ShowReflection, gShowReflection);
  SetOptionValue(kDO_DamageForcesBallTransition, gDamageForcesBallTransition);
  SetOptionValue(kDO_DamageBreaksOrbit, gDamageBreaksOrbit);
  SetOptionValue(kDO_BoostBreaksOrbit, gBoostBreaksOrbit);
  SetOptionValue(kDO_MorphballBreaksLockon, gMorphballBreaksLockon);
  SetOptionValue(kDO_DoubleDashBreaksOrbit, gDoubleDashBreaksOrbit);
  SetOptionValue(kDO_WidescreenBallmode, gWidescreenBallMode);
  SetOptionValue(kDO_PVS, gPVSMode);
  SetOptionValue(kDO_FaceReflectMode, gFaceReflectMode);
  SetOptionValue(kDO_WaterEnable, gWaterEnable);
  SetOptionValue(kDO_WaterProfile, gWaterProfile);
  SetOptionValue(kDO_WaterFogOverWater, gWaterFogOverWater);

  // Player tweaks
  SetOptionValue(kDO_NormalTurnFactor, static_cast< int >(gpTweakPlayer->GetNormalTurnFactor()));
  SetOptionValue(kDO_FreeLookTurnFactor,
                 static_cast< int >(gpTweakPlayer->GetFreeLookTurnFactor()));
  SetOptionValue(kDO_GrappleDistance, gpTweakPlayer->GetGrappleDistance());
  SetOptionValue(kDO_GrappleBeamLength, gpTweakPlayer->GetGrappleBeamLength());
  SetOptionValue(kDO_GrappleSwingTime, gpTweakPlayer->GetGrappleSwingTime());
  SetOptionValue(kDO_GrappleMaxVelocity, gpTweakPlayer->GetGrappleMaxVelocity());
  SetOptionValue(kDO_GrapplePullCloseDistance, gpTweakPlayer->GetGrapplePullCloseDistance());
  SetOptionValue(kDO_GrapplePullDampenDistance, gpTweakPlayer->GetGrapplePullDampenDistance());
  SetOptionValue(kDO_GrapplePullVelocity, gpTweakPlayer->GetGrapplePullVelocity());
  SetOptionValue(kDO_GrappleTurnRate, gpTweakPlayer->GetGrappleTurnRate());
  SetOptionValue(kDO_GrappleJumpForce, gpTweakPlayer->GetGrappleJumpForce());
  SetOptionValue(kDO_GrappleHoldOrbitButton, gpTweakPlayer->GetGrappleHoldOrbitButton());
  SetOptionValue(kDO_GrappleTurnControlsReversed, gpTweakPlayer->GetGrappleTurnControlsReversed());
  SetOptionValue(kDO_GrappleControlScheme, gpTweakPlayer->GetGrappleControlScheme());
  SetOptionValue(kDO_ScanFreezesGame, gpTweakPlayer->GetScanFreezesGame());
  SetOptionValue(kDO_ScanRequiresLineOfSight, gpTweakPlayer->GetScanLineOfSight());
  SetOptionValue(kDO_ShieldAllowsMovement, gpTweakPlayer->GetShieldAllowsMotion());
  SetOptionValue(kDO_DashEnabled, gpTweakPlayer->GetOrbitDash());
  SetOptionValue(kDO_UsesTapHold, gpTweakPlayer->GetOrbitDashUsesTap());
  SetOptionValue(kDO_TapTime, gpTweakPlayer->GetOrbitDashTapTime());
  SetOptionValue(kDO_StickXAxisThreshold, gpTweakPlayer->GetOrbitDashStickThreshold());
  SetOptionValue(kDO_DoubleJumpImpulse, gpTweakPlayer->GetOrbitDashDoubleJumpImpulse());
  SetOptionValue(kDO_VerticalDoubleJumpAccel, gpTweakPlayer->GetOrbitDashVerticalDoubleJumpAccel());
  SetOptionValue(kDO_HorizDoubleJumpAccel, gpTweakPlayer->GetOrbitDashHorizontalDoubleJumpAccel());

  // HUD tweaks
  SetOptionValue(kDO_EnableHud, gpTweakGui->GetEnableHud());
  SetOptionValue(kDO_EnableTargeting, gpTweakGui->GetEnableTargeting());
  SetOptionValue(kDO_EnableAutoMapper, gpTweakGui->GetEnableAutoMapper());
  SetOptionValue(kDO_RadarMode, gpTweakGui->GetRadarMode());
  SetOptionValue(kDO_HUDCameraFOV, gpTweakGui->GetHudCameraFov());
  SetOptionValue(kDO_HUDCameraY, gpTweakGui->GetHudCameraY());
  SetOptionValue(kDO_HUDCameraZ, gpTweakGui->GetHudCameraZ());
  SetOptionValue(kDO_FaceReflectionWidth, gpTweakGui->GetFaceReflectionWidth());
  SetOptionValue(kDO_FaceReflectionHeight, gpTweakGui->GetFaceReflectionHeight());
  SetOptionValue(kDO_FaceReflectionPositionY, gpTweakGui->GetFaceReflectionPositionY());
  SetOptionValue(kDO_FaceReflectionPositionZ, gpTweakGui->GetFaceReflectionPositionZ());
  SetOptionValue(kDO_FaceReflectionAspectRatio, gpTweakGui->GetFaceReflectionAspectRatio());
  SetOptionValue(kDO_ShowOrbitPoint, gpTweakTargeting->GetShowOrbitPoint());

  // Other engine state
  SetOptionValue(kDO_PowerupSuckDistance, gPowerupSuckDistance);
  SetOptionValue(kDO_HardMode, gpGameState->GetHardModeEnabled());
  SetOptionValue(kDO_LoadRELFilesOverBBA, gLoadRELFilesOverBBA);
  SetOptionValue(kDO_FrozenJostleCount, gFrozenJostleCount);
  SetOptionValue(kDO_Brightness, CGraphics::GetBrightness());
  SetOptionValue(kDO_MinimumShakeAmplitude, gMinimumShakeAmplitude);
  if (gpGameState != nullptr) {
    CGameOptions& options = gpGameState->GameOptions();
    SetOptionValue(kDO_SfxMasterVolume, options.GetSfxVolume().mValue);
    SetOptionValue(kDO_MusicMasterVolume, options.GetMusicVolume().mValue);
  }

  // Hyper mode tuning of the "Timer" type, then of the "Phazon Level" type
  SetOptionValue(kDO_HyperModeType, gpTweakPlayer->GetHyperModeType());
  SetOptionValue(kDO_HyperModeInvulnerablePhazonLoss,
                 gpTweakPlayer->GetHyperModeInvulnerablePhazonLoss(0));
  SetOptionValue(kDO_HyperModeInvulnerableTime, gpTweakPlayer->GetHyperModeInvulnerableTime(0));
  SetOptionValue(kDO_HyperModeCorruptionTime, gpTweakPlayer->GetHyperModeCorruptionTime(0));
  SetOptionValue(kDO_HyperModeConstantCorruptionRate,
                 gpTweakPlayer->GetHyperModeConstantCorruptionRate(0));
  SetOptionValue(kDO_HyperModeCorruptionRate, gpTweakPlayer->GetHyperModeCorruptionRate(0));
  SetOptionValue(kDO_HyperModePhazonLevel, gpTweakPlayer->GetHyperModePhazonLevel(0));
  SetOptionValue(kDO_HyperModePhazonCapacity, gpTweakPlayer->GetHyperModePhazonCapacity(0));
  SetOptionValue(kDO_HyperModeDangerPercentage, gpTweakPlayer->GetHyperModeDangerPercentage(0));
  SetOptionValue(kDO_HyperModeBeamLossAmount, gpTweakPlayer->GetHyperModeBeamLossAmount(0));
  SetOptionValue(kDO_HyperModeMissileLossAmount, gpTweakPlayer->GetHyperModeMissileLossAmount(0));
  SetOptionValue(kDO_HyperModePhazonBallRate, gpTweakPlayer->GetHyperModePhazonBallRate(0));
  SetOptionValue(kDO_HyperModeDamageMultiplier, gpTweakPlayer->GetHyperModeDamageMultiplier(0));
  SetOptionValue(kDO_HyperModeInvulnerablePhazonLoss2,
                 gpTweakPlayer->GetHyperModeInvulnerablePhazonLoss(1));
  SetOptionValue(kDO_HyperModeInvulnerableTime2, gpTweakPlayer->GetHyperModeInvulnerableTime(1));
  SetOptionValue(kDO_HyperModeCorruptionTime2, gpTweakPlayer->GetHyperModeCorruptionTime(1));
  SetOptionValue(kDO_HyperModeConstantCorruptionRate2,
                 gpTweakPlayer->GetHyperModeConstantCorruptionRate(1));
  SetOptionValue(kDO_HyperModeCorruptionRate2, gpTweakPlayer->GetHyperModeCorruptionRate(1));
  SetOptionValue(kDO_HyperModePhazonLevel2, gpTweakPlayer->GetHyperModePhazonLevel(1));
  SetOptionValue(kDO_HyperModePhazonCapacity2, gpTweakPlayer->GetHyperModePhazonCapacity(1));
  SetOptionValue(kDO_HyperModeDangerPercentage2, gpTweakPlayer->GetHyperModeDangerPercentage(1));
  SetOptionValue(kDO_HyperModeBeamLossAmount2, gpTweakPlayer->GetHyperModeBeamLossAmount(1));
  SetOptionValue(kDO_HyperModeMissileLossAmount2, gpTweakPlayer->GetHyperModeMissileLossAmount(1));
  SetOptionValue(kDO_HyperModePhazonBallRate2, gpTweakPlayer->GetHyperModePhazonBallRate(1));
  SetOptionValue(kDO_HyperModeDamageMultiplier2, gpTweakPlayer->GetHyperModeDamageMultiplier(1));

  // Revolution controls
  SetOptionValue(kDO_FreeLookGun, gpTweakPlayer->GetRevFreeLookGun());
  SetOptionValue(kDO_OrbitLockGun, gpTweakPlayer->GetRevOrbitLockGun());
  SetOptionValue(kDO_OrbitTagObjects, gpTweakPlayer->GetRevOrbitTagObjects());
  SetOptionValue(kDO_LockAimingCursor, gpTweakPlayer->GetRevLockCursor());
}

// With deferred inlining the inliner's total size budget is read at the end of the file; past the
// default, none of ApplyOptions' ~110 option reads are inlined, while the binary inlines them all.
#pragma inline_max_total_size(100000)

// Guessed name. Applies the options to the tweaks and engine switches.
void CGameDebug::ApplyOptions() {
  lbl_80796EC8 = true;
  CStreamAudioManager::SetMusicUnmute(IsOptionSet(kDO_MusicOnOff));
  gpGameState->GameOptions().SetSoundMode(GetOptionInt(kDO_SoundMode), false);

  // Orbit and player switches
  gAutoAim = IsOptionSet(kDO_AutoAim);
  gAutoAimAtOrbitedObject = IsOptionSet(kDO_AutoAimAtOrbitedObject);
  gFreeLookPreventsOrbitMovement = IsOptionSet(kDO_FreeLookPreventsOrbitMovement);
  gShowReflection = IsOptionSet(kDO_ShowReflection);
  gDamageForcesBallTransition = GetOptionValue(kDO_DamageForcesBallTransition);
  gDamageBreaksOrbit = GetOptionValue(kDO_DamageBreaksOrbit);
  gBoostBreaksOrbit = IsOptionSet(kDO_BoostBreaksOrbit);
  gMorphballBreaksLockon = GetOptionInt(kDO_MorphballBreaksLockon);
  gDoubleDashBreaksOrbit = IsOptionSet(kDO_DoubleDashBreaksOrbit);
  gWidescreenBallMode = IsOptionSet(kDO_WidescreenBallmode);
  gPVSMode = GetOptionInt(kDO_PVS);
  gFaceReflectMode = GetOptionInt(kDO_FaceReflectMode);
  gWaterEnable = IsOptionSet(kDO_WaterEnable);
  gWaterProfile = IsOptionSet(kDO_WaterProfile);
  gWaterFogOverWater = IsOptionSet(kDO_WaterFogOverWater);

  // Player tweaks
  gpTweakPlayer->SetNormalTurnFactor(GetOptionValue(kDO_NormalTurnFactor));
  gpTweakPlayer->SetFreeLookTurnFactor(GetOptionValue(kDO_FreeLookTurnFactor));
  gpTweakPlayer->SetGrappleDistance(GetOptionValue(kDO_GrappleDistance));
  gpTweakPlayer->SetGrappleBeamLength(GetOptionValue(kDO_GrappleBeamLength));
  gpTweakPlayer->SetGrappleSwingTime(GetOptionValue(kDO_GrappleSwingTime));
  gpTweakPlayer->SetGrappleMaxVelocity(GetOptionValue(kDO_GrappleMaxVelocity));
  gpTweakPlayer->SetGrapplePullCloseDistance(GetOptionValue(kDO_GrapplePullCloseDistance));
  gpTweakPlayer->SetGrapplePullDampenDistance(GetOptionValue(kDO_GrapplePullDampenDistance));
  gpTweakPlayer->SetGrapplePullVelocity(GetOptionValue(kDO_GrapplePullVelocity));
  gpTweakPlayer->SetGrappleTurnRate(GetOptionValue(kDO_GrappleTurnRate));
  gpTweakPlayer->SetGrappleJumpForce(GetOptionValue(kDO_GrappleJumpForce));
  gpTweakPlayer->SetGrappleHoldOrbitButton(IsOptionSet(kDO_GrappleHoldOrbitButton));
  gpTweakPlayer->SetGrappleTurnControlsReversed(IsOptionSet(kDO_GrappleTurnControlsReversed));
  gpTweakPlayer->SetGrappleControlScheme(GetOptionInt(kDO_GrappleControlScheme));
  gpTweakPlayer->SetScanFreezesGame(IsOptionSet(kDO_ScanFreezesGame));
  gpTweakPlayer->SetScanLineOfSight(IsOptionSet(kDO_ScanRequiresLineOfSight));
  gpTweakPlayer->SetShieldAllowsMotion(IsOptionSet(kDO_ShieldAllowsMovement));
  gpTweakPlayer->SetOrbitDash(IsOptionSet(kDO_DashEnabled));
  gpTweakPlayer->SetOrbitDashUsesTap(IsOptionSet(kDO_UsesTapHold));
  gpTweakPlayer->SetOrbitDashTapTime(GetOptionValue(kDO_TapTime));
  gpTweakPlayer->SetOrbitDashStickThreshold(GetOptionValue(kDO_StickXAxisThreshold));
  gpTweakPlayer->SetOrbitDashDoubleJumpImpulse(GetOptionValue(kDO_DoubleJumpImpulse));
  gpTweakPlayer->SetOrbitDashVerticalDoubleJumpAccel(GetOptionValue(kDO_VerticalDoubleJumpAccel));
  gpTweakPlayer->SetOrbitDashHorizontalDoubleJumpAccel(GetOptionValue(kDO_HorizDoubleJumpAccel));

  // HUD tweaks; "HUD Camera FOV" is not applied back
  gpTweakGui->SetEnableHud(GetOptionInt(kDO_EnableHud));
  gpTweakGui->SetEnableTargeting(GetOptionInt(kDO_EnableTargeting));
  gpTweakGui->SetEnableAutoMapper(GetOptionInt(kDO_EnableAutoMapper));
  gpTweakGui->SetRadarMode(GetOptionInt(kDO_RadarMode));
  gpTweakGui->SetEnableVisors(GetOptionInt(kDO_EnableVisors));
  gpTweakGui->SetHudCameraY(GetOptionInt(kDO_HUDCameraY));
  gpTweakGui->SetHudCameraZ(GetOptionInt(kDO_HUDCameraZ));
  gpTweakGui->SetFaceReflectionWidth(GetOptionInt(kDO_FaceReflectionWidth));
  gpTweakGui->SetFaceReflectionHeight(GetOptionInt(kDO_FaceReflectionHeight));
  gpTweakGui->SetFaceReflectionPositionY(GetOptionInt(kDO_FaceReflectionPositionY));
  gpTweakGui->SetFaceReflectionPositionZ(GetOptionInt(kDO_FaceReflectionPositionZ));
  gpTweakGui->SetFaceReflectionAspectRatio(GetOptionInt(kDO_FaceReflectionAspectRatio));
  gpTweakTargeting->SetShowOrbitPoint(IsOptionSet(kDO_ShowOrbitPoint));

  // Hyper mode tuning of the "Timer" type, then of the "Phazon Level" type
  gpTweakPlayer->SetHyperModeType(GetOptionInt(kDO_HyperModeType));
  gpTweakPlayer->SetHyperModeInvulnerablePhazonLoss(
      0, IsOptionSet(kDO_HyperModeInvulnerablePhazonLoss));
  gpTweakPlayer->SetHyperModeInvulnerableTime(0, GetOptionValue(kDO_HyperModeInvulnerableTime));
  gpTweakPlayer->SetHyperModeCorruptionTime(0, GetOptionValue(kDO_HyperModeCorruptionTime));
  gpTweakPlayer->SetHyperModeConstantCorruptionRate(
      0, IsOptionSet(kDO_HyperModeConstantCorruptionRate));
  gpTweakPlayer->SetHyperModeCorruptionRate(0, GetOptionValue(kDO_HyperModeCorruptionRate));
  gpTweakPlayer->SetHyperModePhazonLevel(0, GetOptionValue(kDO_HyperModePhazonLevel));
  gpTweakPlayer->SetHyperModePhazonCapacity(0, GetOptionValue(kDO_HyperModePhazonCapacity));
  gpTweakPlayer->SetHyperModeDangerPercentage(0, GetOptionValue(kDO_HyperModeDangerPercentage));
  gpTweakPlayer->SetHyperModeBeamLossAmount(0, GetOptionValue(kDO_HyperModeBeamLossAmount));
  gpTweakPlayer->SetHyperModeMissileLossAmount(0, GetOptionValue(kDO_HyperModeMissileLossAmount));
  gpTweakPlayer->SetHyperModePhazonBallRate(0, GetOptionValue(kDO_HyperModePhazonBallRate));
  gpTweakPlayer->SetHyperModeDamageMultiplier(0, GetOptionValue(kDO_HyperModeDamageMultiplier));
  gpTweakPlayer->SetHyperModeInvulnerablePhazonLoss(
      1, IsOptionSet(kDO_HyperModeInvulnerablePhazonLoss2));
  gpTweakPlayer->SetHyperModeInvulnerableTime(1, GetOptionValue(kDO_HyperModeInvulnerableTime2));
  gpTweakPlayer->SetHyperModeCorruptionTime(1, GetOptionValue(kDO_HyperModeCorruptionTime2));
  gpTweakPlayer->SetHyperModeConstantCorruptionRate(
      1, IsOptionSet(kDO_HyperModeConstantCorruptionRate2));
  gpTweakPlayer->SetHyperModeCorruptionRate(1, GetOptionValue(kDO_HyperModeCorruptionRate2));
  gpTweakPlayer->SetHyperModePhazonLevel(1, GetOptionValue(kDO_HyperModePhazonLevel2));
  gpTweakPlayer->SetHyperModePhazonCapacity(1, GetOptionValue(kDO_HyperModePhazonCapacity2));
  gpTweakPlayer->SetHyperModeDangerPercentage(1, GetOptionValue(kDO_HyperModeDangerPercentage2));
  gpTweakPlayer->SetHyperModeBeamLossAmount(1, GetOptionValue(kDO_HyperModeBeamLossAmount2));
  gpTweakPlayer->SetHyperModeMissileLossAmount(1, GetOptionValue(kDO_HyperModeMissileLossAmount2));
  gpTweakPlayer->SetHyperModePhazonBallRate(1, GetOptionValue(kDO_HyperModePhazonBallRate2));
  gpTweakPlayer->SetHyperModeDamageMultiplier(1, GetOptionValue(kDO_HyperModeDamageMultiplier2));

  // Revolution controls
  gpTweakPlayer->SetRevFreeLookGun(IsOptionSet(kDO_FreeLookGun));
  gpTweakPlayer->SetRevOrbitLockGun(IsOptionSet(kDO_OrbitLockGun));
  gpTweakPlayer->SetRevOrbitTagObjects(IsOptionSet(kDO_OrbitTagObjects));
  gpTweakPlayer->SetRevLockCursor(IsOptionSet(kDO_LockAimingCursor));
  KPADSetObjInterval(GetOptionValue(kDO_DPDDistance));

  // Other engine state
  gPowerupSuckDistance = GetOptionValue(kDO_PowerupSuckDistance);
  gpGameState->SetHardMode(IsOptionSet(kDO_HardMode));
  gLoadRELFilesOverBBA = IsOptionSet(kDO_LoadRELFilesOverBBA);
  gFrozenJostleCount = GetOptionInt(kDO_FrozenJostleCount);
  CGraphics::SetBrightness(GetOptionValue(kDO_Brightness));
  gMinimumShakeAmplitude = GetOptionValue(kDO_MinimumShakeAmplitude);
  lbl_8079B3D9 = !IsOptionSet(kDO_RenderParticles);
  SelectMaterialTevHandler(GetOptionInt(kDO_PerPolyDebugging));
}

// Registers every option in its category, in the order of the binary. The headings name the
// category of the options that follow; a few options are registered away from their category.
// The initializers of the local name tables are separate objects in the binary, not one pooled
// block.
#pragma push
#pragma pool_data off
void CGameDebug::AddDebugOptions() {
  // Programmer
  AddOption(kC_Programmer, kDO_DrawTimeInfo, "DrawTime Info", 0.f, 0.f,
            rstl::max_val(lbl_8079BA40, lbl_8079BA44) - 1, 1.f);

  // Misc (registered among the programmer tools)
  AddOption(kC_Misc, kDO_DumpScreenShot, "Dump Screen Shot", false);

  // Programmer
  AddOption(kC_Programmer, kDO_MemoryMetrics, "Memory Metrics", 2.f, 0.f, 3.f, 1.f);
  AddOptionChoice(kDO_MemoryMetrics, "None", 0.f);
  AddOptionChoice(kDO_MemoryMetrics, "Full", 1.f);
  AddOptionChoice(kDO_MemoryMetrics, "Basic", 2.f);
  AddOptionChoice(kDO_MemoryMetrics, "BasicWithPeaks", 3.f);
  AddOption(kC_Programmer, kDO_CPUAndGPUMetrics, "CPU and GPU Metrics", 3.f, 0.f, 3.f, 1.f);
  AddOptionChoice(kDO_CPUAndGPUMetrics, "None", 0.f);
  AddOptionChoice(kDO_CPUAndGPUMetrics, "CPU", 1.f);
  AddOptionChoice(kDO_CPUAndGPUMetrics, "GPU", 2.f);
  AddOptionChoice(kDO_CPUAndGPUMetrics, "All", 3.f);
  AddOption(kC_Programmer, kDO_DumpMemoryAllocations, "Dump Memory Allocations", false);
  AddOption(kC_Programmer, kDO_DumpSimplePool, "Dump Simple Pool", false);
  AddOption(kC_Programmer, kDO_DumpLoadedTextures, "Dump Loaded Textures", false);
  AddOption(kC_Programmer, kDO_DumpSortedLists, "Dump Sorted Lists", false);
  AddOption(kC_Programmer, kDO_StateManagerNumbers, "State Manager Numbers", 5.f, 0.f, 8.f, 1.f);
  AddOptionChoice(kDO_StateManagerNumbers, "None", 0.f);
  AddOptionChoice(kDO_StateManagerNumbers, "All", 1.f);
  AddOptionChoice(kDO_StateManagerNumbers, "Think", 2.f);
  AddOptionChoice(kDO_StateManagerNumbers, "CameraThink", 3.f);
  AddOptionChoice(kDO_StateManagerNumbers, "ThinkSorted", 4.f);
  AddOptionChoice(kDO_StateManagerNumbers, "ObjectCount", 5.f);
  AddOptionChoice(kDO_StateManagerNumbers, "ObjectDrawTime", 6.f);
  AddOptionChoice(kDO_StateManagerNumbers, "GPU Profiler", 7.f);
  AddOptionChoice(kDO_StateManagerNumbers, "Update Frame", 8.f);
  AddOption(kC_Programmer, kDO_StateMgrRealNames, "State Mgr Real Names", false);
  AddOption(kC_Programmer, kDO_TerminateGame, "Terminate Game", false);
  AddOption(kC_Programmer, kDO_DebugMarkers, "Debug Markers", true);
  AddOption(kC_Programmer, kDO_ShowStreamingControl, "Show Streaming Control", false);
  AddOption(kC_Programmer, kDO_StreamingStatus, "Streaming Status", 0.f, 0.f, 4.f, 1.f);
  AddOptionChoice(kDO_StreamingStatus, "Off", 0.f);
  AddOptionChoice(kDO_StreamingStatus, "Token Counts", 1.f);
  AddOptionChoice(kDO_StreamingStatus, "Token Sizes(Slow)", 2.f);
  AddOptionChoice(kDO_StreamingStatus, "Counts + xtraCPU", 3.f);
  AddOptionChoice(kDO_StreamingStatus, "Sizes + xtraCPU", 4.f);
  AddOption(kC_Programmer, kDO_DebugMemoryCardSystem, "Debug Memory Card System", false);
  AddOption(kC_Programmer, kDO_LoadRELFilesOverBBA, "Load REL Files over BBA", true);

  // Renderer
  AddOption(kC_Renderer, kDO_ParticleCounts, "Particle Counts", false);
  AddOption(kC_Renderer, kDO_RenderParticles, "Render Particles", true);
  AddOption(kC_Renderer, kDO_DecalCounts, "Decal Counts", false);
  AddOption(kC_Renderer, kDO_Bloom, "Bloom", 1.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_Bloom, "OFF", 0.f);
  AddOptionChoice(kDO_Bloom, "ON", 1.f);
  AddOptionChoice(kDO_Bloom, "DEBUG", 2.f);
  AddOption(kC_Renderer, kDO_ShowFramerate, "Show Framerate", true);
  AddOption(kC_Renderer, kDO_DrawRendererBuckets, "Draw Renderer Buckets", false);
  AddOption(kC_Renderer, kDO_DisableFog, "Disable Fog", false);
  AddOption(kC_Renderer, kDO_CameraFilters, "Camera Filters", 0.f, 0.f, 2.f, 1.f);
  AddOption(kC_Renderer, kDO_PVS, "PVS", gPVSMode, 0.f, 3.f, 1.f);
  AddOptionChoice(kDO_PVS, "OFF [Show All]", 0.f);
  AddOptionChoice(kDO_PVS, "Enabled", 1.f);
  AddOptionChoice(kDO_PVS, "Reversed", 2.f);
  AddOptionChoice(kDO_PVS, "Actors Only", 3.f);
  AddOption(kC_Renderer, kDO_ShowObjectPVS, "ShowObjectPVS", 0.f, 0.f, 999.f, 1.f);
  AddOption(kC_Renderer, kDO_ShowLightPVS, "ShowLightPVS", 0.f, 0.f, 1.f, 1.f);
  AddOption(kC_Renderer, kDO_ReviewTextureSize, "Review texture size", false);
  bool designerMode = CDvdFile::FileExists("DesignerMode.txt");
  AddOption(kC_Renderer, kDO_CapTextureSize, "Cap texture size", designerMode);
  AddOption(kC_Renderer, kDO_Wireframe, "Wireframe", 0.f, 0.f, 5.f, 1.f);
  AddOptionChoice(kDO_Wireframe, "Normal", 0.f);
  AddOptionChoice(kDO_Wireframe, "Wire Models", 1.f);
  AddOptionChoice(kDO_Wireframe, "Wire World", 2.f);
  AddOptionChoice(kDO_Wireframe, "Wire All", 3.f);
  AddOptionChoice(kDO_Wireframe, "No World", 4.f);
  AddOptionChoice(kDO_Wireframe, "No World, Wire Models", 5.f);
  AddOption(kC_Renderer, kDO_Brightness, "Brightness", 1.f, 0.f, 2.1f, 0.1f);
  AddOption(kC_Renderer, kDO_PortalsEnabled, "Portals Enabled", 1.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_PortalsEnabled, "OFF", 0.f);
  AddOptionChoice(kDO_PortalsEnabled, "ON", 1.f);
  AddOptionChoice(kDO_PortalsEnabled, "ON(DEBUG)", 2.f);
  AddOption(kC_Renderer, kDO_ShowPortals, "Show Portals", 0.f, 0.f, 4.f, 1.f);
  AddOptionChoice(kDO_ShowPortals, "None", 0.f);
  AddOptionChoice(kDO_ShowPortals, "Culling Stats", 1.f);
  AddOptionChoice(kDO_ShowPortals, "Performance", 2.f);
  AddOptionChoice(kDO_ShowPortals, "Visualize Portals/Objects", 3.f);
  AddOptionChoice(kDO_ShowPortals, "Frustum planes", 4.f);
  AddOption(kC_Renderer, kDO_ShowPortalPlanes, "Show Portal Planes", 0.f, 0.f, 1.f, 1.f);
  AddOption(kC_Renderer, kDO_PerPolyDebugging, "Per Poly Debugging", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_PerPolyDebugging, "None", 0.f);
  AddOptionChoice(kDO_PerPolyDebugging, "Per Poly Passes", 1.f);
  AddOptionChoice(kDO_PerPolyDebugging, "Depth Complexity", 2.f);
  AddOption(kC_Renderer, kDO_DebugRenderingOctree, "Debug Rendering Octree", 0.f, 0.f, 2500.f, 1.f);
  AddOption(kC_Renderer, kDO_RenderAnimationJoints, "RenderAnimationJoints", 0.f, 0.f, 3.f, 1.f);
  AddOptionChoice(kDO_RenderAnimationJoints, "None", 0.f);
  AddOptionChoice(kDO_RenderAnimationJoints, "Joints Only", 1.f);
  AddOptionChoice(kDO_RenderAnimationJoints, "Names Only", 2.f);
  AddOptionChoice(kDO_RenderAnimationJoints, "Joints and Names", 3.f);
  AddOption(kC_Renderer, kDO_RenderAnimationSkeleton, "RenderAnimationSkeleton", false);
  AddOption(kC_Renderer, kDO_RenderPolyBoneCounts, "Render Poly/Bone Counts", false);

  // Gui
  AddOption(kC_Gui, kDO_DisableHUD, "Disable HUD", 0.f, 0.f, 1.f, 1.f);
  AddOptionChoice(kDO_DisableHUD, "Enabled", 0.f);
  AddOptionChoice(kDO_DisableHUD, "Disabled", 1.f);
  AddOption(kC_Gui, kDO_ShowRedundantHints, "Show Redundant Hints", 0.f, 0.f, 2.f, 1.f);
  AddOption(kC_Gui, kDO_RedundantHintQuickTimeout, "Redundant Hint Quick Timeout", false);
  AddOption(kC_Gui, kDO_ProfileGuiElements, "Profile Gui Elements", 0.f, 0.f, 3.f, 1.f);
  AddOption(kC_Gui, kDO_EnableHud, "Enable Hud", gpTweakGui->GetEnableHud(), 0.f, 5.f, 1.f);
  AddOption(kC_Gui, kDO_EnableTargeting, "Enable Targeting", gpTweakGui->GetEnableTargeting(), 0.f,
            1.f, 1.f);
  AddOption(kC_Gui, kDO_EnableAutoMapper, "Enable AutoMapper", gpTweakGui->GetEnableAutoMapper(),
            0.f, 1.f, 1.f);
  AddOption(kC_Gui, kDO_RadarMode, "Radar Mode", gpTweakGui->GetRadarMode(), 0.f, 4.f, 1.f);
  AddOption(kC_Gui, kDO_EnableVisors, "Enable Visors", true);
  AddOption(kC_Gui, kDO_HUDCameraFOV, "HUD Camera FOV", gpTweakGui->GetHudCameraFov(), 0.f, 15.f,
            1.f);
  AddOption(kC_Gui, kDO_HUDCameraY, "HUD Camera Y (forward/backward)", gpTweakGui->GetHudCameraY(),
            0.f, 63.f, 1.f);
  AddOption(kC_Gui, kDO_HUDCameraZ, "HUD Camera Z (up/down)", gpTweakGui->GetHudCameraZ(), 0.f,
            31.f, 1.f);
  AddOption(kC_Gui, kDO_VisorBeamIconsAlwaysShow, "Visor/Beam Icons always show", true);
  AddOption(kC_Gui, kDO_FaceReflectMode, "Face Reflect Mode", gFaceReflectMode, 0.f, 3.f, 1.f);
  AddOption(kC_Gui, kDO_FaceReflectionWidth, "Face Reflection Width",
            gpTweakGui->GetFaceReflectionWidth(), 0.f, 20.f, 1.f);
  AddOption(kC_Gui, kDO_FaceReflectionHeight, "Face Reflection Height",
            gpTweakGui->GetFaceReflectionHeight(), 0.f, 20.f, 1.f);
  AddOption(kC_Gui, kDO_FaceReflectionPositionY, "Face Reflection PositionY",
            gpTweakGui->GetFaceReflectionPositionY(), 0.f, 9.f, 1.f);
  AddOption(kC_Gui, kDO_FaceReflectionPositionZ, "Face Reflection PositionZ",
            gpTweakGui->GetFaceReflectionPositionZ(), 0.f, 20.f, 1.f);
  AddOption(kC_Gui, kDO_FaceReflectionAspectRatio, "Face Reflection Aspect Ratio",
            gpTweakGui->GetFaceReflectionAspectRatio(), 0.f, 20.f, 1.f);
  AddOption(kC_Gui, kDO_WidescreenBallmode, "Widescreen Ballmode", gWidescreenBallMode);
  AddOption(kC_Gui, kDO_ShowSafeFrame, "Show Safe Frame", false);
  AddOption(kC_Gui, kDO_Vertical2PlayerSplitScreen, "Vertical2PlayerSplitScreen", false);
  AddOption(kC_Gui, kDO_SingleScreenForMultiplayer, "SingleScreenForMultiplayer", false);
  AddOption(kC_Gui, kDO_ScanTextDebugger, "ScanTextDebugger", false);
  AddOption(kC_Gui, kDO_AlwaysSortMapSurfaces, "AlwaysSortMapSurfaces", false);

  // PowerupsWeapons
  AddOption(kC_PowerupsWeapons, kDO_PowerBeam, "PowerBeam", 0.f, 0.f, GetPowerUpMax(kDO_PowerBeam),
            1.f);
  AddOption(kC_PowerupsWeapons, kDO_PlasmaBeam, "PlasmaBeam", 0.f, 0.f,
            GetPowerUpMax(kDO_PlasmaBeam), 1.f);
  AddOption(kC_PowerupsWeapons, kDO_NovaBeam, "NovaBeam", 0.f, 0.f, GetPowerUpMax(kDO_NovaBeam),
            1.f);
  AddOption(kC_PowerupsWeapons, kDO_ChargeUpgrade, "ChargeUpgrade", 0.f, 0.f,
            GetPowerUpMax(kDO_ChargeUpgrade), 1.f);
  AddOption(kC_PowerupsWeapons, kDO_Missile, "Missile", 0.f, 0.f, GetPowerUpMax(kDO_Missile), 1.f);
  AddOption(kC_PowerupsWeapons, kDO_IceMissile, "IceMissile", 0.f, 0.f,
            GetPowerUpMax(kDO_IceMissile), 1.f);
  AddOption(kC_PowerupsWeapons, kDO_SeekerMissile, "SeekerMissile", 0.f, 0.f,
            GetPowerUpMax(kDO_SeekerMissile), 1.f);
  AddOption(kC_PowerupsWeapons, kDO_GrappleBeam, "GrappleBeam", 0.f, 0.f,
            GetPowerUpMax(kDO_GrappleBeam), 1.f);
  AddOption(kC_PowerupsWeapons, kDO_GrappleBeamVoltage, "GrappleBeamVoltage", 0.f, 0.f,
            GetPowerUpMax(kDO_GrappleBeamVoltage), 1.f);
  AddOption(kC_PowerupsWeapons, kDO_Bomb, "Bomb", 0.f, 0.f, GetPowerUpMax(kDO_Bomb), 1.f);
  AddOption(kC_PowerupsWeapons, kDO_HyperShot, "HyperShot", 0.f, 0.f, GetPowerUpMax(kDO_HyperShot),
            1.f);
  AddOptionChoice(kDO_HyperShot, "Off", 0.f);
  AddOptionChoice(kDO_HyperShot, "Tap", 1.f);
  AddOptionChoice(kDO_HyperShot, "Hold", 2.f);

  // PowerupsUpgrades
  AddOption(kC_PowerupsUpgrades, kDO_CombatVisor, "CombatVisor", 0.f, 0.f,
            GetPowerUpMax(kDO_CombatVisor), 1.f);
  AddOption(kC_PowerupsUpgrades, kDO_ScanVisor, "ScanVisor", 0.f, 0.f, GetPowerUpMax(kDO_ScanVisor),
            1.f);
  AddOption(kC_PowerupsUpgrades, kDO_CommandVisor, "CommandVisor", 0.f, 0.f,
            GetPowerUpMax(kDO_CommandVisor), 1.f);
  AddOption(kC_PowerupsUpgrades, kDO_XRayVisor, "XRayVisor", 0.f, 0.f, GetPowerUpMax(kDO_XRayVisor),
            1.f);
  AddOption(kC_PowerupsUpgrades, kDO_VariaSuit, "VariaSuit", 0.f, 0.f, GetPowerUpMax(kDO_VariaSuit),
            1.f);
  AddOption(kC_PowerupsUpgrades, kDO_DoubleJump, "DoubleJump", 0.f, 0.f,
            GetPowerUpMax(kDO_DoubleJump), 1.f);
  AddOption(kC_PowerupsUpgrades, kDO_ScrewAttack, "ScrewAttack", 0.f, 0.f,
            GetPowerUpMax(kDO_ScrewAttack), 1.f);

  // PowerupsMisc
  AddOption(kC_PowerupsMisc, kDO_Energy, "Energy", 0.f, 0.f, GetPowerUpMax(kDO_Energy), 1.f);
  AddOption(kC_PowerupsMisc, kDO_EnergyTank, "EnergyTank", 0.f, 0.f, GetPowerUpMax(kDO_EnergyTank),
            1.f);
  AddOption(kC_PowerupsMisc, kDO_ItemPercentage, "ItemPercentage", 0.f, 0.f,
            GetPowerUpMax(kDO_ItemPercentage), 1.f);

  // PowerupsMorphball
  AddOption(kC_PowerupsMorphball, kDO_MorphBall, "MorphBall", 0.f, 0.f,
            GetPowerUpMax(kDO_MorphBall), 1.f);
  AddOption(kC_PowerupsMorphball, kDO_BoostBall, "BoostBall", 0.f, 0.f,
            GetPowerUpMax(kDO_BoostBall), 1.f);
  AddOption(kC_PowerupsMorphball, kDO_SpiderBall, "SpiderBall", 0.f, 0.f,
            GetPowerUpMax(kDO_SpiderBall), 1.f);
  AddOption(kC_PowerupsMorphball, kDO_PhazonBall, "PhazonBall", 0.f, 0.f,
            GetPowerUpMax(kDO_PhazonBall), 1.f);
  AddOption(kC_PowerupsMorphball, kDO_IceBall, "IceBall", 0.f, 0.f, GetPowerUpMax(kDO_IceBall),
            1.f);
  AddOption(kC_PowerupsMorphball, kDO_FireBallFlammable, "FireBallFlammable", 0.f, 0.f,
            GetPowerUpMax(kDO_FireBallFlammable), 1.f);
  AddOption(kC_PowerupsMorphball, kDO_FireBall, "FireBall", 0.f, 0.f, GetPowerUpMax(kDO_FireBall),
            1.f);
  AddOption(kC_PowerupsMorphball, kDO_CannonBall, "CannonBall", 0.f, 0.f,
            GetPowerUpMax(kDO_CannonBall), 1.f);
  AddOption(kC_PowerupsMorphball, kDO_ActivateMorphballBoost, "ActivateMorphballBoost", 0.f, 0.f,
            GetPowerUpMax(kDO_ActivateMorphballBoost), 1.f);

  // Audio
  AddOption(kC_Audio, kDO_MusicOnOff, "Music On/Off", gpTweakGame->GetMusicOnByDefault());
  AddOption(kC_Audio, kDO_EnableAudio, "Enable Audio", true);
  AddOption(kC_Audio, kDO_SoundMode, "Sound Mode", gpGameState->GameOptions().GetSoundMode(), 0.f,
            2.f, 1.f);
  AddOptionChoice(kDO_SoundMode, "Mono", 0.f);
  AddOptionChoice(kDO_SoundMode, "Stereo", 1.f);
  AddOptionChoice(kDO_SoundMode, "Surround", 2.f);
  AddOption(kC_Audio, kDO_SoundAcousticsEnabled, "Sound Acoustics Enabled", true);
  AddOption(kC_Audio, kDO_LoadTweaksFromPCHost, "Load Tweaks from PC Host", false);
  AddOption(kC_Audio, kDO_SaveTweaksToPCHost, "Save Tweaks to PC Host", false);
  AddOption(kC_Audio, kDO_LoadTweaksFromMemoryCard, "Load Tweaks from Memory Card", false);
  AddOption(kC_Audio, kDO_SaveTweaksToMemoryCard, "Save Tweaks to Memory Card", false);
  AddOption(kC_Audio, kDO_SfxMasterVolume, "Sfx Master Volume", 105.f, 0.f, 105.f, 1.f);
  AddOption(kC_Audio, kDO_MusicMasterVolume, "Music Master Volume", 100.f, 0.f, 100.f, 1.f);
  AddOption(kC_Audio, kDO_MinimumShakeAmplitude, "Minimum Shake Amplitude", gMinimumShakeAmplitude,
            0.f, 10.f, 0.01f);
  AddOption(kC_Audio, kDO_CheckMultiplayerConflicts, "Check Multiplayer Conflicts", true);

  // AudioDebug
  AddOption(kC_AudioDebug, kDO_DebugSoundSystem, "Debug Sound System", 0.f, 0.f, 3.f, 1.f);
  AddOptionChoice(kDO_DebugSoundSystem, "Off", 0.f);
  AddOptionChoice(kDO_DebugSoundSystem, "Debug Text", 1.f);
  AddOptionChoice(kDO_DebugSoundSystem, "Debug Snd Mod", 2.f);
  AddOptionChoice(kDO_DebugSoundSystem, "Serial Out", 3.f);
  AddOption(kC_AudioDebug, kDO_ShowSoundPositions, "Show Sound Positions", 0.f, 0.f, 5.f, 1.f);
  AddOptionChoice(kDO_ShowSoundPositions, "Off", 0.f);
  AddOptionChoice(kDO_ShowSoundPositions, "Position", 1.f);
  AddOptionChoice(kDO_ShowSoundPositions, "Pos Min", 2.f);
  AddOptionChoice(kDO_ShowSoundPositions, "Pos Min Txt", 3.f);
  AddOptionChoice(kDO_ShowSoundPositions, "Pos Min Max", 4.f);
  AddOptionChoice(kDO_ShowSoundPositions, "Pos Min Max Txt", 5.f);
  AddOption(kC_AudioDebug, kDO_ShowMusicStreams, "Show Music Streams", false);
  AddOption(kC_AudioDebug, kDO_ShowFModMetrics, "Show FMod Metrics", false);
  AddOption(kC_AudioDebug, kDO_ProfileDSP, "Profile DSP", false);
  AddOption(kC_AudioDebug, kDO_CaptureShortAudioClip, "Capture Short Audio Clip", false);
  AddOption(kC_AudioDebug, kDO_RenderAudioFifo, "Render Audio Fifo", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_RenderAudioFifo, "Off", 0.f);
  AddOptionChoice(kDO_RenderAudioFifo, "On", 1.f);
  AddOptionChoice(kDO_RenderAudioFifo, "Pause", 2.f);
  AddOption(kC_AudioDebug, kDO_ShowAudioListener, "Show Audio Listener", 0.f, 0.f, 2.f, 1.f);

  // Scripting
  AddOption(kC_Scripting, kDO_ScriptMsgDebugger, "Script msg Debugger", 0.f, 0.f, 3.f, 1.f);
  const char* objectTypes[] = {
      "Any",    "Actor",      "Player",          "Trigger",     "Platform", "Patterned",
      "Camera", "CameraHint", "PlayerHint",      "ControlHint", "Weapon",   "Door",
      "Dock",   "Effect",     "LayerController", "Waypoint",    "Sound",
  };
  const char* messageTypes[] = {
      "Any",  "Action",    "Activate", "Deactivate", "Increment",    "Decrement",   "Start",
      "Stop", "SetToZero", "Entered",  "Damage",     "EnteredFluid", "ExitedFluid", "InsideFluid",
  };
  const char* states[] = {
      "Any",     "Active", "Arrive", "Damage",     "Dead",
      "Entered", "Exited", "Inside", "MaxReached", "Zero",
  };
  AddOption(kC_Scripting, kDO_ScriptMsgSender, "Script msg Sender", 0.f, 0.f, 16.f, 1.f);
  AddOption(kC_Scripting, kDO_ScriptMsgTarget, "Script msg Target", 0.f, 0.f, 16.f, 1.f);
  for (int i = 0; i < 17; ++i) {
    AddOptionChoice(kDO_ScriptMsgSender, objectTypes[i], i);
    AddOptionChoice(kDO_ScriptMsgTarget, objectTypes[i], i);
  }
  AddOption(kC_Scripting, kDO_ScriptMsgType, "Script msg Type", 0.f, 0.f, 13.f, 1.f);
  AddOption(kC_Scripting, kDO_ScriptMsgExcludeType, "Script msg Exclude Type", 0.f, 0.f, 13.f, 1.f);
  AddOptionChoice(kDO_ScriptMsgType, messageTypes[0], 0.f);
  AddOptionChoice(kDO_ScriptMsgExcludeType, "None", 0.f);
  for (int i = 1; i < 14; ++i) {
    AddOptionChoice(kDO_ScriptMsgType, messageTypes[i], i);
    AddOptionChoice(kDO_ScriptMsgExcludeType, messageTypes[i], i);
  }
  AddOption(kC_Scripting, kDO_ScriptMsgState, "Script msg State", 0.f, 0.f, 9.f, 1.f);
  AddOption(kC_Scripting, kDO_ScriptMsgExcludeState, "Script msg Exclude State", 0.f, 0.f, 9.f,
            1.f);
  AddOptionChoice(kDO_ScriptMsgState, states[0], 0.f);
  AddOptionChoice(kDO_ScriptMsgExcludeState, "None", 0.f);
  for (int i = 1; i < 10; ++i) {
    AddOptionChoice(kDO_ScriptMsgState, states[i], i);
    AddOptionChoice(kDO_ScriptMsgExcludeState, states[i], i);
  }
  AddOption(kC_Scripting, kDO_CacheRepeatedScriptMessages, "Cache repeated script messages", false);
  AddOption(kC_Scripting, kDO_PCLoadScriptObjects, "PC Load script objects", false);
  AddOption(kC_Scripting, kDO_LogScriptMessageQueue, "Log script message queue", false);
  const char* layerVerbosity[] = {"None", "Events", "AreaInfo"};
  AddOption(kC_Scripting, kDO_ScriptingLayersVerbose, "Scripting Layers Verbose", 0.f, 0.f, 2.f,
            1.f);
  for (int i = 0; i < 3; ++i) {
    AddOptionChoice(kDO_ScriptingLayersVerbose, layerVerbosity[i], i);
  }
  AddOption(kC_Scripting, kDO_LogScriptObjectLoads, "Log script object loads", false);
  AddOption(kC_Scripting, kDO_DumpScriptObject, "Dump script object", false);
  AddOption(kC_Scripting, kDO_ShowGeneratorMessages, "Show Generator Messages", false);

  // Misc
  AddOption(kC_Misc, kDO_ShowWeaponDamageRadius, "Show Weapon Damage Radius", false);
  AddOption(kC_Misc, kDO_ShowScanInfo, "Show Scan Info", false);
  AddOption(kC_Misc, kDO_ShowSplinePaths, "Show Spline Paths", false);
  const char* languages[] = {"English", "German", "French", "Spanish", "Italian", "Dutch"};
  AddOption(kC_Misc, kDO_Language, "Language (requires reset)",
            gpMain->GetOsContext()->GetLanguage(), 0.f, 5.f, 1.f);
  for (int i = 0; i <= 5; ++i) {
    AddOptionChoice(kDO_Language, languages[i], i);
  }
  AddOption(kC_Misc, kDO_GenericMsgs, "Generic Msgs", true);
  AddOption(kC_Misc, kDO_DebugMessagesEnabled, "Debug Messages Enabled", 1.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_DebugMessagesEnabled, "Off", 0.f);
  AddOptionChoice(kDO_DebugMessagesEnabled, "On", 1.f);
  AddOptionChoice(kDO_DebugMessagesEnabled, "Log", 2.f);
  AddOption(kC_Misc, kDO_SaveDebugMessageLog, "Save Debug Message Log", false);
  AddOption(kC_Misc, kDO_DebugSaveFormat, "Debug Save Format", false);
  AddOption(kC_Misc, kDO_DebugMessageColor, "Debug Message Color", 0.f, 0.f, 3.f, 1.f);
  AddOptionChoice(kDO_DebugMessageColor, "White", 0.f);
  AddOptionChoice(kDO_DebugMessageColor, "Black", 1.f);
  AddOptionChoice(kDO_DebugMessageColor, "Grey", 2.f);
  AddOptionChoice(kDO_DebugMessageColor, "Orange", 3.f);
  AddOption(kC_Misc, kDO_FrontendStartScreenVolume, "Frontend StartScreen volume", 95.f, 0.f, 127.f,
            1.f);
  AddOption(kC_Misc, kDO_FrontendStartScreenVolume2, "Frontend StartScreen volume", 95.f, 0.f,
            127.f, 1.f);
  AddOption(kC_Misc, kDO_ShowHintInfo, "Show Hint Info", 0.f, 0.f, 5.f, 1.f);
  AddOption(kC_Misc, kDO_FakePAL50HzUpdate, "Fake PAL 50Hz Update", false);
  AddOption(kC_Misc, kDO_SlowGameDown, "Slow game down", false);
  AddOption(kC_Misc, kDO_ShowBBAMessages, "Show BBA Messages", false);
  AddOption(kC_Misc, kDO_ShowWeaponHomingTargets, "Show Weapon Homing Targets", false);

  // Cheats
  AddOption(kC_Cheats, kDO_InvulnerableSamus, "Invulnerable Samus", false);
  AddOption(kC_Cheats, kDO_GiveAllPowerupsCheat, "Give all powerups cheat", false);
  AddOption(kC_Cheats, kDO_MapCheatEnabled, "Map Cheat Enabled",
            gpTweakAutoMapper->GetMapCheatEnabled());
  AddOption(kC_Cheats, kDO_LogBookCheatEnabled, "LogBook Cheat Enabled", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_LogBookCheatEnabled, "Off", 0.f);
  AddOptionChoice(kDO_LogBookCheatEnabled, "On", 1.f);
  AddOptionChoice(kDO_LogBookCheatEnabled, "Show Memory", 2.f);
  AddOption(kC_Cheats, kDO_KillAllAIs, "Kill All AIs", false);
  AddOption(kC_Cheats, kDO_SlowAllAIs, "Slow All AIs", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_SlowAllAIs, "Off", 0.f);
  AddOptionChoice(kDO_SlowAllAIs, "Slow", 1.f);
  AddOptionChoice(kDO_SlowAllAIs, "Very Slow", 2.f);
  AddOption(kC_Cheats, kDO_DisableAIUpdates, "Disable AI Updates", 0.f, 0.f, 1.f, 1.f);
  AddOptionChoice(kDO_DisableAIUpdates, "Enable", 0.f);
  AddOptionChoice(kDO_DisableAIUpdates, "Disable", 1.f);
  AddOption(kC_Cheats, kDO_AllMultiplayerMusicUnlocked, "All Multiplayer Music Unlocked", false);
  AddOption(kC_Cheats, kDO_AllMultiplayerMapsUnlocked, "All Multiplayer Maps Unlocked", false);

  // Player
  AddOption(kC_Player, kDO_DisablePlayerLockon, "DisablePlayerLockon(restart)",
            gDisablePlayerLockon);
  AddOption(kC_Player, kDO_PlayerPositionInfo, "Player Position Info", false);
  AddOption(kC_Player, kDO_ShowOrbitPoint, "Show Orbit Point",
            gpTweakTargeting->GetShowOrbitPoint());
  AddOption(kC_Player, kDO_ShowZones, "Show Zones", 0.f, 0.f, 7.f, 1.f);
  AddOptionChoice(kDO_ShowZones, "Off", 0.f);
  AddOptionChoice(kDO_ShowZones, "Object List / Orbit", 1.f);
  AddOptionChoice(kDO_ShowZones, "Object List / Scannable", 2.f);
  AddOptionChoice(kDO_ShowZones, "Outside Zone / Orbit", 3.f);
  AddOptionChoice(kDO_ShowZones, "Outside Zone / Scannable", 4.f);
  AddOptionChoice(kDO_ShowZones, "Onscreen / Orbit", 5.f);
  AddOptionChoice(kDO_ShowZones, "Onscreen / Scannable", 6.f);
  AddOptionChoice(kDO_ShowZones, "Aim Target Only", 7.f);
  AddOption(kC_Player, kDO_AutoAim, "Auto Aim", gAutoAim);
  AddOption(kC_Player, kDO_NormalTurnFactor, "NormalTurnFactor",
            gpTweakPlayer->GetNormalTurnFactor(), 1.f, 1000.f, 1.f);
  AddOption(kC_Player, kDO_FreeLookTurnFactor, "FreeLookTurnFactor",
            gpTweakPlayer->GetFreeLookTurnFactor(), 1.f, 1000.f, 1.f);
  AddOption(kC_Player, kDO_ShowReflection, "Show reflection", gShowReflection);
  AddOption(kC_Player, kDO_ScanFreezesGame, "Scan Freezes Game",
            gpTweakPlayer->GetScanFreezesGame());
  AddOption(kC_Player, kDO_ScanRequiresLineOfSight, "Scan Requires Line Of Sight",
            gpTweakPlayer->GetScanLineOfSight());
  AddOption(kC_Player, kDO_ScanSnapsToPOI, "Scan snaps to POI", false);
  AddOption(kC_Player, kDO_DamageForcesBallTransition, "Damage Forces Ball Transition",
            gDamageForcesBallTransition, 0.f, 100.f, 1.f);
  AddOption(kC_Player, kDO_MorphballInvunTime120s, "Morphball invun time 1/20s", 10.f, 1.f, 100.f,
            1.f);
  AddOption(kC_Player, kDO_ShieldAllowsMovement, "Shield Allows Movement",
            gpTweakPlayer->GetShieldAllowsMotion());
  AddOption(kC_Player, kDO_MorphballBreaksLockon, "MorphballBreaksLockon", gMorphballBreaksLockon,
            0.f, 2.f, 1.f);
  AddOptionChoice(kDO_MorphballBreaksLockon, "No", 0.f);
  AddOptionChoice(kDO_MorphballBreaksLockon, "On Transition", 1.f);
  AddOptionChoice(kDO_MorphballBreaksLockon, "Always", 2.f);
  AddOption(kC_Player, kDO_ShowPlayerAnimationAndFlow, "ShowPlayerAnimationAndFlow", false);
  AddOption(kC_Player, kDO_Use25PercentStepUpInPMovement, "Use25PercentStepUpInPMovement", true);
  AddOption(kC_Player, kDO_UseOldPlayerCollisionBox, "UseOldPlayerCollisionBox", false);
  AddOption(kC_Player, kDO_KillPlayer, "Kill Player", false);
  AddOption(kC_Player, kDO_FrozenJostleCount, "Frozen Jostle Count", 2.f, 0.f, 100.f, 1.f);
  AddOption(kC_Player, kDO_ApplyPlayerKnockbackForce, "Apply player knockback force", true);

  // PlayerGun
  AddOption(kC_PlayerGun, kDO_PlayMinorFidget, "Play Minor Fidget", 0.f, 0.f, 5.f, 1.f);
  AddOption(kC_PlayerGun, kDO_PlayMajorFidget, "Play Major Fidget", 0.f, 0.f, 6.f, 1.f);
  AddOption(kC_PlayerGun, kDO_ChargeBeam, "Charge Beam", true);
  AddOption(kC_PlayerGun, kDO_InfiniteComboAmmo, "Infinite Combo Ammo", false);
  AddOption(kC_PlayerGun, kDO_PowerupSuckDistance, "Powerup Suck Distance", gPowerupSuckDistance,
            0.f, 1024.f, 1.f);
  AddOption(kC_PlayerGun, kDO_RenderGun, "Render Gun", true);
  AddOption(kC_PlayerGun, kDO_ShowBeamAmmo, "Show Beam Ammo", false);
  AddOption(kC_PlayerGun, kDO_NormalSeekerAlwaysHomes, "Normal seeker always homes", true);
  AddOption(kC_PlayerGun, kDO_ShowGunState, "Show gun state", 0.f, 0.f, 2.f, 1.f);
  const char* gunStates[] = {"Off", "Gun", "LeftArm"};
  for (int i = 0; i < 3; ++i) {
    AddOptionChoice(kDO_ShowGunState, gunStates[i], i);
  }
  AddOption(kC_PlayerGun, kDO_GunDebug, "Gun Debug", 0.f, 0.f, 8.f, 1.f);
  const char* gunDebugBeams[] = {"Off",     "Power",  "Dark",   "Light", "Annihilator",
                                 "Missile", "Seeker", "Phazon", "All"};
  for (int i = 0; i < 9; ++i) {
    AddOptionChoice(kDO_GunDebug, gunDebugBeams[i], i);
  }
  AddOption(kC_PlayerGun, kDO_GunDebugOutput, "Gun Debug Output", 3.f, 0.f, 3.f, 1.f);
  const char* gunDebugOutputs[] = {"Console", "Debugger", "BBA", "All"};
  for (int i = 0; i < 4; ++i) {
    AddOptionChoice(kDO_GunDebugOutput, gunDebugOutputs[i], i);
  }

  // AI
  AddOption(kC_AI, kDO_ShowPatternedStates, "Show Patterned States", false);
  AddOption(kC_AI, kDO_ShowHP, "Show HP", false);
  AddOption(kC_AI, kDO_ShowDistanceFromPlayer, "Show Distance From Player", 0.f, 0.f, 3.f, 1.f);
  AddOptionChoice(kDO_ShowDistanceFromPlayer, "Off", 0.f);
  AddOptionChoice(kDO_ShowDistanceFromPlayer, "True Distance", 1.f);
  AddOptionChoice(kDO_ShowDistanceFromPlayer, "XY Dist Only", 2.f);
  AddOptionChoice(kDO_ShowDistanceFromPlayer, "Z Dist Only", 3.f);
  const char* pathFindingModes[] = {"None", "Paths", "Paths + Floors", "Paths + Floors + Regions",
                                    "All"};
  AddOption(kC_AI, kDO_PathFinding, "Path Finding", 0.f, 0.f, 4.f, 1.f);
  for (int i = 0; i < 5; ++i) {
    AddOptionChoice(kDO_PathFinding, pathFindingModes[i], i);
  }
  const char* pathFindingPoints[] = {"None", "Points", "Points + Paths"};
  AddOption(kC_AI, kDO_PathFindingPoints, "Path Finding Points", 0.f, 0.f, 2.f, 1.f);
  for (int i = 0; i < 3; ++i) {
    AddOptionChoice(kDO_PathFindingPoints, pathFindingPoints[i], i);
  }
  AddOption(kC_AI, kDO_PathFindingOctree, "Path Finding Octree", 0.f, 0.f, 2.f, 1.f);
  AddOption(kC_AI, kDO_ShowWaypoints, "Show Waypoints", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_ShowWaypoints, "Off", 0.f);
  AddOptionChoice(kDO_ShowWaypoints, "Points", 1.f);
  AddOptionChoice(kDO_ShowWaypoints, "Points+Connections", 2.f);
  AddOption(kC_AI, kDO_AIShotPrediction, "AI Shot Prediction", 1.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_AIShotPrediction, "Off", 0.f);
  AddOptionChoice(kDO_AIShotPrediction, "On", 1.f);
  AddOptionChoice(kDO_AIShotPrediction, "Reduced", 2.f);
  AddOption(kC_AI, kDO_DrawAIDamageLocator, "Draw AI Damage Locator(s)", 0.f, 0.f, 1.f, 1.f);
  AddOption(kC_AI, kDO_DebugBodyStates, "Debug Body States", false);
  AddOption(kC_AI, kDO_ShowTeamPositions, "Show Team Positions", false);
  AddOption(kC_AI, kDO_DebugAnimations, "Debug Animations", false);
  AddOption(kC_AI, kDO_DisableAnimParticles, "Disable Anim Particles", 0.f, 0.f, 1.f, 1.f);
  AddOptionChoice(kDO_DisableAnimParticles, "Enabled", 0.f);
  AddOptionChoice(kDO_DisableAnimParticles, "Disabled", 1.f);
  AddOption(kC_AI, kDO_Shockwaves, "Shockwaves", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_Shockwaves, "No Debug Info", 0.f);
  AddOptionChoice(kDO_Shockwaves, "Debug with shockwave", 1.f);
  AddOptionChoice(kDO_Shockwaves, "Debug w/o shockwave", 2.f);
  AddOption(kC_AI, kDO_ShowBoneTracking, "Show Bone Tracking", false);
  AddOption(kC_AI, kDO_ShowBodyAlignment, "Show Body Alignment", false);
  AddOption(kC_AI, kDO_Attachments, "Attachments", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_Attachments, "Default", 0.f);
  AddOptionChoice(kDO_Attachments, "Hide", 1.f);
  AddOptionChoice(kDO_Attachments, "Cycle", 2.f);
  AddOption(kC_AI, kDO_ShowFootTracking, "Show Foot Tracking", false);

  // AITrace
  AddOption(kC_AITrace, kDO_AITraceViewMode, "View Mode", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_AITraceViewMode, "Off", 0.f);
  AddOptionChoice(kDO_AITraceViewMode, "List", 1.f);
  AddOptionChoice(kDO_AITraceViewMode, "3D", 2.f);
  AddOption(kC_AITrace, kDO_AITracePauseAIsWhileViewing, "Pause AIs While Viewing", true);
  AddOption(kC_AITrace, kDO_AITraceViewStates, " View States        ", true);
  AddOption(kC_AITrace, kDO_AITraceViewFunctions, " View Functions     ", true);
  AddOption(kC_AITrace, kDO_AITraceViewFiredTriggers, " View Fired Triggers", false);
  AddOption(kC_AITrace, kDO_AITraceViewBodyStates, " View BodyStates    ", false);
  AddOption(kC_AITrace, kDO_AITraceViewPathfinding, " View Pathfinding   ", false);
  AddOption(kC_AITrace, kDO_AITraceViewScriptMsgs, " View Script Msgs   ", false);
  AddOption(kC_AITrace, kDO_AITraceViewAnimations, " View Animations    ", false);
  AddOption(kC_AITrace, kDO_AITraceViewAnimEvts, " View Anim Evts     ", false);
  AddOption(kC_AITrace, kDO_AITraceViewDamageInfo, " View Damage Info   ", false);
  AddOption(kC_AITrace, kDO_AITraceViewMaterials, " View Materials     ", false);
  AddOption(kC_AITrace, kDO_AITraceViewCustomMsgs, " View Custom Msgs   ", true);
  AddOption(kC_AITrace, kDO_AITraceTimeFormat, "Time Format", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_AITraceTimeFormat, " Elapsed Think Ticks", 0.f);
  AddOptionChoice(kDO_AITraceTimeFormat, " Elapsed Time", 1.f);
  AddOptionChoice(kDO_AITraceTimeFormat, " Off", 2.f);
  AddOption(kC_AITrace, kDO_AITraceShowCollisionForCurrentAI, "Show Collision For Current AI",
            true);
  AddOption(kC_AITrace, kDO_AITraceRecordingEnabled, "Recording Enabled", true);
  AddOption(kC_AITrace, kDO_AITraceAlsoRecordXLNDXLSGXON, " Also record XLND,XLSG,XON*", false);
  AddOption(kC_AITrace, kDO_AITraceAlsoRecordLoopedSoundStop, " Also record LoopedSoundStop",
            false);
  AddOption(kC_AITrace, kDO_AITraceAlsoRecordFunctionCalls, "Also record Function calls", false);

  // AICreatures
  AddOption(kC_AICreatures, kDO_RundasArmor, "Rundas Armor", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_RundasArmor, "Default", 0.f);
  AddOptionChoice(kDO_RundasArmor, "Always Draw", 1.f);
  AddOptionChoice(kDO_RundasArmor, "Never Draw", 2.f);
  AddOption(kC_AICreatures, kDO_RundasHop, "Rundas Hop", 0.f, 0.f, 1.f, 1.f);
  AddOptionChoice(kDO_RundasHop, "Off", 0.f);
  AddOptionChoice(kDO_RundasHop, "Draw Lines", 1.f);
  AddOption(kC_AICreatures, kDO_Swarms, "Swarms", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_Swarms, "No Debug Info", 0.f);
  AddOptionChoice(kDO_Swarms, "Draw Looped Sound Emitters", 1.f);
  AddOptionChoice(kDO_Swarms, "Draw Looped Sound Emitters + Grid", 2.f);
  AddOption(kC_AICreatures, kDO_FishCloud, "FishCloud", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_FishCloud, "No Debug Info", 0.f);
  AddOptionChoice(kDO_FishCloud, "Draw direction vector", 1.f);
  AddOptionChoice(kDO_FishCloud, "Draw attack properties", 2.f);
  AddOption(kC_AICreatures, kDO_RundasLOSTesting, "Rundas LOS Testing", 0.f, 0.f, 1.f, 1.f);
  // The named values of "Rundas LOS Testing" are added to "Rundas Hop".
  AddOptionChoice(kDO_RundasHop, "Off", 0.f);
  AddOptionChoice(kDO_RundasHop, "ON", 1.f);
  AddOption(kC_AICreatures, kDO_RundasKillPillars, "Rundas Kill Pillars", 0.f, 0.f, 1.f, 1.f);
  AddOptionChoice(kDO_RundasKillPillars, "Off", 0.f);
  AddOptionChoice(kDO_RundasKillPillars, "ON", 1.f);
  AddOption(kC_AICreatures, kDO_RundasErrorCorrect, "Rundas Error Correct", 0.f, 0.f, 1.f, 1.f);
  AddOptionChoice(kDO_RundasErrorCorrect, "ON", 0.f);
  AddOptionChoice(kDO_RundasErrorCorrect, "Off", 1.f);
  AddOption(kC_AICreatures, kDO_RundasBattleStage, "Rundas Battle Stage", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_RundasBattleStage, "Default", 0.f);
  AddOptionChoice(kDO_RundasBattleStage, "Stage 1", 1.f);
  AddOptionChoice(kDO_RundasBattleStage, "Stage 2", 2.f);
  AddOption(kC_AICreatures, kDO_Korakk, "Korakk", 0.f, 0.f, 10.f, 1.f);
  AddOptionChoice(kDO_Korakk, "Normal", 0.f);
  AddOptionChoice(kDO_Korakk, "Force RearUp", 1.f);
  AddOptionChoice(kDO_Korakk, "Force Target Korakk", 2.f);
  AddOptionChoice(kDO_Korakk, "Force Target Rider", 3.f);
  AddOptionChoice(kDO_Korakk, "Show Tongue End", 4.f);
  AddOptionChoice(kDO_Korakk, "Show Stab Range", 5.f);
  AddOptionChoice(kDO_Korakk, "Show Stomp Range", 6.f);
  AddOptionChoice(kDO_Korakk, "Show PhazonBolt Range", 7.f);
  AddOptionChoice(kDO_Korakk, "Show TongueDart Range", 8.f);
  AddOptionChoice(kDO_Korakk, "Show Lance Raycasts", 9.f);
  AddOptionChoice(kDO_Korakk, "Show Clearance Tests", 10.f);
  AddOption(kC_AICreatures, kDO_SwarmBots, "SwarmBots", 0.f, 0.f, 3.f, 1.f);
  AddOptionChoice(kDO_SwarmBots, "Off", 0.f);
  AddOptionChoice(kDO_SwarmBots, "Debug", 1.f);
  AddOptionChoice(kDO_SwarmBots, "Floor Collision", 2.f);
  AddOptionChoice(kDO_SwarmBots, "Kill All But 1", 3.f);
  AddOption(kC_AICreatures, kDO_DarkSamus, "DarkSamus", 0.f, 0.f, 1.f, 1.f);
  AddOptionChoice(kDO_DarkSamus, "Off", 0.f);
  AddOptionChoice(kDO_DarkSamus, "Debug", 1.f);
  AddOption(kC_AICreatures, kDO_Ridley, "Ridley", 0.f, 0.f, 11.f, 1.f);
  AddOptionChoice(kDO_Ridley, "Off", 0.f);
  AddOptionChoice(kDO_Ridley, "Debug", 1.f);
  AddOptionChoice(kDO_Ridley, "Force Big Tube", 2.f);
  AddOptionChoice(kDO_Ridley, "Force Small Tube", 3.f);
  AddOptionChoice(kDO_Ridley, "Default FOV", 4.f);
  AddOptionChoice(kDO_Ridley, "FOV 65", 5.f);
  AddOptionChoice(kDO_Ridley, "FOV 75", 6.f);
  AddOptionChoice(kDO_Ridley, "FOV 85", 7.f);
  AddOptionChoice(kDO_Ridley, "Look Up", 8.f);
  AddOptionChoice(kDO_Ridley, "Look Forward", 9.f);
  AddOptionChoice(kDO_Ridley, "Look Down", 10.f);
  AddOptionChoice(kDO_Ridley, "Skip Ahead to 3", 11.f);

  // RagDolls
  AddOption(kC_RagDolls, kDO_ShowRagdolls, "Show Ragdolls", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_ShowRagdolls, "OFF", 0.f);
  AddOptionChoice(kDO_ShowRagdolls, "Show sticks", 1.f);
  AddOptionChoice(kDO_ShowRagdolls, "Show sticks and spheres", 2.f);
  AddOption(kC_RagDolls, kDO_IsStaticSpeed, "Is Static Speed", 0.5f, 0.f, 10.f, 0.05f);
  AddOption(kC_RagDolls, kDO_MinImpactSoundSpeed, "Min Impact Sound speed", 18.f, 0.f, 100.f, 0.5f);
  AddOption(kC_RagDolls, kDO_ImpactVolumePerSpeed, "Impact volume per speed", 25.f, 0.f, 30.f,
            0.1f);
  AddOption(kC_RagDolls, kDO_MinImpactVolume, "Min impact volume", 105.f, 0.f, 255.f, 1.f);
  AddOption(kC_RagDolls, kDO_MinImpactInterval, "Min impact interval", 0.22f, 0.f, 1.f, 0.01f);

  // Collision
  AddOption(kC_Collision, kDO_Octree, "Octree", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_Octree, "Off", 0.f);
  AddOptionChoice(kDO_Octree, "Draw Leaves", 1.f);
  AddOptionChoice(kDO_Octree, "Draw Lines", 2.f);
  AddOption(kC_Collision, kDO_Collision, "Collision", 0.f, 0.f, 4.f, 1.f);
  AddOptionChoice(kDO_Collision, "Off", 0.f);
  AddOptionChoice(kDO_Collision, "Edges Only", 1.f);
  AddOptionChoice(kDO_Collision, "Edges + Verts", 2.f);
  AddOptionChoice(kDO_Collision, "Edges + Triangles", 3.f);
  AddOptionChoice(kDO_Collision, "Edges + Triangles", 4.f);
  AddOption(kC_Collision, kDO_DrawObjectCollisionBoxes, "Draw Object Collision Boxes", 0.f, 0.f,
            3.f, 1.f);
  AddOptionChoice(kDO_DrawObjectCollisionBoxes, "Off", 0.f);
  AddOptionChoice(kDO_DrawObjectCollisionBoxes, "Draw Tris", 1.f);
  AddOptionChoice(kDO_DrawObjectCollisionBoxes, "Tris + Boxes", 2.f);
  AddOptionChoice(kDO_DrawObjectCollisionBoxes, "Tri+Box+Normal", 3.f);
  AddOption(kC_Collision, kDO_RayCastTest, "Ray Cast Test", 0.f, 0.f, 4.f, 1.f);
  AddOptionChoice(kDO_RayCastTest, "Off", 0.f);
  AddOptionChoice(kDO_RayCastTest, "From Camera", 1.f);
  AddOptionChoice(kDO_RayCastTest, "From Player", 2.f);
  AddOptionChoice(kDO_RayCastTest, "From Camera", 3.f);
  AddOptionChoice(kDO_RayCastTest, "Using Sphere", 4.f);
  AddOption(kC_Collision, kDO_NoStaticCollisionWhenStationary,
            "No Static Collision When Stationary", true);
  AddOption(kC_Collision, kDO_ProjectilesUseRenderMesh, "Projectiles Use Render Mesh", true);
  AddOption(kC_Collision, kDO_ShowTriangleCache, "Show Triangle Cache", false);

  // Camera
  AddOption(kC_Camera, kDO_DebugCamera, "Debug Camera", 0.f, 0.f, 2.f, 1.f);
  AddOption(kC_Camera, kDO_DebugCameraSameControllerRESTART, "Debug Camera Same Controller RESTART",
            true);
  AddOption(kC_Camera, kDO_BallCameraFreelook, "Ball Camera Freelook", false);
  AddOption(kC_Camera, kDO_ShowOtherCameraFunkyStuff, "Show Other Camera Funky Stuff", 0.f, 0.f,
            11.f, 1.f);
  AddOption(kC_Camera, kDO_ShowCameraPosition, "Show Camera Position", false);
  AddOption(kC_Camera, kDO_ShowCameraLookat, "Show Camera Lookat", false);
  AddOption(kC_Camera, kDO_ShowCameraIdealLookat, "Show Camera Ideal Lookat", false);
  AddOption(kC_Camera, kDO_ShowFunkyColliderStuff, "Show Funky Collider Stuff", 0.f, 0.f, 5.f, 1.f);
  AddOption(kC_Camera, kDO_ShowCameraBreadcrumbs, "Show Camera Breadcrumbs", false);
  AddOption(kC_Camera, kDO_ShowPlayerBreadcrumbs, "Show Player Breadcrumbs", false);
  AddOption(kC_Camera, kDO_ShowCameraSplineStuff, "Show Camera Spline Stuff", 0.f, 0.f, 10.f, 1.f);
  AddOption(kC_Camera, kDO_ShowCameraSurfaceStuff, "Show Camera Surface Stuff", 0.f, 0.f, 10.f,
            1.f);
  AddOption(kC_Camera, kDO_ShowSpindleCameraStuff, "Show Spindle Camera stuff", 0.f, 0.f, 8.f, 1.f);
  AddOption(kC_Camera, kDO_ShowBallCameraStateInfo, "Show Ball Camera State Info", 0.f, 0.f, 1.f,
            1.f);
  AddOption(kC_Camera, kDO_ShowInterpolationStuff, "Show Interpolation Stuff", false);
  AddOption(kC_Camera, kDO_ShowCameraShakerInfo, "Show Camera Shaker Info", false);
  AddOption(kC_Camera, kDO_CinematicBarsConstrictViewport, "Cinematic Bars Constrict Viewport",
            true);

  // Lighting
  AddOption(kC_Lighting, kDO_FullBrightActors, "Full Bright Actors", false);
  AddOption(kC_Lighting, kDO_FullBrightWorld, "Full Bright World", false);
  AddOption(kC_Lighting, kDO_DrawAreaLights, "Draw Area Lights", false);
  AddOption(kC_Lighting, kDO_UseNewWorldLights, "Use new world lights", true);
  AddOption(kC_Lighting, kDO_DrawDynamicLights, "Draw dynamic lights", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_DrawDynamicLights, "OFF", 0.f);
  AddOptionChoice(kDO_DrawDynamicLights, "ON", 1.f);
  AddOptionChoice(kDO_DrawDynamicLights, "ON With Z", 2.f);
  AddOption(kC_Lighting, kDO_RenderWorldShadow, "Render World Shadow", false);

  // Water
  AddOption(kC_Water, kDO_WaterEnable, "Enable", gWaterEnable);
  AddOption(kC_Water, kDO_WaterWireFrame, "Wire Frame", false);
  AddOption(kC_Water, kDO_WaterProfile, "Profile", gWaterProfile);
  AddOption(kC_Water, kDO_WaterRefraction, "Refraction", true);
  AddOption(kC_Water, kDO_WaterLightMap, "Light map", true);
  AddOption(kC_Water, kDO_WaterColorMap, "Color map", true);
  AddOption(kC_Water, kDO_WaterColorWarpMap, "Color warp map", true);
  AddOption(kC_Water, kDO_WaterGlossMap, "Gloss map", true);
  AddOption(kC_Water, kDO_WaterEnvMap, "Env map", true);
  AddOption(kC_Water, kDO_WaterFogOverWater, "Fog over water", gWaterFogOverWater);
  AddOption(kC_Water, kDO_WaterShowWaterEntryExit, "Show water entry/exit", 0.f, 0.f, 4.f, 1.f);

  // EnvFx
  AddOption(kC_EnvFx, kDO_EnvFxEnable, "Enable", 1.f, 0.f, 1.f, 1.f);
  AddOption(kC_EnvFx, kDO_EnvFxEnableVisorDrops, "Enable Visor Drops", 1.f, 0.f, 1.f, 1.f);
  AddOption(kC_EnvFx, kDO_EnvFxEnableCellDebugDrawing, "Enable Cell Debug Drawing", 0.f, 0.f, 1.f,
            1.f);

  // Grapple
  AddOption(kC_Grapple, kDO_GrappleDistance, "Distance", 1.f, 0.1f, 100.f, 0.1f);
  AddOption(kC_Grapple, kDO_GrappleBeamLength, "BeamLength", 1.f, 0.1f, 100.f, 0.1f);
  AddOption(kC_Grapple, kDO_GrappleSwingTime, "SwingTime", 1.f, 0.1f, 10.f, 0.1f);
  AddOption(kC_Grapple, kDO_GrappleMaxVelocity, "MaxVelocity", 1.f, 0.1f, 100.f, 0.1f);
  AddOption(kC_Grapple, kDO_GrapplePullCloseDistance, "PullCloseDistance", 1.f, 0.1f, 100.f, 0.1f);
  AddOption(kC_Grapple, kDO_GrapplePullDampenDistance, "PullDampenDistance", 1.f, 0.1f, 100.f,
            0.1f);
  AddOption(kC_Grapple, kDO_GrapplePullVelocity, "PullVelocity", 1.f, 0.1f, 100.f, 0.1f);
  AddOption(kC_Grapple, kDO_GrappleTurnRate, "TurnRate", 30.f, 0.1f, 360.f, 0.1f);
  AddOption(kC_Grapple, kDO_GrappleJumpForce, "JumpForce", 1.f, 0.1f, 100.f, 0.1f);
  AddOption(kC_Grapple, kDO_GrappleHoldOrbitButton, "HoldOrbitButton", false);
  AddOption(kC_Grapple, kDO_GrappleTurnControlsReversed, "TurnControlsReversed", true);
  AddOption(kC_Grapple, kDO_GrappleControlScheme, "ControlScheme", 0.f, 0.f, 2.f, 1.f);

  // OrbitStuff
  AddOption(kC_OrbitStuff, kDO_OrbitLockMode, "OrbitLockMode", 0.f, 0.f, 1.f, 1.f);
  AddOption(kC_OrbitStuff, kDO_BoostBreaksOrbit, "Boost Breaks Orbit", gBoostBreaksOrbit);
  AddOption(kC_OrbitStuff, kDO_DoubleJumpBreaksOrbit, "Double Jump Breaks Orbit",
            gDoubleJumpBreaksOrbit);
  AddOption(kC_OrbitStuff, kDO_DoubleDashBreaksOrbit, "Double Dash Breaks Orbit",
            gDoubleDashBreaksOrbit);
  AddOption(kC_OrbitStuff, kDO_ShowOrbitPoint, "Show Orbit Point",
            gpTweakTargeting->GetShowOrbitPoint());
  AddOption(kC_OrbitStuff, kDO_AutoAimAtOrbitedObject, "Auto Aim at Orbited Object",
            gAutoAimAtOrbitedObject);
  AddOption(kC_OrbitStuff, kDO_FreeLookPreventsOrbitMovement, "FreeLookPreventsOrbitMovement",
            gFreeLookPreventsOrbitMovement);
  AddOption(kC_OrbitStuff, kDO_DamageBreaksOrbit, "Damage Breaks Orbit", gDamageBreaksOrbit, 0.f,
            100.f, 1.f);
  AddOption(kC_OrbitStuff, kDO_OrbitCarcassOption, "Orbit Carcass Option", 2.f, 0.f, 2.f, 1.f);
  AddOption(kC_OrbitStuff, kDO_DashEnabled, "Dash Enabled", gpTweakPlayer->GetOrbitDash());
  AddOption(kC_OrbitStuff, kDO_UsesTapHold, "Uses Tap/Hold", gpTweakPlayer->GetOrbitDashUsesTap());
  AddOption(kC_OrbitStuff, kDO_TapTime, "Tap Time", gpTweakPlayer->GetOrbitDashTapTime(), 0.01f,
            10.f, 0.05f);
  AddOption(kC_OrbitStuff, kDO_StickXAxisThreshold, "Stick X-axis threshold",
            gpTweakPlayer->GetOrbitDashStickThreshold(), 0.01f, 1.f, 0.05f);
  AddOption(kC_OrbitStuff, kDO_DoubleJumpImpulse, "DoubleJumpImpulse",
            gpTweakPlayer->GetOrbitDashDoubleJumpImpulse(), 0.f, 100000.f, 1.f);
  AddOption(kC_OrbitStuff, kDO_HorizDoubleJumpAccel, "HorizDoubleJumpAccel",
            gpTweakPlayer->GetOrbitDashHorizontalDoubleJumpAccel(), 0.f, 100000.f, 1.f);
  AddOption(kC_OrbitStuff, kDO_VerticalDoubleJumpAccel, "VerticalDoubleJumpAccel",
            gpTweakPlayer->GetOrbitDashVerticalDoubleJumpAccel(), 0.f, 100000.f, 1.f);
  AddOption(kC_OrbitStuff, kDO_OrbitDashAroundObjectsOnly, "OrbitDashAroundObjectsOnly", true);
  AddOption(kC_OrbitStuff, kDO_SpeedFactor, "SpeedFactor", 1.f, 0.f, 10.f, 0.1f);
  AddOption(kC_OrbitStuff, kDO_ConstantSpeed, "ConstantSpeed", true);
  AddOption(kC_OrbitStuff, kDO_MaxTime, "MaxTime", 1.f, 0.1f, 10.f, 0.05f);
  AddOption(kC_OrbitStuff, kDO_SpeedRampTime, "SpeedRampTime", 0.4f, 0.1f, 10.f, 0.05f);
  AddOption(kC_OrbitStuff, kDO_DebugOrbitStuff, "Debug Orbit Stuff", 0.f, 0.f, 2.f, 1.f);

  // Profiler
  AddOption(kC_Profiler, kDO_DemoState, "State", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_DemoState, "OFF", 0.f);
  AddOptionChoice(kDO_DemoState, "TIMER", 1.f);
  AddOptionChoice(kDO_DemoState, "EVENTPOINTS", 2.f);
  AddOption(kC_Profiler, kDO_DemoSave, "Save", false);
  AddOption(kC_Profiler, kDO_DemoDisableMusyXInterrupt, "Disable MusyX Interrupt", false);

  // GameRewards
  AddOption(kC_GameRewards, kDO_HardMode, "Hard Mode", false);
  AddOption(kC_GameRewards, kDO_HardModeDamageMultiplier, "Hard Mode Damage Multiplier",
            gpTweakGame->GetHardModeDamageMultiplier(), 1.f, 10.f, 0.1f);
  AddOption(kC_GameRewards, kDO_HardModeWeaponMultiplier, "Hard Mode Weapon Multiplier",
            gpTweakGame->GetHardModeWeaponMultiplier(), 0.f, 10.f, 0.05f);

  // Debug messages go to the console window unless they are turned off.
  if (GetOptionValue(kDO_DebugMessagesEnabled) != 0.f) {
    RAssert_SetDiagnosticPrintCallback(CConsoleOutputWindow::Printf);
    RAssert_SetDebuggerPrintCallback(nullptr);
  } else {
    RAssert_SetDiagnosticPrintCallback(RAssert_DiscardDiagnosticCallback);
    RAssert_SetDebuggerPrintCallback(RAssert_ConsumeDebuggerPrintCallback);
  }

  // HyperMode
  AddOption(kC_HyperMode, kDO_HyperModeType, "HyperMode Type", 0.f, 0.f, 2.f, 1.f);
  AddOptionChoice(kDO_HyperModeType, "Timer", 0.f);
  AddOptionChoice(kDO_HyperModeType, "Phazon Level", 1.f);
  AddOptionChoice(kDO_HyperModeType, "SPD Phazon Level", 2.f);
  AddOption(kC_HyperMode, kDO_HyperModeInvulnerablePhazonLoss, "InvulnerablePhazonLoss",
            gpTweakPlayer->GetHyperModeInvulnerablePhazonLoss(0));
  AddOption(kC_HyperMode, kDO_HyperModeInvulnerableTime, "InvulnerableTime      ",
            gpTweakPlayer->GetHyperModeInvulnerableTime(0), 0.01f, 1000.f, 0.1f);
  AddOption(kC_HyperMode, kDO_HyperModeCorruptionTime, "CorruptionTime        ",
            gpTweakPlayer->GetHyperModeCorruptionTime(0), 0.01f, 1000.f, 0.1f);
  AddOption(kC_HyperMode, kDO_HyperModeConstantCorruptionRate, "ConstantCorruptionRate",
            gpTweakPlayer->GetHyperModeConstantCorruptionRate(0));
  AddOption(kC_HyperMode, kDO_HyperModeCorruptionRate, "CorruptionRate        ",
            gpTweakPlayer->GetHyperModeCorruptionRate(0), 0.01f, 1000.f, 0.1f);
  AddOption(kC_HyperMode, kDO_HyperModePhazonLevel, "PhazonLevel           ",
            gpTweakPlayer->GetHyperModePhazonLevel(0), 0.01f, 1000.f, 0.1f);
  AddOption(kC_HyperMode, kDO_HyperModePhazonCapacity, "PhazonCapacity        ",
            gpTweakPlayer->GetHyperModePhazonCapacity(0), 0.01f, 1000.f, 0.1f);
  AddOption(kC_HyperMode, kDO_HyperModeDangerPercentage, "DangerPercentage      ",
            gpTweakPlayer->GetHyperModeDangerPercentage(0), 0.01f, 1000.f, 0.1f);
  AddOption(kC_HyperMode, kDO_HyperModeBeamLossAmount, "BeamLossAmount        ",
            gpTweakPlayer->GetHyperModeBeamLossAmount(0), 0.01f, 1000.f, 0.1f);
  AddOption(kC_HyperMode, kDO_HyperModeMissileLossAmount, "MissileLossAmount     ",
            gpTweakPlayer->GetHyperModeMissileLossAmount(0), 0.01f, 1000.f, 0.1f);
  AddOption(kC_HyperMode, kDO_HyperModePhazonBallRate, "PhazonBallRate        ",
            gpTweakPlayer->GetHyperModePhazonBallRate(0), 0.01f, 1000.f, 0.1f);
  AddOption(kC_HyperMode, kDO_HyperModeDamageMultiplier, "DamageMultiplier      ",
            gpTweakPlayer->GetHyperModeDamageMultiplier(0), 0.01f, 1000.f, 0.1f);
  // The tuning of the "Phazon Level" hyper mode type, drawn in yellow.
  CColor yellow = CColor::Yellow();
  AddOption(kC_HyperMode, kDO_HyperModeInvulnerablePhazonLoss2, "InvulnerablePhazonLoss", yellow,
            gpTweakPlayer->GetHyperModeInvulnerablePhazonLoss(1));
  AddOption(kC_HyperMode, kDO_HyperModeInvulnerableTime2, "InvulnerableTime      ",
            gpTweakPlayer->GetHyperModeInvulnerableTime(1), 0.01f, 1000.f, 0.1f, yellow);
  AddOption(kC_HyperMode, kDO_HyperModeCorruptionTime2, "CorruptionTime        ",
            gpTweakPlayer->GetHyperModeCorruptionTime(1), 0.01f, 1000.f, 0.1f, yellow);
  AddOption(kC_HyperMode, kDO_HyperModeConstantCorruptionRate2, "ConstantCorruptionRate", yellow,
            gpTweakPlayer->GetHyperModeConstantCorruptionRate(1));
  AddOption(kC_HyperMode, kDO_HyperModeCorruptionRate2, "CorruptionRate        ",
            gpTweakPlayer->GetHyperModeCorruptionRate(1), 0.01f, 1000.f, 0.1f, yellow);
  AddOption(kC_HyperMode, kDO_HyperModePhazonLevel2, "PhazonLevel           ",
            gpTweakPlayer->GetHyperModePhazonLevel(1), 0.01f, 1000.f, 0.1f, yellow);
  AddOption(kC_HyperMode, kDO_HyperModePhazonCapacity2, "PhazonCapacity        ",
            gpTweakPlayer->GetHyperModePhazonCapacity(1), 0.01f, 1000.f, 0.1f, yellow);
  AddOption(kC_HyperMode, kDO_HyperModeDangerPercentage2, "DangerPercentage      ",
            gpTweakPlayer->GetHyperModeDangerPercentage(1), 0.01f, 1000.f, 0.1f, yellow);
  AddOption(kC_HyperMode, kDO_HyperModeBeamLossAmount2, "BeamLossAmount        ",
            gpTweakPlayer->GetHyperModeBeamLossAmount(1), 0.01f, 1000.f, 0.1f, yellow);
  AddOption(kC_HyperMode, kDO_HyperModeMissileLossAmount2, "MissileLossAmount     ",
            gpTweakPlayer->GetHyperModeMissileLossAmount(1), 0.01f, 1000.f, 0.1f, yellow);
  AddOption(kC_HyperMode, kDO_HyperModePhazonBallRate2, "PhazonBallRate        ",
            gpTweakPlayer->GetHyperModePhazonBallRate(1), 0.01f, 1000.f, 0.1f, yellow);
  AddOption(kC_HyperMode, kDO_HyperModeDamageMultiplier2, "DamageMultiplier      ",
            gpTweakPlayer->GetHyperModeDamageMultiplier(1), 0.01f, 1000.f, 0.1f, yellow);

  // Revolution
  AddOption(kC_Revolution, kDO_ControllerDebug, "Controller Debug", 0.f, 0.f, 5.f, 1.f);
  AddOption(kC_Revolution, kDO_FreeLookGun, "FreeLook Gun", gpTweakPlayer->GetRevFreeLookGun());
  AddOption(kC_Revolution, kDO_OrbitLockGun, "Orbit Lock Gun", gpTweakPlayer->GetRevOrbitLockGun());
  AddOption(kC_Revolution, kDO_OrbitTagObjects, "Orbit Tag Objects",
            gpTweakPlayer->GetRevOrbitTagObjects());
  AddOption(kC_Revolution, kDO_ShowAimingCursor, "Show Aiming Cursor", true);
  AddOption(kC_Revolution, kDO_DebugCursorType, "Debug Cursor Type", 0.f, 2.f, 2.f, 1.f);
  AddOptionChoice(kDO_DebugCursorType, "None", 0.f);
  AddOptionChoice(kDO_DebugCursorType, "2D", 1.f);
  AddOptionChoice(kDO_DebugCursorType, "3D", 2.f);
  AddOption(kC_Revolution, kDO_LockAimingCursor, "Lock Aiming Cursor",
            gpTweakPlayer->GetRevLockCursor());
  AddOption(kC_Revolution, kDO_MorphballUsesAccelerometer, "Morphball uses accelerometer", false);
  bool closeSensorBar = CDvdFile::FileExists("DPD.txt");
  float dpdDistance = 0.3f;
  if (closeSensorBar) {
    dpdDistance = 0.15f;
  }
  AddOption(kC_Revolution, kDO_DPDDistance, "DPD distance", dpdDistance, 0.f, 1.f, 0.05f);
  KPADSetObjInterval(dpdDistance);
}
#pragma pop

// Picks the first "<name>_NNN_" prefix (NNN < 100) whose first movie file does not exist on the
// host yet. The name is cleared when every slot is taken.
const rstl::string& CGameDebug::GetMovieCaptureName() {
  if (mMovieCaptureName.size() != 0 && !mMovieCaptureNameChosen) {
    bool found = false;
    char path[264];
    for (int i = 0; i < 100; ++i) {
      strcpy(path, CBasics::Stringize("%s_%03d_%06d.mzip", mMovieCaptureName.data(), i, 0));
      int handle = -1;
      if (CBBASupport_BBAOpen(path, 0, &handle) == 0) {
        CBBASupport_BBAClose(handle);
      } else {
        found = true;
        strcpy(path, CBasics::Stringize("%s_%03d_", mMovieCaptureName.data(), i));
        break;
      }
    }
    mMovieCaptureName.clear();
    if (found) {
      mMovieCaptureName = rstl::string(path);
    }
    mMovieCaptureNameChosen = true;
  }
  return mMovieCaptureName;
}

void CGameDebug::SetMovieCaptureTime(float time) { mMovieCaptureTime = time; }

void CGameDebug::AddOption(int category, int index, const char* name, float value, float min,
                           float max, float step, const CColor& color) {
  InstallOption(
      CDebugOption(category, index, rstl::string_l(name), value, min, max, step, CColor(color)));
}

void CGameDebug::AddOption(int category, int index, const char* name, const CColor& color,
                           bool value) {
  InstallOption(CDebugOption(category, index, rstl::string_l(name), value, CColor(color)));
}

void CGameDebug::AddOption(int category, int index, const char* name, float value, float min,
                           float max, float step) {
  InstallOption(CDebugOption(category, index, rstl::string_l(name), value, min, max, step,
                             CColor(CColor::White())));
}

void CGameDebug::AddOption(int category, int index, const char* name, bool value) {
  InstallOption(
      CDebugOption(category, index, rstl::string_l(name), value, CColor(CColor::White())));
}

void CGameDebug::CloseMenu() {
  mMenu.clear();
  mMenuItems = rstl::vector< CDebugMenu::SItem >();
  mMenuLabels = rstl::vector< rstl::string >();
}

// Each dump empties the buffer; the first one creates the file and later ones append to it.
void CGameDebug::AppendToLog(const char* text) {
  if (mLogSize == kLogBufferSize) {
    return;
  }
  if (mLogBuffer.null()) {
    mLogBuffer = new ("CGameDebug.cpp(2539) : ", (const char*)0) char[kLogBufferSize];
  }
  char* out = mLogBuffer.get() + mLogSize;
  while (true) {
    if (mLogSize == kLogBufferSize) {
      gpfnWarningPrintf("Log buffer full!\n");
      rs_debugger_printf("Log buffer full!\n");
      DumpLog();
      if (mLogBuffer.null()) {
        mLogBuffer = new ("CGameDebug.cpp(2554) : ", (const char*)0) char[kLogBufferSize];
      }
      out = mLogBuffer.get();
    }
    if (mLogSize == kLogBufferSize) {
      rs_debugger_printf("Log buffer could not be dumped.\n");
      return;
    }
    if (*text == '\0') {
      return;
    }
    *out++ = *text++;
    ++mLogSize;
  }
}

void CGameDebug::DumpLog() {
  if (mLogBuffer.null()) {
    return;
  }
  int handle = -1;
  if (CBBASupport_BBAOpen("C:\\FIO\\debuglog.txt", mLogFileCreated ? 1 : 2, &handle) == 0) {
    mLogFileCreated = true;
    CBBASupport_BBAWrite(handle, mLogBuffer.get(), mLogSize);
    CBBASupport_BBAClose(handle);
    FreeLog();
  } else {
    gpfnWarningPrintf("Could not open C:\\FIO\\debuglog.txt\n");
    rs_debugger_printf("Could not open C:\\FIO\\debuglog.txt\n");
  }
}

void CGameDebug::FreeLog() {
  mLogBuffer = nullptr;
  mLogSize = 0;
}

// Leaves the "Log" mode of the debug messages and starts a new log file on the next dump.
void CGameDebug::ResetDebugMessageLog() {
  mLogFileCreated = false;
  if (GetOptionInt(kDO_DebugMessagesEnabled) == 2) {
    CDebugOption* option = GetOption(kDO_DebugMessagesEnabled);
    if (option != nullptr) {
      option->SetValue(1.f);
    }
  }
}

CColor CGameDebug::GetDebugMessageColor() const {
  switch (const_cast< CGameDebug* >(this)->GetOptionInt(kDO_DebugMessageColor)) {
  case 1:
    return CColor::Black();
  case 2:
    return CColor::Grey();
  case 3:
    return CColor::Orange();
  default:
    return CColor::White();
  }
}

const char* CGameDebug::GetCategoryName(int category) {
  switch (category) {
  case kC_Cheats:
    return "Cheats";
  case kC_Player:
    return "Player";
  case kC_Camera:
    return "Camera";
  case kC_AI:
    return "AI";
  case kC_AITrace:
    return "AI Trace";
  case kC_AICreatures:
    return "AI-Creatures";
  case kC_Gui:
    return "Gui";
  case kC_Grapple:
    return "Grapple";
  case kC_PlayerGun:
    return "PlayerGun";
  case kC_OrbitStuff:
    return "Orbit Stuff";
  case kC_PowerupsWeapons:
    return "Powerups (Weapons)";
  case kC_PowerupsUpgrades:
    return "Powerups (Upgrades)";
  case kC_PowerupsMorphball:
    return "Powerups (Morphball)";
  case kC_PowerupsMisc:
    return "Powerups (Misc)";
  case kC_LoadGame:
    return "Load Game";
  case kC_SaveGame:
    return "Save Game";
  case kC_GameRewards:
    return "Game Rewards";
  case kC_HyperMode:
    return "Hyper Mode";
  case kC_Revolution:
    return "Revolution";
  case kC_Misc:
    return "Misc";
  case kC_Scripting:
    return "Scripting";
  case kC_RagDolls:
    return "RagDolls";
  case kC_Collision:
    return "Collision";
  case kC_Programmer:
    return "Programmer";
  case kC_Renderer:
    return "Renderer";
  case kC_Lighting:
    return "Lighting";
  case kC_Demo:
    return "Demo";
  case kC_Water:
    return "Water";
  case kC_EnvFx:
    return "EnvFx";
  case kC_Profiler:
    return "Profiler";
  case kC_Audio:
    return "Audio";
  case kC_AudioDebug:
    return "Audio Debug";
  case kC_Debug:
  default:
    return "Debug";
  }
}

rstl::auto_ptr< IConnection > CGameDebug::ConnectOption(int index, OptionSignal::Functor functor) {
  return mOptionSignals[index].Connect(functor);
}

void CGameDebug::OnOptionChanged(CDebugOption* option) {
  mChangedOptions.insert(option->GetIndex());
}

void CGameDebug::DispatchChangedOptions(CStateManager& mgr) {
  for (rstl::set< int >::iterator it = mChangedOptions.begin(); it != mChangedOptions.end(); ++it) {
    mOptionSignals[*it].Emit(mgr);
  }
  mChangedOptions.clear();
}
