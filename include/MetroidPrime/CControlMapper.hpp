#ifndef _CCONTROLMAPPER
#define _CCONTROLMAPPER

#include "types.h"

#include "rstl/pair.hpp"
#include "rstl/reserved_vector.hpp"

class CFinalInput;

// Maps the game's logical commands to controller inputs. The prototype sits between Echoes and
// Trilogy: it keeps Echoes' per-command enable mask and eight-entry override list (without the
// control-scheme selector), but each command is described by the player-controls tweak as a
// physical control or a combination of two controls (and / or / and-not), as in Trilogy.
// The prototype has 90 commands, including the debug camera and debug menu commands.
class CControlMapper {
public:
  // Names from GetDescriptionForCommand.
  enum ECommands {
    kC_Forward,
    kC_Backward,
    kC_TurnLeft,
    kC_TurnRight,
    kC_StrafeLeft,
    kC_StrafeRight,
    kC_Jump,
    kC_FireBeam,
    kC_AutoFireBeam,
    kC_ChargeBeam,
    kC_FireMissile,
    kC_FireSeeker,
    kC_HyperMode,
    kC_ItemMenu,
    kC_UseItem,
    kC_CombatVisor,
    kC_ScanVisor,
    kC_XRayVisor,
    kC_CommandVisor,
    kC_CycleVisorUp,
    kC_CycleVisorDown,
    kC_OrbitLock,
    kC_GrappleLock,
    kC_GrapplePull,
    kC_SummonShip,
    kC_DismissShip,
    kC_ShipFire,
    kC_ShipPickUpItem,
    kC_ShipDropItem,
    kC_ScanItem,
    kC_StartContextualAction,
    kC_StopContextualAction,
    kC_RollForward,
    kC_RollBackward,
    kC_RollLeft,
    kC_RollRight,
    kC_BoostBall,
    kC_SpiderBall,
    kC_ScrewAttack,
    kC_Bomb,
    kC_MorphIntoBall,
    kC_MorphOutOfBall,
    kC_LookUp,
    kC_LookDown,
    kC_LookLeft,
    kC_LookRight,
    kC_ReorientCamera,
    kC_ViewLock,
    kC_MapScreen,
    kC_MapCircleUp,
    kC_MapCircleDown,
    kC_MapCircleLeft,
    kC_MapCircleRight,
    kC_MapMoveForward,
    kC_MapMoveBackward,
    kC_MapMoveLeft,
    kC_MapMoveRight,
    kC_MapZoomIn,
    kC_MapZoomOut,
    kC_InventoryScreen,
    kC_MenuUp,
    kC_MenuDown,
    kC_MenuLeft,
    kC_MenuRight,
    kC_OptionsScreen,
    kC_LogScreen,
    kC_DebugMoveCamera,
    kC_DebugMoveCameraFast,
    kC_DebugMoveCameraForward,
    kC_DebugMoveCameraBackward,
    kC_DebugMoveCameraRight,
    kC_DebugMoveCameraLeft,
    kC_DebugTurnCameraRight,
    kC_DebugTurnCameraLeft,
    kC_DebugPitchCameraUp,
    kC_DebugPitchCameraDown,
    kC_DebugToggleCamera,
    kC_DebugDropPlayer,
    kC_DebugMenuStart,
    kC_DebugMenuUp,
    kC_DebugMenuDown,
    kC_DebugMenuLeft,
    kC_DebugMenuRight,
    kC_DebugMenuSelect,
    kC_DebugMenuBack,
    kC_DebugMenuIncrementValue,
    kC_DebugMenuDecrementValue,
    kC_DebugMenuValueMultiplier,
    kC_DebugMenuRestartLevel,
    kC_DebugMapTeleport,
    kC_Count
  };

  // kFT_Unfiltered ignores the per-command enable mask.
  enum EFilterType { kFT_Filtered, kFT_Unfiltered };

  CControlMapper();

  // 0x80010E5C, 0x80010CE0 and 0x80010B64. Not implemented yet: they need the tweak's command
  // descriptions (0x8024802C) and mappings (0x80247FDC), which have no declarations yet.
  float GetAnalogInput(ECommands command, const CFinalInput& input,
                       EFilterType filter = kFT_Filtered) const;
  bool GetDigitalInput(ECommands command, const CFinalInput& input,
                       EFilterType filter = kFT_Filtered) const;
  bool GetPressInput(ECommands command, const CFinalInput& input,
                     EFilterType filter = kFT_Filtered) const;

  // Names follow Trilogy's CControlMapper, which shares these functions' shape.
  void ResetCommandFilters();
  void SetCommandEnabled(ECommands command, bool enabled);
  void SetCommandMapping(ECommands command, int control);
  void RestoreCommandMapping(ECommands command);
  void ResetCommandMappings();
  void Reset();

  static const char* GetDescriptionForCommand(ECommands command);

private:
  rstl::reserved_vector< bool, kC_Count > mCommandEnabled;
  rstl::reserved_vector< bool, kC_Count > mCommandOverridden;
  // Command and the control that replaces its primary control.
  rstl::reserved_vector< rstl::pair< ECommands, int >, 8 > mCommandOverrides;
};
CHECK_SIZEOF(CControlMapper, 0x104)

#endif // _CCONTROLMAPPER
