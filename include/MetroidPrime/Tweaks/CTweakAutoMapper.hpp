#ifndef _CTWEAKAUTOMAPPER
#define _CTWEAKAUTOMAPPER

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakAutoMapper;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakAutoMapper {
public:
  explicit CTweakAutoMapper(const SLdrTweakAutoMapper& data) : mData(&data) {}

private:
  const SLdrTweakAutoMapper* mData;
};
CHECK_SIZEOF(CTweakAutoMapper, 0x4)

extern rstl::single_ptr< CTweakAutoMapper > gpTweakAutoMapper;

#endif // _CTWEAKAUTOMAPPER
