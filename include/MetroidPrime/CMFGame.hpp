#ifndef _CMFGAME
#define _CMFGAME

#include "types.h"

#include "MetroidPrime/CControlMapper.hpp"
#include "MetroidPrime/CIOWin.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/rc_ptr.hpp"

class CFinalInput;
class CInGameGuiManager;
class CStateManager;

// The in-game IOWin (CMFGame.cpp, 0x80023EC8..0x800255B8). CMFGameLoader builds it once the
// state manager and the GUI are loaded (0x80219F18). Names follow Echoes' CMFGame; the prototype
// has no multiplayer end screens and no portal transitions, and adds the debug menu, the debug
// camera toggle and a frame-skipping restart after a layer change.
class CMFGame : public CIOWin {
public:
  // Echoes' names for the states it shares. Echoes numbers the cinematic skip 6.
  enum EFlowState {
    kFS_Zero,
    kFS_InGame,
    kFS_Paused,
    kFS_PlayerDied,
    kFS_CinematicSkip,
    kFS_LayerRestart, // Echoes' kFS_State8
  };

  CMFGame(const rstl::ncrc_ptr< CStateManager >& stateManager,
          const rstl::ncrc_ptr< CInGameGuiManager >& guiManager);

  // CIOWin
  ~CMFGame();
  EMessageReturn OnMessage(const CArchitectureMessage& msg, CArchitectureQueue& queue);
  void Draw() const;

  // Echoes' name. Sets the flag that makes the next in-game input quit the gameplay; the console's
  // TITLESCREEN calls it too.
  static void ActivateMultiplayerGui();
  void EnterMapScreen();
  void PauseGame();
  void EnterLogBook();
  void SaveGame();
  void EnterPauseScreenState5();
  void EnterMessageScreen(float time);
  void UnpauseGame();
  void PlayerDied();
  bool IsCameraActiveFlow() const;

private:
  void SetFlowState(EFlowState state);
  // Echoes' name; empty here.
  void RecordMultiplayerResults() const;
  void EndGame(CArchitectureQueue& queue);
  void DrawWorld(bool singleViewport) const;
  void DrawGui(bool singleViewport) const;
  // Guessed name. Reads the "AI Trace" page's View Mode option and returns false either way;
  // whatever it did with the input was compiled out.
  bool ProcessAITraceInput(const CFinalInput& input);
  // Guessed name. Handles what the debug menu's update and input return: 1 removes every IOWin,
  // 2 quits the gameplay and 3 opens the quit-game screen outside the single-player and FRND
  // modes and cinematics.
  void ProcessDebugMenuCommand(int command, CArchitectureQueue& queue, int controller);

  static bool mMultiplayerGuiActive;

  rstl::ncrc_ptr< CStateManager > mStateManager;
  rstl::ncrc_ptr< CInGameGuiManager > mGuiManager;
  EFlowState mFlowState;
  float mFlowTime;
  TUniqueId mSkippedCineCam;
  bool mInitialized : 1;
  bool mPlayerAlive : 1;
  bool x30_26_ : 1; // Only cleared by the constructor
  // Set when the cinematic skip starts and cleared by the next in-game input that does not start
  // one.
  bool x30_27_ : 1;
  // Its start command opens the debug menu and skips cinematics.
  CControlMapper mControlMapper;
};
CHECK_SIZEOF(CMFGame, 0x138)

#endif // _CMFGAME
