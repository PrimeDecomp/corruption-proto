#ifndef _CAUDIOSTATEWIN
#define _CAUDIOSTATEWIN

#include "types.h"

#include "MetroidPrime/CIOWin.hpp"

// Minimal view of the prototype's audio state IOWin (CAudioStateWin.cpp,
// 0x800E8494..0x800E85FC) for main.cpp. Echoes name; like Echoes it adds no members.
class CAudioStateWin : public CIOWin {
public:
  CAudioStateWin(); // 0x800E8570
  EMessageReturn OnMessage(const CArchitectureMessage&, CArchitectureQueue&) override;
};
CHECK_SIZEOF(CAudioStateWin, 0x14)

#endif // _CAUDIOSTATEWIN
