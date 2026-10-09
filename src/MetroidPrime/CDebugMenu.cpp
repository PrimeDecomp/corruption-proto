// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x800492C8..0x80049F68 (8 native functions).
// Source identity: inferred descriptive basename; original target filename unknown and family
// absent from both references. Complete native/helper/callback inventory retained; no speculative
// declarations. Not yet implemented:
// 0x800492C8 +0x868: Draw; standalone debug menu renderer; title/rows, pulse colors, CFont,
// left/right screen bounds. Needs the renderer interface behind 0x80799290 (vtable slots
// 0x3C/0x74/0xAC/0xB4/0xBC/0xC4: model matrix, primitive begin/vertex/colour/end), which has no
// Corruption header yet.
// 0x80049F38 +0x30: registered menu static initializer; raw native and .ctors8065B50C. Stores
// -1, -1, -1, 0, 1, 2, -1 into seven .sbss words; the same initializer is emitted in many other
// units (for example CActor 0x80037158), so it comes from a shared header that is not known yet.

#include "MetroidPrime/CDebugMenu.hpp"

#include "Kyoto/Input/CFinalInput.hpp"

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
