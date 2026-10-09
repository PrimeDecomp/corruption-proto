#ifndef _CHUDMEMOPARMS
#define _CHUDMEMOPARMS

#include "types.h"

// Echoes' class; the constructor (0x800BF048) stores the fields in this order.
class CHUDMemoParms {
public:
  CHUDMemoParms(float dispTime, bool clearMemoWindow, bool fadeOutOnly, bool hintMemo,
                int playerMask, bool fadeInText);

private:
  float mDispTime;
  bool mClearMemoWindow;
  bool mFadeOutOnly;
  bool mHintMemo;
  bool mFadeInText;
  int mPlayerMask;
};
CHECK_SIZEOF(CHUDMemoParms, 0xC)

#endif // _CHUDMEMOPARMS
