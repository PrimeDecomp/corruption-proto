#ifndef _CGAMEOPTIONS
#define _CGAMEOPTIONS

#include "types.h"

#include "rstl/vector.hpp"

class CBitStreamReader;
class CBitStreamWriter;

// The prototype's CGameOptions (0x5c bytes, held by CGameState at 0x68). Only the shape is
// known: its implicit copy assignment (0x8000614C) and destructor (0x80006934) are emitted in
// main.cpp. Field types follow the copy (word/float pairs, a bool, a vector<uint>, a halfword).
class CGameOptions {
public:
  CGameOptions();
  explicit CGameOptions(CBitStreamReader& in);

  void PutTo(CBitStreamWriter& out) const;
  void EnsureOptions(); // Echoes name; called after the options are restored.

  // Echoes names, matched by order and clamp range (0..8, -30..30, -19..19, -10..10). Unlike
  // Echoes, the prototype's setters also take the slider position of the value, which callers
  // compute with fn_8017DD30 (see main.cpp).
  void SetScreenBrightness(int value, float position, bool apply);
  void SetScreenPositionX(int value, float position, bool apply);
  void SetScreenPositionY(int value, float position, bool apply);
  void SetScreenStretch(int value, float position, bool apply);

private:
  uint x0_;
  uint x4_;
  float x8_;
  int xc_;
  float x10_;
  int x14_;
  float x18_;
  int x1c_;
  float x20_;
  int x24_;
  float x28_;
  int x2c_;
  float x30_;
  int x34_;
  float x38_;
  int x3c_;
  float x40_;
  bool x44_;
  rstl::vector< uint > x48_;
  ushort x58_;
};
CHECK_SIZEOF(CGameOptions, 0x5c)

#endif // _CGAMEOPTIONS
