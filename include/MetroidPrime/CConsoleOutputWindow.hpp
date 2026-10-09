#ifndef _CCONSOLEOUTPUTWINDOW
#define _CCONSOLEOUTPUTWINDOW

#include "MetroidPrime/CIOWin.hpp"

#include "Kyoto/Text/CFont.hpp"

#include "rstl/vector.hpp"

class CConsoleOutputWindow : public CIOWin {
public:
  CConsoleOutputWindow(int lineCount, float lineDuration, float fontScale);

  // CIOWin
  ~CConsoleOutputWindow() override;
  EMessageReturn OnMessage(const CArchitectureMessage&, CArchitectureQueue&) override;
  void Draw() const override;

  void Update(float dt);
  void NewLine();                   // Guessed name
  void AddChar(char c);             // Guessed name
  void AddString(const char* text); // Guessed name

  // G2MEAB-only console switch; disabling it scrolls every line out of the window.
  static void SetEnabled(bool enabled); // Guessed name
  static bool IsEnabled();              // Guessed name

  // printf-style entry point; installed as a text callback by main and CGameDebug.
  static void Printf(const char* format, ...); // Guessed name

private:
  static CConsoleOutputWindow* mInstance;
  static bool sEnabled; // Guessed name

  CFont mFont;
  float mLineDuration; // Guessed name; seconds a new line stays visible
  rstl::vector< rstl::string > mLines;
  rstl::vector< float > mLineTimers;
  int mCharsPerLine;
  int mLineIndex;
  int mColumn; // Guessed name
};
CHECK_SIZEOF(CConsoleOutputWindow, 0x4c)

#endif // _CCONSOLEOUTPUTWINDOW
