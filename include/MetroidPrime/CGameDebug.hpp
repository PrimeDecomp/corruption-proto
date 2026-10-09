#ifndef _CGAMEDEBUG
#define _CGAMEDEBUG

#include "types.h"

#include "Kyoto/Graphics/CColor.hpp"
#include "Kyoto/TSignal1.hpp"
#include "MetroidPrime/CDebugMenu.hpp"
#include "MetroidPrime/CDebugOption.hpp"

#include "rstl/auto_ptr.hpp"
#include "rstl/construct.hpp"
#include "rstl/optional_object.hpp"
#include "rstl/reserved_vector.hpp"
#include "rstl/set.hpp"
#include "rstl/single_ptr.hpp"
#include "rstl/string.hpp"
#include "rstl/vector.hpp"

class CStateManager;

// The in-game debug options of the Corruption prototype (CGameDebug.cpp, "CGameDebug.cpp(NN) : "
// allocation strings). Neither Echoes nor Prime has an equivalent; only the class name is known,
// so all member names are guessed from the strings and the call sites.
//
// What it offers:
// - A table of 0x16C CDebugOption slots, addressed by EDebugOption. AddDebugOptions (0x8003CBDC)
//   registers ~360 options in 33 categories (ECategory): cheats, powerup toggles, player/orbit/
//   grapple/gun tuning, HUD, hyper mode tuning, audio, scripting message tracing, camera, AI and
//   creature debug draws, ragdolls, collision, programmer tools (memory metrics, dumps, streaming
//   status, "Terminate Game"), renderer and lighting switches ("Bloom", "PVS", "Wireframe", ...),
//   water, EnvFx, demo recording and the audio debugger. Boolean options hold 0/1; the others
//   hold a float with a range and step, often with named values ("OFF"/"ON"/"DEBUG").
// - Every option reports its changes to CGameDebug, which records the option index and emits a
//   per-option TSignal1<CStateManager&> once per frame (DispatchChangedOptions, called by
//   CMFGame). Actors and other objects subscribe with ConnectOption; CActor uses it for
//   "Draw Object Collision Boxes".
// - Two engine sync passes (not implemented): 0x80043CD4 reads tweak and engine state into the
//   options and 0x80042698 applies the options back to the engine.
// - A CDebugMenu based browser: a category list and one page per category, with per-category
//   selection memory, driven by the CControlMapper debug menu commands (0x800451AC, not
//   implemented) and drawn with the debug font (0x80045B04, 0x80045E40).
// - The debug message colour and the "Log" mode of "Debug Messages Enabled": messages are then
//   gathered in a 2 MB buffer that is written to C:\FIO\debuglog.txt over the broadband adapter.
// - The movie capture name and countdown used by ScreenCapture and CControllerRecorder; a free
//   "<name>_NNN_" prefix is picked by probing the host for "<name>_NNN_000000.mzip".
// - Debug unlocks of the multiplayer music and maps (0x80048A94) and the mapping of the powerup
//   options to player items (0x80048C20).
class CGameDebug {
public:
  // The category names come from GetCategoryName.
  enum ECategory {
    kC_Debug,
    kC_Cheats,
    kC_PowerupsWeapons,
    kC_PowerupsUpgrades,
    kC_PowerupsMorphball,
    kC_PowerupsMisc,
    kC_Player,
    kC_Audio,
    kC_Grapple,
    kC_PlayerGun,
    kC_Gui,
    kC_OrbitStuff,
    kC_LoadGame,
    kC_SaveGame,
    kC_GameRewards,
    kC_HyperMode,
    kC_Revolution,
    kC_Misc,
    kC_Scripting,
    kC_Camera,
    kC_AI,
    kC_AITrace,
    kC_AICreatures,
    kC_RagDolls,
    kC_Collision,
    kC_Programmer,
    kC_Renderer,
    kC_Lighting,
    kC_Demo,
    kC_Water,
    kC_EnvFx,
    kC_Profiler,
    kC_AudioDebug,
    kC_Count,
  };

