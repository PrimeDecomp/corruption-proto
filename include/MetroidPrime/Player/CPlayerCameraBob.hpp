#ifndef _CPLAYERCAMERABOB
#define _CPLAYERCAMERABOB

#include "types.h"

struct SLdrTweakCameraBob;

class CPlayerCameraBob {
public:
  // TweaksAccessors.cpp 0x802493D4: copies the tweak values into static members.
  static void BindTweaks(const SLdrTweakCameraBob& tweaks);
};

#endif // _CPLAYERCAMERABOB
