#ifndef _CGAMESTATE
#define _CGAMESTATE

#include "types.h"

#include "Kyoto/CToken.hpp"
#include "MetroidPrime/Player/CGameOptions.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/rc_ptr.hpp"
#include "rstl/reserved_vector.hpp"
#include "rstl/vector.hpp"

class CBitStreamReader;
class CGameMode;
class CWorldTransManager;
class CBitStreamWriter;

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
  SPreviousGameResults& PreviousGameResults() { return mPreviousGameResults; } // Guessed name
  rstl::vector< CToken >& AudioGroups() { return mAudioGroups; }               // Guessed name
  // Out of line (0x80159C7C); returns the rc_ptr at 0x28. Echoes' name.
  rstl::rc_ptr< CWorldTransManager >& WorldTransitionManager();
  // Echoes names. The hard mode flag is the first bit of the byte at 0x308.
  bool GetHardModeEnabled() const { return mHardMode; }
  void SetHardMode(bool hardMode); // 0x80159C38
  class CPlayerState* GetPlayerState();
  // Echoes' names. Both return the game mode owned through the rstl::auto_ptr at 0x198. Only the
  // first is called so far (CStateManager's constructor, CGameDebug); which twin is const is
  // inferred from Echoes emitting the mutable one first.
  CGameMode& GetGameMode();                                                           // 0x80159BB0
  CGameMode& GetGameMode() const;                                                     // 0x80159BB8
  void SetQueuedScriptMsgEnabled(bool enabled) { mQueuedScriptMsgEnabled = enabled; } // Guessed

private:
  uchar x0_[0x68];
  CGameOptions mGameOptions;
  uchar xc4_[0xdc];
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
