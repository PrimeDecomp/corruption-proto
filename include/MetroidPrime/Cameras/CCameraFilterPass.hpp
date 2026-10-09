#ifndef _CCAMERAFILTERPASS
#define _CCAMERAFILTERPASS

#include "types.h"

class CColor;
class CTexture;

// Minimal declaration; Echoes' camera filter pass, whose methods sit in CCameraFilter.cpp
// (0x800C21A4..0x800C47E4).
class CCameraFilterPass {
public:
  // Echoes' values.
  enum EFilterType {
    kFT_Passthru,
    kFT_Multiply,
    kFT_Invert,
    kFT_Add,
    kFT_Subtract,
    kFT_Blend,
    kFT_Widescreen,
    kFT_SceneAdd,
    kFT_NoColor,
    kFT_InvDstMultiply,
  };
  enum EFilterShape {
    kFS_Fullscreen,
    kFS_FullscreenHalvesLeftRight,
    kFS_FullscreenHalvesTopBottom,
    kFS_FullscreenQuarters,
    kFS_CinemaBars,
    kFS_ScanLinesEven,
    kFS_ScanLinesOdd,
    kFS_RandomStatic,
    kFS_DialogBox,
    kFS_CinematicPlaceholderLabel,
    kFS_CookieCutterDepthRandomStatic,
  };

  // Echoes' name and signature (0x800C2F04). CMFGame darkens the screen with it while it skips
  // a cinematic and after a layer change.
  static void DrawFilter(EFilterType type, EFilterShape shape, const CColor& color,
                         const CTexture* texture, float lod);
};

#endif // _CCAMERAFILTERPASS
