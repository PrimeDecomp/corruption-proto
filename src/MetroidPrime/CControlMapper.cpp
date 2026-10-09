// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x800105C4..0x80011658 (14 native functions).
// Source identity: inferred from complete class behavior and corroborated by both reference source
// paths. Not yet implemented:
// 0x8001073C +0xAC: GetCommandMapping; returns (control, 0) from the override list when the
// command is overridden, otherwise the player-controls tweak's mapping (0x80247FDC).
// 0x80010B64 +0x17C: GetPressInput; 0x80010CE0 +0x17C: GetDigitalInput. With the filter or an
// enabled command, they read the tweak's command description (0x8024802C): type 1 tests the
// primary control (CFinalInput 0x8051A374 / 0x8051A384); type 2 combines primary and secondary
// with and (0), or (1) or and-not (2).
// 0x80010E5C +0x130: GetAnalogInput; type 2 adds the two analog values (0) or falls back to the
// secondary control while the primary is within 0.05 of zero (1).
// These need declarations for the tweak accessors and CFinalInput's analog getter (0x8051A604).
// 0x80011628 +0x30: registered static initializer (.ctors8065B4C8). Stores -1, -1, -1, 0, 1, 2,
// -1 into seven .sbss words; the same initializer is emitted by many units (for example CActor
// 0x80037158 and CDebugMenu 0x80049F38), so it comes from a shared header that is not known yet.

#include "MetroidPrime/CControlMapper.hpp"

#include "Kyoto/Alloc/Assert.hpp"

const char* CControlMapper::GetDescriptionForCommand(ECommands command) {
  switch (command) {
  case kC_Forward:
    return "Forward                 ";
  case kC_Backward:
    return "Backward                ";
  case kC_TurnLeft:
    return "TurnLeft                ";
  case kC_TurnRight:
    return "TurnRight               ";
  case kC_StrafeLeft:
    return "StrafeLeft              ";
  case kC_StrafeRight:
    return "StrafeRight             ";
  case kC_Jump:
    return "Jump                    ";
  case kC_FireBeam:
    return "FireBeam                ";
  case kC_AutoFireBeam:
    return "AutoFireBeam            ";
  case kC_ChargeBeam:
    return "ChargeBeam              ";
  case kC_FireMissile:
    return "FireMissile             ";
  case kC_FireSeeker:
    return "FireSeeker              ";
  case kC_HyperMode:
    return "HyperMode               ";
  case kC_ItemMenu:
    return "ItemMenu                ";
  case kC_UseItem:
    return "UseItem                 ";
  case kC_CombatVisor:
    return "CombatVisor             ";
  case kC_ScanVisor:
    return "ScanVisor               ";
  case kC_XRayVisor:
    return "XRayVisor               ";
  case kC_CommandVisor:
    return "CommandVisor            ";
  case kC_CycleVisorUp:
    return "CycleVisorUp            ";
  case kC_CycleVisorDown:
    return "CycleVisorDown          ";
  case kC_OrbitLock:
    return "OrbitLock               ";
  case kC_GrappleLock:
    return "GrappleLock             ";
  case kC_GrapplePull:
    return "GrapplePull             ";
  case kC_SummonShip:
    return "SummonShip              ";
  case kC_DismissShip:
    return "DismissShip             ";
  case kC_ShipFire:
    return "ShipFire                ";
  case kC_ShipPickUpItem:
    return "ShipPickUpItem          ";
  case kC_ShipDropItem:
    return "ShipDropItem            ";
  case kC_ScanItem:
    return "ScanItem                ";
  case kC_StartContextualAction:
    return "StartContextualAction   ";
  case kC_StopContextualAction:
    return "StopContextualAction    ";
  case kC_RollForward:
    return "RollForward             ";
  case kC_RollBackward:
    return "RollBackward            ";
  case kC_RollLeft:
    return "RollLeft                ";
  case kC_RollRight:
    return "RollRight               ";
  case kC_BoostBall:
    return "BoostBall               ";
  case kC_SpiderBall:
    return "SpiderBall              ";
  case kC_ScrewAttack:
    return "ScrewAttack             ";
  case kC_Bomb:
    return "Bomb                    ";
  case kC_MorphIntoBall:
    return "MorphIntoBall           ";
  case kC_MorphOutOfBall:
    return "MorphOutOfBall          ";
  case kC_LookUp:
    return "LookUp                  ";
  case kC_LookDown:
    return "LookDown                ";
  case kC_LookLeft:
    return "LookLeft                ";
  case kC_LookRight:
    return "LookRight               ";
  case kC_ReorientCamera:
    return "ReorientCamera          ";
  case kC_ViewLock:
    return "ViewLock                ";
  case kC_MapScreen:
    return "MapScreen               ";
  case kC_MapCircleUp:
    return "MapCircleUp             ";
  case kC_MapCircleDown:
    return "MapCircleDown           ";
  case kC_MapCircleLeft:
    return "MapCircleLeft           ";
  case kC_MapCircleRight:
    return "MapCircleRight          ";
  case kC_MapMoveForward:
    return "MapMoveForward          ";
  case kC_MapMoveBackward:
    return "MapMoveBackward         ";
  case kC_MapMoveLeft:
    return "MapMoveLeft             ";
  case kC_MapMoveRight:
    return "MapMoveRight            ";
  case kC_MapZoomIn:
    return "MapZoomIn               ";
  case kC_MapZoomOut:
    return "MapZoomOut              ";
  case kC_InventoryScreen:
    return "InventoryScreen         ";
  case kC_MenuUp:
    return "MenuUp                  ";
  case kC_MenuDown:
    return "MenuDown                ";
  case kC_MenuLeft:
    return "MenuLeft                ";
  case kC_MenuRight:
    return "MenuRight               ";
  case kC_OptionsScreen:
    return "OptionsScreen           ";
  case kC_LogScreen:
    return "LogScreen               ";
  case kC_DebugMoveCamera:
    return "DebugMoveCamera         ";
  case kC_DebugMoveCameraFast:
    return "DebugMoveCameraFast     ";
  case kC_DebugMoveCameraForward:
    return "DebugMoveCameraForward  ";
  case kC_DebugMoveCameraBackward:
    return "DebugMoveCameraBackward ";
  case kC_DebugMoveCameraRight:
    return "DebugMoveCameraRight    ";
  case kC_DebugMoveCameraLeft:
    return "DebugMoveCameraLeft     ";
  case kC_DebugTurnCameraRight:
    return "DebugTurnCameraRight    ";
  case kC_DebugTurnCameraLeft:
    return "DebugTurnCameraLeft     ";
  case kC_DebugPitchCameraUp:
    return "DebugPitchCameraUp      ";
  case kC_DebugPitchCameraDown:
    return "DebugPitchCameraDown    ";
  case kC_DebugToggleCamera:
    return "DebugToggleCamera       ";
  case kC_DebugDropPlayer:
    return "DebugDropPlayer         ";
  case kC_DebugMenuStart:
    return "DebugMenuStart          ";
  case kC_DebugMenuUp:
    return "DebugMenuUp             ";
  case kC_DebugMenuDown:
    return "DebugMenuDown           ";
  case kC_DebugMenuLeft:
    return "DebugMenuLeft           ";
  case kC_DebugMenuRight:
    return "DebugMenuRight          ";
  case kC_DebugMenuSelect:
    return "DebugMenuSelect         ";
  case kC_DebugMenuBack:
    return "DebugMenuBack           ";
  case kC_DebugMenuIncrementValue:
    return "DebugMenuIncrementValue ";
  case kC_DebugMenuDecrementValue:
    return "DebugMenuDecrementValue ";
  case kC_DebugMenuValueMultiplier:
    return "DebugMenuValueMultiplier";
  case kC_DebugMenuRestartLevel:
    return "DebugMenuRestartLevel   ";
  case kC_DebugMapTeleport:
    return "DebugMapTeleport        ";
  default:
    break;
  }
  return "UNKNOWN command";
}

