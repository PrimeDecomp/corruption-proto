#ifndef _CGAMEPROFILER
#define _CGAMEPROFILER

#include "types.h"

// Prototype-only frame profiler (CGameProfiler.cpp); neither Prime nor Echoes has it. Minimal:
// only what main.cpp uses. The singleton is asserted with "gpProfiler == NULL" and "CGameProfiler
// should be a singleton!".
class CGameProfiler {
public:
  void Update(float dt);                 // Guessed name. Per-frame update, run first in RsMain.
  void EnableProfileGroup(const char*);  // Guessed name. Asserts "profile.mEnableCount == 0".
  void DisableProfileGroup(const char*); // Guessed name.
};

// Creates the singleton. Named after the assert text "Profiler already allocated in
// allocate_profiler." (CGameProfiler.cpp line 155).
void allocate_profiler(int);
// Guessed name, by analogy. Deletes the singleton and resets the static profile-group state.
void free_profiler();

extern CGameProfiler* gpGameProfiler; // Guessed name.

#endif // _CGAMEPROFILER
