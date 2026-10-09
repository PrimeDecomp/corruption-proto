#ifndef _CSTATEMANAGER
#define _CSTATEMANAGER

#include "types.h"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/CAssetId.hpp"
#include "Kyoto/CRandom16.hpp"
#include "Kyoto/Input/CFinalInput.hpp"
#include "Kyoto/TOneStatic.hpp"
#include "Kyoto/TSignal2.hpp"
#include "Kyoto/TSignal3.hpp"
#include "Kyoto/TToken.hpp"
#include "MetroidPrime/CGameProfileStats.hpp"
#include "MetroidPrime/CWeaponMgr.hpp"
#include "MetroidPrime/Player/CPlayerState.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/map.hpp"
#include "rstl/rc_ptr.hpp"
#include "rstl/single_ptr.hpp"
#include "rstl/string.hpp"

class CActor;
class CActorModelParticles;
class CArchitectureQueue;
class CDependencyGroup;
class CDisplayManager;
class CEnvFxManager;
class CFluidPlaneManager;
class CDamageInfo;
class CMapWorldInfo;
class CPlayer;
class CRenderManager;
class CRumbleManager;
class CSaveGameInterface;
class CScriptMailbox;
class CStringPropertyManager;
class CStateManagerAssetFactory;
class CStateManagerCallbackLists;
class CStateManagerCollision;
class CStateManagerObject;
class CTexture;
class CVector3f;
class CUserEvaluatorDescription;
class CWorldLayerState;
class CWorldTransManager;

// The size comes from CMFGameLoader, which allocates the manager through
// TOneStatic<CStateManager>'s operator new (0x8021A380) and asserts a 0x220-byte limit there.
//
// Unlike Echoes' monolithic class (0x2950 bytes), Corruption's manager is a 0x220-byte hub
// whose state lives in separately allocated objects:
// - CStateManagerObject (0x4): the entity database. Object lists, unique ids, the script message
//   queue and its delivery, the world, the graveyard, the editor-id map, the mailbox, the
//   map-world info and the player. Callers across the DOL reach it through the pointer and its
//   out-of-line getters instead of inline CStateManager accessors.
// - CStateManagerCallbackLists (0x0): 63 signals. Instead of calling each subsystem in turn, the
//   update emits 46 per-phase signals with the frame time, CRenderManager emits two groups of
//   render signals, and CStateManagerObject fires the entity added/removed/active signals that
//   CGameArea, CSortedLists and CStateManagerCollision listen to (Echoes updated those
//   directly). The destructor emits the 0x5B8 signal first and the 0x5D0 signal last.
// - CStateManagerCollision (0x8): the sorted lists and the movement passes (Echoes'
//   UpdateSortedLists, MovePlatforms, MoveActors and the player move), by what its update
//   calls do.
// - CDisplayManager (0x14): wraps the player camera manager; the update asks it about the
//   cinematic camera and has it update the cameras.
// - CRenderManager (0x18): the render passes and their signals.
// The rest of the members keep Echoes' order: the rumble manager, the per-frame input, the
// weapon, fluid, EnvFx and actor-model-particle managers, the audio-group token, the
// world-transition manager, the save-game screen, the frame counter, the shadow token, the
// random generator, the game state, the hint, boss and HUD-memo state and the flags.
//
// Frame flow (Update, 0x80292E9C; called by CMFGame with the frame time and the architecture
// queue, kept at 0xC for the duration of the call):
// 1. The frame time is scaled: by the cinematic camera's slow-motion factor and doubled
//    (capped at 1/30 s) while the game runs at max speed, or else by the "PhazonEnraged
//    slowdown" curve (0x8028F448). "dt > FLT_EPSILON" gates the moving and thinking passes,
//    and a dead player (death time > 0) skips the decals, the moving passes, touch, the actor
//    model particles and EnvFx, as in Echoes.
// 2. A CStopwatch is started and a CGameProfileStats is built from the "State Manager Numbers"
//    debug option; the particle, decal and projectile seeds are set from the frame counter.
// 3. The update then walks through its phases, emitting the CStateManagerCallbackLists signals
//    (0x0..0x438) around the work the state manager still does itself. Most phases have a
//    "before" and an "after" signal; the frame start, map world sphere, play time, render clock,
//    power-ups, PreThink, fluid planes and gameplay checks only have a "before" one. The work, in
//    order: entity index check (debug), map
//    world sphere, play time, hints, power-up timers and the render clock (running only); the
//    script message the game state queued; PreThink (and the fluid planes); decals, sorted
//    lists, platforms and actors (running only); player input; the player move, sorted lists
//    again, touch and the first message dispatch (running only); the actor model particles;
//    Think; the "Kill Player" debug option; the queued HUD memo; the escape timer; the world,
//    dynamic layers, rumble and EnvFx; area sounds; docks; the map-screen hint; the game mode;
//    message dispatch; cameras; message dispatch; PostThink; the world state's area; travel to
//    the next area; the graveyard.
// 4. After the groups of phases the stopwatch's time is written into a static table of 14
//    sections (Prethink, SortedLists, Moving, MovePlayer, SortedLists2, Touch logic, Think,
//    Gamestate, Scripting, Update cameras, PostThink, World/env upd, Graveyard; PreRender adds
//    CRenderManager's render time). Phases that are skipped still record their section.
// 5. "State Manager Numbers" then prints into its own option: 1 lists every section with its
//    delta, average and peak, 5 the object count, 8 the frame counter, 2 and 4 the per-object
//    think statistics (4 sorted by time). The low-memory report (0x80294748) follows, then the
//    "END OF FRAME" marker when requested; the frame counter is bumped and the queue cleared.
class CStateManager : public TOneStatic< CStateManager > {
public:
  // Echoes' values; 0x174 is compared against them all through the update.
  enum EGameState {
    kGS_Running,
    kGS_SoftPaused,
    kGS_Paused,
  };

