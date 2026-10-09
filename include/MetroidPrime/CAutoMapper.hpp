#ifndef _CAUTOMAPPER
#define _CAUTOMAPPER

#include "types.h"

// Minimal declaration; Echoes' map screen (CAutoMapper.cpp). Only the map mode CMFGame sets is
// placed.
class CAutoMapper {
public:
  // Echoes' values.
  enum EMapMode { kMM_Normal, kMM_Teleport };

  void SetMapMode(EMapMode mode) { mMapMode = mode; }

private:
  uchar x0_[0x2b0];
  EMapMode mMapMode;
};

#endif // _CAUTOMAPPER
