#ifndef _CSAVEREGION
#define _CSAVEREGION

#include "types.h"

class COsContext;

// Echoes-correlated. The 128-byte buffer that survives a soft reset: CheckReset writes the
// current game options into it and hands it to OSSetSaveRegion; RsMain reads the options back
// from the non-volatile copy at boot.
class CSaveRegion {
public:
  enum { kSaveBufferSize = 128 };

  explicit CSaveRegion(COsContext& context);

  static void* GetSaveBuffer() { return mSaveBuffer; }
  static const void* GetNonVolatileSettingsBuffer() { return mNonVolatileSettingsBuf; }

  // Guessed names (from Echoes). The final 128 bytes of the 18-MiB arena are kept for restart.
  static uchar* GetSaveRegionEnd() { return reinterpret_cast< uchar* >(0x81200000); }
  static uchar* GetSaveRegionStart() { return GetSaveRegionEnd() - kSaveBufferSize; }

private:
  static void* mSaveBuffer;
  static const void* mNonVolatileSettingsBuf;
};

#endif // _CSAVEREGION
