#ifndef _CSCANTEXTDEBUGMANAGER
#define _CSCANTEXTDEBUGMANAGER

#include "types.h"

#include "Kyoto/TToken.hpp"
#include "MetroidPrime/CObjectTagToFilenameMapping.hpp"

#include "rstl/single_ptr.hpp"
#include "rstl/string.hpp"
#include "rstl/vector.hpp"

// Minimal view of the prototype's scan-text debug manager (CScanTextDebugManager.cpp,
// 0x8020B6CC..0x8020CD80; the class name follows the file name). CGameArchitectureSupport owns
// one at 0x98, publishes it at 0x807990EC and updates it on every tick; its implicit destructor
// is emitted in main.cpp (0x8000BC9C..0x8000BE9C). Neither Echoes nor Prime has it; member
// names are placeholders.
class CStringTable;

class CScanTextDebugManager {
public:
  CScanTextDebugManager(); // 0x8020C338
  void Update(float dt);   // 0x8020C1C0

private:
  // Built over the simple pool's referenced tags (rs_new at 0x8020BA98).
  rstl::single_ptr< CObjectTagToFilenameMapping > x0_;
  // The manager reads 0x10-byte entries (two 64-bit asset ids) from here, but the destructor
  // is the one shared with CMain's leading vector, so the element type is left as uint.
  rstl::vector< uint > x4_;
  // Guessed type: the current debug string table ("Current debug string %03d/%03d: %s").
  rstl::single_ptr< TCachedToken< CStringTable > > x14_;
  int x18_;
  bool x1c_;
};
CHECK_SIZEOF(CScanTextDebugManager, 0x20)

// Guessed name. 0x807990EC.
extern CScanTextDebugManager* gpScanTextDebugManager;

#endif // _CSCANTEXTDEBUGMANAGER
