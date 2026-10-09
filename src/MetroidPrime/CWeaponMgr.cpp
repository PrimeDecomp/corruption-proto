// NonMatching translation-unit scaffold.
// G2MEAB .text 0x800B0FE4..0x800B18D8 (end exclusive).
// Echoes' CWeaponMgr.cpp, in the same order. The map's find, erase and insert helpers and the
// static initializer for TGameTypes.hpp's SDA constants (0x800B18A8) are emitted with it.
// Not paired: 0x800B1820 +0x88, a 0x58-byte block copy of the map's (id, counts) pair that
// insert_into calls where this rstl constructs the node in place.

#include "MetroidPrime/CWeaponMgr.hpp"

CWeaponMgr::CWeaponMgr() {}

// 57%: the target fills the vector from an immediate zero rather than an SDA constant, and its
// reserved_vector copy into the pair is an unrolled copy with no placement-new null check.
void CWeaponMgr::Add(TUniqueId uid, EWeaponType type) {
  rstl::pair< TUniqueId, Vec > newIndex(uid, Vec(0));
  newIndex.second[type] += 1;
  mWeapons.insert(newIndex);
}

void CWeaponMgr::Remove(TUniqueId uid) {
  rstl::map< TUniqueId, Vec >::iterator iter = mWeapons.find(uid);
  if (iter != mWeapons.end()) {
    mWeapons.erase(iter);
  }
}

void CWeaponMgr::IncrCount(TUniqueId uid, EWeaponType type) {
  Vec* vec = GetIndex(uid);
  if (vec == nullptr) {
    Add(uid, type);
  } else {
    (*vec)[type]++;
  }
}

void CWeaponMgr::DecrCount(TUniqueId uid, EWeaponType type) {
  Vec* vecP = GetIndex(uid);
  if (!vecP) {
    return;
  }

  Vec& vec = *vecP;
  vec[type]--;

  bool empty = true;
  Vec::iterator it = vec.begin(), end = vec.end();
  for (; it != end; ++it) {
    if (*it > 0) {
      empty = false;
      break;
    }
  }
  if (empty) {
    Remove(uid);
  }
}

int CWeaponMgr::GetNumActive(TUniqueId uid, EWeaponType type) const {
  Vec* vec = GetIndex(uid);
  if (vec) {
    return (*vec)[type];
  } else {
    return 0;
  }
}

CWeaponMgr::Vec* CWeaponMgr::GetIndex(TUniqueId uid) const {
  rstl::map< TUniqueId, Vec >::const_iterator iter = mWeapons.find(uid);
  if (iter != mWeapons.end()) {
    return const_cast< Vec* >(&iter->second);
  }
  return nullptr;
}
