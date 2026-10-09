// G2MEAB prototype NonMatching translation unit.
// .text: 0x802B3D08..0x802B9354 (70 native functions). Deferred inlining emits functions in
// reverse source order. Functions not yet implemented:
// 0x802B41D0 +0xFD0: AcceptScriptMsg; registers bindings, runs On<Message> handlers
// 0x802B51A0 +0x4C: owned native method/helper retained; exact source-level name unresolved
// 0x802B51EC +0x64: owned native method/helper retained; exact source-level name unresolved
// 0x802B5250 +0x130: emitted TSignal2 subscription allocation; source55
// 0x802B5380 +0x40: owned native method/helper retained; exact source-level name unresolved
// 0x802B53C0 +0x70: owned native method/helper retained; exact source-level name unresolved
// 0x802B5430 +0x90: owned native method/helper retained; exact source-level name unresolved
// 0x802B54C0 +0x64: emitted nonstatic two-argument signal callback
// 0x802B5524 +0x40: shared gamebinding Lua C closure trampoline
// 0x802B5768 +0x388: RandomWeightedChoice binding (from the registration table)
// 0x802B5AF0 +0x78: owned native method/helper retained; exact source-level name unresolved
// 0x802B5B68 +0x2C: owned native method/helper retained; exact source-level name unresolved
// 0x802B5B94 +0x90: owned native method/helper retained; exact source-level name unresolved
// 0x802B5DD8 +0x120: SetGlobalState binding (from the registration table)
// 0x802B5EF8 +0x130: GetGlobalState binding (from the registration table)
// 0x802B6028 +0x1AC: SetPlayerInventoryAmount binding (from the registration table)
// 0x802B61D4 +0x1D4: GetPlayerInventoryCapacity binding (from the registration table)
// 0x802B63A8 +0x1D8: GetPlayerInventoryAmount binding (from the registration table)
// 0x802B6630 +0x2FC: SendEventMessage binding (from the registration table)
// 0x802B6A74 +0x4C: owned native method/helper retained; exact source-level name unresolved
// 0x802B6AC0 +0xBC: owned native method/helper retained; exact source-level name unresolved
// 0x802B6CC4 +0x4C: owned native method/helper retained; exact source-level name unresolved
// 0x802B6D10 +0xBC: owned native method/helper retained; exact source-level name unresolved
// 0x802B6DCC +0x1A0: SendEvent binding (from the registration table)
// 0x802B7080 +0x14C: SetObjectTranslation binding (from the registration table)
// 0x802B71CC +0x29C: SetObjectTransform binding (from the registration table)
// 0x802B7468 +0x208: owned native method/helper retained; exact source-level name unresolved
// 0x802B7670 +0x148: GetObjectTranslation binding (from the registration table)
// 0x802B77B8 +0x2B0: GetObjectTransform binding (from the registration table)
// 0x802B7A68 +0xBC: owned native method/helper retained; exact source-level name unresolved
// 0x802B7B24 +0x330: GetObjectConnectionList binding (from the registration table)
// 0x802B86FC +0xC: raw signal disconnect wrapper: clearword8; noGhidralisting
// 0x802B8708 +0x94: emitted LUA signal subscription destructor
// 0x802B879C +0x50: owned native method/helper retained; exact source-level name unresolved
// 0x802B87EC +0x24: owned native method/helper retained; exact source-level name unresolved
// 0x802B92B0 +0x74: owned native method/helper retained; exact source-level name unresolved
// 0x802B9324 +0x30: registered static initializer; .ctors8065B9A0,seven independent SDA constants

#include "MetroidPrime/ScriptObjects/CScriptLUA.hpp"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Text/CStringTokenizer.hpp"
#include "MetroidPrime/CStateManager.hpp"
#include "MetroidPrime/CStateManagerObject.hpp"
#include "MetroidPrime/CVisorParameters.hpp"
#include "MetroidPrime/ScriptLoader.hpp"
#include "MetroidPrime/ScriptLoader/SLdrLUAScript.hpp"
#include "rstl/StringExtras.hpp"

