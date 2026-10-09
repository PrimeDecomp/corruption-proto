// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x801A6734..0x801A71B0 (10 native functions).
// Source identity: asserted original basename; architecture error-output window.
// Complete proposed native interval retained; historical source arrangement inferred.
// Not yet implemented (DrawError and its two local helpers; they need the unnamed default font
// global 0x80799294 and two unnamed CTevCombiners passes 0x80782418 / *0x80796CB0):
// 0x801A68C4 +0x2B4: error text draw; original source line 356
// 0x801A6B78 +0x164: emitted native method/helper; original source-level symbol unresolved
// 0x801A6CDC +0x204: emitted native method/helper; original source-level symbol unresolved
#include "MetroidPrime/CErrorOutputWindow.hpp"

#include "MetroidPrime/CArchitectureMessage.hpp"

#include "Kyoto/Audio/CStreamAudioManager.hpp"
#include "Kyoto/CMemoryCardSys.hpp"
#include "Kyoto/Graphics/CMoviePlayer.hpp"
#include "Kyoto/Input/IController.hpp"
#include "MetaRender/CCubeRenderer.hpp"

#include "dolphin/dvd.h"

// No Corruption header declares the controller global yet.
extern IController* gpController;

CErrorOutputWindow::CErrorOutputWindow(EFlag flag)
: CIOWin(rstl::string_l("Error output window"))
, mState(kS_Zero)
, x18_24_(true)
, x18_25_(true)
, x18_26_(true)
, x18_27_(flag == kF_Zero)
, mErrorString(nullptr) {}

bool CErrorOutputWindow::GetIsContinueDraw() const { return mState != kS_One; }

CIOWin::EMessageReturn CErrorOutputWindow::OnMessage(const CArchitectureMessage& msg,
                                                     CArchitectureQueue&) {
  switch (msg.GetType()) {
  case kAM_UserInput:
    return mState != kS_Zero ? kMR_Exit : kMR_Normal;

  case kAM_FrameBegin:
    UpdateWindow();
    // fallthrough

  case kAM_TimerTick:
  case kAM_FrameEnd:
    return mState != kS_Zero ? kMR_Exit : kMR_Normal;
  default:
    break;
  }
  return kMR_Normal;
}

void CErrorOutputWindow::UpdateWindow() {
  int driveStatus = DVDGetDriveStatus();
  const wchar_t* errMsg = nullptr;
  bool showError = mState != kS_Zero;
  if (CMemoryCardSys::mIsCardBusy) {
    driveStatus = 0;
  }
  static int sLastDvdStatus = 0;
  if (driveStatus != sLastDvdStatus) {
    sLastDvdStatus = driveStatus;
  }
  switch (driveStatus) {
  case 5:
    errMsg = L"The Disc Cover is open.\nIf you want to continue the game,\nplease close the Disc "
             L"Cover.";
    break;
  case 4:
  case 6:
    // The prototype still asks for the Echoes disc.
    errMsg = L"Please insert the\nMetroid Prime 2 Echoes Game Disc.";
    break;
  case 0xb:
    errMsg = L"The Game Disc could not be read.\nPlease read the Nintendo GameCube\nInstruction "
             L"Booklet\nfor more information.";
    break;
  default:
    break;
  }
  if (driveStatus != 2 && driveStatus != 1) {
    showError = errMsg != nullptr;
    if (errMsg != nullptr) {
      mErrorString = errMsg;
    }
  }
  if (!showError) {
    if (mState != kS_Zero) {
      SetState(kS_Zero);
    }
  } else {
    SetState(kS_One);
  }
}

void CErrorOutputWindow::Draw() const {
  switch (mState) {
  case kS_Zero:
    break;
  case kS_One:
    DrawError();
    if (gpRender != nullptr) {
      gpRender->SetRequestRGBA6(true);
    }
    break;
  }
}

void CErrorOutputWindow::SetState(EState state) {
  if (state != kS_Zero && gpController != nullptr) {
    for (int i = 0; i < 4; ++i) {
      gpController->SetMotorState(static_cast< EIOPort >(i), kMS_Stop);
    }
  }

  if (state != mState) {
    if (state != kS_Zero) {
      if (gpRender != nullptr) {
        gpRender->SetRequestRGBA6(true);
      }
      if (x18_27_) {
        x18_25_ = CStreamAudioManager::GetMusicUnmute();
        x18_26_ = CStreamAudioManager::GetSfxUnmute();
        x18_24_ = CMoviePlayer::GetAudioEnabled();
        CStreamAudioManager::SetMusicUnmute(false);
        CStreamAudioManager::SetSfxUnmute(false);
        CMoviePlayer::SetAudioEnabled(false);
      }
    } else if (x18_27_) {
      CStreamAudioManager::SetMusicUnmute(x18_25_);
      CStreamAudioManager::SetSfxUnmute(x18_26_);
      CMoviePlayer::SetAudioEnabled(x18_24_);
    }
    mState = state;
  }
}
