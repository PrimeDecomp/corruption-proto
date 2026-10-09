#ifndef _COBJECTLISTSMALL
#define _COBJECTLISTSMALL

#include "MetroidPrime/TGameTypes.hpp"

class CEntity;

// Class name from CObjectListSmall.cpp's "CObjectListSmall.cpp(41) : " assert. This replaces
// Echoes' CFilteredObjectList: CStateManagerObject owns five of them (0x24 bytes each). Each
// subclass overrides only IsQualified. The base constructor (0x8026285C) sets up an empty list
// of 0x4C-byte blocks of 16 entity pointers, and the destructor (0x8026277C) frees those blocks.
class CObjectListSmall {
public:
  virtual ~CObjectListSmall();
  virtual bool IsQualified(const CEntity& entity) const; // The base one returns true.

  // Echoes' CFilteredObjectList names.
  void RemoveObject(TUniqueId uid);           // 0x80261F60
  void RemoveObject(CEntity& entity);         // 0x8026214C
  void AddObject(CEntity& entity);            // 0x80262294
  bool Contains(const CEntity& entity) const; // 0x8026265C

private:
  uchar x4_[0x20];
};
CHECK_SIZEOF(CObjectListSmall, 0x24)

#endif // _COBJECTLISTSMALL
