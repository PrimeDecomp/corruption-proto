#include "Kyoto/Text/CFont.hpp"
#include "Kyoto/Alloc/CMemory.hpp"

extern "C" void* memcpy(void* destination, const void* source, unsigned long length);
extern "C" void DCFlushRange(void* address, unsigned long length);
extern const float kFontAdvance;
extern const float kFontCellSize;

struct CFontGraphicsState {
  unsigned int state;
  unsigned int value;
  unsigned char flag;
};

extern "C" CFontGraphicsState fn_804BAE18();
extern "C" void fn_804BADE4(const CFontGraphicsState* state);
extern "C" void fn_804BAB64(int textureWidth, int textureHeight, int x, int y, int sourceX,
                            int sourceY, int sourceWidth, int sourceHeight, int width, int height,
                            const CColor* color);
extern "C" void fn_804C0274(int width, int height, int format, const void* data, int, int);

void CFont::DrawString(const char* text, long x, long y, const CColor& color) const {
  CFontGraphicsState state = fn_804BAE18();
  BindSystemFont();

  unsigned char character = *text;
  while (character != 0) {
    ++text;
    fn_804BAB64(256, 256, x, y, (character % 16) * 16, character & 0xf0, 16, 16,
                static_cast< int >(kFontCellSize * mScale),
                static_cast< int >(kFontCellSize * mScale), &color);
    x += static_cast< int >(kFontAdvance * mScale);
    character = *text;
  }

  fn_804BADE4(&state);
}

int CFont::CharsWidth(const char*, unsigned int length) const {
  return length * static_cast< int >(kFontAdvance * mScale);
}

int CFont::StringWidth(const char* text) const {
  int width = 0;
  while (*text != 0) {
    ++text;
    width += static_cast< int >(kFontAdvance * mScale);
  }
  return width;
}

int CFont::CharWidth(char) const { return static_cast< int >(kFontAdvance * mScale); }

CFont::~CFont() {}

CFont::CFont(const CFont& other) : mFontSize(other.mFontSize), mScale(other.mScale) {}

CFont::CFont(float scale) : mFontSize(static_cast< int >(kFontCellSize * scale)), mScale(scale) {
  if (!sFontInitialized) {
    sFontInitialized = true;
    unsigned char* tiled = new ("DolphinCFont.cpp(193) : ", (const char*)0) unsigned char[65536];
    LinearToTile8(tiled, sSystemFont);
    memcpy(sSystemFont, tiled, sizeof(sSystemFont));
    CMemory::Free(tiled);
    DCFlushRange(sSystemFont, sizeof(sSystemFont));
  }
}

void CFont::BindSystemFont() { fn_804C0274(256, 256, 1, sSystemFont, 0, 0); }

void CFont::LinearToTile8(unsigned char* destination, const unsigned char* source) {
  int offset;
  int rowOffset;
  short y;
  y = 0;
  rowOffset = 0;
  for (; y < 256; y += 4) {
    offset = rowOffset;
    for (short x = 0; x < 256; x += 8) {
      TileCopy8(destination, source + offset);
      destination += 32;
      offset += 8;
    }
    rowOffset += 1024;
  }
}

void CFont::TileCopy8(unsigned char* destination, const unsigned char* source) {
  for (unsigned int row = 0; row < 4; ++row) {
    for (unsigned int column = 0; column < 8; ++column) {
      destination[column] = source[column];
    }
    source += 256;
    destination += 8;
  }
}
