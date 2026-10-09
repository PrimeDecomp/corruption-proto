#ifndef _CINGAMETWEAKMANAGER
#define _CINGAMETWEAKMANAGER

#include "types.h"

#include "rstl/string.hpp"

// Class and header name from the "CInGameTweakManager.h(105)" asserts; the source file is
// CIngameTweakManager.cpp. Minimal: only what main.cpp uses.
class CInGameTweakManager {
public:
  // main.cpp handles the "Load/Save Tweaks from/to PC Host/Memory Card" debug options with these,
  // passing "c:/AudioTweaks.txt" to the PC-host pair and "AudioTweaks" to the memory-card pair.
  bool ReadFromMemoryCard(const rstl::string& name); // Echoes name.
  void WriteToMemoryCard(const rstl::string& name);  // Guessed name. Zipped text out to the card.
  void ReadFromPCHost(const rstl::string& path);     // Guessed name. CFIO text input stream.
  void WriteToPCHost(const rstl::string& path);      // Guessed name. CFIO text output stream.
};

extern CInGameTweakManager* gpTweakManager;

#endif // _CINGAMETWEAKMANAGER
