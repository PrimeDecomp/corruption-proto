#ifndef _CWORLDLAYERSTATE
#define _CWORLDLAYERSTATE

#include "types.h"

#include "rstl/bit_vector.hpp"
#include "rstl/rc_ptr.hpp"
#include "rstl/string.hpp"
#include "rstl/vector.hpp"

// Minimal declaration; Echoes' class. Its implicit destructor (instance at 0x8009726C, in
// CAutoMapper.cpp) has Echoes' layout: a vector at 0x0 (destructor 0x8003B130), the save-layer
// bits at 0x10 and two shared vectors, of strings (0x24) and of ints (0x2C). CStateManager keeps
// one through an rc_ptr at 0x150.
class CWorldLayerState {
private:
  uchar x0_[0x10]; // Echoes' rstl::vector< CWorldLayers::Area > mAreaLayers
  // Echoes' names.
  rstl::bit_vector< rstl::rmemory_allocator > mSaveLayers;
  rstl::rc_ptr< rstl::vector< rstl::string > > mLayerNames;
  rstl::rc_ptr< rstl::vector< int > > mLayerNameOffsets;
};
CHECK_SIZEOF(CWorldLayerState, 0x34)

#endif // _CWORLDLAYERSTATE
