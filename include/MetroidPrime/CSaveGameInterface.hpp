#ifndef _CSAVEGAMEINTERFACE
#define _CSAVEGAMEINTERFACE

#include "types.h"

#include "MetroidPrime/CIOWin.hpp"

class CFinalInput;

// Echoes' enum.
enum ESaveContext { kSC_FrontEnd, kSC_InGame };

// Minimal declaration. CStateManager's save-game screen (Echoes' CSaveGameScreen); the class
// name is guessed after CSaveGameInterface.cpp, which holds its destructor (0x801A26F4).
class CSaveGameInterface {
public:
  // Echoes' signature (0x801A28E8). CStateManager::DeferStateTransition builds it in game with
  // the game state's card serial.
  CSaveGameInterface(ESaveContext saveContext, u64 cardSerial);
  ~CSaveGameInterface();

  // Echoes' accessor. CStateManager::DeleteSaveGameScreen reads it at +0x80.
  CIOWin::EMessageReturn GetMessageReturn() const { return mMessageReturn; }
  // Echoes' names and signatures, in Echoes' order: 0x801A1E24, 0x801A1EA8 and 0x801A1ED4.
  // CMFGame drops the screen once its update stops returning kMR_Normal while paused.
  void Draw() const;
  void ProcessUserInput(const CFinalInput& input);
  CIOWin::EMessageReturn Update(float dt);

private:
  uchar x0_[0x80];
  CIOWin::EMessageReturn mMessageReturn;
  uchar x84_[0xb0 - 0x84];
};

#endif // _CSAVEGAMEINTERFACE
