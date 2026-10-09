#ifndef _CSCRIPTOBJECTLOADERHELPER
#define _CSCRIPTOBJECTLOADERHELPER

#include "MetroidPrime/TGameTypes.hpp"

class CStateManager;

// Minimal: only what CStateManagerObject uses. CStateManagerObject owns one (0x18 bytes; the
// constructor is 0x802849D0 and the destructor 0x80284908).
class CScriptObjectLoaderHelper {
public:
  CScriptObjectLoaderHelper();
  ~CScriptObjectLoaderHelper();

  // Echoes' name and signature (0x80283438).
  void FreeScriptObjects(TAreaId area, CStateManager& mgr);

private:
  uchar x0_[0x18];
};
CHECK_SIZEOF(CScriptObjectLoaderHelper, 0x18)

#endif // _CSCRIPTOBJECTLOADERHELPER
