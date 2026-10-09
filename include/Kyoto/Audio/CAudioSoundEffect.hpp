#ifndef _CAUDIOSOUNDEFFECT
#define _CAUDIOSOUNDEFFECT

#include "types.h"

#include "Kyoto/Audio/CAudioHandle.hpp"

class CVector3f;

// Minimal declaration of the CAUD resource that CActor plays. Both functions return an invalid
// handle when an audio debug option is set; otherwise they allocate a voice and play it with
// the volume clamped to 0..1 (and the pan to -1..1).
class CAudioSoundEffect {
public:
  // Guessed names (0x8056BDC8, 0x8056BEA0).
  CAudioHandle PlaySpatial(int areaId, const CVector3f& position, float volume);
  CAudioHandle PlayPanned(int areaId, float volume, float pan);
};

#endif // _CAUDIOSOUNDEFFECT
