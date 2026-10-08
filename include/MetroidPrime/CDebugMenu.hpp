#ifndef _CDEBUGMENU
#define _CDEBUGMENU

#include "types.h"

#include "Kyoto/Graphics/CColor.hpp"
#include "Kyoto/Text/CFont.hpp"

#include "rstl/string.hpp"

// Guessed name (the unit's inferred basename). A small self-contained debug menu: a title and a
// fixed table of rows drawn with the debug CFont over a translucent, pulsing backdrop. The title
// is drawn as " - <title> - " and the selected row is framed by two arrow glyphs (0x8D, 0x88) in
// green. Up/down scroll with auto repeat and one of three select commands picks the current row.
// No Echoes/Prime equivalent is known.
class CDebugMenu {
public:
  // Guessed name. One row: the label, the id the menu reports when the row is picked, and the
  // colour the label is drawn in (0xC bytes).
  struct SItem {
    const char* mLabel;
    int mId;
    CColor mColor;
  };

  // 0x80049DB8. The initial selection is clamped to the row range; the font scale goes to the
  // CFont and the line spacing scales the font size into the row height.
  CDebugMenu(const rstl::string& title, float fontScale, const SItem* items, int itemCount,
             int selection, float lineSpacing);

  // Guessed names.
  void Draw();                 // 0x800492C8; also records the left and right edges
  int GetRightEdge() const;    // 0x80049B30
  int GetLeftEdge() const;     // 0x80049B38
  void Update(float dt);       // 0x80049D24; held-direction auto repeat

private:
  enum EScrollDirection {
    kSD_None,
    kSD_Up,
    kSD_Down,
  };

  rstl::string mTitle;
  const SItem* mItems;
  int mItemCount;
  int mSelection;
  float mTime;
  float mLastRepeatTime;
  EScrollDirection mScrollDirection;
  int x28_inputArmed; // Set once all select commands have been released
  CFont mFont;
  float mLineSpacing;
  // CControlMapper, constructed by 0x80010F8C and queried with GetPressInput/GetDigitalInput
  // for commands 0x4E..0x54. There is no Corruption CControlMapper header yet.
  uchar x38_controlMapper[0x104];
  int mLeftEdge;
  int mRightEdge;
};
CHECK_SIZEOF(CDebugMenu, 0x144)

#endif // _CDEBUGMENU
