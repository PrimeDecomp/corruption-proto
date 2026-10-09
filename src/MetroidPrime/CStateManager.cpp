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
// 0x80292440 +0xE0: KillPlayer(float, TUniqueId, TUniqueId) (Echoes' guessed name)
// 0x80292520 +0xA4: owned native method/helper retained; exact source-level name unresolved
// 0x802925C4 +0x4CC: owned native method/helper retained; exact source-level name unresolved
// 0x80292B40 +0x108: unconfirmed; looks like Echoes' UpdateAreaSounds
// 0x80292C48 +0x34: owned native method/helper retained; exact source-level name unresolved
// 0x80292C7C +0xD4: player input step of the update (like Echoes' ProcessPlayerInput)
// 0x80292D50 +0x14C: owned native method/helper retained; exact source-level name unresolved
// 0x80294264 +0xFC: vector push_back of the 0x1C-byte stat entries (vector.h(482) assert)
// 0x802943E4 +0x60: owned native method/helper retained; exact source-level name unresolved
// 0x80294444 +0x7C: TSignal2<CStateManager&, float>::Emit
// 0x802945D0 +0x178: Echoes' PreThinkObjects(float)
// 0x80294748 +0x248: memory/timing debug text ("LOW MEMORY: area ..")
// 0x80294990 +0x338: Echoes' Think(float); also takes the update's CGameProfileStats
// 0x80294CC8 +0x64: owned native method/helper retained; exact source-level name unresolved
// 0x80294D2C +0x178: owned native method/helper retained; exact source-level name unresolved
// 0x80294EA4 +0x304: unconfirmed; looks like Echoes' CrossTouchActors
// 0x802951A8 +0x164: object list check ("ENTITY INDEX MISMATCH")
// 0x8029530C +0x138: recalculates the map world sphere
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
#include "Kyoto/Basics/CBasics.hpp"
#include "Kyoto/Basics/CStopwatch.hpp"
#include "Kyoto/CARAMManager.hpp"
#include "Kyoto/CARAMToken.hpp"
#include "Kyoto/CDependencyGroup.hpp"
#include "Kyoto/CFrameDelayedKiller.hpp"
#include "Kyoto/CSimplePool.hpp"
#include "Kyoto/Graphics/CGraphicsPalette.hpp"
#include "Kyoto/Graphics/CTexture.hpp"
#include "Kyoto/Math/CVector3f.hpp"
#include "Kyoto/Math/CloseEnough.hpp"
#include "Kyoto/Particles/CElementGen.hpp"
#include "Kyoto/Particles/CParticleElectric.hpp"
#include "Kyoto/Particles/CParticleSpawnSystem.hpp"
#include "Kyoto/Particles/CSortedParticleSystem.hpp"
#include "Kyoto/SObjectTag.hpp"
#include "MetroidPrime/CActorModelParticles.hpp"
#include "MetroidPrime/CControllerRecorder.hpp"
#include "MetroidPrime/CDamageInfo.hpp"
#include "MetroidPrime/CDbgDraw.hpp"
#include "MetroidPrime/CDebugOption.hpp"
#include "MetroidPrime/CDecalManager.hpp"
#include "MetroidPrime/CDisplayManager.hpp"
#include "MetroidPrime/CEnvFxManager.hpp"
#include "MetroidPrime/CFluidPlaneManager.hpp"
#include "MetroidPrime/CGameArea.hpp"
#include "MetroidPrime/CGameDebug.hpp"
#include "MetroidPrime/CMain.hpp"
#include "MetroidPrime/CObjectListSmall.hpp"
#include "MetroidPrime/CRedundantHintManager.hpp"
#include "MetroidPrime/CRenderManager.hpp"
#include "MetroidPrime/CRumbleManager.hpp"
#include "MetroidPrime/CSaveGameInterface.hpp"
#include "MetroidPrime/CScriptMsgUtils.hpp"
#include "MetroidPrime/CStateManagerAssetFactory.hpp"
#include "MetroidPrime/CStateManagerCallbackLists.hpp"
#include "MetroidPrime/CStateManagerCollision.hpp"
#include "MetroidPrime/CStateManagerObject.hpp"
#include "MetroidPrime/CWorld.hpp"
#include "MetroidPrime/CWorldLayerState.hpp"
#include "MetroidPrime/Cameras/CCameraManager.hpp"
#include "MetroidPrime/Cameras/CCinematicCamera.hpp"
#include "MetroidPrime/Cameras/CGameCamera.hpp"
#include "MetroidPrime/ConsoleCommands.hpp"
#include "MetroidPrime/Player/CGameMode.hpp"
#include "MetroidPrime/Player/CGameState.hpp"
#include "MetroidPrime/Player/CPlayer.hpp"
#include "MetroidPrime/Player/CWorldTransManager.hpp"
#include "MetroidPrime/ScriptObjects/CScriptCinematicCamera.hpp"
#include "MetroidPrime/ScriptObjects/CScriptEffect.hpp"
#include "MetroidPrime/ScriptObjects/CScriptSpecialFunction.hpp"
#include "MetroidPrime/TCastTo.hpp"
#include "MetroidPrime/Weapons/WeaponTypes.hpp"
#include "Weapons/CDecal.hpp"
#include "Weapons/CProjectileWeapon.hpp"

