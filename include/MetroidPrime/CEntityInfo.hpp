#ifndef _CENTITYINFO
#define _CENTITYINFO

#include "Kyoto/CVParamTransfer.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/vector.hpp"
#include "types.h"

// State and message names come from the G2MEAB name tables in CScriptLUA.cpp's rodata
// (lbl_8068AAA0 and lbl_8068ADB8), which pair each name with its FourCC. The tables end with
// a null name whose value is -1.
enum EScriptObjectState {
  kSS_Active = 0x41435456,
  kSS_AnimStart = 0x414e4d53,
  kSS_AnimOver = 0x414e4d4f,
  kSS_Approach = 0x41505243,
  kSS_Arrived = 0x41525256,
  kSS_AttachedAnimatedObject = 0x41544f42,
  kSS_AttachedCollisionObject = 0x4154434c,
  kSS_Attack = 0x4154544b,
  kSS_BeginScan = 0x4253434e,
  kSS_CameraPath = 0x43505448,
  kSS_CameraTarget = 0x43544754,
  kSS_CameraTime = 0x4354494d,
  kSS_Closed = 0x434c4f53,
  kSS_Connect = 0x434f4e4e,
  kSS_Damage = 0x44414d47,
  kSS_DarkXDamage = 0x44524b58,
  kSS_Dead = 0x44454144,
  kSS_DeathRattle = 0x5241544c,
  kSS_DeGenerate = 0x44474e52,
  kSS_DrawAfter = 0x4c445741,
  kSS_DrawBefore = 0x4c445742,
  kSS_EndScan = 0x4553434e,
  kSS_Entered = 0x454e5452,
  kSS_Exited = 0x45584954,
  kSS_Footstep = 0x464f4f54,
  kSS_Freeze = 0x4652455a,
  kSS_Generate = 0x47524e54,
  kSS_IceXDamage = 0x49444d47,
  kSS_BallIceXDamage = 0x42494447,
  kSS_Inactive = 0x49435456,
  kSS_InheritBounds = 0x49424e44,
  kSS_Inside = 0x494e5344,
  kSS_Locked = 0x4c4f434b,
  kSS_MaxReached = 0x4d415852,
  kSS_Modify = 0x4d444659,
  kSS_Open = 0x4f50454e,
  kSS_Patrol = 0x5054524c,
  kSS_Play = 0x504c4159,
  kSS_PressA = 0x50525341,
  kSS_PressB = 0x50525342,
  kSS_PressX = 0x50525358,
  kSS_PressY = 0x50525359,
  kSS_PressZ = 0x5052535a,
  kSS_PressStart = 0x50525354,
  kSS_ReflectedDamage = 0x52454644,
  kSS_Relay = 0x524c4159,
  kSS_ResistedDamage = 0x52455344,
  kSS_Retreat = 0x52545254,
  kSS_RotationStart = 0x524f5453,
  kSS_RotationOver = 0x524f544f,
  kSS_ScanDone = 0x53434e44,
  kSS_ScanSource = 0x53434e53,
  kSS_Sequence = 0x53514e43,
  kSS_Slave = 0x534c4156,
  kSS_SpawnResidue = 0x52445545,
  kSS_SpawnSmallCreatures = 0x53534352,
  kSS_SpawnMediumCreatures = 0x534d4352,
  kSS_SpawnLargeCreatures = 0x534c4352,
  kSS_ThinkAfter = 0x4c544b41,
  kSS_ThinkBefore = 0x4c544b42,
  kSS_UnFreeze = 0x5546525a,
  kSS_Unlocked = 0x554c434b,
  kSS_XDamage = 0x58444d47,
  kSS_BallXDamage = 0x42584447,
  kSS_Zero = 0x5a45524f,
  kSS_NonZero = 0x215a4552,
  kSS_Left = 0x4c454654,
  kSS_Right = 0x52474854,
  kSS_Up = 0x55502020,
  kSS_Down = 0x444f574e,
  kSS_AILogicState1 = 0x41495331,
  kSS_AILogicState2 = 0x41495332,
  kSS_AILogicState3 = 0x41495333,
  kSS_InFront = 0x58494e46,
  kSS_InBack = 0x58494e42,
  kSS_Outside = 0x584f5554,
  kSS_FrontToBack = 0x58463242,
  kSS_BackToFront = 0x58423246,
  kSS_InternalState0 = 0x49533030,
  kSS_InternalState1 = 0x49533031,
  kSS_InternalState2 = 0x49533032,
  kSS_InternalState3 = 0x49533033,
  kSS_InternalState4 = 0x49533034,
  kSS_InternalState5 = 0x49533035,
  kSS_InternalState6 = 0x49533036,
  kSS_InternalState7 = 0x49533037,
  kSS_InternalState8 = 0x49533038,
  kSS_InternalState9 = 0x49533039,
  kSS_InternalState10 = 0x49533130,
  kSS_InternalState11 = 0x49533131,
  kSS_InternalState12 = 0x49533132,
  kSS_InternalState13 = 0x49533133,
  kSS_InternalState14 = 0x49533134,
  kSS_InternalState15 = 0x49533135,
  kSS_InternalState16 = 0x49533136,
  kSS_InternalState17 = 0x49533137,
  kSS_InternalState18 = 0x49533138,
  kSS_InternalState19 = 0x49533139,
  kSS_InvalidState = -1,
};

