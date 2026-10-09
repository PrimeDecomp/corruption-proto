#ifndef _CCHARANIMMEMORYMETRICS
#define _CCHARANIMMEMORYMETRICS

#include <types.h>

class CCharAnimMemoryMetrics {
public:
  enum EAnimSubSystem {
    kASS_Zero,
    kASS_One,
    kASS_Two,
  };

  static void SubtractFromTotalSize(uint size, EAnimSubSystem subSystem);
  static void AddToTotalSize(uint size, EAnimSubSystem subSystem);
  static uint GetTotalSize(); // Prime's name; 0x804AAEDC

private:
  static uint sTotalSize;
};

#endif // _CCHARANIMMEMORYMETRICS
