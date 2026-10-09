#ifndef _CMAPWORLDINFO
#define _CMAPWORLDINFO

#include "MetroidPrime/TGameTypes.hpp"

// Minimal: only what CStateManagerObject uses. Echoes' names and signatures. The rc_ptr release
// that CStateManagerObject's destructor calls (0x800970A4) deletes it through 0x800970F4.
class CMapWorldInfo {
public:
  ~CMapWorldInfo();

  bool IsAreaVisited(TAreaId areaId) const;          // 0x80119AA0
  void SetIsMapped(TAreaId areaId, bool mapped);     // 0x8011A20C
  void SetAreaVisited(TAreaId areaId, bool visited); // 0x8011A30C
};

#endif // _CMAPWORLDINFO
