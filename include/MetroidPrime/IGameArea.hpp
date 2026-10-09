#ifndef _IGAMEAREA
#define _IGAMEAREA

#include "types.h"

#include "Kyoto/CAssetId.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/string.hpp"

class CTransform4f;

// Echoes' area interface. CGameArea's vtable (lbl_806B270C) has Echoes' nine slots; only the
// name at 0x28 is checked against the prototype (0x8004BFAC returns a copy of CGameArea+0x70),
// the other declarations are Echoes'.
class IGameArea {
public:
  virtual ~IGameArea();
  virtual const CTransform4f& IGetTM() const = 0;
  virtual CAssetId IGetStringTableAssetId() const = 0;
  virtual uint IGetNumAttachedAreas() const = 0;
  virtual TAreaId IGetAttachedAreaId(int index) const = 0;
  virtual bool IIsActive() const = 0;
  virtual CAssetId IGetAreaAssetId() const = 0;
  virtual int IGetAreaSaveId() const = 0;
  // Guessed name, as in Echoes.
  virtual rstl::string IGetInternalAreaName() const = 0;
};

#endif // _IGAMEAREA
