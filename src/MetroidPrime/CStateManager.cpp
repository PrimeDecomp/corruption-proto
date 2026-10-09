// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x8028F448..0x80298128 (104 native functions).
// Source identity: asserted target basename; reference-corrobated root placement.
// Complete emitted native/helper inventory retained; no speculative declarations. Functions
// identified with an Echoes counterpart keep their fn_ symbol until their signature is confirmed.
// 0x8028F448 +0x13C: frame-time scale from the "PhazonEnragedSlowdownUSER" curve (0x218) while
//   x212 bit 0x40 is set; reseeds the randoms ("Random() called when not deterministic"). The
//   update calls it outside cinematics.
// 0x8028F5A0 +0x60: owned native method/helper retained; exact source-level name unresolved
// 0x8028F600 +0x5C: calls 0x80051A60 (CGameArea) with the manager on every area; the update
//   calls it after the world update, like Echoes' UpdateDynamicLayers
// 0x8028F68C +0xE4: Echoes' SetGameState(EGameState): world load pause (0x80037584), rumble
//   disable and the CAudioManager voice context
// 0x8028F770 +0xC4: owned native method/helper retained; exact source-level name unresolved
// 0x8028F834 +0x58: Echoes' DeleteSaveGameScreen (x210 bit 0x10 from the screen's +0x80)
// 0x8028F88C +0x78: creates the save-game screen (CStateManager.cpp(3736), 0xB0 bytes)
// 0x8028F904 +0x1AC: Echoes' ShowPausedHUDMemo; the update calls it when the queued memo is due
// 0x8028FAB0 +0xD4: owned native method/helper retained; exact source-level name unresolved
// 0x8028FB84 +0xE4: Echoes' SkipCinematic (the special-function id at 0x1A4); CMFGame calls it
// 0x8028FE38 +0x200: Echoes' UpdateHintState(float)
// 0x80290038 +0x178: Echoes' UpdateEscapeSequenceTimer(float); calls KillPlayer
// 0x802901B0 +0xC: owned native method/helper retained; exact source-level name unresolved
// 0x802901BC +0x40: owned native method/helper retained; exact source-level name unresolved
// 0x802901FC +0x34: owned native method/helper retained; exact source-level name unresolved
// 0x80290230 +0x498: player debug text ("P|..", "Vel|..", movement/surface)
// 0x802906C8 +0x190: SetActorAreaId(CActor&, TAreaId) (Echoes' name)
// 0x80290858 +0x58: owned native method/helper retained; exact source-level name unresolved
// 0x802908B0 +0x28: owned native method/helper retained; exact source-level name unresolved
// 0x802908D8 +0x50: owned native method/helper retained; exact source-level name unresolved
// 0x80290928 +0x430: owned native method/helper retained; exact source-level name unresolved
// 0x80290D58 +0xF4: owned native method/helper retained; exact source-level name unresolved
// 0x80290E4C +0xFC: owned native method/helper retained; exact source-level name unresolved
// 0x80290F48 +0x2A8: unconfirmed; looks like Echoes' ApplyKnockBack
// 0x802911F0 +0x8C: owned native method/helper retained; exact source-level name unresolved
// 0x8029127C +0x2E4: owned native method/helper retained; exact source-level name unresolved
// 0x80291560 +0x198: owned native method/helper retained; exact source-level name unresolved
// 0x802916F8 +0x34C: owned native method/helper retained; exact source-level name unresolved
// 0x80291A44 +0x128: TestBombHittingWater (Echoes' name)
// 0x80291B6C +0x5E0: unconfirmed; like Echoes' ApplyLocalDamage (position, direction, damagee,
//   ids, CDamageInfo); the update's "Kill Player" option calls it with 10000 damage
// 0x8029214C +0x198: owned native method/helper retained; exact source-level name unresolved
// 0x802922E4 +0x15C: owned native method/helper retained; exact source-level name unresolved
// 0x80292440 +0xE0: KillPlayer(float, TUniqueId, TUniqueId) (Echoes' name)
// 0x80292520 +0xA4: owned native method/helper retained; exact source-level name unresolved
// 0x802925C4 +0x4CC: owned native method/helper retained; exact source-level name unresolved
// 0x80292A90 +0xB0: Echoes' DisplayAlertAboutOutOfAmmo(player, item); called for expired power-ups
// 0x80292B40 +0x108: unconfirmed; looks like Echoes' UpdateAreaSounds
// 0x80292C48 +0x34: owned native method/helper retained; exact source-level name unresolved
// 0x80292C7C +0xD4: player input step of the update (like Echoes' ProcessPlayerInput)
// 0x80292D50 +0x14C: owned native method/helper retained; exact source-level name unresolved
// 0x80292E9C +0x13AC: Update(float, CArchitectureQueue&) (Echoes' name; CMFGame passes the frame
//   time and its queue). See the frame flow in the header.
// 0x80294248 +0x1C: sort predicate of the update's per-object stat dump (unsigned time at +0x18,
//   descending)
// 0x80294264 +0xFC: vector push_back of the 0x1C-byte stat entries (vector.h(482) assert)
// 0x80294360 +0x84: that vector's destructor
// 0x802943E4 +0x60: owned native method/helper retained; exact source-level name unresolved
// 0x80294444 +0x7C: TSignal2<CStateManager&, float>::Emit
// 0x802944C0 +0x54: CGameProfileStats destructor (the update's local; constructor 0x802D8348)
// 0x80294514 +0x74: destructor of the map behind sProfileCountersA (the destructor deletes it)
// 0x80294588 +0x48: Echoes' PostUpdatePlayer(float)
// 0x802945D0 +0x178: Echoes' PreThinkObjects(float)
// 0x80294748 +0x248: memory/timing debug text ("LOW MEMORY: area ..")
// 0x80294990 +0x338: Echoes' Think(float); also takes the update's CGameProfileStats
// 0x80294CC8 +0x64: owned native method/helper retained; exact source-level name unresolved
// 0x80294D2C +0x178: owned native method/helper retained; exact source-level name unresolved
// 0x80294EA4 +0x304: unconfirmed; looks like Echoes' CrossTouchActors
// 0x802951A8 +0x164: object list check ("ENTITY INDEX MISMATCH")
// 0x8029530C +0x138: recalculates the map world sphere
// 0x80295528 +0x158: FrameBegin (Echoes' name). Stores the frame at 0x1B0, the texture and palette
//   frame counters and CGameDebug+0xA188; counts frames at 0x1B4 before a cinematic starts.
//   Argument type unproven (CMFGame passes a message parameter word).
// 0x80295680 +0x738: world setup like Echoes' InitializeState; calls SetWorld
// 0x80295DB8 +0x2F8: player spawn ("Invalid transform in Spawn Point")
// 0x802960B0 +0x170: creates the render manager and the player
// 0x80296D58 +0x74: destructor of the map behind sProfileCountersB
// 0x80297808 +0x1EC: owned native method/helper retained; exact source-level name unresolved
// 0x802979F4 +0xC8: owned native method/helper retained; exact source-level name unresolved
// 0x80297ABC +0xBC: owned native method/helper retained; exact source-level name unresolved
// 0x80297B78 +0x88: owned native method/helper retained; exact source-level name unresolved
// 0x80297CE8 +0x80: free_node_and_sub_nodes of the sProfileCountersA map
// 0x80297D68 +0x80: free_node_and_sub_nodes of the sProfileCountersB map
// 0x80297DE8 +0x16C: owned native method/helper retained; exact source-level name unresolved
// 0x80297F54 +0xA8: owned native method/helper retained; exact source-level name unresolved
// 0x80297FFC +0xFC: owned native method/helper retained; exact source-level name unresolved
// 0x802980F8 +0x30: registered static initializer; .ctors8065B960,seven independent SDA constants

