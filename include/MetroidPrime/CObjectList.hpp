#ifndef _COBJECTLIST
#define _COBJECTLIST

#include "MetroidPrime/TGameTypes.hpp"

class CEntity;

// G2MEAB lists hold 2048 entries: the constructor fills 0x800 slots and a unique id's low 16 bits
// index them. The vtable at 0x806B1E78 has a single slot, the always-true filter at 0x80011978.
class CObjectList {
  struct SObjectListEntry {
    CEntity* mEntity;
    short mNext;
    short mPrev;
  };

public:
  virtual uchar IsQualified(const CEntity& entity);

  // Echoes' order and names: the const overload is emitted first (0x80011678), the mutable one
  // second (0x800116C8). Their bodies are identical, so the const-ness follows that order.
  CEntity* GetObjectById(TUniqueId uid);
  const CEntity* GetObjectById(TUniqueId uid) const;

private:
  SObjectListEntry mObjects[2048];
  int mListType;
  short mFirstId;
  short mCount;
  bool x400c_;
};
CHECK_SIZEOF(CObjectList, 0x4010)

#endif // _COBJECTLIST