  CStateManagerObject& ObjectManager() { return *mObjectManager; } // Guessed name
  const CStateManagerObject& ObjectManager() const { return *mObjectManager; }
  CStateManagerCallbackLists& CallbackLists() { return *mCallbackLists; } // Guessed name
  CRenderManager* RenderManager() { return mRenderManager.get(); }        // Guessed name
  // Guessed name. The readers reach the display manager through it, so they get its const
  // camera-manager getter (0x802A34D0); FrameBegin does.
  const CDisplayManager& GetDisplayManager() const { return *mDisplayManager; }
  // CScriptLUA's RandomRange (0x802B5C24) inlines this warning before using the generator.
  CRandom16* Random() {
    if (!mRandomAvailable) {
      gpfnWarningPrintf("BUG THIS! Random() called when not deterministic!\n");
      rs_debugger_printf("BUG THIS! Random() called when not deterministic!\n");
    }
    return &mRandom;
  }
  bool IsRandomAvailable() const { return mRandomAvailable; }
  // Echoes' name; SpecialSkipCinematic inlines it.
  void SetSkipCinematicSpecialFunction(TUniqueId id) { mSpecialFunctionId = id; }
  // Prime's name. The update (0x80292E9C) seeds CDecal and CProjectileWeapon with it and bumps
  // it at the end; the script message logs print it.
  uint GetUpdateFrameIndex() const { return mUpdateFrameIdx; }