#include "MetroidPrime/CStateManager.hpp"

#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/CARAMManager.hpp"
#include "Kyoto/CARAMToken.hpp"
#include "Kyoto/CDependencyGroup.hpp"
#include "Kyoto/CFrameDelayedKiller.hpp"
#include "Kyoto/CSimplePool.hpp"
#include "Kyoto/SObjectTag.hpp"
#include "MetroidPrime/CActorModelParticles.hpp"
#include "MetroidPrime/CDisplayManager.hpp"
#include "MetroidPrime/CEnvFxManager.hpp"
#include "MetroidPrime/CFluidPlaneManager.hpp"
#include "MetroidPrime/CMain.hpp"
#include "MetroidPrime/CObjectListSmall.hpp"
#include "MetroidPrime/CRenderManager.hpp"
#include "MetroidPrime/CRumbleManager.hpp"
#include "MetroidPrime/CSaveGameInterface.hpp"
#include "MetroidPrime/CStateManagerAssetFactory.hpp"
#include "MetroidPrime/CStateManagerCallbackLists.hpp"
#include "MetroidPrime/CStateManagerCollision.hpp"
#include "MetroidPrime/CStateManagerObject.hpp"
#include "MetroidPrime/CWorldLayerState.hpp"
#include "MetroidPrime/Cameras/CGameCamera.hpp"
#include "MetroidPrime/ConsoleCommands.hpp"
#include "MetroidPrime/Player/CGameMode.hpp"
#include "MetroidPrime/Player/CGameState.hpp"
#include "MetroidPrime/Player/CPlayer.hpp"
#include "MetroidPrime/Player/CWorldTransManager.hpp"
#include "MetroidPrime/TCastTo.hpp"