#include "rstl/algorithm.hpp"
#include "rstl/math.hpp"
#include "rstl/pair.hpp"
#include "rstl/string.hpp"
#include "rstl/vector.hpp"

#include <float.h>

CStateManager::CStateManager(const rstl::ncrc_ptr< CStringPropertyManager >& stringProperties,
                             const rstl::ncrc_ptr< CScriptMailbox >& mailbox,
                             const rstl::ncrc_ptr< CMapWorldInfo >& mapWorldInfo,
                             const rstl::ncrc_ptr< CWorldTransManager >& worldTransManager,
                             const rstl::ncrc_ptr< CWorldLayerState >& worldLayerState)
: mCallbackLists(rs_new(206) CStateManagerCallbackLists())
, mObjectManager(rs_new(215) CStateManagerObject(*this, stringProperties, mailbox, mapWorldInfo))
, mCollision(rs_new(216) CStateManagerCollision(*this, *mObjectManager))
, mArchQueue(nullptr)
, mDisplayManager(nullptr)
, mRenderManager(nullptr)
, x128_(kInvalidUniqueId)
, mWeaponMgr(rs_new(226) CWeaponMgr())
, mFluidPlaneManager(rs_new(227) CFluidPlaneManager())
, mEnvFxManager(rs_new(228) CEnvFxManager())
, mActorModelParticles(rs_new(229) CActorModelParticles())
, mAssetFactory(rs_new(217) CStateManagerAssetFactory(*this))
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
  mRumbleManager = rs_new(299) CRumbleManager(kIOP_Player1);
  gpGameState->SetQueuedScriptMsgEnabled(true);
  InitializeStateManagerConsoleCommands(this);
  CMemory::SetOutOfMemoryCallback(MemoryAllocatorAllocationFailedCallback, this);
  mShadowTex.Lock();
  sProfileCountersA = rs_new(367) rstl::map< rstl::string, CGameProfileStats::SStats >();
  sProfileCountersB = rs_new(368) rstl::map< rstl::string, SProfileCountersB >();
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

