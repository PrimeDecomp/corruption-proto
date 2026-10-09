#ifndef _CSCOPEDPROFILER
#define _CSCOPEDPROFILER

#include "types.h"

#include "rstl/string.hpp"

// Echoes' guessed class name for the named profiling scopes on the stack ("*GUI_Draw"). Unlike
// Echoes' empty stubs, the prototype's live in CGameProfileStats.cpp: the constructor (0x802D84BC)
// stores the start time and copies the name, and the destructor (0x802D8420) hands the elapsed
// time to the name's counter while "State Manager Numbers" is 6.
class CScopedProfiler {
public:
  CScopedProfiler(const rstl::string& name, bool enabled);
  ~CScopedProfiler();
  // Echoes' guessed name. 0x802D851C resets the counters at the start of the world draw.
  static void BeginFrame();

private:
  s64 mStartTime;
  rstl::string mName;
  bool mEnabled;
};

#endif // _CSCOPEDPROFILER