enum EScriptObjectMessage {
  kSM_Action = 0x4143544e,
  kSM_Activate = 0x41435456,
  kSM_Alert = 0x414c5254,
  kSM_Arrived = 0x41525256,
  kSM_Attach = 0x41544348,
  kSM_AttachInstance = 0x41544349,
  kSM_ClearOriginator = 0x434f5247,
  kSM_Close = 0x434c4f53,
  kSM_Deactivate = 0x44435456,
  kSM_Decrement = 0x44454352,
  kSM_Escape = 0x45534350,
  kSM_FadeIn = 0x46414449,
  kSM_FadeOut = 0x4641444f,
  kSM_Follow = 0x464f4c57,
  kSM_Increment = 0x494e4352,
  kSM_Kill = 0x4b494c4c,
  kSM_Load = 0x4c4f4144,
  kSM_Lock = 0x4c4f434b,
  kSM_Next = 0x4e455854,
  kSM_None = 0x4e4f4e45,
  kSM_Open = 0x4f50454e,
  kSM_Play = 0x504c4159,
  kSM_Reset = 0x52534554,
  kSM_ResetAndStart = 0x52535453,
  kSM_SetOriginator = 0x534f5247,
  kSM_SetToMax = 0x534d4158,
  kSM_SetToZero = 0x5a45524f,
  kSM_Start = 0x53545254,
  kSM_Stop = 0x53544f50,
  kSM_StopAndReset = 0x53545052,
  kSM_ToggleActive = 0x54435456,
  kSM_ToggleOpen = 0x544f504e,
  kSM_Unload = 0x554c4f44,
  kSM_Unlock = 0x554c434b,
  kSM_Left = 0x4c454654,
  kSM_Right = 0x52474854,
  kSM_Up = 0x55502020,
  kSM_Down = 0x444f574e,
  kSM_Landed = 0x584c4e44,
  kSM_LandedOnStaticGround = 0x584c5347,
  kSM_Entered = 0x58454e54,
  kSM_Clear = 0x58434c52,
  kSM_OffGround = 0x584f4646,
  kSM_OnIce = 0x584f4e49,
  kSM_OnOrganic = 0x584f4e4f,
  kSM_OnDirt = 0x584f4e44,
  kSM_HitObject = 0x58484954,
  kSM_OnPlatform = 0x584f4e50,
  kSM_Falling = 0x5846414c,
  kSM_Create = 0x58435254,
  kSM_Delete = 0x5844454c,
  kSM_AreaLoaded = 0x58414c44,
  kSM_AreaUnloading = 0x5841554c,
  kSM_WorldLoaded = 0x58574c44,
  kSM_EnteredFluid = 0x58454e46,
  kSM_InsideFluid = 0x58494e46,
  kSM_ExitedFluid = 0x58455846,
  kSM_Launching = 0x584c4155,
  kSM_Damage = 0x58444d47,
  kSM_ResistedDamage = 0x58524447,
  kSM_AcidOnVisor = 0x58414f56,
  kSM_InShrubbery = 0x58494e53,
  kSM_EnteredPhazonPool = 0x5845505a,
  kSM_InsidePhazonPool = 0x5849505a,
  kSM_ExitedPhazonPool = 0x5858505a,
  kSM_AIUpdateDisabled = 0x58415544,
  kSM_ReflectedDamage = 0x58584447,
  kSM_InternalMessage0 = 0x494d3030,
  kSM_InternalMessage1 = 0x494d3031,
  kSM_InternalMessage2 = 0x494d3032,
  kSM_InternalMessage3 = 0x494d3033,
  kSM_InternalMessage4 = 0x494d3034,
  kSM_InternalMessage5 = 0x494d3035,
  kSM_InternalMessage6 = 0x494d3036,
  kSM_InternalMessage7 = 0x494d3037,
  kSM_InternalMessage8 = 0x494d3038,
  kSM_InternalMessage9 = 0x494d3039,
  kSM_InternalMessage10 = 0x494d3130,
  kSM_InternalMessage11 = 0x494d3131,
  kSM_InternalMessage12 = 0x494d3132,
  kSM_InternalMessage13 = 0x494d3133,
  kSM_InternalMessage14 = 0x494d3134,
  kSM_InternalMessage15 = 0x494d3135,
  kSM_InternalMessage16 = 0x494d3136,
  kSM_InternalMessage17 = 0x494d3137,
  kSM_InternalMessage18 = 0x494d3138,
  kSM_InternalMessage19 = 0x494d3139,
  kSM_Invalid = -1,
};

