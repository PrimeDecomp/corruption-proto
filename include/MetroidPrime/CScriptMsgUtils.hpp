#ifndef _CSCRIPTMSGUTILS
#define _CSCRIPTMSGUTILS

#include "types.h"

#include "MetroidPrime/CEntityInfo.hpp"

class CEntity;

// Guessed name (CScriptMsgUtils.cpp, 0x803ED360). Builds the originator of a script message sent
// on behalf of an entity: from the entity's ids and its word at 0x5C, or an invalid originator
// for null.
SScriptMsgOriginator MakeScriptMsgOriginator(const CEntity* entity);

#endif // _CSCRIPTMSGUTILS