  // Option indices, named after the menu strings AddDebugOptions registers them with. The
  // comment gives the category, the kind of value and the menu text; named values follow the
  // colon. Options 0x23..0x2E repeat the hyper mode tuning for the second tweak set and are drawn
  // in yellow.
  enum EDebugOption {
    kDO_GiveAllPowerupsCheat = 0x0,       // Cheats, bool "Give all powerups cheat"
    kDO_InvulnerableSamus = 0x1,          // Cheats, bool "Invulnerable Samus"
    kDO_PlayerPositionInfo = 0x2,         // Player, bool "Player Position Info"
    kDO_ShowZones = 0x3,                  // Player, value "Show Zones"
    kDO_AutoAim = 0x4,                    // Player, bool "Auto Aim"
    kDO_NormalTurnFactor = 0x5,           // Player, value "NormalTurnFactor"
    kDO_FreeLookTurnFactor = 0x6,         // Player, value "FreeLookTurnFactor"
    kDO_ShowReflection = 0x7,             // Player, bool "Show reflection"
    kDO_ScanFreezesGame = 0x8,            // Player, bool "Scan Freezes Game"
    kDO_ScanRequiresLineOfSight = 0x9,    // Player, bool "Scan Requires Line Of Sight"
    kDO_ScanSnapsToPOI = 0xA,             // Player, bool "Scan snaps to POI"
    kDO_DamageForcesBallTransition = 0xB, // Player, value "Damage Forces Ball Transition"
    kDO_MorphballInvunTime120s = 0xC,     // Player, value "Morphball invun time 1/20s"
    kDO_ShieldAllowsMovement = 0xD,       // Player, bool "Shield Allows Movement"
    kDO_DisablePlayerLockon = 0xE,        // Player, bool "DisablePlayerLockon(restart)"
    kDO_MorphballBreaksLockon =
        0xF, // Player, value "MorphballBreaksLockon": No/On Transition/Always
    kDO_ShowPlayerAnimationAndFlow = 0x10,       // Player, bool "ShowPlayerAnimationAndFlow"
    kDO_UseOldPlayerCollisionBox = 0x11,         // Player, bool "UseOldPlayerCollisionBox"
    kDO_Use25PercentStepUpInPMovement = 0x12,    // Player, bool "Use25PercentStepUpInPMovement"
    kDO_KillPlayer = 0x13,                       // Player, bool "Kill Player"
    kDO_FrozenJostleCount = 0x14,                // Player, value "Frozen Jostle Count"
    kDO_ApplyPlayerKnockbackForce = 0x15,        // Player, bool "Apply player knockback force"
    kDO_HyperModeType = 0x16,                    // Hyper Mode, value "HyperMode Type"
    kDO_HyperModeInvulnerablePhazonLoss = 0x17,  // Hyper Mode, bool "InvulnerablePhazonLoss"
    kDO_HyperModeInvulnerableTime = 0x18,        // Hyper Mode, value "InvulnerableTime"
    kDO_HyperModeCorruptionTime = 0x19,          // Hyper Mode, value "CorruptionTime"
    kDO_HyperModeConstantCorruptionRate = 0x1A,  // Hyper Mode, bool "ConstantCorruptionRate"
    kDO_HyperModeCorruptionRate = 0x1B,          // Hyper Mode, value "CorruptionRate"
    kDO_HyperModePhazonLevel = 0x1C,             // Hyper Mode, value "PhazonLevel"
    kDO_HyperModePhazonCapacity = 0x1D,          // Hyper Mode, value "PhazonCapacity"
    kDO_HyperModeDangerPercentage = 0x1E,        // Hyper Mode, value "DangerPercentage"
    kDO_HyperModeBeamLossAmount = 0x1F,          // Hyper Mode, value "BeamLossAmount"
    kDO_HyperModeMissileLossAmount = 0x20,       // Hyper Mode, value "MissileLossAmount"
    kDO_HyperModePhazonBallRate = 0x21,          // Hyper Mode, value "PhazonBallRate"
    kDO_HyperModeDamageMultiplier = 0x22,        // Hyper Mode, value "DamageMultiplier"
    kDO_HyperModeInvulnerablePhazonLoss2 = 0x23, // Hyper Mode, bool "InvulnerablePhazonLoss"
    kDO_HyperModeInvulnerableTime2 = 0x24,       // Hyper Mode, value "InvulnerableTime"
    kDO_HyperModeCorruptionTime2 = 0x25,         // Hyper Mode, value "CorruptionTime"
    kDO_HyperModeConstantCorruptionRate2 = 0x26, // Hyper Mode, bool "ConstantCorruptionRate"
    kDO_HyperModeCorruptionRate2 = 0x27,         // Hyper Mode, value "CorruptionRate"
    kDO_HyperModePhazonLevel2 = 0x28,            // Hyper Mode, value "PhazonLevel"
    kDO_HyperModePhazonCapacity2 = 0x29,         // Hyper Mode, value "PhazonCapacity"
    kDO_HyperModeDangerPercentage2 = 0x2A,       // Hyper Mode, value "DangerPercentage"
    kDO_HyperModeBeamLossAmount2 = 0x2B,         // Hyper Mode, value "BeamLossAmount"
    kDO_HyperModeMissileLossAmount2 = 0x2C,      // Hyper Mode, value "MissileLossAmount"
    kDO_HyperModePhazonBallRate2 = 0x2D,         // Hyper Mode, value "PhazonBallRate"
    kDO_HyperModeDamageMultiplier2 = 0x2E,       // Hyper Mode, value "DamageMultiplier"
    kDO_EnableAudio = 0x2F,                      // Audio, bool "Enable Audio"
    kDO_MusicOnOff = 0x30,                       // Audio, bool "Music On/Off"
    kDO_SoundMode = 0x31,                        // Audio, value "Sound Mode": Mono/Stereo/Surround
    kDO_SoundAcousticsEnabled = 0x32,            // Audio, bool "Sound Acoustics Enabled"
    kDO_LoadTweaksFromPCHost = 0x33,             // Audio, bool "Load Tweaks from PC Host"
    kDO_SaveTweaksToPCHost = 0x34,               // Audio, bool "Save Tweaks to PC Host"
    kDO_LoadTweaksFromMemoryCard = 0x35,         // Audio, bool "Load Tweaks from Memory Card"
    kDO_SaveTweaksToMemoryCard = 0x36,           // Audio, bool "Save Tweaks to Memory Card"
    kDO_FrontendStartScreenVolume = 0x37,        // Misc, value "Frontend StartScreen volume"
    kDO_FrontendStartScreenVolume2 = 0x38,       // Misc, value "Frontend StartScreen volume"
    kDO_SfxMasterVolume = 0x39,                  // Audio, value "Sfx Master Volume"
    kDO_MusicMasterVolume = 0x3A,                // Audio, value "Music Master Volume"
    kDO_MinimumShakeAmplitude = 0x3B,            // Audio, value "Minimum Shake Amplitude"
    kDO_CheckMultiplayerConflicts = 0x3C,        // Audio, bool "Check Multiplayer Conflicts"
    kDO_GrappleDistance = 0x3D,                  // Grapple, value "Distance"
    kDO_GrappleBeamLength = 0x3E,                // Grapple, value "BeamLength"
    kDO_GrappleSwingTime = 0x3F,                 // Grapple, value "SwingTime"
    kDO_GrappleMaxVelocity = 0x40,               // Grapple, value "MaxVelocity"
    kDO_GrapplePullCloseDistance = 0x41,         // Grapple, value "PullCloseDistance"
    kDO_GrapplePullDampenDistance = 0x42,        // Grapple, value "PullDampenDistance"
    kDO_GrapplePullVelocity = 0x43,              // Grapple, value "PullVelocity"
    kDO_GrappleTurnRate = 0x44,                  // Grapple, value "TurnRate"
    kDO_GrappleJumpForce = 0x45,                 // Grapple, value "JumpForce"
    kDO_GrappleHoldOrbitButton = 0x46,           // Grapple, bool "HoldOrbitButton"
    kDO_GrappleTurnControlsReversed = 0x47,      // Grapple, bool "TurnControlsReversed"
    kDO_GrappleControlScheme = 0x48,             // Grapple, value "ControlScheme"
    kDO_ChargeBeam = 0x49,                       // PlayerGun, bool "Charge Beam"
    kDO_PlayMinorFidget = 0x4A,                  // PlayerGun, value "Play Minor Fidget"
    kDO_PlayMajorFidget = 0x4B,                  // PlayerGun, value "Play Major Fidget"
    kDO_InfiniteComboAmmo = 0x4C,                // PlayerGun, bool "Infinite Combo Ammo"
    kDO_PowerupSuckDistance = 0x4D,              // PlayerGun, value "Powerup Suck Distance"
    kDO_RenderGun = 0x4E,                        // PlayerGun, bool "Render Gun"
    kDO_ShowBeamAmmo = 0x4F,                     // PlayerGun, bool "Show Beam Ammo"
    kDO_NormalSeekerAlwaysHomes = 0x50,          // PlayerGun, bool "Normal seeker always homes"
    kDO_ShowGunState = 0x51,                     // PlayerGun, value "Show gun state"
    kDO_GunDebug = 0x52,                         // PlayerGun, value "Gun Debug"
    kDO_GunDebugOutput = 0x53,                   // PlayerGun, value "Gun Debug Output"
    kDO_MapCheatEnabled = 0x54,                  // Cheats, bool "Map Cheat Enabled"
    kDO_LogBookCheatEnabled = 0x55, // Cheats, value "LogBook Cheat Enabled": Off/On/Show Memory
    kDO_ShowRedundantHints = 0x56,  // Gui, value "Show Redundant Hints"
    kDO_RedundantHintQuickTimeout = 0x57,   // Gui, bool "Redundant Hint Quick Timeout"
    kDO_DisableHUD = 0x58,                  // Gui, value "Disable HUD": Enabled/Disabled
    kDO_ProfileGuiElements = 0x59,          // Gui, value "Profile Gui Elements"
    kDO_EnableHud = 0x5A,                   // Gui, value "Enable Hud"
    kDO_EnableTargeting = 0x5B,             // Gui, value "Enable Targeting"
    kDO_EnableAutoMapper = 0x5C,            // Gui, value "Enable AutoMapper"
    kDO_RadarMode = 0x5D,                   // Gui, value "Radar Mode"
    kDO_EnableVisors = 0x5E,                // Gui, bool "Enable Visors"
    kDO_HUDCameraFOV = 0x5F,                // Gui, value "HUD Camera FOV"
    kDO_HUDCameraY = 0x60,                  // Gui, value "HUD Camera Y (forward/backward)"
    kDO_HUDCameraZ = 0x61,                  // Gui, value "HUD Camera Z (up/down)"
    kDO_VisorBeamIconsAlwaysShow = 0x62,    // Gui, bool "Visor/Beam Icons always show"
    kDO_FaceReflectMode = 0x63,             // Gui, value "Face Reflect Mode"
    kDO_FaceReflectionWidth = 0x64,         // Gui, value "Face Reflection Width"
    kDO_FaceReflectionHeight = 0x65,        // Gui, value "Face Reflection Height"
    kDO_FaceReflectionPositionY = 0x66,     // Gui, value "Face Reflection PositionY"
    kDO_FaceReflectionPositionZ = 0x67,     // Gui, value "Face Reflection PositionZ"
    kDO_FaceReflectionAspectRatio = 0x68,   // Gui, value "Face Reflection Aspect Ratio"
    kDO_WidescreenBallmode = 0x69,          // Gui, bool "Widescreen Ballmode"
    kDO_ShowSafeFrame = 0x6A,               // Gui, bool "Show Safe Frame"
    kDO_Vertical2PlayerSplitScreen = 0x6B,  // Gui, bool "Vertical2PlayerSplitScreen"
    kDO_SingleScreenForMultiplayer = 0x6C,  // Gui, bool "SingleScreenForMultiplayer"
    kDO_ScanTextDebugger = 0x6D,            // Gui, bool "ScanTextDebugger"
    kDO_AlwaysSortMapSurfaces = 0x6E,       // Gui, bool "AlwaysSortMapSurfaces"
    kDO_AllMultiplayerMusicUnlocked = 0x6F, // Cheats, bool "All Multiplayer Music Unlocked"
    kDO_AllMultiplayerMapsUnlocked = 0x70,  // Cheats, bool "All Multiplayer Maps Unlocked"
    kDO_PowerBeam = 0x71,                   // Powerups (Weapons), value "PowerBeam"
    kDO_NovaBeam = 0x72,                    // Powerups (Weapons), value "NovaBeam"
    kDO_PlasmaBeam = 0x73,                  // Powerups (Weapons), value "PlasmaBeam"
    kDO_ChargeUpgrade = 0x74,               // Powerups (Weapons), value "ChargeUpgrade"
    kDO_Missile = 0x75,                     // Powerups (Weapons), value "Missile"
    kDO_IceMissile = 0x76,                  // Powerups (Weapons), value "IceMissile"
    kDO_SeekerMissile = 0x77,               // Powerups (Weapons), value "SeekerMissile"
    kDO_GrappleBeam = 0x78,                 // Powerups (Weapons), value "GrappleBeam"
    kDO_GrappleBeamVoltage = 0x79,          // Powerups (Weapons), value "GrappleBeamVoltage"
    kDO_Bomb = 0x7A,                        // Powerups (Weapons), value "Bomb"
    kDO_HyperShot = 0x7B,                   // Powerups (Weapons), value "HyperShot": Off/Tap/Hold
    kDO_CombatVisor = 0x7C,                 // Powerups (Upgrades), value "CombatVisor"
    kDO_ScanVisor = 0x7D,                   // Powerups (Upgrades), value "ScanVisor"
    kDO_CommandVisor = 0x7E,                // Powerups (Upgrades), value "CommandVisor"
    kDO_XRayVisor = 0x7F,                   // Powerups (Upgrades), value "XRayVisor"
    kDO_VariaSuit = 0x80,                   // Powerups (Upgrades), value "VariaSuit"
    kDO_DoubleJump = 0x81,                  // Powerups (Upgrades), value "DoubleJump"
    kDO_ScrewAttack = 0x82,                 // Powerups (Upgrades), value "ScrewAttack"
    kDO_Energy = 0x83,                      // Powerups (Misc), value "Energy"
    kDO_EnergyTank = 0x84,                  // Powerups (Misc), value "EnergyTank"
    kDO_MorphBall = 0x85,                   // Powerups (Morphball), value "MorphBall"
    kDO_BoostBall = 0x86,                   // Powerups (Morphball), value "BoostBall"
    kDO_SpiderBall = 0x87,                  // Powerups (Morphball), value "SpiderBall"
    kDO_PhazonBall = 0x88,                  // Powerups (Morphball), value "PhazonBall"
    kDO_IceBall = 0x89,                     // Powerups (Morphball), value "IceBall"
    kDO_FireBallFlammable = 0x8A,           // Powerups (Morphball), value "FireBallFlammable"
    kDO_FireBall = 0x8B,                    // Powerups (Morphball), value "FireBall"
    kDO_CannonBall = 0x8C,                  // Powerups (Morphball), value "CannonBall"
    kDO_ActivateMorphballBoost = 0x8D,      // Powerups (Morphball), value "ActivateMorphballBoost"
    kDO_ItemPercentage = 0x8E,              // Powerups (Misc), value "ItemPercentage"
    kDO_DebugMessagesEnabled = 0x8F,        // Misc, value "Debug Messages Enabled": Off/On/Log
    kDO_SaveDebugMessageLog = 0x90,         // Misc, bool "Save Debug Message Log"
    kDO_DebugSaveFormat = 0x91,             // Misc, bool "Debug Save Format"
    kDO_DebugMessageColor = 0x92,     // Misc, value "Debug Message Color": White/Black/Grey/Orange
    kDO_GenericMsgs = 0x93,           // Misc, bool "Generic Msgs"
    kDO_DumpScreenShot = 0x94,        // Misc, bool "Dump Screen Shot"
    kDO_ShowHintInfo = 0x95,          // Misc, value "Show Hint Info"
    kDO_ScriptMsgDebugger = 0x96,     // Scripting, value "Script msg Debugger"
    kDO_ScriptMsgSender = 0x97,       // Scripting, value "Script msg Sender"
    kDO_ScriptMsgTarget = 0x98,       // Scripting, value "Script msg Target"
    kDO_ScriptMsgType = 0x99,         // Scripting, value "Script msg Type"
    kDO_ScriptMsgExcludeType = 0x9A,  // Scripting, value "Script msg Exclude Type"
    kDO_ScriptMsgState = 0x9B,        // Scripting, value "Script msg State"
    kDO_ScriptMsgExcludeState = 0x9C, // Scripting, value "Script msg Exclude State"
    kDO_LogScriptMessageQueue = 0x9D, // Scripting, bool "Log script message queue"
    kDO_CacheRepeatedScriptMessages = 0x9E, // Scripting, bool "Cache repeated script messages"
    kDO_ShowWeaponDamageRadius = 0x9F,      // Misc, bool "Show Weapon Damage Radius"
    kDO_ShowScanInfo = 0xA0,                // Misc, bool "Show Scan Info"
    kDO_ShowSplinePaths = 0xA1,             // Misc, bool "Show Spline Paths"
    kDO_Language = 0xA2,                    // Misc, value "Language (requires reset)"
    kDO_FakePAL50HzUpdate = 0xA3,           // Misc, bool "Fake PAL 50Hz Update"
    kDO_SlowGameDown = 0xA4,                // Misc, bool "Slow game down"
    kDO_ShowBBAMessages = 0xA5,             // Misc, bool "Show BBA Messages"
    kDO_ShowGeneratorMessages = 0xA6,       // Scripting, bool "Show Generator Messages"
    kDO_ShowWeaponHomingTargets = 0xA7,     // Misc, bool "Show Weapon Homing Targets"
    kDO_DebugCamera = 0xA8,                 // Camera, value "Debug Camera"
    kDO_ShowOtherCameraFunkyStuff = 0xA9,   // Camera, value "Show Other Camera Funky Stuff"
    kDO_ShowBallCameraStateInfo = 0xAA,     // Camera, value "Show Ball Camera State Info"
    kDO_DebugCameraSameControllerRESTART =
        0xAB,                          // Camera, bool "Debug Camera Same Controller RESTART"
    kDO_BallCameraFreelook = 0xAC,     // Camera, bool "Ball Camera Freelook"
    kDO_ShowCameraSplineStuff = 0xAD,  // Camera, value "Show Camera Spline Stuff"
    kDO_ShowCameraSurfaceStuff = 0xAE, // Camera, value "Show Camera Surface Stuff"
    kDO_ShowCameraBreadcrumbs = 0xAF,  // Camera, bool "Show Camera Breadcrumbs"
    kDO_ShowPlayerBreadcrumbs = 0xB0,  // Camera, bool "Show Player Breadcrumbs"
    kDO_ShowCameraPosition = 0xB1,     // Camera, bool "Show Camera Position"
    kDO_ShowCameraLookat = 0xB2,       // Camera, bool "Show Camera Lookat"
    kDO_ShowCameraIdealLookat = 0xB3,  // Camera, bool "Show Camera Ideal Lookat"
    kDO_ShowFunkyColliderStuff = 0xB4, // Camera, value "Show Funky Collider Stuff"
    kDO_ShowSpindleCameraStuff = 0xB5, // Camera, value "Show Spindle Camera stuff"
    kDO_ShowInterpolationStuff = 0xB6, // Camera, bool "Show Interpolation Stuff"
    kDO_ShowCameraShakerInfo = 0xB7,   // Camera, bool "Show Camera Shaker Info"
    kDO_CinematicBarsConstrictViewport = 0xB8, // Camera, bool "Cinematic Bars Constrict Viewport"
    kDO_ShowPatternedStates = 0xB9,            // AI, bool "Show Patterned States"
    kDO_ShowHP = 0xBA,                         // AI, bool "Show HP"
    kDO_ShowDistanceFromPlayer = 0xBB,         // AI, value "Show Distance From Player"
    kDO_PathFinding = 0xBC,                    // AI, value "Path Finding"
    kDO_PathFindingPoints = 0xBD,              // AI, value "Path Finding Points"
    kDO_PathFindingOctree = 0xBE,              // AI, value "Path Finding Octree"
    kDO_ShowWaypoints = 0xBF,        // AI, value "Show Waypoints": Off/Points/Points+Connections
    kDO_AIShotPrediction = 0xC0,     // AI, value "AI Shot Prediction": Off/On/Reduced
    kDO_KillAllAIs = 0xC1,           // Cheats, bool "Kill All AIs"
    kDO_SlowAllAIs = 0xC2,           // Cheats, value "Slow All AIs": Off/Slow/Very Slow
    kDO_DisableAIUpdates = 0xC3,     // Cheats, value "Disable AI Updates": Enable/Disable
    kDO_DrawAIDamageLocator = 0xC4,  // AI, value "Draw AI Damage Locator(s)"
    kDO_DebugBodyStates = 0xC5,      // AI, bool "Debug Body States"
    kDO_ShowTeamPositions = 0xC6,    // AI, bool "Show Team Positions"
    kDO_DebugAnimations = 0xC7,      // AI, bool "Debug Animations"
    kDO_DisableAnimParticles = 0xC8, // AI, value "Disable Anim Particles": Enabled/Disabled
    kDO_Shockwaves = 0xC9,           // AI, value "Shockwaves"
    kDO_ShowBoneTracking = 0xCA,     // AI, bool "Show Bone Tracking"
    kDO_ShowBodyAlignment = 0xCB,    // AI, bool "Show Body Alignment"
    kDO_Attachments = 0xCC,          // AI, value "Attachments": Default/Hide/Cycle
    kDO_ShowFootTracking = 0xCD,     // AI, bool "Show Foot Tracking"
    kDO_AITraceViewMode = 0xCE,      // AI Trace, value "View Mode": Off/List/3D
    kDO_AITraceViewStates = 0xCF,    // AI Trace, bool "View States"
    kDO_AITraceViewFunctions = 0xD0, // AI Trace, bool "View Functions"
    kDO_AITraceViewFiredTriggers = 0xD1,         // AI Trace, bool "View Fired Triggers"
    kDO_AITraceViewBodyStates = 0xD2,            // AI Trace, bool "View BodyStates"
    kDO_AITraceViewPathfinding = 0xD3,           // AI Trace, bool "View Pathfinding"
    kDO_AITraceViewAnimations = 0xD4,            // AI Trace, bool "View Animations"
    kDO_AITraceViewAnimEvts = 0xD5,              // AI Trace, bool "View Anim Evts"
    kDO_AITraceViewScriptMsgs = 0xD6,            // AI Trace, bool "View Script Msgs"
    kDO_AITraceViewDamageInfo = 0xD7,            // AI Trace, bool "View Damage Info"
    kDO_AITraceViewMaterials = 0xD8,             // AI Trace, bool "View Materials"
    kDO_AITraceViewCustomMsgs = 0xD9,            // AI Trace, bool "View Custom Msgs"
    kDO_AITracePauseAIsWhileViewing = 0xDA,      // AI Trace, bool "Pause AIs While Viewing"
    kDO_AITraceTimeFormat = 0xDB,                // AI Trace, value "Time Format"
    kDO_AITraceShowCollisionForCurrentAI = 0xDC, // AI Trace, bool "Show Collision For Current AI"
    kDO_AITraceRecordingEnabled = 0xDD,          // AI Trace, bool "Recording Enabled"
    kDO_AITraceAlsoRecordXLNDXLSGXON = 0xDE,     // AI Trace, bool "Also record XLND,XLSG,XON*"
    kDO_AITraceAlsoRecordLoopedSoundStop = 0xDF, // AI Trace, bool "Also record LoopedSoundStop"
    kDO_AITraceAlsoRecordFunctionCalls = 0xE0,   // AI Trace, bool "Also record Function calls"
    kDO_Swarms = 0xE1,                           // AI-Creatures, value "Swarms"
    kDO_FishCloud = 0xE2,                        // AI-Creatures, value "FishCloud"
    kDO_RundasArmor = 0xE3, // AI-Creatures, value "Rundas Armor": Default/Always Draw/Never Draw
    kDO_RundasHop = 0xE4,   // AI-Creatures, value "Rundas Hop": Off/Draw Lines/Off/ON
    kDO_RundasLOSTesting = 0xE5,   // AI-Creatures, value "Rundas LOS Testing"
    kDO_RundasKillPillars = 0xE6,  // AI-Creatures, value "Rundas Kill Pillars": Off/ON
    kDO_RundasErrorCorrect = 0xE7, // AI-Creatures, value "Rundas Error Correct": ON/Off
    kDO_RundasBattleStage = 0xE8,  // AI-Creatures, value "Rundas Battle Stage"
    kDO_Korakk = 0xE9,             // AI-Creatures, value "Korakk"
    kDO_SwarmBots =
        0xEA, // AI-Creatures, value "SwarmBots": Off/Debug/Floor Collision/Kill All But 1
    kDO_DarkSamus = 0xEB,                // AI-Creatures, value "DarkSamus": Off/Debug
    kDO_Ridley = 0xEC,                   // AI-Creatures, value "Ridley"
    kDO_ShowRagdolls = 0xED,             // RagDolls, value "Show Ragdolls"
    kDO_IsStaticSpeed = 0xEE,            // RagDolls, value "Is Static Speed"
    kDO_MinImpactSoundSpeed = 0xEF,      // RagDolls, value "Min Impact Sound speed"
    kDO_ImpactVolumePerSpeed = 0xF0,     // RagDolls, value "Impact volume per speed"
    kDO_MinImpactVolume = 0xF1,          // RagDolls, value "Min impact volume"
    kDO_MinImpactInterval = 0xF2,        // RagDolls, value "Min impact interval"
    kDO_Octree = 0xF3,                   // Collision, value "Octree": Off/Draw Leaves/Draw Lines
    kDO_Collision = 0xF4,                // Collision, value "Collision"
    kDO_DrawObjectCollisionBoxes = 0xF5, // Collision, value "Draw Object Collision Boxes"
    kDO_RayCastTest = 0xF6,              // Collision, value "Ray Cast Test"
    kDO_NoStaticCollisionWhenStationary =
        0xF7,                            // Collision, bool "No Static Collision When Stationary"
    kDO_ProjectilesUseRenderMesh = 0xF8, // Collision, bool "Projectiles Use Render Mesh"
    kDO_ShowTriangleCache = 0xF9,        // Collision, bool "Show Triangle Cache"
    kDO_DrawTimeInfo = 0xFA,             // Programmer, value "DrawTime Info"
    kDO_MemoryMetrics = 0xFB, // Programmer, value "Memory Metrics": None/Full/Basic/BasicWithPeaks
    kDO_DumpMemoryAllocations = 0xFC,   // Programmer, bool "Dump Memory Allocations"
    kDO_DumpSimplePool = 0xFD,          // Programmer, bool "Dump Simple Pool"
    kDO_DumpLoadedTextures = 0xFE,      // Programmer, bool "Dump Loaded Textures"
    kDO_DumpSortedLists = 0xFF,         // Programmer, bool "Dump Sorted Lists"
    kDO_StateManagerNumbers = 0x100,    // Programmer, value "State Manager Numbers"
    kDO_StateMgrRealNames = 0x101,      // Programmer, bool "State Mgr Real Names"
    kDO_ScriptingLayersVerbose = 0x102, // Scripting, value "Scripting Layers Verbose"
    // 0x103: not registered
    // 0x104: not registered
    kDO_LogScriptObjectLoads = 0x105,  // Scripting, bool "Log script object loads"
    kDO_DumpScriptObject = 0x106,      // Scripting, bool "Dump script object"
    kDO_TerminateGame = 0x107,         // Programmer, bool "Terminate Game"
    kDO_PCLoadScriptObjects = 0x108,   // Scripting, bool "PC Load script objects"
    kDO_DebugMemoryCardSystem = 0x109, // Programmer, bool "Debug Memory Card System"
    kDO_LoadRELFilesOverBBA = 0x10A,   // Programmer, bool "Load REL Files over BBA"
    kDO_DebugMarkers = 0x10B,          // Programmer, bool "Debug Markers"
    kDO_ShowStreamingControl = 0x10C,  // Programmer, bool "Show Streaming Control"
    kDO_StreamingStatus = 0x10D,       // Programmer, value "Streaming Status"
    kDO_CPUAndGPUMetrics = 0x10E,      // Programmer, value "CPU and GPU Metrics": None/CPU/GPU/All
    kDO_Bloom = 0x10F,                 // Renderer, value "Bloom": OFF/ON/DEBUG
    kDO_ShowFramerate = 0x110,         // Renderer, bool "Show Framerate"
    kDO_DrawRendererBuckets = 0x111,   // Renderer, bool "Draw Renderer Buckets"
    kDO_DisableFog = 0x112,            // Renderer, bool "Disable Fog"
    kDO_CameraFilters = 0x113,         // Renderer, value "Camera Filters"
    kDO_PVS = 0x114,           // Renderer, value "PVS": OFF [Show All]/Enabled/Reversed/Actors Only
    kDO_ShowObjectPVS = 0x115, // Renderer, value "ShowObjectPVS"
    kDO_ShowLightPVS = 0x116,  // Renderer, value "ShowLightPVS"
    kDO_ReviewTextureSize = 0x117, // Renderer, bool "Review texture size"
    kDO_CapTextureSize = 0x118,    // Renderer, bool "Cap texture size"
    kDO_Wireframe = 0x119,         // Renderer, value "Wireframe"
    kDO_Brightness = 0x11A,        // Renderer, value "Brightness"
    kDO_RenderParticles = 0x11B,   // Renderer, bool "Render Particles"
    kDO_ParticleCounts = 0x11C,    // Renderer, bool "Particle Counts"
    kDO_DecalCounts = 0x11D,       // Renderer, bool "Decal Counts"
    // 0x11E: not registered
    kDO_PortalsEnabled = 0x11F,          // Renderer, value "Portals Enabled": OFF/ON/ON(DEBUG)
    kDO_ShowPortals = 0x120,             // Renderer, value "Show Portals"
    kDO_ShowPortalPlanes = 0x121,        // Renderer, value "Show Portal Planes"
    kDO_PerPolyDebugging = 0x122,        // Renderer, value "Per Poly Debugging"
    kDO_DebugRenderingOctree = 0x123,    // Renderer, value "Debug Rendering Octree"
    kDO_RenderPolyBoneCounts = 0x124,    // Renderer, bool "Render Poly/Bone Counts"
    kDO_RenderAnimationJoints = 0x125,   // Renderer, value "RenderAnimationJoints"
    kDO_RenderAnimationSkeleton = 0x126, // Renderer, bool "RenderAnimationSkeleton"
    kDO_FullBrightActors = 0x127,        // Lighting, bool "Full Bright Actors"
    kDO_FullBrightWorld = 0x128,         // Lighting, bool "Full Bright World"
    kDO_DrawAreaLights = 0x129,          // Lighting, bool "Draw Area Lights"
    kDO_UseNewWorldLights = 0x12A,       // Lighting, bool "Use new world lights"
    kDO_DrawDynamicLights = 0x12B,       // Lighting, value "Draw dynamic lights": OFF/ON/ON With Z
    kDO_RenderWorldShadow = 0x12C,       // Lighting, bool "Render World Shadow"
    kDO_WaterEnable = 0x12D,             // Water, bool "Enable"
    kDO_WaterWireFrame = 0x12E,          // Water, bool "Wire Frame"
    kDO_WaterProfile = 0x12F,            // Water, bool "Profile"
    kDO_WaterRefraction = 0x130,         // Water, bool "Refraction"
    kDO_WaterLightMap = 0x131,           // Water, bool "Light map"
    kDO_WaterColorMap = 0x132,           // Water, bool "Color map"
    kDO_WaterColorWarpMap = 0x133,       // Water, bool "Color warp map"
    kDO_WaterGlossMap = 0x134,           // Water, bool "Gloss map"
    kDO_WaterEnvMap = 0x135,             // Water, bool "Env map"
    kDO_WaterFogOverWater = 0x136,       // Water, bool "Fog over water"
    kDO_WaterShowWaterEntryExit = 0x137, // Water, value "Show water entry/exit"
    kDO_EnvFxEnable = 0x138,             // EnvFx, value "Enable"
    kDO_EnvFxEnableVisorDrops = 0x139,   // EnvFx, value "Enable Visor Drops"
    kDO_EnvFxEnableCellDebugDrawing = 0x13A,   // EnvFx, value "Enable Cell Debug Drawing"
    kDO_DashEnabled = 0x13B,                   // Orbit Stuff, bool "Dash Enabled"
    kDO_UsesTapHold = 0x13C,                   // Orbit Stuff, bool "Uses Tap/Hold"
    kDO_TapTime = 0x13D,                       // Orbit Stuff, value "Tap Time"
    kDO_StickXAxisThreshold = 0x13E,           // Orbit Stuff, value "Stick X-axis threshold"
    kDO_DoubleJumpImpulse = 0x13F,             // Orbit Stuff, value "DoubleJumpImpulse"
    kDO_VerticalDoubleJumpAccel = 0x140,       // Orbit Stuff, value "VerticalDoubleJumpAccel"
    kDO_HorizDoubleJumpAccel = 0x141,          // Orbit Stuff, value "HorizDoubleJumpAccel"
    kDO_OrbitDashAroundObjectsOnly = 0x142,    // Orbit Stuff, bool "OrbitDashAroundObjectsOnly"
    kDO_SpeedFactor = 0x143,                   // Orbit Stuff, value "SpeedFactor"
    kDO_ConstantSpeed = 0x144,                 // Orbit Stuff, bool "ConstantSpeed"
    kDO_MaxTime = 0x145,                       // Orbit Stuff, value "MaxTime"
    kDO_SpeedRampTime = 0x146,                 // Orbit Stuff, value "SpeedRampTime"
    kDO_DebugOrbitStuff = 0x147,               // Orbit Stuff, value "Debug Orbit Stuff"
    kDO_ShowOrbitPoint = 0x148,                // Player, bool "Show Orbit Point"
    kDO_AutoAimAtOrbitedObject = 0x149,        // Orbit Stuff, bool "Auto Aim at Orbited Object"
    kDO_FreeLookPreventsOrbitMovement = 0x14A, // Orbit Stuff, bool "FreeLookPreventsOrbitMovement"
    kDO_DamageBreaksOrbit = 0x14B,             // Orbit Stuff, value "Damage Breaks Orbit"
    kDO_OrbitCarcassOption = 0x14C,            // Orbit Stuff, value "Orbit Carcass Option"
    kDO_BoostBreaksOrbit = 0x14D,              // Orbit Stuff, bool "Boost Breaks Orbit"
    kDO_DoubleJumpBreaksOrbit = 0x14E,         // Orbit Stuff, bool "Double Jump Breaks Orbit"
    kDO_DoubleDashBreaksOrbit = 0x14F,         // Orbit Stuff, bool "Double Dash Breaks Orbit"
    kDO_OrbitLockMode = 0x150,                 // Orbit Stuff, value "OrbitLockMode"
    kDO_DemoState = 0x151,                     // Profiler, value "State": OFF/TIMER/EVENTPOINTS
    kDO_DemoSave = 0x152,                      // Profiler, bool "Save"
    kDO_DemoDisableMusyXInterrupt = 0x153,     // Profiler, bool "Disable MusyX Interrupt"
    // 0x154: not registered
    kDO_ControllerDebug = 0x155,            // Revolution, value "Controller Debug"
    kDO_FreeLookGun = 0x156,                // Revolution, bool "FreeLook Gun"
    kDO_OrbitLockGun = 0x157,               // Revolution, bool "Orbit Lock Gun"
    kDO_OrbitTagObjects = 0x158,            // Revolution, bool "Orbit Tag Objects"
    kDO_MorphballUsesAccelerometer = 0x159, // Revolution, bool "Morphball uses accelerometer"
    kDO_ShowAimingCursor = 0x15A,           // Revolution, bool "Show Aiming Cursor"
    kDO_DebugCursorType = 0x15B,            // Revolution, value "Debug Cursor Type": None/2D/3D
    // 0x15C: not registered
    // 0x15D: not registered
    kDO_LockAimingCursor = 0x15E, // Revolution, bool "Lock Aiming Cursor"
    kDO_DPDDistance = 0x15F,      // Revolution, value "DPD distance"
    // 0x160: not registered
    kDO_HardMode = 0x161,                 // Game Rewards, bool "Hard Mode"
    kDO_HardModeDamageMultiplier = 0x162, // Game Rewards, value "Hard Mode Damage Multiplier"
    kDO_HardModeWeaponMultiplier = 0x163, // Game Rewards, value "Hard Mode Weapon Multiplier"
    kDO_DebugSoundSystem = 0x164,         // Audio Debug, value "Debug Sound System"
    kDO_ShowSoundPositions = 0x165,       // Audio Debug, value "Show Sound Positions"
    kDO_ShowFModMetrics = 0x166,          // Audio Debug, bool "Show FMod Metrics"
    kDO_ProfileDSP = 0x167,               // Audio Debug, bool "Profile DSP"
    kDO_ShowMusicStreams = 0x168,         // Audio Debug, bool "Show Music Streams"
    kDO_CaptureShortAudioClip = 0x169,    // Audio Debug, bool "Capture Short Audio Clip"
    kDO_RenderAudioFifo = 0x16A,          // Audio Debug, value "Render Audio Fifo": Off/On/Pause
    kDO_ShowAudioListener = 0x16B,        // Audio Debug, value "Show Audio Listener"
    kDO_Count = 0x16C,
  };