#include <string.h>

// Guessed names. Name/value tables for the script states and messages, terminated by a null
// name whose value is the invalid id.
template < typename T >
struct SScriptNameEntry {
  const char* mName;
  T mValue;
};

static const SScriptNameEntry< EScriptObjectState > skStateNames[] = {
    {"Active", kSS_Active},
    {"AnimStart", kSS_AnimStart},
    {"AnimOver", kSS_AnimOver},
    {"Approach", kSS_Approach},
    {"Arrived", kSS_Arrived},
    {"AttachedAnimatedObject", kSS_AttachedAnimatedObject},
    {"AttachedCollisionObject", kSS_AttachedCollisionObject},
    {"Attack", kSS_Attack},
    {"BeginScan", kSS_BeginScan},
    {"CameraPath", kSS_CameraPath},
    {"CameraTarget", kSS_CameraTarget},
    {"CameraTime", kSS_CameraTime},
    {"Closed", kSS_Closed},
    {"Connect", kSS_Connect},
    {"Damage", kSS_Damage},
    {"DarkXDamage", kSS_DarkXDamage},
    {"Dead", kSS_Dead},
    {"DeathRattle", kSS_DeathRattle},
    {"DeGenerate", kSS_DeGenerate},
    {"DrawAfter", kSS_DrawAfter},
    {"DrawBefore", kSS_DrawBefore},
    {"EndScan", kSS_EndScan},
    {"Entered", kSS_Entered},
    {"Exited", kSS_Exited},
    {"Footstep", kSS_Footstep},
    {"Freeze", kSS_Freeze},
    {"Generate", kSS_Generate},
    {"IceXDamage", kSS_IceXDamage},
    {"BallIceXDamage", kSS_BallIceXDamage},
    {"Inactive", kSS_Inactive},
    {"InheritBounds", kSS_InheritBounds},
    {"Inside", kSS_Inside},
    {"Locked", kSS_Locked},
    {"MaxReached", kSS_MaxReached},
    {"Modify", kSS_Modify},
    {"Open", kSS_Open},
    {"Patrol", kSS_Patrol},
    {"Play", kSS_Play},
    {"PressA", kSS_PressA},
    {"PressB", kSS_PressB},
    {"PressX", kSS_PressX},
    {"PressY", kSS_PressY},
    {"PressZ", kSS_PressZ},
    {"PressStart", kSS_PressStart},
    {"ReflectedDamage", kSS_ReflectedDamage},
    {"Relay", kSS_Relay},
    {"ResistedDamage", kSS_ResistedDamage},
    {"Retreat", kSS_Retreat},
    {"RotationStart", kSS_RotationStart},
    {"RotationOver", kSS_RotationOver},
    {"ScanDone", kSS_ScanDone},
    {"ScanSource", kSS_ScanSource},
    {"Sequence", kSS_Sequence},
    {"Slave", kSS_Slave},
    {"SpawnResidue", kSS_SpawnResidue},
    {"SpawnSmallCreatures", kSS_SpawnSmallCreatures},
    {"SpawnMediumCreatures", kSS_SpawnMediumCreatures},
    {"SpawnLargeCreatures", kSS_SpawnLargeCreatures},
    {"ThinkAfter", kSS_ThinkAfter},
    {"ThinkBefore", kSS_ThinkBefore},
    {"UnFreeze", kSS_UnFreeze},
    {"Unlocked", kSS_Unlocked},
    {"XDamage", kSS_XDamage},
    {"BallXDamage", kSS_BallXDamage},
    {"Zero", kSS_Zero},
    {"NonZero", kSS_NonZero},
    {"Left", kSS_Left},
    {"Right", kSS_Right},
    {"Up", kSS_Up},
    {"Down", kSS_Down},
    {"AILogicState1", kSS_AILogicState1},
    {"AILogicState2", kSS_AILogicState2},
    {"AILogicState3", kSS_AILogicState3},
    {"InFront", kSS_InFront},
    {"InBack", kSS_InBack},
    {"Outside", kSS_Outside},
    {"FrontToBack", kSS_FrontToBack},
    {"BackToFront", kSS_BackToFront},
    {"InternalState0", kSS_InternalState0},
    {"InternalState1", kSS_InternalState1},
    {"InternalState2", kSS_InternalState2},
    {"InternalState3", kSS_InternalState3},
    {"InternalState4", kSS_InternalState4},
    {"InternalState5", kSS_InternalState5},
    {"InternalState6", kSS_InternalState6},
    {"InternalState7", kSS_InternalState7},
    {"InternalState8", kSS_InternalState8},
    {"InternalState9", kSS_InternalState9},
    {"InternalState10", kSS_InternalState10},
    {"InternalState11", kSS_InternalState11},
    {"InternalState12", kSS_InternalState12},
    {"InternalState13", kSS_InternalState13},
    {"InternalState14", kSS_InternalState14},
    {"InternalState15", kSS_InternalState15},
    {"InternalState16", kSS_InternalState16},
    {"InternalState17", kSS_InternalState17},
    {"InternalState18", kSS_InternalState18},
    {"InternalState19", kSS_InternalState19},
    {nullptr, kSS_InvalidState},
};

