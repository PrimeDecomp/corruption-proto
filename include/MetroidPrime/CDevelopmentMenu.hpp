#ifndef _CDEVELOPMENTMENU
#define _CDEVELOPMENTMENU

#include "types.h"

#include "Kyoto/Graphics/CColor.hpp"
#include "Kyoto/Math/CVector2i.hpp"

#include "rstl/string.hpp"
#include "rstl/vector.hpp"

// Guessed name (the unit's inferred basename). The text menu of the development front end
// (CFrontEndUIDevelopment keeps one at 0x4C and builds it, including the world/area/layer
// pickers). Each entry is a line of debug-font text with an optional list of choices that the
// player cycles through with left/right; up/down move between the visible entries. Both the pad
// (through a CControlMapper) and the development keyboard (COsContext_GetKeyState) drive it.
// Entries are kept sorted by sort key, then by name. The constructor and destructor are emitted
// in CFrontEndUIDevelopment. No Echoes/Prime equivalent is known.
class CDevelopmentMenu {
public:
  // Guessed name. One value an entry can take (0x24 bytes); its name is appended to the entry's.
  struct SChoice {
    rstl::string mName;
    rstl::string x10_;
    uint x20_;
  };

  // Guessed name. One line of the menu (0x44 bytes).
  struct SEntry {
    // Guessed names.
    void AddChoice(const SChoice& choice);
    void Draw(const CVector2i& pos, const CColor& color) const;
    void Activate();

    rstl::string mName;
    rstl::string x10_;
    uint mId;
    rstl::vector< SChoice > mChoices;
    int mSelectedChoice;
    int mSortKey;
    float mFontScale;
    bool mVisible;
  };

  // Guessed names.
  SChoice& GetSelectedChoice(uint id);
  int FindEntry(uint id) const;
  void RemoveEntries(const rstl::vector< SEntry >& entries);
  void AddEntries(const rstl::vector< SEntry >& entries);
  void RemoveEntry(int index);
  void AddEntry(const SEntry& entry);
  void Draw(const CVector2i& pos, const CColor& selectedColor, const CColor& color) const;
  void ActivateSelection();

private:
  int x0_;
  rstl::vector< SEntry > mEntries;
  int mSelection;
  // CControlMapper; queried with GetPressInput for the menu commands 0x4F..0x53. There is no
  // Corruption CControlMapper header yet.
  uchar x18_controlMapper[0x104];
};

// Entries sort by key, then alphabetically.
bool operator<(const CDevelopmentMenu::SEntry& a, const CDevelopmentMenu::SEntry& b);

#endif // _CDEVELOPMENTMENU
