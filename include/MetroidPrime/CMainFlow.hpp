#ifndef _CMAINFLOW
#define _CMAINFLOW

#include "types.h"

#include "MetroidPrime/CIOWin.hpp"

// Minimal view of the prototype's main flow IOWin (CMainFlow.cpp, 0x80023698..0x80023EC8) for
// main.cpp. Echoes name; it is 0x18 bytes like Echoes (CIOWin plus the game state).
class CMainFlow : public CIOWin {
public:
  CMainFlow(); // 0x80023E30
  EMessageReturn OnMessage(const CArchitectureMessage&, CArchitectureQueue&) override;

private:
  int mGameState;
};
CHECK_SIZEOF(CMainFlow, 0x18)

#endif // _CMAINFLOW
