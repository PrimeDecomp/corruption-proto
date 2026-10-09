#ifndef _CINGAMEGUIMANAGER
#define _CINGAMEGUIMANAGER

#include "types.h"

#include "MetroidPrime/CPlayerGuiManager.hpp"

class CArchitectureQueue;
class CFinalInput;
class CStateManager;

// Minimal declaration. The in-game GUI that CMFGame owns (Echoes' CInGameGuiManagerSet). The
// class name is guessed after CInGameGuiManager.cpp (0x800E66CC..0x800E7708), which holds these
// methods and the destructor; the per-player manager it wraps sits in CPlayerGuiManager.cpp. The
// methods keep Echoes' names and order; most forward to the player GUI manager at 0x2C.
class CInGameGuiManager {
public:
  ~CInGameGuiManager(); // 0x800E6DCC

  bool GetIsGameDraw() const; // 0x800E66CC
  // 0x800E66F0. Only the first controller's input reaches the player GUI manager.
  void ProcessControllerInput(const CStateManager& mgr, const CFinalInput& input,
                              CArchitectureQueue& queue);
  void Update(const CStateManager& mgr, float dt, CArchitectureQueue& queue,
              bool cameraActive);                                    // 0x800E6720
  void PauseGame(const CStateManager& mgr, EInGameGuiState state);   // 0x800E6768
  void PreDraw(CStateManager& mgr, bool cameraActive);               // 0x800E678C
  void PrepareScanDisplay(const CStateManager& mgr);                 // 0x800E67EC
  bool IsInPausedState() const;                                      // 0x800E6810
  void Draw(const CStateManager& mgr) const;                         // 0x800E6834
  void StartFadeIn();                                                // 0x800E689C
  CPlayerGuiManager& GetPlayerGuiManager() { return *mPlayerGuiManager; } // Echoes' name

private:
  uchar x0_[0x2c];
  CPlayerGuiManager* mPlayerGuiManager;
};

#endif // _CINGAMEGUIMANAGER
