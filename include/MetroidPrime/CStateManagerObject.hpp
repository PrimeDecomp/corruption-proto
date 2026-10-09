#ifndef _CSTATEMANAGEROBJECT
#define _CSTATEMANAGEROBJECT

#include "MetroidPrime/CObjectList.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/auto_ptr.hpp"
#include "rstl/bit_vector.hpp"
#include "rstl/list.hpp"
#include "rstl/pair.hpp"
#include "rstl/rc_ptr.hpp"
#include "rstl/red_black_tree.hpp"
#include "rstl/reserved_vector.hpp"
#include "rstl/single_ptr.hpp"

class CEntity;
class CMapWorldInfo;
class CObjectListSmall;
class CPlayer;
class CScriptMailbox;
class CScriptMsg;
class CScriptMsgQueue;
class CScriptObjectLoaderHelper;
class CStateManager;
class CWorld;

// Guessed name: the repo's inferred split name for the unit whose methods (0x80308934, which
// inserts and notifies, and 0x80308C04, which looks up) receive the pointer held at 0x1118.
class CStringPropertyManager;

// Guessed name, after CStateManagerObject.cpp, the unit that holds this constructor (0x8029A72C)
// and AllocateUniqueId's assert. CStateManager allocates it (0x1138 bytes) and keeps it at 0x4,
// the same way it keeps CStateManagerCallbackLists.cpp's object at 0x0 and
// CStateManagerCollision.cpp's object at 0x8. It owns the object lists and unique-id allocation
// that Echoes kept on CStateManager itself.
//
// What moved here from Echoes' CStateManager (layout from the constructor at 0x8029A72C and the
// destructor at 0x8029A4E0): the object index array, the object lists and their unowned
// views, the allocated-index bits, the script message queue, the world, the graveyard, the
// script object loader helper, the next/previous area ids, the editor-id map and the mailbox
// and map-world-info handles. The player pointer also lives here (Echoes kept four players on
// CStateManager). The add/remove/active-change callbacks it fires live in
// CStateManagerCallbackLists (lists at 0x570, 0x588 and 0x5A0 of that object).
class CStateManagerObject {
public:
  // Echoes' types; Corruption keys the multimap with the 26-bit TEditorId compare.
  typedef rstl::red_black_tree< TEditorId, rstl::pair< TEditorId, TUniqueId >, 1 > TIdList;
  typedef rstl::pair< TIdList::const_iterator, TIdList::const_iterator > TIdListResult;

  TUniqueId AllocateUniqueId();

  // Names from Echoes' CStateManager, whose ObjectById is also emitted first. ObjectById reads the
  // first list directly; GetObjectById goes through the const overload.
  CEntity* ObjectById(TUniqueId uid);
  const CEntity* GetObjectById(TUniqueId uid) const;

  // Guessed names. The const getter's result feeds the lookup (0x80308C04) and the mutable one's
  // feeds the insert (0x80308934); the getter at 0x80298190 is emitted before 0x80298198.
  const CStringPropertyManager* GetStringPropertyManager() const;
  CStringPropertyManager* StringPropertyManager();

  // Echoes' names. The world, the area ids and the object lists are no longer inline: callers
  // across the DOL branch to these out-of-line getters.
  CWorld* World();
  const CWorld* GetWorld() const;
  bool HasWorld() const;
  TAreaId GetNextAreaId() const;
  TAreaId GetPreviousAreaId() const;
  // RemoveObject (0x80299B18) calls the first one and then CObjectList's mutable GetObjectById,
  // so it is the mutable overload. Index 0 is the list of all objects.
  CObjectList& ObjectListById(int id);
  const CObjectList& GetObjectListById(int id) const;
  // Guessed names. Same shape over the five CObjectListSmall lists at 0x1080, which take the
  // place of Echoes' filtered object lists.
  CObjectListSmall& ObjectListSmallById(int id);
  const CObjectListSmall& GetObjectListSmallById(int id) const;

