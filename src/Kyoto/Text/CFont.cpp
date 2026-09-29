#include "Kyoto/Text/CFont.hpp"
#include "Kyoto/Alloc/CMemory.hpp"

extern "C" void* memcpy(void* destination, const void* source, unsigned long length);
extern "C" void DCFlushRange(void* address, unsigned long length);
extern const float kFontAdvance;
extern const float kFontCellSize;

struct CFontGraphicsState {
  unsigned int words[2];
  unsigned char flag;
};

extern "C" void fn_804BAE18(CFontGraphicsState* state);
extern "C" void fn_804BADE4(const CFontGraphicsState* state);
extern "C" void fn_804BAB64(int textureWidth, int textureHeight, int x, int y,
                              int sourceX, int sourceY, int sourceWidth, int sourceHeight,
                              int width, int height, const CColor* color);
extern "C" void fn_804C0274(int width, int height, int format, const void* data, int, int);

void CFont::DrawString(const char* text, long x, long y, const CColor& color) const {
  CFontGraphicsState previous;
  fn_804BAE18(&previous);
  CFontGraphicsState state;
  state.words[0] = previous.words[0];
  state.words[1] = previous.words[1];
  state.flag = previous.flag;
  BindSystemFont();

  const float cellSize = kFontCellSize * mScale;
  const float advance = kFontAdvance * mScale;
  while (*text != 0) {
    const char character = *text;
    fn_804BAB64(256, 256, x, y, (character % 16) * 16, character & 0xf0, 16, 16,
                static_cast<int>(cellSize), static_cast<int>(cellSize), &color);
    ++text;
    x += static_cast<int>(advance);
  }

  fn_804BADE4(&state);
}

int CFont::CharsWidth(const char*, unsigned int length) const {
  return length * static_cast<int>(kFontAdvance * mScale);
}

int CFont::StringWidth(const char* text) const {
  int width = 0;
  while (*text != 0) {
    ++text;
    width += static_cast<int>(kFontAdvance * mScale);
  }
  return width;
}

int CFont::CharWidth(char) const { return static_cast<int>(kFontAdvance * mScale); }

CFont::~CFont() {}

CFont::CFont(const CFont& other) : mFontSize(other.mFontSize), mScale(other.mScale) {}

CFont::CFont(float scale) : mFontSize(static_cast<int>(kFontCellSize * scale)), mScale(scale) {
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
  for (int row = 0; row < 4; ++row) {
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
    destination[3] = source[3];
    destination[4] = source[4];
    destination[5] = source[5];
    destination[6] = source[6];
    destination[7] = source[7];
    destination += 8;
    source += 256;
  }
}
