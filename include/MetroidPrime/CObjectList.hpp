#ifndef _COBJECTLIST
#define _COBJECTLIST

#include "MetroidPrime/TGameTypes.hpp"

class CEntity;

// Echoes' enum, with the render actor list added at 2 (the type ids GameObjectLists.cpp's
// constructors pass, in the order CStateManagerObject creates them).
enum EGameObjectList {
  kOL_Invalid = -1,
  kOL_All,
  kOL_Actor,
  kOL_RenderActor, // Guessed name, after the CRenderActor cast its list filters on.
  kOL_PhysicsActor,
  kOL_GameLight,
  kOL_ListeningAi,
  kOL_AiWaypoint,
  kOL_Platform,
  kOL_Trigger,
};

// G2MEAB lists hold 2048 entries: the constructor fills 0x800 slots and a unique id's low 16 bits
// index them. The vtable at 0x806B1E78 has a single slot, the always-true filter at 0x80011978.
class CObjectList {
  struct SObjectListEntry {
    CEntity* mEntity;
    short mNext;
    short mPrev;
    // Weak (0x80011A84); the constructor builds the array with it and then resets every entry.
    SObjectListEntry() : mEntity(nullptr), mNext(-1), mPrev(-1) {}
  };

public:
  // 0x80011980. Echoes' constructor.
  CObjectList(EGameObjectList listType, bool dynamic);
  virtual uchar IsQualified(const CEntity& entity);

  // Echoes' order and names: the const overload is emitted first (0x80011678), the mutable one
  // second (0x800116C8). Their bodies are identical, so the const-ness follows that order.
  CEntity* GetObjectById(TUniqueId uid);
  const CEntity* GetObjectById(TUniqueId uid) const;
  // Echoes' name. Only one overload is emitted (0x80011668); CStateManager's destructor reads
  // the list it gets from ObjectListById through it.
  CEntity* operator[](int idx);
  // Echoes' names; CStateManagerObject's add, remove and list-update paths call these.
  void RemoveObject(TUniqueId uid);   // 0x80011718
  void AddObject(CEntity& entity);    // 0x8001180C
  int size() const { return mCount; } // Echoes' name

  // Echoes' guessed name. CStateManagerObject's constructor keeps the dynamic lists in a second
  // view, which UpdateObjectInLists re-filters when an entity changes.
  bool IsDynamic() const { return mDynamic; }

private:
  SObjectListEntry mObjects[2048];
  EGameObjectList mListType;
  short mFirstId;
  short mCount;
  bool mDynamic; // Echoes' guessed name
};
CHECK_SIZEOF(CObjectList, 0x4010)

#endif // _COBJECTLIST
