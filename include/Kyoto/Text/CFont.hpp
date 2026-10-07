#ifndef _CFONT
#define _CFONT

#include "Kyoto/Graphics/CColor.hpp"
#include "types.h"

class CFont {
public:
  explicit CFont(float scale);
  CFont(const CFont& other);
  ~CFont();
  int CharWidth(char) const;
  int CharsWidth(const char* text, unsigned int length) const;
  int StringWidth(const char* text) const;
  int GetFontSize() const { return mFontSize; }
  void DrawString(const char* str, long x, long y, const CColor& col) const;

private:
  static void BindSystemFont();
  static void LinearToTile8(unsigned char* destination, const unsigned char* source);
  static void TileCopy8(unsigned char* destination, const unsigned char* source);

  static unsigned char sSystemFont[65536];
  static bool sFontInitialized;

  int mFontSize;
  float mScale;
};
CHECK_SIZEOF(CFont, 0x8)

#endif // _CFONT
