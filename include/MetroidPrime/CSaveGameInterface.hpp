#ifndef _CSAVEGAMEINTERFACE
#define _CSAVEGAMEINTERFACE

#include "types.h"

// Minimal declaration. CStateManager's save-game screen (Echoes' CSaveGameScreen); the class
// name is guessed after CSaveGameInterface.cpp, which holds its destructor (0x801A26F4).
class CSaveGameInterface {
public:
  ~CSaveGameInterface();
};

#endif // _CSAVEGAMEINTERFACE
