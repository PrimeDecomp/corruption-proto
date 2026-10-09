#ifndef _CINGAMETWEAKMANAGER
#define _CINGAMETWEAKMANAGER

#include "types.h"

#include "Kyoto/CAssetId.hpp"

#include "rstl/string.hpp"
#include "rstl/vector.hpp"

// Prime's layout with the 64-bit asset id: 0x58 bytes, strings at 0x4, 0x14 and 0x34 (main.cpp
// emits the destructor at 0x8000954C). Prime's names.
class CTweakValue {
public:
  struct Audio {
    float mFadeIn;
    float mFadeOut;
    float mVolume;
    rstl::string mFileName;
    CAssetId mRes;
  };
  enum EType {};

private:
  EType mType;
  rstl::string mKey;
  rstl::string mStr;
  Audio mAudio;
  union {
    uint mInt;
    float mFlt;
  };
};
CHECK_SIZEOF(CTweakValue, 0x58)

// Class and header name from the "CInGameTweakManager.h(105)" asserts; the source file is
// CIngameTweakManager.cpp. Minimal: only what main.cpp uses.
class CInGameTweakManager {
public:
  CInGameTweakManager(); // 0x8017F410

  // main.cpp handles the "Load/Save Tweaks from/to PC Host/Memory Card" debug options with these,
  // passing "c:/AudioTweaks.txt" to the PC-host pair and "AudioTweaks" to the memory-card pair.
  bool ReadFromMemoryCard(const rstl::string& name); // Echoes name.
  void WriteToMemoryCard(const rstl::string& name);  // Guessed name. Zipped text out to the card.
  void ReadFromPCHost(const rstl::string& path);     // Guessed name. CFIO text input stream.
  void WriteToPCHost(const rstl::string& path);      // Guessed name. CFIO text output stream.

private:
  rstl::vector< CTweakValue > mValues; // Prime's name
};

extern CInGameTweakManager* gpTweakManager;

#endif // _CINGAMETWEAKMANAGER
