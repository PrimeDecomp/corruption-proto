#ifndef _CFRONTENDUI
#define _CFRONTENDUI

#include "types.h"

// Minimal declaration (CFrontEndUI.cpp).
class CFrontEndUI {
public:
  // 0x80023394 returns the build label ("Build v3.068 3/2/2006 14:55:13"); CMFGame and
  // CFrontEndUIDevelopment draw it.
  static const char* GetVersionInfo();
};

#endif // _CFRONTENDUI
