// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80011658..0x80011B5C (18 native functions).
// Retains the pre-existing organizational path and exact configured boundaries.
// Original source scope and header/helper emission remain uncertain.
// Listed below are the native entries not implemented yet.
// 0x80011658 +0x10
// 0x80011668 +0x10
// 0x80011678 +0x50
// 0x800116C8 +0x50
// 0x80011718 +0xBC
// 0x800117D4 +0x38
// 0x8001180C +0x120
// 0x8001192C +0x4C
// 0x80011A9C +0x30
// 0x80011ACC +0x18
// 0x80011AE4 +0x2C
// 0x80011B10 +0x8
// 0x80011B18 +0x4
// 0x80011B1C +0x4
// 0x80011B20 +0x3C

#include "MetroidPrime/CObjectList.hpp"

#include "MetroidPrime/CEntity.hpp"

// Echoes' body over 2048 entries.
CObjectList::CObjectList(EGameObjectList listType, bool dynamic)
: mListType(listType), mFirstId(-1), mCount(0), mDynamic(dynamic) {
  for (int i = 0; i < 2048; ++i) {
    mObjects[i] = SObjectListEntry();
  }
}

uchar CObjectList::IsQualified(const CEntity& entity) { return true; }
