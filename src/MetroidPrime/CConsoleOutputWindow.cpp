// NonMatching translation-unit scaffold.
// G2MEAB .text 0x800DD114..0x800DDAEC (end exclusive).
// Complete native/helper inventory: 14 functions; implementation remains pending.
// Boundary evidence is retained outside this repository in the agent workflow.
// Not yet implemented:
// 0x800DD180 Draw: also draws a "(DVD) (ARAM)" busy status line; depends on unnamed CGameDebug
//   option state (0x8003BF38, object+0x2CB4) and an unnamed CDvdFile busy flag.
// 0x800DD3B8 OnMessage: needs MakeMsg::GetParmTimerTick / CArchMsgParmReal32 (Decode.cpp).
// 0x800DDABC static initializer: seven SDA constants (-1,-1,-1,0,1,2,-1) from a shared header.
#include "MetroidPrime/CConsoleOutputWindow.hpp"

#include "rstl/math.hpp"

#include <stdarg.h>
#include <stdio.h>

CConsoleOutputWindow* CConsoleOutputWindow::mInstance = nullptr;
bool CConsoleOutputWindow::sEnabled = true;

void CConsoleOutputWindow::Printf(const char* format, ...) {
  char buffer[0x200];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  if (mInstance != nullptr) {
    mInstance->AddString(buffer);
  }
}

CConsoleOutputWindow::CConsoleOutputWindow(int lineCount, float lineDuration, float fontScale)
: CIOWin(rstl::string_l("ConsoleOutputWindow"))
, mFont(fontScale)
, mLineDuration(lineDuration)
, mCharsPerLine(632.f / mFont.CharWidth('0'))
, mLineIndex(0)
, mColumn(0) {
  mLines.reserve(lineCount);
  mLineTimers.reserve(lineCount);
  for (int i = 0; i < lineCount; ++i) {
    mLines.push_back(rstl::string("", mCharsPerLine + 1));
    mLineTimers.push_back(0.f);
  }
  mInstance = this;
}

CConsoleOutputWindow::~CConsoleOutputWindow() { mInstance = nullptr; }

void CConsoleOutputWindow::AddString(const char* text) {
  if (IsEnabled()) {
    for (; *text != '\0'; ++text) {
      AddChar(*text);
    }
  }
}

void CConsoleOutputWindow::AddChar(char c) {
  if (mColumn == mCharsPerLine) {
    NewLine();
  }

  switch (c) {
  case '\t':
    for (int i = 0; i < 3; ++i) {
      AddChar(' ');
    }
    break;
  case '\n':
    NewLine();
    break;
  default:
    mLines[mLineIndex].append(1, c);
    ++mColumn;
    break;
  }
}

void CConsoleOutputWindow::NewLine() {
  mLineTimers[mLineIndex] = mLineDuration;
  mLineIndex = (mLineIndex + 1) % mLines.size();
  mColumn = 0;
  mLines[mLineIndex].clear();
  AddChar(' ');
}

void CConsoleOutputWindow::Update(float dt) {
  for (int i = 0; i < mLines.size(); ++i) {
    mLineTimers[i] = rstl::max_val(0.f, mLineTimers[i] - dt);
  }
}

bool CConsoleOutputWindow::IsEnabled() { return sEnabled; }

void CConsoleOutputWindow::SetEnabled(bool enabled) {
  sEnabled = enabled;
  if (!enabled && mInstance != nullptr) {
    const int lineCount = mInstance->mLines.size();
    for (int i = 0; i < lineCount; ++i) {
      mInstance->NewLine();
    }
  }
}
