// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x800492C8..0x80049F68 (8 native functions).
// Source identity: inferred descriptive basename; original target filename unknown and family
// absent from both references. All eight functions are implemented. The static initializer
// (0x80049F38, -1, -1, -1, 0, 1, 2, -1 into seven .sbss words) is the compiler-emitted one for
// the TGameTypes.hpp ids.
// Draw (0x800492C8) differs from the target in register allocation, and its model matrix call
// goes through slot 0x40 instead of 0x3C: IRenderer.hpp has one virtual too many before
// SetModelMatrix compared with the prototype's CCubeRenderer vtable (0x806E2180).

#include "MetroidPrime/CDebugMenu.hpp"

#include "Kyoto/Graphics/CGraphics.hpp"
#include "Kyoto/Input/CFinalInput.hpp"
#include "Kyoto/Math/CAbsAngle.hpp"
#include "Kyoto/Math/CTransform4f.hpp"
#include "Kyoto/Math/CVector3f.hpp"
#include "MetaRender/CCubeRenderer.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include <math.h>

int CDebugMenu::kNoItem = -1;

bool CDebugMenu::AreSelectButtonsReleased(const CFinalInput& input) {
  if (mControlMapper.GetPressInput(CControlMapper::kC_DebugMenuSelect, input,
                                   CControlMapper::kFT_Unfiltered) ||
      mControlMapper.GetPressInput(CControlMapper::kC_DebugMenuStart, input,
                                   CControlMapper::kFT_Unfiltered) ||
      mControlMapper.GetPressInput(CControlMapper::kC_DebugMenuBack, input,
                                   CControlMapper::kFT_Unfiltered)) {
    return false;
  }
  return true;
}

CDebugMenu::CDebugMenu(const rstl::string& title, float fontScale, const SItem* items,
                       int itemCount, int selection, float lineSpacing)
: mTitle(title)
, mItems(items)
, mItemCount(itemCount)
, mSelection(selection < 0 ? 0 : (itemCount - 1 < selection ? itemCount - 1 : selection))
, mTime(0.f)
, mLastRepeatTime(-0.3f)
, mScrollDirection(kSD_None)
, x28_inputArmed(0)
, mFont(fontScale)
, mLineSpacing(lineSpacing)
, mLeftEdge(0)
, mRightEdge(0) {}

void CDebugMenu::Update(float dt) {
  mTime += dt;
  if (mTime - mLastRepeatTime > 0.3f) {
    mLastRepeatTime = mTime;
    switch (mScrollDirection) {
    case kSD_Up:
      if (mSelection == 0) {
        mSelection = mItemCount - 1;
      } else {
        mSelection = mSelection - 1;
      }
      break;
    case kSD_Down:
      if (mSelection == mItemCount - 1) {
        mSelection = 0;
      } else {
        mSelection = mSelection + 1;
      }
      break;
    }
  }
}

rstl::pair< int, int > CDebugMenu::ProcessInput(const CFinalInput& input) {
  if (input.ControllerNumber() != 0) {
    return rstl::pair< int, int >(kNoItem, kNoItem);
  }
  if (!x28_inputArmed) {
    // Ignore the press that opened the menu.
    if (!AreSelectButtonsReleased(input)) {
      return rstl::pair< int, int >(kNoItem, kNoItem);
    }
    x28_inputArmed = 1;
  }
  if (mControlMapper.GetDigitalInput(CControlMapper::kC_DebugMenuUp, input,
                                     CControlMapper::kFT_Unfiltered) &&
      mScrollDirection != kSD_Up) {
    // Step on the next Update.
    mScrollDirection = kSD_Up;
    mLastRepeatTime = mTime - 0.3f;
  }
  if (mControlMapper.GetDigitalInput(CControlMapper::kC_DebugMenuDown, input,
                                     CControlMapper::kFT_Unfiltered) &&
      mScrollDirection != kSD_Down) {
    mScrollDirection = kSD_Down;
    mLastRepeatTime = mTime - 0.3f;
  }
  if (!mControlMapper.GetDigitalInput(CControlMapper::kC_DebugMenuUp, input,
                                      CControlMapper::kFT_Unfiltered) &&
      !mControlMapper.GetDigitalInput(CControlMapper::kC_DebugMenuDown, input,
                                      CControlMapper::kFT_Unfiltered)) {
    mScrollDirection = kSD_None;
  }
  if (mControlMapper.GetPressInput(CControlMapper::kC_DebugMenuStart, input,
                                   CControlMapper::kFT_Unfiltered) ||
      mControlMapper.GetPressInput(CControlMapper::kC_DebugMenuSelect, input,
                                   CControlMapper::kFT_Unfiltered) ||
      mControlMapper.GetPressInput(CControlMapper::kC_DebugMenuBack, input,
                                   CControlMapper::kFT_Unfiltered)) {
    return rstl::pair< int, int >(mItems[mSelection].mId, mSelection);
  }
  return rstl::pair< int, int >(kNoItem, kNoItem);
}