  // 0x80296F90. CMFGameLoader (0x80219CF8) builds it. The first three arguments go to
  // CStateManagerObject; the last two are kept here. Like Echoes, it builds the managers it owns,
  // the shadow token and the generator, then installs the out-of-memory callback; unlike Echoes,
  // it also prints the game type, news the rumble manager, enables the game state's queued script
  // message, hands itself to the console commands and news the two profile-counter tables.
  CStateManager(const rstl::ncrc_ptr< CStringPropertyManager >& stringProperties,
                const rstl::ncrc_ptr< CScriptMailbox >& mailbox,
                const rstl::ncrc_ptr< CMapWorldInfo >& mapWorldInfo,
                const rstl::ncrc_ptr< CWorldTransManager >& worldTransManager,
                const rstl::ncrc_ptr< CWorldLayerState >& worldLayerState);
  // 0x80296220. Emits the 0x5B8 signal, sets mTearingDown, stops the rumble, cleans up EnvFx,
  // deletes every object (players and cameras last), frees the profile-stat tables and emits the
  // 0x5D0 signal before the members go.
  ~CStateManager();

  // Echoes' name. CMFGame calls it with the frame time and its queue; see the frame flow above.
  void Update(float dt, CArchitectureQueue& queue);
  // Echoes' name. CMFGame passes the frame number of its kAM_FrameBegin message.
  void FrameBegin(int frame);

  // Echoes' names, in Echoes' order. The constructor installs the callback with CMemory; unlike
  // Echoes, it first reports the last and current areas.
  void SwapOutTexturesToARAM(int, uint);
  static const bool MemoryAllocatorAllocationFailedCallback(const void* context, uint size);
  bool SwapOutAllPossibleMemory();

  // Echoes' names and signatures; they write the same fields.
  void SetBossParams(TUniqueId bossId, float maxEnergy, uint stringIdx);
  void QueueMessage(int frameCount, CAssetId msg, float f1);
  // Echoes' names. Unlike Echoes' (uid, type), they take the owner, whose count goes up or down,
  // and the weapon itself, whose id the signals pass on. CGameProjectile, CEnergyProjectile,
  // CPlasmaProjectile and CBomb call them with their owner id, their own id and their weapon
  // type.
  void RemoveWeaponId(TUniqueId owner, TUniqueId weapon, EWeaponType type);
  void AddWeaponId(TUniqueId owner, TUniqueId weapon, EWeaponType type);
  // Guessed name. Restarts the "PhazonEnragedSlowdownUSER" time curve.
  void StartPhazonEnragedSlowdown();

  // Echoes' name and signature. Like Echoes, it pauses the world's loading during the soft pause
  // and turns the rumble off; the CAudioManager voice context replaces Echoes' sfx channel.
  void SetGameState(EGameState state);
  // Echoes' name. CMFGame calls it; 1 when the cinematic was skipped through the camera manager,
  // 2 when the special function handled it, 0 without one.
  int SpecialSkipCinematic();

  // Echoes' names and signatures, unless noted.
  void ShowPausedHUDMemo(CAssetId strg, float time);
  void UpdateEscapeSequenceTimer(float dt);
  // Guessed name, as in Echoes. 0x80292440 clears the victim's alive flag and tells the game mode.
  void KillPlayer(float previousHealth, TUniqueId victim, TUniqueId killer);
  void UpdateHintState(float dt);
  void UpdateDynamicLayers();
  void UpdateAreaSounds();
  void ProcessPlayerInput();
  void PreThinkObjects(float dt);
  // Unlike Echoes, it also takes the update's statistics.
  void Think(float dt, CGameProfileStats& stats);
  void PostUpdatePlayer(float dt);
  void CrossTouchActors();
  void DisplayAlertAboutOutOfAmmo(const CPlayer& player, CPlayerState::EItemType type);
  bool ApplyLocalDamage(const CVector3f& pos, const CVector3f& dir, CActor& damagee, float damage,
                        TUniqueId source, TUniqueId owner, const CDamageInfo& damageInfo,
                        bool radiusDamage);