static const SScriptNameEntry< EScriptObjectMessage > skMessageNames[] = {
    {"Action", kSM_Action},
    {"Activate", kSM_Activate},
    {"Alert", kSM_Alert},
    {"Arrived", kSM_Arrived},
    {"Attach", kSM_Attach},
    {"AttachInstance", kSM_AttachInstance},
    {"ClearOriginator", kSM_ClearOriginator},
    {"Close", kSM_Close},
    {"Deactivate", kSM_Deactivate},
    {"Decrement", kSM_Decrement},
    {"Escape", kSM_Escape},
    {"FadeIn", kSM_FadeIn},
    {"FadeOut", kSM_FadeOut},
    {"Follow", kSM_Follow},
    {"Increment", kSM_Increment},
    {"Kill", kSM_Kill},
    {"Load", kSM_Load},
    {"Lock", kSM_Lock},
    {"Next", kSM_Next},
    {"None", kSM_None},
    {"Open", kSM_Open},
    {"Play", kSM_Play},
    {"Reset", kSM_Reset},
    {"ResetAndStart", kSM_ResetAndStart},
    {"SetOriginator", kSM_SetOriginator},
    {"SetToMax", kSM_SetToMax},
    {"SetToZero", kSM_SetToZero},
    {"Start", kSM_Start},
    {"Stop", kSM_Stop},
    {"StopAndReset", kSM_StopAndReset},
    {"ToggleActive", kSM_ToggleActive},
    {"ToggleOpen", kSM_ToggleOpen},
    {"Unload", kSM_Unload},
    {"Unlock", kSM_Unlock},
    {"Left", kSM_Left},
    {"Right", kSM_Right},
    {"Up", kSM_Up},
    {"Down", kSM_Down},
    {"Landed", kSM_Landed},
    {"LandedOnStaticGround", kSM_LandedOnStaticGround},
    {"Entered", kSM_Entered},
    {"Clear", kSM_Clear},
    {"OffGround", kSM_OffGround},
    {"OnIce", kSM_OnIce},
    {"OnOrganic", kSM_OnOrganic},
    {"OnDirt", kSM_OnDirt},
    {"HitObject", kSM_HitObject},
    {"OnPlatform", kSM_OnPlatform},
    {"Falling", kSM_Falling},
    {"Create", kSM_Create},
    {"Delete", kSM_Delete},
    {"AreaLoaded", kSM_AreaLoaded},
    {"AreaUnloading", kSM_AreaUnloading},
    {"WorldLoaded", kSM_WorldLoaded},
    {"EnteredFluid", kSM_EnteredFluid},
    {"InsideFluid", kSM_InsideFluid},
    {"ExitedFluid", kSM_ExitedFluid},
    {"Launching", kSM_Launching},
    {"Damage", kSM_Damage},
    {"ResistedDamage", kSM_ResistedDamage},
    {"AcidOnVisor", kSM_AcidOnVisor},
    {"InShrubbery", kSM_InShrubbery},
    {"EnteredPhazonPool", kSM_EnteredPhazonPool},
    {"InsidePhazonPool", kSM_InsidePhazonPool},
    {"ExitedPhazonPool", kSM_ExitedPhazonPool},
    {"AIUpdateDisabled", kSM_AIUpdateDisabled},
    {"ReflectedDamage", kSM_ReflectedDamage},
    {"InternalMessage0", kSM_InternalMessage0},
    {"InternalMessage1", kSM_InternalMessage1},
    {"InternalMessage2", kSM_InternalMessage2},
    {"InternalMessage3", kSM_InternalMessage3},
    {"InternalMessage4", kSM_InternalMessage4},
    {"InternalMessage5", kSM_InternalMessage5},
    {"InternalMessage6", kSM_InternalMessage6},
    {"InternalMessage7", kSM_InternalMessage7},
    {"InternalMessage8", kSM_InternalMessage8},
    {"InternalMessage9", kSM_InternalMessage9},
    {"InternalMessage10", kSM_InternalMessage10},
    {"InternalMessage11", kSM_InternalMessage11},
    {"InternalMessage12", kSM_InternalMessage12},
    {"InternalMessage13", kSM_InternalMessage13},
    {"InternalMessage14", kSM_InternalMessage14},
    {"InternalMessage15", kSM_InternalMessage15},
    {"InternalMessage16", kSM_InternalMessage16},
    {"InternalMessage17", kSM_InternalMessage17},
    {"InternalMessage18", kSM_InternalMessage18},
    {"InternalMessage19", kSM_InternalMessage19},
    {nullptr, kSM_Invalid},
};

