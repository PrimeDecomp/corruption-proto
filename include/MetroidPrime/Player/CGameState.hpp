#ifndef _CGAMESTATE
#define _CGAMESTATE

#include "types.h"

#include "Kyoto/CToken.hpp"
#include "MetroidPrime/Player/CGameOptions.hpp"

#include "rstl/reserved_vector.hpp"
#include "rstl/vector.hpp"

class CBitStreamReader;
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

private:
  uchar x0_[0x68];
  CGameOptions mGameOptions;
  uchar xc4_[0xdc];
  SPreviousGameResults mPreviousGameResults;
  rstl::vector< CToken > mAudioGroups;
};

extern CGameState* gpGameState;

#endif // _CGAMESTATE