// 0x80295528. Echoes stores the frame and swaps textures out; the prototype also reports
// cinematics whose camera only became active a few frames after it was rendered.
void CStateManager::FrameBegin(int frame) {
  mRenderFrameIndex = frame;
  if (mUpdateFrameIdx != 0) {
    if (mDisplayManager->IsCinematicActive()) {
      if (mCinematicGlitchFrames != 0 && mCinematicGlitchFrames <= 3) {
        const CScriptCinematicCamera* cinematic = TCastToConstPtr< CScriptCinematicCamera >(
            mObjectManager->GetObjectById(mDisplayManager->PlayerCameraManager()
                                              ->GetCinematicCamera()
                                              ->GetCinematicObjectId()));
        if (cinematic != nullptr) {
          gpfnWarningPrintf("BUG THIS: %d frame Cinematic Glitch before camera '%s'!\n",
                            mCinematicGlitchFrames, cinematic->GetName().data());
          rs_debugger_printf("BUG THIS: %d frame Cinematic Glitch before camera '%s'!\n",
                             mCinematicGlitchFrames, cinematic->GetName().data());
        } else {
          gpfnWarningPrintf("BUG THIS: %d frame Cinematic Glitch before unknown camera!\n",
                            mCinematicGlitchFrames);
          rs_debugger_printf("BUG THIS: %d frame Cinematic Glitch before unknown camera!\n",
                             mCinematicGlitchFrames);
        }
      }
      mCinematicGlitchFrames = 0;
    } else {
      ++mCinematicGlitchFrames;
    }
  }
  CTexture::sCurrentFrameCount = mRenderFrameIndex;
  CGraphicsPalette::sCurrentFrameCount = mRenderFrameIndex;
  gpGameDebug->SetFrameIndex(mRenderFrameIndex);
  SwapOutTexturesToARAM(2, 0x180000);
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

// 0x80294588. Unlike Echoes, there is a single player.
void CStateManager::PostUpdatePlayer(float dt) { mObjectManager->Player()->PostUpdate(dt, *this); }

// Guessed names. The update's section timings (0x806BA990): each records the microseconds since
// the update started; "State Manager Numbers" 1 prints each section's delta with its average and
// peak. A null name ends the table.
struct SUpdateSection {
  const char* mName;
  s64 mTime;
  int mPeak;
  int mTotal;
};
enum EUpdateSection {
  kUS_Prethink,
  kUS_SortedLists,
  kUS_Moving,
  kUS_MovePlayer,
  kUS_SortedLists2,
  kUS_TouchLogic,
  kUS_Think,
  kUS_Gamestate,
  kUS_Scripting,
  kUS_UpdateCameras,
  kUS_PostThink,
  kUS_WorldEnvUpdate,
  kUS_Graveyard,
  kUS_PreRender,
};

// Guessed name. The render time CRenderManager.cpp records (0x80799E50); the PreRender section
// adds it to the update's.
extern s64 sRenderTime;

// The mode 4 dump sorts the statistics by time, longest first.
static bool SortStatsByTime(const rstl::pair< rstl::string, CGameProfileStats::SStats >& a,
                            const rstl::pair< rstl::string, CGameProfileStats::SStats >& b) {
  return a.second.mTime > b.second.mTime;
}

// 0x80292E9C. See the frame flow in the header.
void CStateManager::Update(float inputDt, CArchitectureQueue& queue) {
  mArchQueue = &queue;
  float dt = inputDt;
  if (mDisplayManager->IsCinematicActive()) {
    if ((mDisplayManager->GetCinematicCamera()->GetFlags() & 0x100) != 0) {
      dt *= mDisplayManager->GetCinematicCamera()->GetSlowMotionScale();
    }
    if (gpMain->IsMaxSpeed()) {
      // The original also doubles the unscaled time into a value it never uses (0x80292F1C);
      // Echoes passes that time to the camera managers.
      dt *= 2.f;
      if (1.f / 30.f < dt) {
        dt = 1.f / 30.f;
      }
    }
  } else {
    dt = ApplyPhazonEnragedSlowdown(inputDt);
  }
  const bool moving = dt > FLT_EPSILON;

  CStopwatch stopwatch;
  const int numbers = gpGameDebug->GetOptionInt(CGameDebug::kDO_StateManagerNumbers);
  CGameProfileStats stats(numbers, !gpGameDebug->IsOptionSet(CGameDebug::kDO_StateMgrRealNames));
  static SUpdateSection sUpdateSections[] = {
      {"Prethink"},    {"SortedLists"},   {"Moving"},    {"MovePlayer"}, {"SortedLists2"},
      {"Touch logic"}, {"Think"},         {"Gamestate"}, {"Scripting"},  {"Update cameras"},
      {"PostThink"},   {"World/env upd"}, {"Graveyard"}, {"PreRender"},  {nullptr},
  };
  SUpdateSection* const sections = sUpdateSections;
  static int sFrameCount = 0;
  static int sTotalTime = 0;
  static int sPeakTime = 0;

  if (gpGameDebug->IsMovieCaptureRunning() && gpGameDebug->GetMovieCaptureName().length() == 0) {
    CControllerRecorder::SetMovieCaptureName(*this);
  }
  if (gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptingLayersVerbose) == 2) {
    mObjectManager->World()->GetArea(mObjectManager->GetNextAreaId())->DumpScriptLayers(*this);
    gpGameDebug->SetOptionValue(CGameDebug::kDO_ScriptingLayersVerbose, 1.f);
  }

  CElementGen::SetGlobalSeed(mUpdateFrameIdx);
  CParticleElectric::SetGlobalSeed(mUpdateFrameIdx);
  CParticleSpawnSystem::SetGlobalSeed(mUpdateFrameIdx);
  CSortedParticleSystem::SetGlobalSeed(mUpdateFrameIdx);
  CDecal::SetGlobalSeed(mUpdateFrameIdx);
  CProjectileWeapon::SetGlobalSeed(mUpdateFrameIdx);
  mCurTimeMod900 += dt;
  if (mCurTimeMod900 > 900.f) {
    mCurTimeMod900 -= 900.f;
  }
  mPauseHudMessage = kInvalidAssetId;
  mLightAmmoDepletedPlayers = 0;
  mDarkAmmoDepletedPlayers = 0;

  mCallbackLists->FrameStart().Emit(*this, dt);
  CScriptEffect::ResetParticleCounts();
  CheckEntityIndices();
  CObjectList& allObjects = mObjectManager->ObjectListById(0);
  mCallbackLists->PreMapWorldSphere().Emit(*this, dt);
  UpdateMapWorldSphere();
  mCallbackLists->PrePlayTime().Emit(*this, dt);

  const bool playerDead = mObjectManager->GetPlayer()->GetDeathTime() > 0.f;
  if (mGameState == kGS_Running) {
    if (!mDisplayManager->IsInCinematicCamera()) {
      gpGameState->SetTotalPlayTime(dt + gpGameState->GetTotalPlayTime());
      UpdateHintState(dt);
    }
    mCallbackLists->PreRenderClock().Emit(*this, dt);
    mRenderManager->Update(dt);
    mCallbackLists->PrePowerUps().Emit(*this, dt);
    for (int item = 0; item < CPlayerState::kIT_Max; ++item) {
      const CPlayerState::EItemType type = static_cast< CPlayerState::EItemType >(item);
      CPlayerState::CPowerUp& powerUp = gpGameState->GetPlayerState()->PowerUp(type);
      if (powerUp.mTimeLeft > 0.f) {
        powerUp.mTimeLeft -= dt;
        if (powerUp.mTimeLeft < 0.f) {
          powerUp.mTimeLeft = 0.f;
          powerUp.mAmount = 0;
          DisplayAlertAboutOutOfAmmo(*mObjectManager->Player(), type);
        }
      }
    }
  }

  if (mGameState != kGS_Paused) {
    CGameState& gameState = *gpGameState;
    if (gameState.IsQueuedScriptMsgEnabled() &&
        gameState.GetQueuedScriptMsgTarget().value != kInvalidEditorId.value) {
      const EScriptObjectMessage msg =
          static_cast< EScriptObjectMessage >(gameState.GetQueuedScriptMsg());
      const TUniqueId target = mObjectManager->GetIdForScript(gameState.GetQueuedScriptMsgTarget());
      if (target != kInvalidUniqueId && mObjectManager->GetObjectById(target) != nullptr) {
        mObjectManager->SendScriptMsg(
            CScriptMsg(msg, kInvalidUniqueId, target,
                       MakeScriptMsgOriginator(mObjectManager->GetPlayer()), kSS_InvalidState));
        gameState.ClearQueuedScriptMsg();
      }
    }
    mCallbackLists->PrePreThink().Emit(*this, dt);
    if (moving) {
      PreThinkObjects(dt);
    }
    mCallbackLists->PreFluidPlanes().Emit(*this, dt);
    if (moving) {
      mFluidPlaneManager->Update(dt);
    }
  }

  if (mGameState == kGS_Running) {
    if (!playerDead) {
      CDecalManager::Update(dt, *this);
    }
    sections[kUS_Prethink].mTime = stopwatch.GetElapsedMicros();
    mCallbackLists->PreSortedLists().Emit(*this, dt);
    mCollision->UpdateSortedLists();
    mCallbackLists->PostSortedLists().Emit(*this, dt);
    sections[kUS_SortedLists].mTime = stopwatch.GetElapsedMicros();
    mCallbackLists->PreMovePlatforms().Emit(*this, dt);
    if (!playerDead && moving) {
      mCollision->MovePlatforms(dt);
    }
    mCallbackLists->PostMovePlatforms().Emit(*this, dt);
    mCallbackLists->PreMoveActors().Emit(*this, dt);
    if (!playerDead && moving) {
      mCollision->MoveActors(dt);
    }
    mCallbackLists->PostMoveActors().Emit(*this, dt);
    sections[kUS_Moving].mTime = stopwatch.GetElapsedMicros();
  }

  mCallbackLists->PrePlayerInput().Emit(*this, dt);
  ProcessPlayerInput();
  mCallbackLists->PostPlayerInput().Emit(*this, dt);

  if (mGameState == kGS_Running) {
    if (mGameState != kGS_SoftPaused) {
      mCallbackLists->PreMovePlayer().Emit(*this, dt);
      mCollision->MovePlayer(dt);
      mCallbackLists->PostMovePlayer().Emit(*this, dt);
    }
    sections[kUS_MovePlayer].mTime = stopwatch.GetElapsedMicros();
    mCallbackLists->PreSortedLists2().Emit(*this, dt);
    mCollision->UpdateSortedLists();
    mCallbackLists->PostSortedLists2().Emit(*this, dt);
    sections[kUS_SortedLists2].mTime = stopwatch.GetElapsedMicros();
    mCallbackLists->PreTouch().Emit(*this, dt);
    if (!playerDead) {
      CrossTouchActors();
      mObjectManager->DispatchScriptMessages();
    }
    mCallbackLists->PostTouch().Emit(*this, dt);
    sections[kUS_TouchLogic].mTime = stopwatch.GetElapsedMicros();
  } else {
    sections[kUS_Prethink].mTime = stopwatch.GetElapsedMicros();
    sections[kUS_SortedLists].mTime = stopwatch.GetElapsedMicros();
    sections[kUS_Moving].mTime = stopwatch.GetElapsedMicros();
    sections[kUS_MovePlayer].mTime = stopwatch.GetElapsedMicros();
    sections[kUS_SortedLists2].mTime = stopwatch.GetElapsedMicros();
    sections[kUS_TouchLogic].mTime = stopwatch.GetElapsedMicros();
  }

  if (!playerDead && mGameState == kGS_Running) {
    mActorModelParticles->Update(dt, *this);
  }
  if (mGameState == kGS_Running || mGameState == kGS_SoftPaused) {
    mCallbackLists->PreThink().Emit(*this, dt);
    if (moving) {
      Think(dt, stats);
    }
    mCallbackLists->PostThink().Emit(*this, dt);
  }

  mCallbackLists->PreGameplayChecks().Emit(*this, dt);
  if (gpGameDebug->IsOptionSet(CGameDebug::kDO_KillPlayer)) {
    gpGameDebug->SetOptionValue(CGameDebug::kDO_KillPlayer, 0.f);
    CPlayer* player = mObjectManager->Player();
    ApplyLocalDamage(player->GetTranslation(), CVector3f::Zero(), *player, 10000.f,
                     kInvalidUniqueId, player->GetUniqueId(),
                     CDamageInfo(CWeaponMode(kWT_DebugKill), 10000.f, false, false, kInvalidAssetId,
                                 kInvalidAssetId, kInvalidAssetId, 0.f, 0.f),
                     false);
  }
  sections[kUS_Think].mTime = stopwatch.GetElapsedMicros();

  if (mPausedHudMemoFrameCount == mHudMessageFrameCount) {
    ShowPausedHUDMemo(mPausedHudMemoAssetId, mQueuedHudMemoDismissalDelay);
    --mPausedHudMemoFrameCount;
    mPausedHudMemoAssetId = kInvalidAssetId;
  }
  if (!playerDead && mGameState == kGS_Running && !mDisplayManager->IsInCinematicCamera()) {
    UpdateEscapeSequenceTimer(dt);
  }

  mCallbackLists->PreWorldUpdate().Emit(*this, dt);
  mObjectManager->GetWorld()->Update(dt);
  mCallbackLists->PostWorldUpdate().Emit(*this, dt);
  UpdateDynamicLayers();
  mRumbleManager->Update(dt);
  if (!playerDead) {
    mEnvFxManager->Update(dt, *this);
  }
  mCallbackLists->PreAreaSounds().Emit(*this, dt);
  UpdateAreaSounds();
  mCallbackLists->PostAreaSounds().Emit(*this, dt);
  mCallbackLists->PreDocks().Emit(*this, dt);
  mObjectManager->GetWorld()->GetArea(mObjectManager->GetNextAreaId())->UpdateDocks(*this);
  mCallbackLists->PostDocks().Emit(*this, dt);

  if (mInMapScreen) {
    CRedundantHintManager& hintOptions = gpGameState->HintOptions();
    const CRedundantHintManager::SHintState* hint = hintOptions.GetCurrentDisplayedHint();
    if (hint != nullptr && hint->CanContinue()) {
      hintOptions.DismissDisplayedHint();
    }
    mInMapScreen = false;
  }
  mCallbackLists->PreGameMode().Emit(*this, dt);
  const CGameState& gameState = *gpGameState;
  gameState.GetGameMode().Update(dt, *this);
  mCallbackLists->PostGameMode().Emit(*this, dt);
  sections[kUS_Gamestate].mTime = stopwatch.GetElapsedMicros();

  mCallbackLists->PreDispatch().Emit(*this, dt);
  mObjectManager->DispatchScriptMessages();
  mCallbackLists->PostDispatch().Emit(*this, dt);
  sections[kUS_Scripting].mTime = stopwatch.GetElapsedMicros();

  if (mGameState != kGS_SoftPaused) {
    mCallbackLists->PreCameras().Emit(*this, dt);
    mDisplayManager->Update(dt, *this);
    mCallbackLists->PostCameras().Emit(*this, dt);
  }
  sections[kUS_UpdateCameras].mTime = stopwatch.GetElapsedMicros();

  mCallbackLists->PreDispatch2().Emit(*this, dt);
  mObjectManager->DispatchScriptMessages();
  mCallbackLists->PostDispatch2().Emit(*this, dt);
  if (mGameState != kGS_Paused) {
    mCallbackLists->PrePostUpdatePlayer().Emit(*this, dt);
    PostUpdatePlayer(dt);
    mCallbackLists->PostPostUpdatePlayer().Emit(*this, dt);
  }
  sections[kUS_PostThink].mTime = stopwatch.GetElapsedMicros();

  mCallbackLists->PreWorldState().Emit(*this, dt);
  gpGameState->CurrentWorldState().SetAreaId(mObjectManager->GetNextAreaId());
  mCallbackLists->PostWorldState().Emit(*this, dt);
  mCallbackLists->PreTravel().Emit(*this, dt);
  mObjectManager->GetWorld()->TravelToArea(mObjectManager->GetNextAreaId(), *this,
                                           CWorld::kATT_LoadAdjacent);
  mCallbackLists->PostTravel().Emit(*this, dt);
  sections[kUS_WorldEnvUpdate].mTime = stopwatch.GetElapsedMicros();

  mCallbackLists->PreGraveyard().Emit(*this, dt);
  mObjectManager->ClearGraveyard();
  mCallbackLists->PostGraveyard().Emit(*this, dt);
  sections[kUS_Graveyard].mTime = stopwatch.GetElapsedMicros();

  if (mGameState == kGS_Running) {
    gpDbgDraw->Update(dt, *this);
  }
  sections[kUS_PreRender].mTime = sections[kUS_Graveyard].mTime + sRenderTime;

  CDebugOption* numbersOption = gpGameDebug->GetOption(CGameDebug::kDO_StateManagerNumbers);
  numbersOption->ClearMessages();
  if (numbers == 1) {
    ++sFrameCount;
    numbersOption->AddMessage(rstl::string_l("CStateManager update:"));
    int lastTime = 0;
    for (SUpdateSection* section = sUpdateSections; section->mName != nullptr; ++section) {
      const int time = section->mTime;
      const int delta = time - lastTime;
      if (delta > section->mPeak) {
        section->mPeak = delta;
      }
      section->mTotal += delta;
      numbersOption->AddMessage(CBasics::Stringize("  %s - %5d(%5d) [%5d] us", section->mName,
                                                   delta, section->mTotal / sFrameCount,
                                                   section->mPeak));
      lastTime = time;
    }
    sTotalTime += lastTime;
    if (lastTime > sPeakTime) {
      sPeakTime = lastTime;
    }
    numbersOption->AddMessage(CBasics::Stringize("Total Time - %5d(%5d) [%5d] us", lastTime,
                                                 sTotalTime / sFrameCount, sPeakTime));
  } else if (numbers == 5) {
    numbersOption->AddMessage(CBasics::Stringize("Objects:%5d", allObjects.size()));
  } else if (numbers == 8) {
    numbersOption->AddMessage(CBasics::Stringize("Update Frame:%5d", mUpdateFrameIdx));
  }

  const CGameProfileStats::TStatsMap& objectStats = stats.GetStats();
  if (objectStats.size() != 0) {
    if (numbers == 2) {
      for (CGameProfileStats::TStatsMap::const_iterator it = objectStats.begin();
           it != objectStats.end(); ++it) {
        numbersOption->AddMessage(CBasics::Stringize("%s - Num:%3d/%3d - Time:%5d",
                                                     it->first.data(), it->second.x4_,
                                                     it->second.x0_, it->second.mTime));
      }
    } else if (numbers == 4) {
      rstl::vector< rstl::pair< rstl::string, CGameProfileStats::SStats > > sorted;
      sorted.reserve(objectStats.size());
      for (CGameProfileStats::TStatsMap::const_iterator it = objectStats.begin();
           it != objectStats.end(); ++it) {
        sorted.push_back(*it);
      }
      rstl::sort(sorted.begin(), sorted.end(), SortStatsByTime);
      for (rstl::vector< rstl::pair< rstl::string, CGameProfileStats::SStats > >::iterator it =
               sorted.begin();
           it != sorted.end(); ++it) {
        numbersOption->AddMessage(CBasics::Stringize("%s - Num:%3d/%3d - Time:%5d",
                                                     it->first.data(), it->second.x4_,
                                                     it->second.x0_, it->second.mTime));
      }
    }
  }

  ReportLowMemory();
  if (mLogEndOfFrame) {
    mLogEndOfFrame = false;
    rs_debugger_printf("---------- END OF FRAME ----------\n");
  }
  ++mUpdateFrameIdx;
  mArchQueue = nullptr;
}

