#ifndef _CGAMEPROFILESTATS
#define _CGAMEPROFILESTATS

#include "types.h"

#include "Kyoto/Basics/CStopwatch.hpp"

#include "rstl/map.hpp"
#include "rstl/string.hpp"

class CEntity;

// The per-object think statistics of CStateManager's update (CGameProfileStats.cpp). The update
// builds one on the stack from the "State Manager Numbers" option and passes it to Think; for
// modes 2 and 4 it then prints every entry ("%s - Num:%3d/%3d - Time:%5d"), sorted by time for
// mode 4. Its destructor is emitted in CStateManager.cpp (0x802944C0) and shares the map's
// destructor with CStateManager::sProfileCountersA. All names are guessed.
class CGameProfileStats {
public:
  // 0xC bytes (the sorted copies are 0x1C-byte pairs). The update prints the second and first
  // words as the counts ("Num:%3d/%3d") and the last as the time in microseconds.
  struct SStats {
    int x0_;
    int x4_;
    uint mTime;
  };
  typedef rstl::map< rstl::string, SStats > TStatsMap;

  // 0x802D8348. Per-object statistics are kept for modes 2 and 4; the flag is set while "State
  // Mgr Real Names" is off.
  CGameProfileStats(int mode, bool flag);

  const TStatsMap& GetStats() const { return mStats; }
  // 0x802D82E4 and 0x802D8180. While collecting, they record the stopwatch time before an
  // entity thinks and add the elapsed time to the entity's entry afterwards.
  void BeginEntity();
  void EndEntity(const CEntity& entity);

private:
  TStatsMap mStats;
  CStopwatch mStopwatch;
  uchar x20_[0x8];
  bool mCollect : 1;
  bool mFlag : 1;
  int mMode;
};
CHECK_SIZEOF(CGameProfileStats, 0x30)

#endif // _CGAMEPROFILESTATS
