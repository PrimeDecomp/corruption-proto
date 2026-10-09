// NonMatching translation-unit scaffold.
// G2MEAB .text 0x801DA6D8..0x801DB8F0; 29 retained native functions.
// Inferred development-menu emitter; original filename is unknown.
// Constructors and destructor remain in CFrontEndUIDevelopment.
// Preserve iterator/pointer swap variants and all sort/vector helpers.
// Not yet implemented (both need CControlMapper, which has no Corruption header yet):
// 0x801DABC4 +0x22C: menu input. Returns false when the menu is empty; GetPressInput 0x4F or
//   key 0x1C moves the selection up and 0x50 or key 0x1E moves it down, skipping hidden
//   entries and wrapping; then the selected entry handles its own input. Returns whether
//   anything changed.
// 0x801DAFF8 +0x1C0: entry input. GetPressInput 0x52 or keys 0x07/0x1D step to the next choice
//   (wrapping), 0x53 or key 0x1B to the previous one.

#include "MetroidPrime/CDevelopmentMenu.hpp"

#include "Kyoto/Text/CFont.hpp"

#include "rstl/algorithm.hpp"

// Guessed name. The 16-pixel debug font line height, kept next to the CFont constants.
extern const int kFontLineHeight;

bool operator<(const CDevelopmentMenu::SEntry& a, const CDevelopmentMenu::SEntry& b) {
  if (a.mSortKey == b.mSortKey) {
    return a.mName < b.mName;
  }
  return a.mSortKey < b.mSortKey;
}

void CDevelopmentMenu::SEntry::Activate() {}

void CDevelopmentMenu::SEntry::Draw(const CVector2i& pos, const CColor& color) const {
  CFont font(mFontScale);
  rstl::string text(mName);
  if (mChoices.size() != 0) {
    text.append(mChoices[mSelectedChoice].mName);
  }
  font.DrawString(text.data(), pos.GetX(), pos.GetY(), color);
}

void CDevelopmentMenu::SEntry::AddChoice(const SChoice& choice) {
  if (mChoices.size() == mChoices.capacity()) {
    mChoices.reserve(mChoices.size() == 0 ? 1 : mChoices.size() * 2);
  }
  mChoices.push_back(choice);
}

// The first entry is never activated (the check is > 0, not >= 0). SEntry::Activate is empty.
void CDevelopmentMenu::ActivateSelection() {
  if (mSelection > 0 && mSelection < mEntries.size()) {
    mEntries[mSelection].Activate();
  }
}

void CDevelopmentMenu::Draw(const CVector2i& pos, const CColor& selectedColor,
                            const CColor& color) const {
  CVector2i cursor = pos;
  for (int i = 0; i < mEntries.size(); ++i) {
    if (mEntries[i].mVisible) {
      CColor entryColor = i == mSelection ? selectedColor : color;
      mEntries[i].Draw(cursor, entryColor);
      cursor.SetY(cursor.GetY() -
                  static_cast< int >(1.2f * kFontLineHeight * mEntries[i].mFontScale));
    }
  }
}

void CDevelopmentMenu::AddEntry(const SEntry& entry) {
  if (mEntries.size() == mEntries.capacity()) {
    mEntries.reserve(mEntries.size() == 0 ? 1 : mEntries.size() * 2);
  }
  mEntries.push_back(entry);
  rstl::sort(mEntries.begin(), mEntries.end());
}

void CDevelopmentMenu::RemoveEntry(int index) { mEntries.erase(mEntries.begin() + index); }

// Adds every entry whose id is not in the menu yet.
void CDevelopmentMenu::AddEntries(const rstl::vector< SEntry >& entries) {
  for (int i = 0; i < entries.size(); ++i) {
    const SEntry& entry = entries[i];
    if (FindEntry(entry.mId) == -1) {
      AddEntry(entry);
    }
  }
}

// Removes every entry whose id is in the menu.
void CDevelopmentMenu::RemoveEntries(const rstl::vector< SEntry >& entries) {
  for (int i = 0; i < entries.size(); ++i) {
    int index = FindEntry(entries[i].mId);
    if (index != -1) {
      RemoveEntry(index);
    }
  }
}

int CDevelopmentMenu::FindEntry(uint id) const {
  for (int i = 0; i < mEntries.size(); ++i) {
    if (mEntries[i].mId == id) {
      return i;
    }
  }
  return -1;
}

CDevelopmentMenu::SChoice& CDevelopmentMenu::GetSelectedChoice(uint id) {
  SEntry& entry = mEntries[FindEntry(id)];
  return entry.mChoices[entry.mSelectedChoice];
}