// 0x80292A90. Unlike Echoes, it only tells the item depletion objects; there is no HUD memo, and
// the player is unused.
void CStateManager::DisplayAlertAboutOutOfAmmo(const CPlayer& player,
                                               CPlayerState::EItemType type) {
  CObjectList* allList = &mObjectManager->ObjectListById(0);
  for (int i = allList->GetFirstObjectIndex(); i != -1; i = allList->GetNextObjectIndex(i)) {
    CScriptSpecialFunction* const special = TCastToPtr< CScriptSpecialFunction >((*allList)[i]);
    if (special != nullptr && special->GetFunction() == CScriptSpecialFunction::kSF_ItemDepletion) {
      special->OnItemDepleted(*this, type);
    }
  }
}

// 0x80290038. Unlike Echoes, there is a single player, and the game state hands out its player
// state.
void CStateManager::UpdateEscapeSequenceTimer(float dt) {
  if (close_enough(mEscapeTotalTime, 0.f)) {
    mEscapeTotalTime = gpGameState->GetEscapeTime();
  }
  const float totalTime = mEscapeTotalTime;
  if (gpGameState->GetEscapeTime() > 0.f) {
    gpGameState->SetEscapeTime(rstl::max_val(FLT_EPSILON, gpGameState->GetEscapeTime() - dt));
    const CGameState& gameState = *gpGameState;
    if (gpGameState->GetEscapeTime() <= FLT_EPSILON &&
        gameState.GetPlayerState()->IsPlayerAlive()) {
      KillPlayer(0.f, mObjectManager->Player()->GetUniqueId(), kInvalidUniqueId);
    }

    static float sNextEscapeRumble = 0.f;
    sNextEscapeRumble -= dt;
    if (sNextEscapeRumble < 0.f) {
      const float factor = 1.f - gpGameState->GetEscapeTime() / totalTime;
      mRumbleManager->Rumble(*this, kRFX_PlayerBump, 0.75f, kRP_One);
      sNextEscapeRumble = -12.f * (factor * factor) + 15.f;
    }
  }
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
