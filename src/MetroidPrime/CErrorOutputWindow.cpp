// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x801A6734..0x801A71B0 (10 native functions).
// Source identity: asserted original basename; architecture error-output window.
// Complete proposed native interval retained; historical source arrangement inferred.
#include "MetroidPrime/CErrorOutputWindow.hpp"

#include "MetroidPrime/CArchitectureMessage.hpp"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Audio/CStreamAudioManager.hpp"
#include "Kyoto/Basics/CCast.hpp"
#include "Kyoto/CMemoryCardSys.hpp"
#include "Kyoto/Graphics/CGraphics.hpp"
#include "Kyoto/Graphics/CMoviePlayer.hpp"
#include "Kyoto/Input/IController.hpp"
#include "Kyoto/Math/CTransform4f.hpp"
#include "Kyoto/Math/CUnitVector3f.hpp"
#include "Kyoto/Math/CVector2f.hpp"
#include "Kyoto/Math/CVector3f.hpp"
#include "Kyoto/Text/CTextExecuteBuffer.hpp"
#include "Kyoto/Text/CTextRenderBuffer.hpp"
#include "MetaRender/CCubeRenderer.hpp"

#include "rstl/pair.hpp"

#include "dolphin/dvd.h"

// No Corruption header declares the controller global yet.
extern IController* gpController;
// 0x80799294; set up by main (NewMain). Echoes/Prime declare it in CGameGlobalObjects.hpp.
extern const TToken< CRasterFont >* gpDefaultFont;

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

// Guessed name. The tail of Echoes' DrawError: renders the text in screen space.
static void RenderErrorText(const CTextExecuteBuffer& buffer, CColor color) {
  CViewport viewport = CGraphics::GetViewport();
  const float top = CCast::LtoF(viewport.mTop);
  const float bottom = CCast::LtoF(viewport.mTop + viewport.mHeight);
  CGraphics::SetViewPointMatrix(CTransform4f::Identity());
  CGraphics::SetOrtho(viewport.mLeft, viewport.mLeft + viewport.mWidth, bottom, top, -4096.f,
                      4096.f);
  CGraphics::SetBlendMode(kBM_Blend, kBF_SrcAlpha, kBF_InvSrcAlpha, kLO_Clear);
  CGraphics::SetCullMode(kCM_None);
  CGraphics::SetDepthWriteMode(true, kE_Always, false);
  CGraphics::SetAlphaCompare(kAF_Always, 0, kAO_And, kAF_Always, 0);
  const CTransform4f xf =
      CTransform4f::FromColumns(CVector3f::Right(), CVector3f::Forward(), CVector3f::Down(),
                                CVector3f(0.f, 0.f, viewport.mHeight));
  CGraphics::SetModelMatrix(xf);
  buffer.BuildRenderBuffer().Render(color, 0.f);
  CGraphics::SetCullMode(kCM_Front);
}

// Guessed name. A local version of Echoes' CCubeRenderer::SetViewportOrtho(true, -4096, 4096):
// a viewport-centred orthographic projection. Returns the min and max corners.
static rstl::pair< CVector2f, CVector2f > SetCenteredViewportOrtho() {
  const CViewport& viewport = CGraphics::GetViewport();
  const int halfWidth = viewport.mWidth / 2;
  const int halfHeight = viewport.mHeight / 2;
  const float left = -halfWidth;
  const float bottom = -halfHeight;
  const float right = halfWidth;
  const float top = halfHeight;
  CGraphics::SetOrtho(left, right, top, bottom, -4096.f, 4096.f);
  CGraphics::SetViewPointMatrix(CTransform4f::Identity());
  CGraphics::SetModelMatrix(CTransform4f::Identity());
  return rstl::pair< CVector2f, CVector2f >(CVector2f(left, bottom), CVector2f(right, top));
}

// Unlike Echoes, the black backdrop is always drawn (no audio flag check) and is coloured with
// the kEnvConstColor TEV pass instead of a primitive colour.
void CErrorOutputWindow::DrawError() const {
  RS_VERIFY_THROW(356, mErrorString != NULL, false, "No error string?!");
  if (mErrorString == nullptr) {
    return;
  }

  CViewport viewport = CGraphics::GetViewport();
  CTextExecuteBuffer execBuffer;
  execBuffer.AddWordWrapping(true);
  execBuffer.BeginBlock(0, 0, viewport.mWidth, viewport.mHeight, false, kTD_Horizontal,
                        kJustification_Center, kVerticalJustification_Center);
  execBuffer.AddFont(TToken< CRasterFont >(*gpDefaultFont));
  execBuffer.AddString(rstl::wstring_l(mErrorString));
  execBuffer.EndBlock();

  CGraphics::SetBlendMode(kBM_Blend, kBF_SrcAlpha, kBF_InvSrcAlpha, kLO_Clear);
  const rstl::pair< CVector2f, CVector2f > corners = SetCenteredViewportOrtho();
  const CVector2f& lt = corners.first;
  const CVector2f& rb = corners.second;
  CGraphics::SetDepthWriteMode(false, kE_Always, false);
  CGraphics::SetTevOp(kTS_Stage0, CGraphics::kEnvConstColor);
  CGraphics::SetTevOp(kTS_Stage1, CGraphics::kEnvPassthru);
  CGraphics::SetTevRegisterColor(0, CColor::Black());
  CGraphics::StreamBegin(kP_TriangleStrip);
  CGraphics::StreamVertex(CVector3f(lt.GetX() - 1.f, 0.f, 1.f + rb.GetY()));
  CGraphics::StreamVertex(CVector3f(lt.GetX() - 1.f, 0.f, lt.GetY() - 1.f));
  CGraphics::StreamVertex(CVector3f(1.f + rb.GetX(), 0.f, 1.f + rb.GetY()));
  CGraphics::StreamVertex(CVector3f(1.f + rb.GetX(), 0.f, lt.GetY() - 1.f));
  CGraphics::StreamEnd();

  RenderErrorText(execBuffer, CColor::White());
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
