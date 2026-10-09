#ifndef _CGAMESTATE
#define _CGAMESTATE

#include "types.h"

#include "Kyoto/CToken.hpp"
#include "MetroidPrime/CRedundantHintManager.hpp"
#include "MetroidPrime/Player/CGameOptions.hpp"
#include "MetroidPrime/Player/CPersistentOptions.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/rc_ptr.hpp"
#include "rstl/reserved_vector.hpp"
#include "rstl/vector.hpp"

class CBitStreamReader;
class CGameMode;
class CPlayerState;
class CWorldTransManager;
class CBitStreamWriter;

// Minimal view of Echoes' per-world state (CGameState.cpp). Echoes' name.
class CWorldState {
public:
  void SetAreaId(TAreaId areaId); // 0x8015D190; stores it at 0x8
};

// Minimal view of the prototype's CGameState for main.cpp; only the members main touches are
// placed, the rest is padding. Nested types and accessor names follow Echoes.
class CGameState {
public:
  struct SPlayerResult {
    SPlayerResult()
    : mPlayerSelection(0), mScore(0), mDeaths(0), xc_(false), mRumbleEnabled(false) {}

    uint mPlayerSelection;
    int mScore;
    int mDeaths;
    bool xc_;
    bool mRumbleEnabled;
  };

  struct SPreviousGameResults {
    SPreviousGameResults()
    : mGameMode(0), mShowResults(false), x8_(0), mPlayerCount(0), mPlayers(4, SPlayerResult()) {}
    explicit SPreviousGameResults(CBitStreamReader& in);
    void PutTo(CBitStreamWriter& out) const;

    uint mGameMode;
    bool mShowResults;
    int x8_;
    int mPlayerCount;
    rstl::reserved_vector< SPlayerResult, 4 > mPlayers;
  };

  CGameOptions& GameOptions() { return mGameOptions; }
  CPersistentOptions& SystemOptions() { return mSystemOptions; } // Echoes' name
  void InitializeMemoryStates(); // Echoes' name; 0x8015BAD8
  // Echoes' name. CStateManager hands it to the in-game save screen.
  u64 GetCardSerial() const { return mCardSerial; }
  // Echoes' names. CStateManager's update adds the frame time to the play time while running;
  // the setter (0x80159C50) clamps it.
  double GetTotalPlayTime() const { return mTotalPlayTime; }
  void SetTotalPlayTime(double time);
  // Echoes' names. CStateManager counts the escape sequence down through them.
  float GetEscapeTime() const { return mEscapeTime; }
  void SetEscapeTime(float time); // 0x80159C48
  // Echoes' name. 0x80159D20 looks up the state of the world id at 0x0.
  CWorldState& CurrentWorldState();
  // Echoes' name for its CHintOptions, which this class replaces.
  CRedundantHintManager& HintOptions() { return mHintOptions; }
  SPreviousGameResults& PreviousGameResults() { return mPreviousGameResults; } // Guessed name
  rstl::vector< CToken >& AudioGroups() { return mAudioGroups; }               // Guessed name
  // Out of line (0x80159C7C); returns the rc_ptr at 0x28. Echoes' name.
  rstl::rc_ptr< CWorldTransManager >& WorldTransitionManager();
  // Echoes names. The hard mode flag is the first bit of the byte at 0x308.
  bool GetHardModeEnabled() const { return mHardMode; }
  void SetHardMode(bool hardMode); // 0x80159C38
  class CPlayerState* GetPlayerState();
  // Its identical twin just before it (0x80159C84). KillPlayer (0x80292440) clears the alive bit
  // through 0x80159C8C, so that one is mutable; CScriptSpecialFunction (0x80110DB4) only passes
  // 0x80159C84's result to the const GetItemAmount, and the escape timer only reads it.
  const class CPlayerState* GetPlayerState() const;
  // Echoes' names. Both return the game mode owned through the rstl::auto_ptr at 0x198. Only the
  // first is called so far (CStateManager's constructor, CGameDebug); which twin is const is
  // inferred from Echoes emitting the mutable one first.
  CGameMode& GetGameMode();                                                           // 0x80159BB0
  CGameMode& GetGameMode() const;                                                     // 0x80159BB8
  void SetQueuedScriptMsgEnabled(bool enabled) { mQueuedScriptMsgEnabled = enabled; } // Guessed
  // Guessed names, after the fields below.
  bool IsQueuedScriptMsgEnabled() const { return mQueuedScriptMsgEnabled; }
  TEditorId GetQueuedScriptMsgTarget() const { return mQueuedScriptMsgTarget; }
  int GetQueuedScriptMsg() const { return mQueuedScriptMsg; }
  void ClearQueuedScriptMsg() {
    mQueuedScriptMsg = -1;
    mQueuedScriptMsgTarget = kInvalidEditorId;
  }

private:
  uchar x0_[0x30];
  double mTotalPlayTime; // Echoes' name
  float mEscapeTime;     // Echoes' name
  CPersistentOptions mSystemOptions; // Echoes' name
  CGameOptions mGameOptions;
  CRedundantHintManager mHintOptions;
  uchar xd8_[0x108 - 0xd8];
  u64 mCardSerial; // Echoes' name
  uchar x110_[0x1a0 - 0x110];
  SPreviousGameResults mPreviousGameResults;
  rstl::vector< CToken > mAudioGroups;
  uchar x204_[0x104];
  bool mHardMode : 1;
  // Guessed names. A script message queued for the object with an editor id: a console command
  // (0x80209600) stores both and clears the flag, CStateManager's constructor sets the flag, and
  // the update sends the message while the flag is set, then resets both to invalid.
  TEditorId mQueuedScriptMsgTarget;
  int mQueuedScriptMsg;         // An EScriptObjectMessage
  bool mQueuedScriptMsgEnabled; // 0x314
};

extern CGameState* gpGameState;

#endif // _CGAMESTATE