CStateManager::CStateManager(const rstl::ncrc_ptr< CStringPropertyManager >& stringProperties,
                             const rstl::ncrc_ptr< CScriptMailbox >& mailbox,
                             const rstl::ncrc_ptr< CMapWorldInfo >& mapWorldInfo,
                             const rstl::ncrc_ptr< CWorldTransManager >& worldTransManager,
                             const rstl::ncrc_ptr< CWorldLayerState >& worldLayerState)
: mCallbackLists(new ("CStateManager.cpp(206) : ", (const char*)0) CStateManagerCallbackLists())
, mObjectManager(new ("CStateManager.cpp(215) : ", (const char*)0)
                     CStateManagerObject(*this, stringProperties, mailbox, mapWorldInfo))
, mCollision(new ("CStateManager.cpp(216) : ", (const char*)0)
                 CStateManagerCollision(*this, *mObjectManager))
, mArchQueue(nullptr)
, mDisplayManager(nullptr)
, mRenderManager(nullptr)
, x128_(kInvalidUniqueId)
, mWeaponMgr(new ("CStateManager.cpp(226) : ", (const char*)0) CWeaponMgr())
, mFluidPlaneManager(new ("CStateManager.cpp(227) : ", (const char*)0) CFluidPlaneManager())
, mEnvFxManager(new ("CStateManager.cpp(228) : ", (const char*)0) CEnvFxManager())
, mActorModelParticles(new ("CStateManager.cpp(229) : ", (const char*)0) CActorModelParticles())
, mAssetFactory(new ("CStateManager.cpp(217) : ", (const char*)0) CStateManagerAssetFactory(*this))
, mAudioGroupDependencies(static_cast< CDependencyGroup* >(nullptr))
, mWorldTransManager(worldTransManager)
, mCurrentWorldLayerState(worldLayerState)
, mSaveGameScreen(nullptr)
, mUpdateFrameIdx(0)
, mShadowTex(gpSimplePool->GetObj("DefaultShadow"))
, mRandom(0)
, mRandomAvailable(false)
, mGameState(kGS_Running)
, mInitPhase(0)
, mHintIdx(-1)
, mHintPeriods(0)
, mPauseHudMessage(kInvalidAssetId)
, mEscapeTotalTime(0.f)
, mCurTimeMod900(0.f)
, mBossId(kInvalidUniqueId)
, mBossHealth(0.f)
, mBossLanguageTableIndex(0)
, mSpecialFunctionId(kInvalidUniqueId)
, mPlayerActorHead(kInvalidUniqueId)
, mHudMessageTime(0.f)
, mRenderFrameIndex(0)
, mCinematicGlitchFrames(0)
, mHudMessageFrameCount(0)
, mPausedHudMemoFrameCount(-1)
, mPausedHudMemoAssetId(kInvalidAssetId)
, mQueuedHudMemoDismissalDelay(0.f)
, mMapTeleportWorldId(kInvalidAssetId)
, mDeferredTransition(0)
, mPlayerLineOfSightPairs(0)
, mNextPlayerLineOfSightPair(0)
, x210_24_(false)
, x210_25_(true)
, mInMapScreen(false)
, x210_27_(false)
, mLogEndOfFrame(false)
, x210_29_(false)
, x210_30_(false)
, mTearingDown(false)
, x211_24_(false)
, mLightAmmoDepletedPlayers(0)
, mDarkAmmoDepletedPlayers(0)
, mPhazonEnragedSlowdown(false)
, mPhazonEnragedSlowdownTime(0.f)
, mPhazonEnragedSlowdownCurve(gpSimplePool->GetObj("PhazonEnragedSlowdownUSER")) {
  rs_debugger_printf("Game type is %s\n",
                     SObjectTag::Type2Text(gpGameState->GetGameMode().GetGameType()));
  mRumbleManager = new ("CStateManager.cpp(299) : ", (const char*)0) CRumbleManager(kIOP_Player1);
  gpGameState->SetQueuedScriptMsgEnabled(true);
  InitializeStateManagerConsoleCommands(this);
  CMemory::SetOutOfMemoryCallback(MemoryAllocatorAllocationFailedCallback, this);
  mShadowTex.Lock();
  sProfileCountersA = new ("CStateManager.cpp(367) : ", (const char*)0)
      rstl::map< rstl::string, SProfileCountersA >();
  sProfileCountersB = new ("CStateManager.cpp(368) : ", (const char*)0)
      rstl::map< rstl::string, SProfileCountersB >();
}

