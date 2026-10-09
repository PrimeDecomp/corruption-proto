#ifndef _CSAMUSHUD
#define _CSAMUSHUD

#include "types.h"

#include "Kyoto/SObjectTag.hpp"

#include "rstl/string.hpp"

class CHUDMemoParms;

// Minimal declaration: Echoes' static memo entry points.
class CSamusHud {
public:
  static void DisplayHudMemo(const rstl::wstring& text, const CHUDMemoParms& info); // 0x80066288
  static void DeferHintMemo(CAssetId stringTable, int index,
                            const CHUDMemoParms& info); // 0x80066214
};

#endif // _CSAMUSHUD
