#ifndef KYOTO_TEXT_CFONT_HPP
#define KYOTO_TEXT_CFONT_HPP

class CColor;

class CFont {
public:
  explicit CFont(float scale);
  CFont(const CFont& other);
  ~CFont();

  void DrawString(const char* text, long x, long y, const CColor& color) const;
  int CharsWidth(const char* text, unsigned int length) const;
  int StringWidth(const char* text) const;
  int CharWidth(char character) const;

private:
  static void BindSystemFont();
  static void LinearToTile8(unsigned char* destination, const unsigned char* source);
  static void TileCopy8(unsigned char* destination, const unsigned char* source);

  static unsigned char sSystemFont[65536];
  static bool sFontInitialized;

  int mFontSize;
  float mScale;
};

#endif