// 0x80296220. Echoes' teardown: every object except the players and cameras is sent a delete
// message, then removed and deleted; then the cameras (from a copy of the camera list) and the
// player. Around it the prototype emits its two teardown signals and frees the profile tables.
CStateManager::~CStateManager() {
  mCallbackLists->StateManagerDestroying().Emit(*this);
  mTearingDown = true;
  CMemory::OffsetFakeStatics(-0x24214);
  mRumbleManager->HardStopAll();
  mEnvFxManager->Cleanup();
  mRandomAvailable = true;

  CObjectList& objects = mObjectManager->ObjectListById(0);
  mObjectManager->ClearGraveyard();
  for (int i = 0; i != 0x800; ++i) {
    CEntity* entity = objects[i];
    if (entity != nullptr && TCastToConstPtr< CPlayer >(entity) == nullptr &&
        TCastToConstPtr< CGameCamera >(entity) == nullptr) {
      mObjectManager->DeliverScriptMsg(
          CScriptMsg(kSM_Delete, kInvalidUniqueId, entity->GetUniqueId(),
                     SScriptMsgOriginator(kInvalidUniqueId), kSS_InvalidState));
    }
  }
  for (int i = 0; i != 0x800; ++i) {
    CEntity* entity = objects[i];
    if (entity != nullptr && TCastToConstPtr< CPlayer >(entity) == nullptr &&
        TCastToConstPtr< CGameCamera >(entity) == nullptr) {
      mObjectManager->RemoveObject(entity->GetUniqueId());
      delete entity;
    }
  }
  mObjectManager->ClearGraveyard();

  const CObjectListSmall cameras(mObjectManager->GetObjectListSmallById(3));
  for (CObjectListSmall::TList::const_iterator it = cameras.begin(); it != cameras.end(); ++it) {
    if (const CGameCamera* camera = TCastToConstPtr< CGameCamera >(*it)) {
      mObjectManager->DeliverScriptMsg(
          CScriptMsg(kSM_Delete, kInvalidUniqueId, camera->GetUniqueId(),
                     SScriptMsgOriginator(kInvalidUniqueId), kSS_InvalidState));
      mObjectManager->RemoveObject(camera->GetUniqueId());
      delete camera;
    }
  }
  CPlayer* player = mObjectManager->Player();
  mObjectManager->DeliverScriptMsg(CScriptMsg(kSM_Delete, kInvalidUniqueId, player->GetUniqueId(),
                                              SScriptMsgOriginator(kInvalidUniqueId),
                                              kSS_InvalidState));
  mObjectManager->RemoveObject(player->GetUniqueId());
  delete player;

  CMemory::SetOutOfMemoryCallback(nullptr, nullptr);
  delete sProfileCountersA;
  sProfileCountersA = nullptr;
  delete sProfileCountersB;
  sProfileCountersB = nullptr;
  gpMain->SetThirtyFps(false);
  mCallbackLists->StateManagerDestroyed().Emit(*this);
}