  // Guessed name. The per-option signal emitted once per frame after the option changed.
  typedef TSignal1< CStateManager& > OptionSignal;

  CGameDebug();

  // Guessed names.
  CDebugOption* GetOption(int index) {
    rstl::optional_object< CDebugOption >& slot = mOptions[index];
    return slot ? &slot.data() : nullptr;
  }
  // -1 when the option was never registered.
  float GetOptionValue(int index) {
    const CDebugOption* option = GetOption(index);
    return option != nullptr ? option->GetValue() : -1.f;
  }
  bool IsOptionSet(int index) { return GetOptionValue(index) != 0.f; }
  int GetOptionInt(int index) { return static_cast< int >(GetOptionValue(index)); }

  // Guessed names. Register an option in its slot (the first registration wins) and add a named
  // value to an option. The color defaults to white.
  void AddOption(int category, int index, const char* name, bool value);
  void AddOption(int category, int index, const char* name, float value, float min, float max,
                 float step);
  void AddOption(int category, int index, const char* name, bool value, const CColor& color);
  void AddOption(int category, int index, const char* name, float value, float min, float max,
                 float step, const CColor& color);
  void AddOptionChoice(int index, const char* name, float value);
  void AddDebugOptions(); // 0x8003CBDC, not implemented

  // Guessed names. Emits the signal of every option changed since the last call.
  void DispatchChangedOptions(CStateManager& mgr);
  rstl::auto_ptr< IConnection > ConnectOption(int index, OptionSignal::Functor functor);