  // Two name-keyed tables of counters, kept in .sbss (0x80799E5C, 0x80799E60).
  // CGameProfileStats.cpp prints them ("%s-%3d/%3d-%5d") and adds to them, CRenderManager.cpp
  // reads the first; the constructor news both (lines 367 and 368) and the destructor deletes
  // them. They are two map instantiations (separate node-freeing instances at 0x80297CE8 and
  // 0x80297D68), so their value types differ. The first shares its destructor (0x80294514) with
  // CGameProfileStats' map, so it holds the same statistics; what the second counts is not
  // known. All names guessed, and so is their owner.
  struct SProfileCountersB {
    int x0_;
    int x4_;
    int x8_;
  };
  static rstl::map< rstl::string, CGameProfileStats::SStats >* sProfileCountersA;
  static rstl::map< rstl::string, SProfileCountersB >* sProfileCountersB;

private:
  // Guessed names. The update's debug passes: the entity index check ("ENTITY INDEX MISMATCH",
  // 0x802951A8), the map world sphere (0x8029530C) and the low-memory report (0x80294748).
  void CheckEntityIndices();
  void UpdateMapWorldSphere();
  void ReportLowMemory();
  // Guessed name. 0x8028F448 plays the "PhazonEnragedSlowdownUSER" curve while it runs and
  // returns the scaled frame time.
  float ApplyPhazonEnragedSlowdown(float dt);

