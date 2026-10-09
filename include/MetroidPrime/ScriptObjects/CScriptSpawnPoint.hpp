#ifndef _CSCRIPTSPAWNPOINT
#define _CSCRIPTSPAWNPOINT

#include "types.h"

#include "MetroidPrime/CEntity.hpp"

class CTransform4f;

// Minimal declaration. TypesMatch id 0x4D; the vtable (lbl_806B3840) starts with its destructor
// (0x800BEDF0). As in Prime it is an entity whose transform follows CEntity (0x5C); the layout is
// not modelled.
class CScriptSpawnPoint : public CEntity {
public:
  // Guessed name. 0x800BEA38 assigns the transform at 0x5C; the console's SETTRANSFORM calls it.
  void SetTransform(const CTransform4f& xf);
};

#endif // _CSCRIPTSPAWNPOINT