  static const char* GetCategoryName(int category);
  CColor GetDebugMessageColor() const;
  void ResetDebugMessageLog();
  void AppendToLog(const char* text);
  void DumpLog();
  void FreeLog();
  void CloseMenu();
  // Guessed name. main reads the menu's presence flag (+0xA130) directly.
  bool IsMenuOpen() const { return mMenu.valid(); }

  // Guessed names. The movie capture name ("<name>_NNN_" once a free slot was chosen) and the
  // remaining capture time that main counts down.
  void SetMovieCaptureTime(float time);
  const rstl::string& GetMovieCaptureName();

  // Guessed name. 0x80048C20: the player item of a powerup option (kDO_PowerBeam ..
  // kDO_ItemPercentage), -1 for any other option.
  static int GetPlayerItemForOption(int index);

private:
  // Guessed name. A reserved_vector style array whose N elements are default-constructed
  // (0x800488EC; the loop is 0x8004892C).
  template < typename T, int N >
  class TFilledArray {
  public:
    TFilledArray(int) : mCount(N) { Construct(data(), N); }

    T& operator[](int idx) { return data()[idx]; }

  private:
    static void Construct(T* it, int count) {
      for (int i = 0; i < count; ++i, ++it) {
        new (it) T();
      }
    }
    T* data() { return reinterpret_cast< T* >(mData); }