  // The constructor news these (0x5E8, 0x1138 and 0x1C038 bytes); their constructors live in
  // CStateManagerCallbackLists.cpp, CStateManagerObject.cpp and CStateManagerCollision.cpp, and
  // the class names are guessed from those files. The destructor deletes them last.
  rstl::single_ptr< CStateManagerCallbackLists > mCallbackLists; // Guessed name
  rstl::single_ptr< CStateManagerObject > mObjectManager;        // Guessed name
  rstl::single_ptr< CStateManagerCollision > mCollision;         // Guessed name
  CArchitectureQueue* mArchQueue; // Echoes' name; only valid during Update
  int x10_;
  // Guessed names. Both are created with the player (0x802960B0); CRenderManager's constructor
  // (0x802AA5F4, 0x780 bytes) keeps the state manager at +4.
  rstl::single_ptr< CDisplayManager > mDisplayManager;
  rstl::single_ptr< CRenderManager > mRenderManager;
  // Echoes keeps one per player. The constructor news it (0x48 bytes) but the destructor only
  // stops it and never deletes it.
  CRumbleManager* mRumbleManager;
  // Echoes keeps the frame's input here too. The prototype's CFinalInput is 0x108 bytes (its
  // default constructor, 0x8051A8AC, builds arrays up to +0x6C); the header still describes
  // Prime's 0x2C-byte layout, so the rest is padding.
  CFinalInput mFinalInput;
  uchar x4C_[0x108 - sizeof(CFinalInput)];
  TUniqueId x128_; // Initialized to kInvalidUniqueId
  // Echoes' names. Echoes keeps these in one CStateManagerContainer; here each one is newed
  // (0x14, 0x11C, 0x1468 and 0x140 bytes, CStateManager.cpp lines 226..229).
  rstl::single_ptr< CWeaponMgr > mWeaponMgr;
  rstl::single_ptr< CFluidPlaneManager > mFluidPlaneManager;
  rstl::single_ptr< CEnvFxManager > mEnvFxManager;
  rstl::single_ptr< CActorModelParticles > mActorModelParticles;
  // Guessed name, after CStateManagerAssetFactory.cpp. 0x40 bytes, polymorphic, built with the
  // state manager (line 217).
  rstl::single_ptr< CStateManagerAssetFactory > mAssetFactory;
  TToken< CDependencyGroup > mAudioGroupDependencies; // Echoes' name; built empty
  // Echoes' names; copies of the last two constructor arguments. The second's implicit
  // destructor (0x8009726C) has the layout of Echoes' CWorldLayerState.
  rstl::ncrc_ptr< CWorldTransManager > mWorldTransManager;
  rstl::ncrc_ptr< CWorldLayerState > mCurrentWorldLayerState;
  // Echoes' member name. The class is Echoes' CSaveGameScreen, but its destructor (0x801A26F4)
  // sits in CSaveGameInterface.cpp, so the class name is guessed after that file.
  rstl::single_ptr< CSaveGameInterface > mSaveGameScreen;
  uint mUpdateFrameIdx; // Prime's name
  // Echoes' names. As in Echoes, the constructor builds the "DefaultShadow" token, then seeds
  // the generator with 0 and clears the flag.
  TCachedToken< CTexture > mShadowTex;
  CRandom16 mRandom;
  bool mRandomAvailable : 1;
  // Echoes' names, initialized in Echoes' order: the game state, the init phase, the hint
  // index and periods, the pause HUD message, the escape and time-mod-900 timers, the boss id,
  // health and string, the skip-cinematic special function and the player actor head.
  EGameState mGameState;
  int mInitPhase;
  int mHintIdx;
  uint mHintPeriods;
  CAssetId mPauseHudMessage;
  float mEscapeTotalTime;
  float mCurTimeMod900;
  TUniqueId mBossId;
  float mBossHealth;
  uint mBossLanguageTableIndex;
  TUniqueId mSpecialFunctionId;
  TUniqueId mPlayerActorHead;
  float mHudMessageTime;
  // Echoes' name. FrameBegin (0x80295528) stores its argument here and in the texture and
  // palette frame counters. It sits where Echoes kept its shadow list.
  int mRenderFrameIndex;
  // Guessed name. FrameBegin counts the frames rendered while the cinematic camera is not yet
  // active and warns ("BUG THIS: %d frame Cinematic Glitch before camera '%s'!") when the
  // cinematic starts after one to three of them.
  uint mCinematicGlitchFrames;
  // Echoes' names. The update shows the queued memo when the two frame counts meet.
  uint mHudMessageFrameCount; // Unsigned here, unlike Echoes: the update compares them unsigned
  uint mPausedHudMemoFrameCount;
  CAssetId mPausedHudMemoAssetId;
  float mQueuedHudMemoDismissalDelay;
  CAssetId mMapTeleportWorldId; // Echoes' name
  int mDeferredTransition;      // Echoes' name
  uchar mPlayerLineOfSightPairs;
  uchar mNextPlayerLineOfSightPair;
  // Guessed names. Two signals that projectiles and bombs fire through the state manager.
  // AddWeaponId passes the state manager, the new weapon's id and its type after counting it in;
  // RemoveWeaponId passes the state manager and the removed weapon's id.
  TSignal3< CStateManager&, TUniqueId, EWeaponType > mWeaponAdded;
  TSignal2< CStateManager&, TUniqueId > mWeaponRemoved;
  // The constructor clears them all except x210_25. Echoes has three more flags before its
  // map-screen flag; this order is the prototype's.
  bool x210_24_ : 1; // CMFGame checks it after the update
  bool x210_25_ : 1;
  bool mInMapScreen : 1;   // Echoes' name; the update dismisses the displayed hint and clears it
  bool x210_27_ : 1;       // Set when the save-game screen is deleted (0x8028F834)
  bool mLogEndOfFrame : 1; // Guessed name. The update prints "END OF FRAME" and clears it.
  bool x210_29_ : 1;
  bool x210_30_ : 1;
  bool mTearingDown : 1; // Echoes' name; set first in the destructor
  bool x211_24_ : 1;
  uint mLightAmmoDepletedPlayers : 4; // Echoes' names; the update clears both
  uint mDarkAmmoDepletedPlayers : 4;
  // Guessed names. The "PhazonEnragedSlowdownUSER" CUserEvaluator curve, its play time and
  // whether it runs; while it does, it scales the frame time outside cinematics.
  bool mPhazonEnragedSlowdown : 1;
  float mPhazonEnragedSlowdownTime;
  TToken< CUserEvaluatorDescription > mPhazonEnragedSlowdownCurve;
};
CHECK_SIZEOF(CStateManager, 0x220)

#endif // _CSTATEMANAGER