struct SConnection {
  EScriptObjectState state;
  EScriptObjectMessage msg;
  TEditorId objId;
  int xc_;
};
CHECK_SIZEOF(SConnection, 0x10)

// Layout from the CEntity constructor, which reads each field once.
class CEntityInfo {
public:
  TAreaId GetAreaId() const { return mAreaId; }
  const rstl::vector< SConnection >& GetConnectionList() const { return mConnections; }
  TEditorId GetEditorId() const { return mEditorId; }
  bool GetActive() const { return mActive; }

private:
  TAreaId mAreaId;
  rstl::vector< SConnection > mConnections;
  TEditorId mEditorId;
  bool mActive : 1;
  // Echoes names these UpdateWhileOccluded and UpdateDuringCinematicSkip; CEntity copies
  // both into its own flags.
  bool x18_25_ : 1;
  bool x18_26_ : 1;
  int x1c_;
};
CHECK_SIZEOF(CEntityInfo, 0x20)

// Guessed name. CScriptMsg embeds this at 0x8; its constructor stores the originator id, a
// -1 word and a default CVParamTransfer.
struct SScriptMsgOriginator {
  TUniqueId mId;
  int x4_;
  CVParamTransfer x8_;
};
CHECK_SIZEOF(SScriptMsgOriginator, 0x10)

class CScriptMsg {
public:
  TUniqueId GetSenderId() const { return mSenderId; }
  TUniqueId GetTargetId() const { return mTargetId; }
  const SScriptMsgOriginator& GetOriginator() const { return mOriginator; }
  EScriptObjectMessage GetMessage() const { return mMsg; }
  EScriptObjectState GetState() const { return mState; }

private:
  TUniqueId mSenderId;
  TUniqueId mTargetId;
  SScriptMsgOriginator mOriginator;
  EScriptObjectMessage mMsg;
  EScriptObjectState mState;
};
CHECK_SIZEOF(CScriptMsg, 0x20)

struct SLdrEditorProperties;
CEntityInfo& LdrToEntityInfo(CEntityInfo& info, const SLdrEditorProperties& properties);

#endif // _CENTITYINFO