// 0x80295524. Empty, as in Echoes; FrameBegin (0x80295528) still calls it with (2, 0x180000).
void CStateManager::SwapOutTexturesToARAM(int, uint) {}

// 0x80295470
const bool CStateManager::MemoryAllocatorAllocationFailedCallback(const void* context, uint) {
  CStateManager* mgr = static_cast< CStateManager* >(const_cast< void* >(context));
  gpfnWarningPrintf("Out of memory, last area %d, current area %d\n",
                    mgr->ObjectManager().GetPreviousAreaId().Value(),
                    mgr->ObjectManager().GetNextAreaId().Value());
  rs_debugger_printf("Out of memory, last area %d, current area %d\n",
                     mgr->ObjectManager().GetPreviousAreaId().Value(),
                     mgr->ObjectManager().GetNextAreaId().Value());
  return mgr->SwapOutAllPossibleMemory();
}

// 0x80295444
bool CStateManager::SwapOutAllPossibleMemory() {
  CFrameDelayedKiller::StallAndFlushAllAllocations();
  CARAMManager::WaitForAllDMAsToComplete();
  CARAMToken::UpdateAllDMAs();
  return true;
}

// 0x8028FD50
void CStateManager::AddWeaponId(TUniqueId owner, TUniqueId weapon, EWeaponType type) {
  mWeaponMgr->IncrCount(owner, type);
  mWeaponAdded.Emit(*this, weapon, type);
}

// 0x8028FC68
void CStateManager::RemoveWeaponId(TUniqueId owner, TUniqueId weapon, EWeaponType type) {
  mWeaponMgr->DecrCount(owner, type);
  mWeaponRemoved.Emit(*this, weapon);
}

// 0x8028F678
void CStateManager::SetBossParams(TUniqueId bossId, float maxEnergy, uint stringIdx) {
  mBossId = bossId;
  mBossHealth = maxEnergy;
  mBossLanguageTableIndex = stringIdx;
}

// 0x8028F65C
void CStateManager::QueueMessage(int frameCount, CAssetId msg, float f1) {
  mPausedHudMemoFrameCount = frameCount;
  mPausedHudMemoAssetId = msg;
  mQueuedHudMemoDismissalDelay = f1;
}

// 0x8028F584
void CStateManager::StartPhazonEnragedSlowdown() {
  mPhazonEnragedSlowdown = true;
  mPhazonEnragedSlowdownTime = 0.f;
}