  // Echoes' names.
  void AddToGraveyard(CEntity* entity);
  void ClearGraveyard();
  void DeliverScriptMsg(const CScriptMsg& msg);
  TEditorId GetEditorIdForUniqueId(TUniqueId uid) const;
  TUniqueId GetIdForScript(TEditorId editorId) const;
  TIdListResult GetIdListForScript(TEditorId editorId) const;

  // Echoes' name for the helper; it now returns the owned pointer at 0x10F8. CGameArea and the
  // script object loaders call the mutable one, the pickup and sound scripts the const one.
  CScriptObjectLoaderHelper* ScriptObjectLoaderHelper();
  const CScriptObjectLoaderHelper* GetScriptObjectLoaderHelper() const; // Guessed name
  TIdList& ScriptIdMap(); // Guessed name; CScriptObjectLoaderHelper takes the map's address.

  // Echoes' name.
  CScriptMailbox* Mailbox() const;
  CScriptMsgQueue* ScriptMsgQueue() const; // Guessed name, after CScriptMsgQueue.cpp.
  // Echoes' name for the mutable one. SetCurrentAreaId (0x8029A080) asks the mutable one about
  // and marks the visited area, then passes the const one to the map world.
  CMapWorldInfo* MapWorldInfo();
  const CMapWorldInfo* GetMapWorldInfo() const; // Guessed name

  // Prime's names. CPlayer.cpp builds the object that CStateManager installs with SetPlayer
  // and then adds with AddObject; CStateManager hands the mutable one to CPlayer's methods.
  const CPlayer* GetPlayer() const;
  CPlayer* Player();
  void SetPlayer(CPlayer* player); // Guessed name

private:
  const CObjectList& GetAllObjectList() const { return *mObjectLists[0]; } // Guessed name

  CStateManager* mStateMgr;
  ushort mLastUniqueId; // Named by the "mLastUniqueId != startId" assert.
  // Echoes' name. The constructor fills all 0x800 slots with zero.
  rstl::reserved_vector< ushort, 2048 > mObjectIndexArray;
  // The constructor creates nine lists; index 0 (at 0x1014) is the list every lookup uses.
  rstl::reserved_vector< rstl::auto_ptr< CObjectList >, 9 > mObjectLists;
  // Echoes' name. Starts empty and owns nothing (the destructor skips it).
  rstl::reserved_vector< CObjectList*, 9 > mDynamicObjectLists;
  // Five small lists (CObjectListSmall.cpp's add/remove at 0x80262294/0x80261F60), deleted
  // through their virtual destructor. Echoes keeps six CFilteredObjectLists here.
  rstl::reserved_vector< rstl::auto_ptr< CObjectListSmall >, 5 > mObjectListsSmall;
  rstl::reserved_vector< CObjectListSmall*, 5 > mDynamicObjectListsSmall;
  rstl::bit_vector<> mAllocatedObjectIndices; // Echoes' name; built as (0x800, false).
  // Echoes embeds this as ScriptMsgArray (0xC0 messages and two indices); here it is a separate
  // 0x1808-byte object, allocated by the constructor.
  CScriptMsgQueue* mScriptMsgs;
  rstl::single_ptr< CWorld > mWorld;
  // Echoes' name and type: batches of up to 32 entities deleted together.
  rstl::list< rstl::reserved_vector< CEntity*, 32 > > mGraveyard;
  rstl::single_ptr< CScriptObjectLoaderHelper > mScriptObjectLoaderHelper; // 0x18 bytes
  TAreaId mNextAreaId;
  TAreaId mPreviousAreaId;
  TIdList mScriptIdMap;
  rstl::ncrc_ptr< CStringPropertyManager > mStringPropertyManager;
  // These two and the one above are copied from the constructor's last three arguments.
  rstl::ncrc_ptr< CScriptMailbox > mMailbox;
  rstl::ncrc_ptr< CMapWorldInfo > mMapWorldInfo;
  CPlayer* mPlayer;
  // Set around the deferred message pump (0x80298594), which only runs when it is clear.
  bool x1134_24_ : 1;
};
CHECK_SIZEOF(CStateManagerObject, 0x1138)

#endif // _CSTATEMANAGEROBJECT
