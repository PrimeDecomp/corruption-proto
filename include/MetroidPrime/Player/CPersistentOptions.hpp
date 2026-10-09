#ifndef _CPERSISTENTOPTIONS
#define _CPERSISTENTOPTIONS

#include "types.h"

#include "MetroidPrime/Player/CGameStateEnvVarManager.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/pair.hpp"
#include "rstl/vector.hpp"

// Minimal view after Echoes (where the name is guessed): the system-wide environment variables
// plus the seen cinematics and the selected save slot.
class CPersistentOptions : public CGameStateEnvVarManager {
private:
  rstl::vector< rstl::pair< CAssetId, TEditorId > > mCinematicStates;
  int mSaveIdx;
};
CHECK_SIZEOF(CPersistentOptions, 0x2c)

#endif // _CPERSISTENTOPTIONS
