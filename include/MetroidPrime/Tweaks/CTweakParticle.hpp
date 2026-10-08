#ifndef _CTWEAKPARTICLE
#define _CTWEAKPARTICLE

#include "types.h"

#include "rstl/single_ptr.hpp"
#include "rstl/string.hpp"

struct SLdrTweakParticle;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakParticle {
public:
  explicit CTweakParticle(const SLdrTweakParticle& data) : mData(&data) {}

private:
  const SLdrTweakParticle* mData;
  rstl::string x4_;
  rstl::string x14_;
  rstl::string x24_;
};
CHECK_SIZEOF(CTweakParticle, 0x34)

extern rstl::single_ptr< CTweakParticle > gpTweakParticle;

#endif // _CTWEAKPARTICLE