CControlMapper::CControlMapper()
: mCommandEnabled(kC_Count, true), mCommandOverridden(kC_Count, false) {
  Reset();
}

void CControlMapper::ResetCommandFilters() {
  mCommandEnabled.clear();
  for (int i = 0; i < kC_Count; ++i) {
    bool enabled = true;
    mCommandEnabled.push_back(enabled);
  }
}

void CControlMapper::SetCommandEnabled(ECommands command, bool enabled) {
  mCommandEnabled[command] = enabled;
}

// Replaces the command's primary control. At most eight commands can be remapped at once.
void CControlMapper::SetCommandMapping(ECommands command, int control) {
  if (!mCommandOverridden.empty()) {
    if (mCommandOverridden[command]) {
      for (rstl::pair< ECommands, int >* it = mCommandOverrides.begin();
           it != mCommandOverrides.end(); ++it) {
        if (it->first == command) {
          it->second = control;
        }
      }
    } else if (mCommandOverrides.size() == mCommandOverrides.capacity()) {
      rs_debugger_printf("Unable to remap command (%s) because remapping list is full\n",
                         GetDescriptionForCommand(command));
      gpfnWarningPrintf("Unable to remap command (%s) because remapping list is full\n",
                        GetDescriptionForCommand(command));
    } else {
      mCommandOverridden[command] = true;
      mCommandOverrides.push_back(rstl::pair< ECommands, int >(command, control));
    }
  }
}

void CControlMapper::RestoreCommandMapping(ECommands command) {
  if (!mCommandOverridden.empty()) {
    if (command < mCommandOverridden.size() && mCommandOverridden[command]) {
      mCommandOverridden[command] = false;
      for (rstl::pair< ECommands, int >* it = mCommandOverrides.begin();
           it != mCommandOverrides.end(); ++it) {
        if (it->first == command) {
          mCommandOverrides.erase(it);
          return;
        }
      }
    } else {
      rs_debugger_printf("Tried to restore a command that was not remapped (%s)\n",
                         GetDescriptionForCommand(command));
      gpfnWarningPrintf("Tried to restore a command that was not remapped (%s)\n",
                        GetDescriptionForCommand(command));
    }
  }
}

void CControlMapper::ResetCommandMappings() {
  mCommandOverridden.clear();
  for (int i = 0; i < kC_Count; ++i) {
    bool overridden = false;
    mCommandOverridden.push_back(overridden);
  }
  mCommandOverrides.clear();
}

void CControlMapper::Reset() {
  ResetCommandFilters();
  ResetCommandMappings();
}
