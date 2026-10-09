// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x8003B898..0x800492C8 (93 native functions).
// Source identity: asserted target basename; absent from both retail source inventories.
// Complete native/helper/callback inventory retained; no speculative declarations.
// Remaining (not implemented) functions:
// 0x8003C9B0 +0xD4: debug movie-capture name sanitization (SetMovieCaptureName; strips
//   ":*?\"<>|\\/\n\t%" with a find_first_of(const char*) that rstl::string lacks)
// 0x8003CA84 +0xD8: emitted string find helper used by movie name sanitization
// 0x8003CB5C +0x80: emitted string iterator search helper
// 0x8003CBDC +0x5ABC: CGameDebug AddDebugOptions; complete prototype debug option registration
//   (the option table is recorded in CGameDebug::EDebugOption)
// 0x80042698 +0x163C: apply debug-option state to engine systems
// 0x80043CD4 +0x14D8: populate debug-option values from engine/tweak state
// 0x800451AC +0x880: debug menu input and selected option handling
// 0x80045A2C +0xD8: debug menu/timing update
// 0x80045B04 +0x33C: debug text/menu rendering with CFont and log appenders
// 0x80045E40 +0x5F8: debug text drawing helper
// 0x80046438 +0x154: retained native/emitted helper; exact historical name/type unresolved
// 0x8004658C +0xB4: retained native/emitted helper; exact historical name/type unresolved
// 0x80046640 +0xA4: retained native/emitted helper; exact historical name/type unresolved
// 0x800466E4 +0xD58: debug menu construction from category/options; vector.h assertion482
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
#include "Kyoto/Basics/CBasics.hpp"
#include "Kyoto/Network/CBBASupport.hpp"

#include <string.h>

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

void CGameDebug::AddOption(int category, int index, const char* name, bool value,
                           const CColor& color) {
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