int CDebugMenu::GetLeftEdge() const { return mLeftEdge; }

int CDebugMenu::GetRightEdge() const { return mRightEdge; }

// Guessed name. A brightness between 0 and 0.5 that pulses over a 20 second period.
static inline float GetPulse(float t) {
  const CAbsAngle angle =
      CAbsAngle::FromDegrees(360.f * (static_cast< float >(fmod(t, 20.0)) / 20.f));
  return (1.f + static_cast< float >(sin(angle.AsRadians()))) * 0.25f;
}

void CDebugMenu::Draw() {
  static CColor sSelectedColor = CColor::Green();

  const int fontSize = mFont.GetFontSize();
  const CViewport& viewport = CGraphics::GetViewport();
  const int rowHeight = static_cast< int >(mLineSpacing * static_cast< float >(fontSize));
  const int halfRow = rowHeight / 2;
  const int totalHeight = rowHeight * (mItemCount + 2);
  const int width = viewport.mWidth;
  int y = viewport.mHeight / 2 + totalHeight / 2 - halfRow;

  int maxLength = 0;
  for (int i = 0; i < mItemCount; ++i) {
    const int length = strlen(mItems[i].mLabel);
    if (length > maxLength) {
      maxLength = length;
    }
  }
  if (static_cast< int >(mTitle.size()) + 6 > maxLength) {
    maxLength = mTitle.size() + 6;
  }
  const int x = width / 2 - (maxLength * fontSize) / 2;

  CGraphics::SetOrtho(static_cast< float >(CGraphics::GetViewport().mLeft),
                      static_cast< float >(CGraphics::GetViewport().mLeft + viewport.mWidth),
                      static_cast< float >(CGraphics::GetViewport().mTop + viewport.mHeight),
                      static_cast< float >(CGraphics::GetViewport().mTop), -1.f, 1.f);
  CGraphics::SetViewPointMatrix(CTransform4f::Identity());
  gpRender->SetModelMatrix(CTransform4f::Identity());
  gpRender->SetBlendMode_AlphaBlended();

  // Four corners pulsing out of phase.
  const float t = CGraphics::GetSecondsMod900();
  const float pulse0 = GetPulse(t);
  const float pulse1 = GetPulse(5.f + t);
  const float pulse2 = GetPulse(10.f + t);
  const float pulse3 = GetPulse(15.f + t);

  gpRender->BeginTriangleStrip(4);
  gpRender->PrimColor(pulse0, pulse0, pulse0, 0.75f);
  const int frameTop = y + rowHeight;
  const int frameLeft = x + fontSize;
  const float left = static_cast< float >(frameLeft) - static_cast< float >(fontSize * 3);
  const float bottom = static_cast< float >(frameTop) - static_cast< float >(totalHeight) -
                       static_cast< float >(halfRow);
  gpRender->PrimVertex(CVector3f(left, 0.f, bottom));
  gpRender->PrimColor(pulse1, pulse1, pulse1, 0.75f);
  const float right =
      static_cast< float >(frameLeft + 10) + static_cast< float >(fontSize * maxLength);
  gpRender->PrimVertex(CVector3f(right, 0.f, bottom));
  gpRender->PrimColor(pulse2, pulse2, pulse2, 0.75f);
  gpRender->PrimVertex(CVector3f(left, 0.f, static_cast< float >(halfRow + frameTop)));
  gpRender->PrimColor(pulse3, pulse3, pulse3, 0.75f);
  gpRender->PrimVertex(CVector3f(right, 0.f, static_cast< float >(halfRow + frameTop)));
  gpRender->EndPrimitive();

  // Row -1 is the title, drawn three times with a growing offset and half a row of extra space.
  for (int i = -1; i < mItemCount; ++i) {
    if (i == -1) {
      rstl::string title = rstl::string_l(" - ") + mTitle + rstl::string_l(" - ");
      mFont.DrawString(title.data(), x, y, CColor::White());
      mFont.DrawString(title.data(), x + 1, y + 1, CColor::White());
      mFont.DrawString(title.data(), x + 2, y + 2, CColor::White());
      y -= halfRow;
    } else if (i == mSelection) {
      mFont.DrawString("\x8D", x - fontSize, y, sSelectedColor);
      mFont.DrawString(mItems[i].mLabel, x + 1, y - 1, sSelectedColor);
      mFont.DrawString("\x88", x + fontSize * maxLength, y, sSelectedColor);
    } else {
      mFont.DrawString(mItems[i].mLabel, x, y, mItems[i].mColor);
    }
    y -= rowHeight;
  }
  mLeftEdge = x - fontSize;
  mRightEdge = x + fontSize * maxLength;
}