static inline int ToLowerCase(char c) {
  bool upper = false;
  if (c >= 'A' && c <= 'Z') {
    upper = true;
  }
  return upper ? c + 0x20 : c;
}

// Guessed name.
static inline bool EqualsIgnoreCase(const char* a, const char* b) {
  while (*a != '\0' && *b != '\0') {
    if (ToLowerCase(*a) != ToLowerCase(*b)) {
      break;
    }
    ++a;
    ++b;
  }
  return *a == '\0' && *b == '\0';
}

// Guessed name. Mode 1 compares exactly and mode 0 ignores ASCII case.
static inline bool NamesMatch(const char* name, const char* other, int mode) {
  if (name == nullptr) {
    return false;
  }
  switch (mode) {
  case 1:
    return strcmp(name, other) == 0;
  case 0:
    return EqualsIgnoreCase(name, other);
  default:
    return false;
  }
}

// Guessed name. Returns the table's terminating value when no entry matches.
template < typename T >
T LookupScriptName(const SScriptNameEntry< T >* table, const char* name, int mode) {
  for (; table->mName != nullptr; ++table) {
    if (NamesMatch(name, table->mName, mode)) {
      return table->mValue;
    }
  }
  return table->mValue;
}

// Guessed names. The handler names are standalone arrays (.sdata2 and .rodata), not pooled
// literals.
static const char kOnThink[] = "OnThink";
static const char kOnGlobalEvent[] = "OnGlobalEvent";

// Guessed name. Installed with lua_atpanic by the constructor.
static int LuaPanic(lua_State* L) {
  rs_debugger_printf("%s\n", lua_tostring(L, 1));
  return -1;
}

