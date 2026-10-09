#ifndef _CPLAYERGUIMANAGER
#define _CPLAYERGUIMANAGER

#include "types.h"

#include "Kyoto/CAssetId.hpp"

class CAutoMapper;
class CStateManager;

// Echoes' values (CInGameGuiManagerCommon.hpp).
enum EInGameGuiState {
  kIGGS_Zero,
  kIGGS_InGame,
  kIGGS_MapScreen,
  kIGGS_PauseGame,
  kIGGS_PauseLogBook,
  kIGGS_PauseSaveGame,
  kIGGS_PauseHUDMessage,
  kIGGS_QuitGame, // Echoes' guessed name
};

// Minimal declaration. The per-player GUI manager (CPlayerGuiManager.cpp, 0x8025BC48..0x8025FC44;
// Echoes' CInGameGuiManager), which CInGameGuiManager keeps at 0x2C. Only what CMFGame uses.
class CPlayerGuiManager {
public:
  // Echoes' names and signatures, in Echoes' order.
  bool IsInPausedState() const; // 0x8025C8D0
  // 0x8025C930: stores the message and its time at 0x48 and 0x50, then pauses with
  // kIGGS_PauseHUDMessage.
  void ShowPauseGameHudMessage(const CStateManager& mgr, CAssetId message, float time);
  void PauseGame(const CStateManager& mgr, EInGameGuiState state); // 0x8025C968
  CAutoMapper& GetAutoMapper() { return *mAutoMapper; }

private:
  uchar x0_[0x30];
  CAutoMapper* mAutoMapper;
};

#endif // _CPLAYERGUIMANAGER
