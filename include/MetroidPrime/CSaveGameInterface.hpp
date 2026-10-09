#ifndef _CSAVEGAMEINTERFACE
#define _CSAVEGAMEINTERFACE

#include "types.h"

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
};

#endif // _CSAVEGAMEINTERFACE
