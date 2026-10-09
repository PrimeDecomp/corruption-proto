#ifndef _CSCRIPTLUA
#define _CSCRIPTLUA

#include "MetroidPrime/CActor.hpp"

#include "Lua/LuaPlus.h"

#include "rstl/auto_ptr.hpp"
#include "rstl/map.hpp"
#include "rstl/single_ptr.hpp"
#include "rstl/string.hpp"

// Guessed name. The 0x18-byte TSignal2 connection the script holds for OnGlobalEvent; it is
// deleted through its vtable.
class CScriptLUASignalConnection {
public:
  virtual ~CScriptLUASignalConnection();
};

// Guessed name. The weighted-random helper owned at 0x178; its destructor (fn_802B5AF0) is
// emitted in CScriptLUA.cpp and destroys a rstl::vector< float > at 0x34.
class CScriptLUAWeightedRandom {
public:
  virtual ~CScriptLUAWeightedRandom();
};

class CScriptLUA : public CActor {
public:
  CScriptLUA(TUniqueId uid, const rstl::string& name, const CEntityInfo& info,
             const CTransform4f& xf, const rstl::string& stateAliases,
             const rstl::string& messageAliases, const rstl::string& initializationScript,
             const rstl::string& script);
  ~CScriptLUA();

  void Think(float dt, CStateManager& mgr);
  void AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg);

  // Game bindings registered as Lua globals under their own names.
  int Print(lua_State* L);
  int GetObjectId(lua_State* L);
  int GetObjectName(lua_State* L);
  int GetObjectActive(lua_State* L);
  int RandomRange(lua_State* L);

  void OnGlobalEvent(const rstl::string& event, const rstl::string& value); // Guessed name
  void RunScript(const char* buffer, int size, const char* name);           // Guessed name

private:
  LuaPlus::LuaStateOwner mLuaState;
  rstl::string mInitializationScript;
  rstl::string mScript;
  rstl::map< EScriptObjectMessage, rstl::string > mMessageToAlias;
  rstl::map< rstl::string, EScriptObjectMessage > mAliasToMessage;
  rstl::map< rstl::string, EScriptObjectState > mAliasToState;
  rstl::map< EScriptObjectState, rstl::string > mStateToAlias;
  rstl::auto_ptr< CScriptLUASignalConnection > mGlobalEventConnection;
  CStateManager* mStateMgr;
  rstl::single_ptr< CScriptLUAWeightedRandom > x178_;
};
CHECK_SIZEOF(CScriptLUA, 0x180)

#endif // _CSCRIPTLUA
