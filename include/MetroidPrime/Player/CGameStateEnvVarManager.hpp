#ifndef _CGAMESTATEENVVARMANAGER
#define _CGAMESTATEENVVARMANAGER

#include "types.h"

#include "MetroidPrime/Player/CEnvironmentVariable.hpp"

#include "rstl/map.hpp"
#include "rstl/string.hpp"

// Minimal view. Class name from the MP2 Wii SEL (via Echoes); the lookup is the R3ME01 SEL export
// GetEnvVar__23CGameStateEnvVarManagerCFPCc. The prototype's functions live in CGameState.cpp.
class CGameStateEnvVarManager {
public:
  // Echoes' guessed names; the scope selects the system or per-game variable list.
  enum EVariableScope { kVS_System, kVS_Game };

  // 0x8015E64C: returns the named variable, or null. Const, although callers modify the result.
  CEnvironmentVariable* GetEnvVar(const char* name) const;

private:
  EVariableScope mScope;
  rstl::map< rstl::string, CEnvironmentVariable > mVariables;
};
CHECK_SIZEOF(CGameStateEnvVarManager, 0x18)

#endif // _CGAMESTATEENVVARMANAGER