CScriptLUA::CScriptLUA(TUniqueId uid, const rstl::string& name, const CEntityInfo& info,
                       const CTransform4f& xf, const rstl::string& stateAliases,
                       const rstl::string& messageAliases, const rstl::string& initializationScript,
                       const rstl::string& script)
: CActor(uid, name, info, 0, xf, CMaterialList(), CVisorParameters::None())
, mInitializationScript(initializationScript)
, mScript(script)
, mStateMgr(nullptr) {
  lua_atpanic(mLuaState->GetCState(), LuaPanic);

  CStringTokenizer stateTokens(stateAliases.data());
  while (!stateTokens.IsExhausted()) {
    rstl::string alias = stateTokens.ReadToken(nullptr, '"');
    rstl::string stateName = stateTokens.ReadToken(nullptr, '"');
    if (alias.length() == 0 || stateName.length() == 0) {
      continue;
    }
    EScriptObjectState state = LookupScriptName(skStateNames, stateName.data(), 1);
    mAliasToState.insert(rstl::pair< rstl::string, EScriptObjectState >(alias, state));
    mStateToAlias.insert(rstl::pair< EScriptObjectState, rstl::string >(state, alias));
  }

  CStringTokenizer messageTokens(messageAliases.data());
  while (!messageTokens.IsExhausted()) {
    rstl::string alias = messageTokens.ReadToken(nullptr, '"');
    rstl::string messageName = messageTokens.ReadToken(nullptr, '"');
    if (alias.length() == 0 || messageName.length() == 0) {
      continue;
    }
    EScriptObjectMessage msg = LookupScriptName(skMessageNames, messageName.data(), 1);
    mMessageToAlias.insert(rstl::pair< EScriptObjectMessage, rstl::string >(msg, alias));
    mAliasToMessage.insert(rstl::pair< rstl::string, EScriptObjectMessage >(alias, msg));
  }
}

CScriptLUA::~CScriptLUA() {}

// With no argument the original returns 1 without pushing a value.
int CScriptLUA::GetObjectName(lua_State* L) {
  LuaPlus::LuaObject result;
  if (lua_gettop(L) < 1) {
    result.AssignString(mLuaState, GetName().data());
  } else {
    if (!lua_isstring(L, 1)) {
      lua_pushstring(L, "incorrect argument to function 'GetObjectName'");
      lua_error(L);
    }
    // Script ids travel as decimal strings; only the low 16 bits (the slot) are kept.
    const TUniqueId id = TUniqueId(
        static_cast< ushort >(CStringExtras::ConvertToInteger(rstl::string_l(lua_tostring(L, 1)))));
    const CEntity* entity = mStateMgr->ObjectManager().GetObjectById(id);
    if (entity == nullptr) {
      result.AssignNil(mLuaState);
    } else {
      result.AssignString(mLuaState, entity->GetName().data());
    }
    result.PushStack();
  }
  return 1;
}

int CScriptLUA::GetObjectId(lua_State* L) {
  LuaPlus::LuaObject result;
  result.AssignString(mLuaState, CStringExtras::CreateFromInteger(GetUniqueId().value).data());
  result.PushStack();
  return 1;
}

int CScriptLUA::GetObjectActive(lua_State* L) {
  TUniqueId id = GetUniqueId();
  if (lua_gettop(L) > 0) {
    if (!lua_isstring(L, 1)) {
      lua_pushstring(L, "incorrect argument to function 'GetObjectActive'");
      lua_error(L);
    }
    id = TUniqueId(
        static_cast< ushort >(CStringExtras::ConvertToInteger(rstl::string_l(lua_tostring(L, 1)))));
  }
  const CEntity* entity = mStateMgr->ObjectManager().GetObjectById(id);
  LuaPlus::LuaObject result;
  result.AssignBoolean(mLuaState, entity != nullptr && entity->GetActive());
  result.PushStack();
  return 1;
}

