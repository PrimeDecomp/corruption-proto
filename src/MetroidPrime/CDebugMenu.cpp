// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x800492C8..0x80049F68 (8 native functions).
// Source identity: inferred descriptive basename; original target filename unknown and family
// absent from both references. Complete native/helper/callback inventory retained; no speculative
// declarations. Not yet implemented:
// 0x800492C8 +0x868: Draw; standalone debug menu renderer; title/rows, pulse colors, CFont,
// left/right screen bounds. Needs the renderer interface behind 0x80799290 (vtable slots
// 0x3C/0x74/0xAC/0xB4/0xBC/0xC4: model matrix, primitive begin/vertex/colour/end), which has no
// Corruption header yet.
// 0x80049B40 +0x1E4: menu input selection returns command/item pair and
// uses local debounce helper. GetDigitalInput 0x4F/0x50 picks the scroll direction (resetting
// the repeat timer to 0.3s before the next step); GetPressInput 0x4E/0x53/0x54 returns
// (item id, index), otherwise the invalid pair at 0x80795950 (-1, 0).
// 0x80049DB8 +0xEC: menu constructor; title/entries/CFont/embedded CControlMapper and edge state
// 0x80049EA4 +0x94: menu input debounce helper; true when none of the GetPressInput commands
// 0x53/0x4E/0x54 is pressed
// The three functions above need CControlMapper, which has no Corruption header yet.
// 0x80049F38 +0x30: registered menu static initializer; raw native and .ctors8065B50C. Stores
// -1, -1, -1, 0, 1, 2, -1 into seven .sbss words; the same initializer is emitted in many other
// units (for example CActor 0x80037158), so it comes from a shared header that is not known yet.

#include "MetroidPrime/CDebugMenu.hpp"

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

int CDebugMenu::GetLeftEdge() const { return mLeftEdge; }

int CDebugMenu::GetRightEdge() const { return mRightEdge; }