    int mCount;
    uchar mData[N * sizeof(T)];
  };

  void InstallOption(const CDebugOption& option);
  void OnOptionChanged(CDebugOption* option);

  enum {
    kLogBufferSize = 0x200000,
  };

  rstl::reserved_vector< rstl::optional_object< CDebugOption >, kDO_Count > mOptions;
  rstl::reserved_vector< rstl::auto_ptr< IConnection >, kDO_Count > mOptionConnections;
  TFilledArray< OptionSignal, kDO_Count > mOptionSignals;
  rstl::set< int > mChangedOptions;
  // The selected row of each category page; the menu input stores it.
  rstl::reserved_vector< int, kC_Count > mCategorySelection;
  int x9FE8_menuPage; // 0 or 1; passed to the menu builder (0x800466E4)
  rstl::optional_object< CDebugMenu > mMenu;
  rstl::vector< CDebugMenu::SItem > mMenuItems;
  rstl::vector< rstl::string > mMenuLabels;
  int xA154_currentCategory; // 0 shows the category list
  int xA158_;
  int xA15C_backItemId;
  int xA160_exitItemId;
  int xA164_controller;
  bool xA168_;
  bool xA169_; // Cleared by CMFGame
  bool xA16A_;
  bool xA16B_; // main
  bool xA16C_; // main; not initialized
  bool mMovieCaptureNameChosen;
  rstl::string mMovieCaptureName;
  int xA180_;
  float mMovieCaptureTime;
  int xA188_;
  rstl::single_ptr< char > mLogBuffer;
  int mLogSize;
  bool mLogFileCreated; // Later dumps append to C:\FIO\debuglog.txt
};
CHECK_SIZEOF(CGameDebug, 0xA198)

// 0x8079710C; also CGameGlobalObjects+0x130.
extern CGameDebug* gpGameDebug;

#endif // _CGAMEDEBUG
