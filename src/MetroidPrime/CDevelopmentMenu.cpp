// NonMatching translation-unit scaffold.
// G2MEAB .text 0x801DA6D8..0x801DB8F0; 29 retained native functions.
// Inferred development-menu emitter; original filename is unknown.
// Constructors and destructor remain in CFrontEndUIDevelopment.
// Preserve iterator/pointer swap variants and all sort/vector helpers.

#include "MetroidPrime/CDevelopmentMenu.hpp"

#include "Kyoto/Basics/COsContext.hpp"
#include "Kyoto/Text/CFont.hpp"
#include "MetroidPrime/CMain.hpp"

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

// The development keyboard drives the menu as well as the pad.
static inline bool IsKeyJustPressed(int key) {
  return gpMain->GetOsContext()->GetOsKeyState(key).JustPressed();
}

bool CDevelopmentMenu::SEntry::ProcessInput(const CControlMapper& mapper,
                                            const CFinalInput& input) {
  bool changed = false;
  if (mapper.GetPressInput(CControlMapper::kC_DebugMenuRight, input,
                           CControlMapper::kFT_Unfiltered) ||
      IsKeyJustPressed(7) || IsKeyJustPressed(0x1d)) {
    ++mSelectedChoice;
    if (mChoices.size() != 0) {
      mSelectedChoice %= mChoices.size();
    }
    changed = true;
  }
  // The previous choice is on DebugMenuSelect, not DebugMenuLeft.
  if (mapper.GetPressInput(CControlMapper::kC_DebugMenuSelect, input,
                           CControlMapper::kFT_Unfiltered) ||
      (IsKeyJustPressed(0x1b) && mChoices.size() != 0)) {
    --mSelectedChoice;
    if (mSelectedChoice < 0) {
      mSelectedChoice = mChoices.size() - 1;
    }
    changed = true;
  }
  return changed;
}

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

bool CDevelopmentMenu::ProcessInput(const CFinalInput& input) {
  int numEntries = 0;
  for (int i = 0; i < mEntries.size(); ++i) {
    ++numEntries;
  }
  if (numEntries == 0) {
    return false;
  }

  bool changed = false;
  if (mControlMapper.GetPressInput(CControlMapper::kC_DebugMenuUp, input,
                                   CControlMapper::kFT_Unfiltered) ||
      IsKeyJustPressed(0x1c)) {
    changed = true;
    do {
      mSelection = mSelection - 1;
      mSelection = mSelection + mEntries.size();
      mSelection = mSelection % mEntries.size();
    } while (!mEntries[mSelection].mVisible);
  }
  if (mControlMapper.GetPressInput(CControlMapper::kC_DebugMenuDown, input,
                                   CControlMapper::kFT_Unfiltered) ||
      IsKeyJustPressed(0x1e)) {
    changed = true;
    do {
      mSelection = mSelection + 1;
      mSelection = mSelection % mEntries.size();
    } while (!mEntries[mSelection].mVisible);
  }
  changed |= mEntries[mSelection].ProcessInput(mControlMapper, input);
  return changed;
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
