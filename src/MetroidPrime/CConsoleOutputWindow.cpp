// NonMatching translation-unit scaffold.
// G2MEAB .text 0x800DD114..0x800DDAEC (end exclusive).
// Complete native/helper inventory: 14 functions; implementation remains pending.
// Boundary evidence is retained outside this repository in the agent workflow.
// Not yet implemented:
// 0x800DDABC static initializer: seven SDA constants (-1,-1,-1,0,1,2,-1) from a shared header.
#include "MetroidPrime/CConsoleOutputWindow.hpp"

#include "MetroidPrime/CArchitectureMessage.hpp"
#include "MetroidPrime/CGameDebug.hpp"

#include "Kyoto/CDvdFile.hpp"
#include "Kyoto/Graphics/CGraphics.hpp"

#include "rstl/math.hpp"

#include "dolphin/ar.h"
#include "dolphin/dvd.h"

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

CIOWin::EMessageReturn CConsoleOutputWindow::OnMessage(const CArchitectureMessage& msg,
                                                       CArchitectureQueue&) {
  switch (msg.GetType()) {
  case kAM_UserInput:
    return kMR_Normal;
  case kAM_TimerTick:
    Update(MakeMsg::GetParmTimerTick(msg).GetReal());
    return kMR_Normal;
  default:
    return kMR_Normal;
  }
}

void CConsoleOutputWindow::Update(float dt) {
  for (int i = 0; i < mLines.size(); ++i) {
    mLineTimers[i] = rstl::max_val(0.f, mLineTimers[i] - dt);
  }
}

// Unlike Echoes, G2MEAB draws in the "Debug Message Color" debug option colour, tests the line
// timer before drawing the newest line, and adds a DVD/ARAM activity indicator at the bottom.
void CConsoleOutputWindow::Draw() const {
  int row = 0;
  const int startIndex = (mLineIndex - 1 + mLines.size()) % mLines.size();
  int index = startIndex;
  const CColor color = gpGameDebug->GetDebugMessageColor();
  CGraphics::SetDepthRange(0.f, 1.f);
  CGraphics::SetBlendMode(kBM_Blend, kBF_SrcAlpha, kBF_InvSrcAlpha, kLO_Clear);

  if (startIndex >= 0 && startIndex < mLines.size()) {
    const int lineCount = mLines.size();
    while (mLineTimers[index] > 0.f && row < lineCount) {
      mFont.DrawString(mLines[index].c_str(), 18, row * (mFont.GetFontSize() + 2) + 12, color);
      index = (index - 1 + lineCount) % lineCount;
      ++row;
    }
  }

  const int driveStatus = DVDGetDriveStatus();
  const u32 aramStatus = ARGetDMAStatus();
  const char* activity = nullptr;
  if (driveStatus == DVD_STATE_BUSY) {
    CDvdFile::mDvdActivity = true;
  }
  if (CDvdFile::mDvdActivity && aramStatus != 0) {
    activity = "(DVD) (ARAM)";
  } else if (CDvdFile::mDvdActivity) {
    activity = "(DVD)";
  } else if (aramStatus != 0) {
    activity = "(ARAM)";
  }
  CDvdFile::mDvdActivity = false;

  if (activity != nullptr && gpGameDebug->IsOptionSet(CGameDebug::kDO_DebugMessagesEnabled)) {
    mFont.DrawString(activity, 18, CGraphics::GetViewport().mHeight - mFont.GetFontSize() * 2,
                     color);
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
