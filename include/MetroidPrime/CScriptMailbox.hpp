#ifndef _CSCRIPTMAILBOX
#define _CSCRIPTMAILBOX

#include "MetroidPrime/TGameTypes.hpp"

class CStateManager;

// Minimal: only what CStateManagerObject uses. The rc_ptr release that CStateManagerObject's
// destructor calls (0x8015EB8C) deletes it through 0x800BDB94.
class CScriptMailbox {
public:
  ~CScriptMailbox();

  // Echoes' name and signature (0x800BD8F0); the caller passes its own area id's address.
  void SendMsgs(const TAreaId& areaId, CStateManager& mgr);
};

#endif // _CSCRIPTMAILBOX