int CScriptLUA::Print(lua_State* L) {
  int count = lua_gettop(L);
  for (int i = 1; i <= count; ++i) {
    if (!lua_isstring(L, i)) {
      lua_pushstring(L, "incorrect argument to function 'Print'");
      lua_error(L);
    }
    rs_debugger_printf("%s", lua_tostring(L, i));
  }
  return 0;
}

int CScriptLUA::RandomRange(lua_State* L) {
  LuaPlus::LuaObject result;
  if (lua_gettop(L) != 2) {
    lua_pushstring(L, "missing arguments to function 'RandomRange'");
    lua_error(L);
  } else {
    if (!lua_isstring(L, 1) && !lua_isnumber(L, 1)) {
      lua_pushstring(L, "incorrect argument to function 'RandomRange'");
      lua_error(L);
    }
    if (!lua_isstring(L, 2) && !lua_isnumber(L, 2)) {
      lua_pushstring(L, "incorrect argument to function 'RandomRange'");
      lua_error(L);
    }
    const float min = lua_tonumber(L, 1);
    const float max = lua_tonumber(L, 2);
    result.AssignNumber(mLuaState, mStateMgr->Random()->Range(min, max));
    result.PushStack();
    return 1;
  }
  return 0;
}

void CScriptLUA::RunScript(const char* buffer, int size, const char* name) {
  if (mLuaState->LoadBuffer(buffer, size, name) != 0) {
    rs_debugger_printf("LUA Load Error(%s): %s\n", name, lua_tostring(mLuaState->GetCState(), -1));
  } else if (mLuaState->PCall(0, 0, 0) != 0) {
    rs_debugger_printf("LUA Run Error(%s): %s\n", name, lua_tostring(mLuaState->GetCState(), -1));
  }
}

void CScriptLUA::OnGlobalEvent(const rstl::string& event, const rstl::string& value) {
  LuaPlus::LuaObject handler = mLuaState->GetGlobal(kOnGlobalEvent);
  if (handler.IsFunction()) {
    LuaPlus::LuaAutoBlock block(mLuaState->GetCState());
    handler.PushStack();
    mLuaState->PushString(event.data());
    mLuaState->PushString(value.data());
    if (mLuaState->PCall(2, 0, 0) != 0) {
      rs_debugger_printf("LUA Run Error(%s): %s\n", "OnGlobalEvent",
                         lua_tostring(mLuaState->GetCState(), -1));
    }
  }
}

void CScriptLUA::Think(float dt, CStateManager& mgr) {
  if (!GetActive()) {
    return;
  }
  LuaPlus::LuaObject handler = mLuaState->GetGlobal(kOnThink);
  if (handler.IsFunction()) {
    LuaPlus::LuaAutoBlock block(mLuaState->GetCState());
    handler.PushStack();
    mLuaState->PushNumber(dt);
    if (mLuaState->PCall(1, 0, 0) != 0) {
      rs_debugger_printf("LUA Run Error(%s): %s\n", "OnThink",
                         lua_tostring(mLuaState->GetCState(), -1));
    }
  }
}

// The four strings feed the constructor in order: state aliases, message aliases, the
// initialization script and the script.
CEntity* LoadLUAScript(CStateManager& mgr, CInputStream& input, CEntityInfo& info) {
  SLdrLUAScript sldrThis;
#include "MetroidPrime/ScriptLoader/SLdrLUAScript.inc"
  return RS_NEW(1191) CScriptLUA(
      mgr.ObjectManager().AllocateUniqueId(), sldrThis.editorProperties.name,
      LdrToEntityInfo(info, sldrThis.editorProperties), LdrToTransform4f(sldrThis.editorProperties),
      sldrThis.unknown_0xed4a2787, sldrThis.unknown_0x9facea01, sldrThis.unknown_0xea46b664,
      sldrThis.unknown_0xa1ecc54b);
}
