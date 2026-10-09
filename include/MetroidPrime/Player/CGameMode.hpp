#ifndef _CGAMEMODE
#define _CGAMEMODE

#include "types.h"

#include "Kyoto/SObjectTag.hpp"

// Minimal declaration; Echoes' abstract game mode, which CGameState owns. The prototype's
// interface differs from Echoes': only the slot CStateManager's constructor calls is named, and
// the twelve before it hold its place in the vtable (CGMSinglePlayer's is at 0x806B6C80).
class CGameMode {
public:
  virtual ~CGameMode();
  virtual void x0C_() = 0;
  virtual void x10_() = 0;
  virtual void x14_() = 0;
  virtual void x18_() = 0;
  virtual void x1C_() = 0;
  virtual void x20_() = 0;
  virtual void x24_() = 0;
  virtual void x28_() = 0;
  virtual void x2C_() = 0;
  virtual void x30_() = 0;
  virtual void x34_() = 0;
  virtual void x38_() = 0;
  // Guessed name. CGMSinglePlayer returns 'SNGL'; the constructor prints it ("Game type is %s").
  virtual FourCC GetGameType() const = 0;
};

#endif // _CGAMEMODE
