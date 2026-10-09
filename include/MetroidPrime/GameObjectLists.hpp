#ifndef _GAMEOBJECTLISTS
#define _GAMEOBJECTLISTS

#include "MetroidPrime/CObjectList.hpp"
#include "MetroidPrime/CObjectListSmall.hpp"

// The filtered object lists CStateManagerObject creates. Each one only overrides IsQualified.
//
// The CObjectList subclasses are Echoes' (GameObjectLists.cpp), plus a render actor list. Their
// list type ids and the dynamic flag (only the listening AI list's) follow Echoes.

class CActorList : public CObjectList {
public:
  CActorList();
  uchar IsQualified(const CEntity& entity);
};

// Guessed name: it holds the entities with CRenderActor's cast flag (2), which CRenderActor's
// constructor (0x8029F944) adds; CPhysicsActor's constructor adds 4 and calls it.
class CRenderActorList : public CObjectList {
public:
  CRenderActorList();
  uchar IsQualified(const CEntity& entity);
};

class CPhysicsActorList : public CObjectList {
public:
  CPhysicsActorList();
  uchar IsQualified(const CEntity& entity);
};

// Guessed name (Echoes').
class CTriggerList : public CObjectList {
public:
  CTriggerList();
  uchar IsQualified(const CEntity& entity);
};

class CListeningAiList : public CObjectList {
public:
  CListeningAiList();
  // 0x80148484: Echoes' check, a CPatterned whose IsListening (vtable slot 0xD8) returns true.
  uchar IsQualified(const CEntity& entity);
};

class CAiWaypointList : public CObjectList {
public:
  CAiWaypointList();
  uchar IsQualified(const CEntity& entity);
};

// Guessed name (Echoes').
class CPlatformList : public CObjectList {
public:
  CPlatformList();
  uchar IsQualified(const CEntity& entity);
};

class CGameLightList : public CObjectList {
public:
  CGameLightList();
  uchar IsQualified(const CEntity& entity);
};

// The CObjectListSmall subclasses take the place of Echoes' CFilteredObjectList subclasses, in
// the same order without Echoes' forgotten-object list. They are defined in CObjectListSmall.cpp,
// which emits them; all five are static (not dynamic).

// Guessed names, after Echoes' list indices.
enum EGameObjectListSmall {
  kOLS_Door,
  kOLS_Dock,
  kOLS_Type106,
  kOLS_GameCamera,
  kOLS_GrapplePoint,
};

// Guessed name.
class CDoorListSmall : public CObjectListSmall {
public:
  CDoorListSmall();
  bool IsQualified(const CEntity& entity) const;
};

// Guessed name.
class CDockListSmall : public CObjectListSmall {
public:
  CDockListSmall();
  bool IsQualified(const CEntity& entity) const;
};

// Guessed name. Its entities have type id 0x6A, which no TypesMatch override in the DOL
// answers (Echoes' list at the same index is likewise for an unidentified type, 124). The
// parasite AI functions (CParasiteAiFunctions.cpp) cast nearby objects to the same type.
class CType106ListSmall : public CObjectListSmall {
public:
  CType106ListSmall();
  bool IsQualified(const CEntity& entity) const;
};

// Guessed name.
class CGameCameraListSmall : public CObjectListSmall {
public:
  CGameCameraListSmall();
  bool IsQualified(const CEntity& entity) const;
};

// Guessed name.
class CGrapplePointListSmall : public CObjectListSmall {
public:
  CGrapplePointListSmall();
  bool IsQualified(const CEntity& entity) const;
};

#endif // _GAMEOBJECTLISTS
