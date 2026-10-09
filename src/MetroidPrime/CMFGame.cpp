// NonMatching translation unit.
// G2MEAB .text: 0x80023EC8..0x800255B8 (25 native functions). The functions are emitted in the
// reverse of Echoes' source order, which this file follows. Besides them it holds the copy
// constructor of CArchMsgParmUserInput (0x800251B4), the release of the two rc_ptrs (0x800254E8,
// 0x80025538) and the static initializer for TGameTypes.hpp's constants (0x80025588).
// Known differences: the prototype's CFinalInput is 0x108 bytes (the delta time and controller,
// then a 0x100-byte block with its own copy constructor, 0x80018E14), not Echoes' 0x2C, so
// OnMessage's frame is smaller; the string pool starts with an unreferenced
// "/Audio/multi-defbgm-speed-doon32.dsp"; and DrawGui keeps the two rc_ptr targets in registers
// across the GUI draw where the original reloads them.

#include "MetroidPrime/CMFGame.hpp"

#include "Kyoto/Audio/CAudioManager.hpp"
#include "Kyoto/Basics/CBasics.hpp"
#include "Kyoto/CFrameDelayedKiller.hpp"
#include "Kyoto/Graphics/CColor.hpp"
#include "Kyoto/Graphics/CGraphics.hpp"
#include "Kyoto/Input/IController.hpp"
#include "Kyoto/Text/ScreenText.hpp"
#include "MetaRender/CCubeRenderer.hpp"
#include "Kyoto/Math/CMath.hpp"
#include "MetroidPrime/CArchitectureMessage.hpp"
#include "MetroidPrime/CArchitectureQueue.hpp"
#include "MetroidPrime/CAutoMapper.hpp"
#include "MetroidPrime/CControllerRecorder.hpp"
#include "MetroidPrime/CDecalManager.hpp"
#include "MetroidPrime/CDisplayManager.hpp"
#include "MetroidPrime/CFrontEndUI.hpp"
#include "MetroidPrime/CGameDebug.hpp"
#include "MetroidPrime/CInGameGuiManager.hpp"
#include "MetroidPrime/CMain.hpp"
#include "MetroidPrime/CRenderManager.hpp"
#include "MetroidPrime/CSaveGameInterface.hpp"
#include "MetroidPrime/CScopedProfiler.hpp"
#include "MetroidPrime/CStateManager.hpp"
#include "MetroidPrime/CStateManagerObject.hpp"
#include "MetroidPrime/Cameras/CCameraFilterPass.hpp"
#include "MetroidPrime/Cameras/CCinematicCamera.hpp"
#include "MetroidPrime/Player/CGameMode.hpp"
#include "MetroidPrime/Player/CGameState.hpp"
#include "MetroidPrime/Player/CPlayer.hpp"
#include "MetroidPrime/Player/CPlayerState.hpp"
#include "MetroidPrime/Player/CWorldTransManager.hpp"

#include "rstl/math.hpp"

extern IController* gpController;
extern const TToken< CRasterFont >* gpDefaultFont;

bool CMFGame::mMultiplayerGuiActive;

CMFGame::CMFGame(const rstl::ncrc_ptr< CStateManager >& stateManager,
                 const rstl::ncrc_ptr< CInGameGuiManager >& guiManager)
: CIOWin(rstl::string_l("CMFGame"))
, mStateManager(stateManager)
, mGuiManager(guiManager)
, mFlowState(kFS_Zero)
, mFlowTime(0.f)
, mSkippedCineCam(kInvalidUniqueId)
, mInitialized(false)
, mPlayerAlive(true)
, x30_26_(false)
, x30_27_(true) {
  gpMain->SetGameFlowBuilt(true);
  CControllerRecorder::RecordWorld(*mStateManager);
  mMultiplayerGuiActive = false;
  gpGameState->PreviousGameResults().mShowResults = false;
}

CMFGame::~CMFGame() {
  gpMain->SetGameFlowBuilt(false);
  gpMain->SetMaxSpeed(false);
  CAudioManager::SetMuted(false);
  gpGameDebug->SetDemoSaveable(false);
  gpGameDebug->ResetDebugMessageLog();
  CDecalManager::Reinitialize();
  CGraphics::SetViewport(0, 0, CGraphics::GetRenderMode().fbWidth,
                         CGraphics::GetRenderMode().xfbHeight);
  CGraphics::SetScissor(0, 0, CGraphics::GetRenderMode().fbWidth,
                        CGraphics::GetRenderMode().xfbHeight);
}

void CMFGame::SetFlowState(EFlowState state) {
  switch (mFlowState) {
  case kFS_CinematicSkip:
    gpMain->SetMaxSpeed(false);
    mGuiManager->StartFadeIn();
    mSkippedCineCam = kInvalidUniqueId;
    CAudioManager::SetMuted(false);
    break;
  default:
    break;
  }

  mFlowState = state;
  switch (state) {
  case kFS_CinematicSkip:
    CAudioManager::SetMuted(true);
    x30_27_ = true;
    break;
  default:
    break;
  }
}

CIOWin::EMessageReturn CMFGame::OnMessage(const CArchitectureMessage& msg,
                                          CArchitectureQueue& queue) {
  switch (msg.GetType()) {
  case kAM_FrameBegin:
    mStateManager->FrameBegin(MakeMsg::GetParmFrameBegin(msg).GetInt32());
    break;
  case kAM_TimerTick: {
    const bool wasInitialized = mInitialized;
    mInitialized = true;
    const float dt = MakeMsg::GetParmTimerTick(msg).GetReal();
    if (mStateManager->mSaveGameScreen.get() &&
        mStateManager->mSaveGameScreen->Update(dt) != kMR_Normal && mFlowState == kFS_Paused) {
      mStateManager->DeleteSaveGameScreen();
    }

    if (mFlowState == kFS_Zero) {
      SetFlowState(kFS_InGame);
    }

    switch (mFlowState) {
    case kFS_LayerRestart:
      mFlowTime += dt;
      mStateManager->UpdateDynamicLayers();
      if (!mStateManager->HasPendingLayerLoads() && mFlowTime >= 1.f / 60.f) {
        mStateManager->x211_24_ = false;
        SetFlowState(kFS_InGame);
      }
      return kMR_Exit;
    case kFS_CinematicSkip: {
      mFlowTime += dt;
      bool finished = true;
      if (mStateManager->mDisplayManager->IsCinematicActive()) {
        const CCinematicCamera* cineCam = mStateManager->GetDisplayManager().GetCinematicCamera();
        finished = false;
        // Unlike Echoes, the result is not used.
        const bool canSkip = (cineCam->GetFlags() & 0x200) ||
                             ((cineCam->GetFlags() & 4) && cineCam->CanSkip(*mStateManager));
        if (mFlowTime >= 1.f && mStateManager->SpecialSkipCinematic() == 1) {
          finished = true;
        }
        if ((cineCam->GetFlags() & 8) && mSkippedCineCam != cineCam->GetCinematicObjectId()) {
          finished = true;
        }
      }
      if (finished) {
        SetFlowState(kFS_InGame);
        break;
      }
    }
    // Fall through.
    case kFS_InGame:
      if (gpGameDebug->IsMenuOpen()) {
        ProcessDebugMenuCommand(gpGameDebug->UpdateMenu(dt), queue, 0);
        break;
      }

      mStateManager->SetRandomAvailable(true);
      switch (mStateManager->mDeferredTransition) {
      case kSMT_InGame:
        mStateManager->Update(dt, queue);
        if (mStateManager->x210_24_) {
          CGraphics::SetIsBeginSceneClearFb(false);
        }
        break;
      case kSMT_MapScreen:
        EnterMapScreen();
        break;
      case kSMT_PauseGame:
        PauseGame();
        break;
      case kSMT_Unk:
        EnterLogBook();
        break;
      case kSMT_LogBook:
        SaveGame();
        break;
      case kSMT_SaveGame:
        EnterPauseScreenState5();
        break;
      case kSMT_MessageScreen:
        EnterMessageScreen(mStateManager->mHudMessageTime);
        break;
      }
      if (gpGameState->GetGameMode().IsGameOver()) {
        EndGame(queue);
      }
      if (mPlayerAlive) {
        const CGameState& gameState = *gpGameState;
        if (!gameState.GetPlayerState()->IsPlayerAlive()) {
          PlayerDied();
        }
      }
      mStateManager->SetRandomAvailable(false);
      break;
    case kFS_Paused:
      if (!mGuiManager->IsInPausedState()) {
        UnpauseGame();
        if (mStateManager->GetPauseHUDMessage() != kInvalidAssetId) {
          mStateManager->IncrementHUDMessageFrameCounter();
        }
      }
      break;
    case kFS_PlayerDied:
      if (gpGameState->GetGameMode().IsGameOver()) {
        EndGame(queue);
      } else {
        mStateManager->SetRandomAvailable(true);
        mStateManager->Update(dt, queue);
        mStateManager->SetRandomAvailable(false);
      }
      break;
    }

    mGuiManager->Update(*mStateManager, dt, queue, IsCameraActiveFlow());
    if (!wasInitialized) {
      gpGameState->WorldTransitionManager()->EndTransition();
    }
    if (gpGameState->GetGameMode().GetGameType() == 'FRND') {
      gpMain->ReleaseStreamToken();
    }
    return kMR_Exit;
  }
  case kAM_UserInput: {
    if (!mInitialized) {
      break;
    }
    const CArchMsgParmUserInput parm = MakeMsg::GetParmUserInput(msg);
    CFinalInput input = parm.GetUserInput();
    const int controller = input.ControllerNumber();
    bool stopRumble = true;
    if (controller == 0 && mStateManager->mSaveGameScreen.get()) {
      mStateManager->mSaveGameScreen->ProcessUserInput(input);
    }

    if (gpGameState->GetControlMapper().GetPressInput(CControlMapper::kC_DebugToggleCamera,
                                                      input)) {
      gpGameDebug->SetOptionValue(CGameDebug::kDO_DebugCamera,
                                  static_cast< int >(
                                      gpGameDebug->GetOptionInt(CGameDebug::kDO_DebugCamera) == 0));
      if (gpGameDebug->GetOptionInt(CGameDebug::kDO_DebugCamera) == 0) {
        gpfnWarningPrintf("--> DEBUG CAMERA OFF\n");
      } else {
        gpfnWarningPrintf("--> DEBUG CAMERA ON\n");
      }
    }

    if (mFlowState == kFS_InGame) {
      if (mMultiplayerGuiActive) {
        queue.Push(MakeMsg::CreateQuitGameplay(kAMT_Game));
        mMultiplayerGuiActive = false;
      }

      if (gpGameDebug->IsMenuOpen()) {
        ProcessDebugMenuCommand(
            gpGameDebug->ProcessMenuInput(
                mStateManager->ObjectManager().GetPlayer()->GetControlMapper(), input),
            queue, input.ControllerNumber());
        if (gpGameState->GetControlMapper().GetDigitalInput(
                CControlMapper::kC_DebugMenuRestartLevel, input, CControlMapper::kFT_Unfiltered)) {
          gpGameDebug->CloseMenu();
          gpMain->SetRestartMode(CMain::kRM_None);
          queue.Push(MakeMsg::CreateQuitGameplay(kAMT_Game));
        }
      } else {
        if (mStateManager->x211_24_) {
          SetFlowState(kFS_LayerRestart);
          mFlowTime = 0.f;
          break;
        }
        if (ProcessAITraceInput(input) == true) {
          break;
        }
        if (input.ControllerNumber() == 0) {
          if (mControlMapper.GetDigitalInput(CControlMapper::kC_DebugMenuStart, input,
                                             CControlMapper::kFT_Unfiltered) &&
              mStateManager->mDisplayManager->IsCinematicActive() && !x30_27_) {
            const CCinematicCamera* cineCam =
                mStateManager->GetDisplayManager().GetCinematicCamera();
            // Unlike Echoes, the result is not used.
            const bool canSkip = (cineCam->GetFlags() & 0x200) ||
                                 ((cineCam->GetFlags() & 4) && cineCam->CanSkip(*mStateManager));
            if (gpGameState->GetGameMode().GetGameType() != 'FRND') {
              mSkippedCineCam = cineCam->GetCinematicObjectId();
              SetFlowState(kFS_CinematicSkip);
              mFlowTime = 0.f;
              break;
            }
          }
          x30_27_ = false;
        }
        if (mControlMapper.GetPressInput(CControlMapper::kC_DebugMenuStart, input,
                                         CControlMapper::kFT_Unfiltered)) {
          gpGameDebug->OpenMenu(0, controller);
          break;
        }
        mStateManager->SetRandomAvailable(true);
        mStateManager->ProcessInput(input);
        gpGameDebug->DispatchChangedOptions(*mStateManager);
        mStateManager->SetRandomAvailable(false);
        stopRumble = false;
      }
    } else if (mFlowState != kFS_PlayerDied && mFlowState != kFS_CinematicSkip &&
               mFlowState != kFS_LayerRestart) {
      stopRumble = true;
    }

    if (!gpGameDebug->IsMenuOpen()) {
      mGuiManager->ProcessControllerInput(*mStateManager, input, queue);
    }
    if (gpGameDebug->IsMenuOpen()) {
      stopRumble = true;
    }
    if (stopRumble) {
      gpController->SetMotorState(kIOP_Player1, kMS_Stop);
      gpController->SetMotorState(kIOP_Player2, kMS_Stop);
      gpController->SetMotorState(kIOP_Player3, kMS_Stop);
      gpController->SetMotorState(kIOP_Player4, kMS_Stop);
    }
    break;
  }
  case kAM_FrameEnd:
    mStateManager->FrameEnd();
    if (mStateManager->x210_24_) {
      queue.Push(MakeMsg::CreateQuitGameplay(kAMT_Game));
    }
    break;
  case kAM_QuitGameplay:
    RecordMultiplayerResults();
    CFrameDelayedKiller::StallAndFlushAllAllocations();
    return kMR_RemoveIOWin;
  }
  return kMR_Normal;
}

bool CMFGame::ProcessAITraceInput(const CFinalInput& input) {
  if (gpGameDebug->GetOptionInt(CGameDebug::kDO_AITraceViewMode) == 0) {
    return false;
  }
  // The input handling of the trace view is compiled out.
  return false;
}

void CMFGame::DrawWorld(bool singleViewport) const {
  if (mGuiManager->GetIsGameDraw()) {
    gpMain->SetGameFrameDrawn(true);
    CScopedProfiler::BeginFrame();
    mStateManager->mDisplayManager->PreRender();
    {
      CScopedProfiler totalProfile(rstl::string(CBasics::Stringize("*TotalPlayer")), true);
      mGuiManager->PrepareScanDisplay(*mStateManager);
    }
    mStateManager->RenderManager()->DrawWorld(*mGuiManager);
    mStateManager->RenderManager()->EndPlayerRender();
  }
}

void CMFGame::DrawGui(bool singleViewport) const {
  if (!singleViewport) {
    {
      CScopedProfiler prerenderProfile(rstl::string_l("*GUI_Prerender"), true);
      mGuiManager->PreDraw(*mStateManager, IsCameraActiveFlow());
    }
    {
      CScopedProfiler drawProfile(rstl::string_l("*GUI_Draw"), true);
      mGuiManager->Draw(*mStateManager);
    }
    mStateManager->RenderManager()->EndPlayerRender();
  }
  mStateManager->DrawDebugStuff();
  CGraphics::SetViewport(0, 0, CGraphics::GetRenderMode().fbWidth,
                         CGraphics::GetRenderMode().xfbHeight);
  CGraphics::SetScissor(0, 0, CGraphics::GetRenderMode().fbWidth,
                        CGraphics::GetRenderMode().xfbHeight);
  if (mStateManager->mSaveGameScreen.get()) {
    mStateManager->mSaveGameScreen->Draw();
  }
}

void CMFGame::Draw() const {
  if (!mStateManager.IsNull()) {
    mStateManager->Touch();
  }

  switch (mFlowState) {
  case kFS_InGame:
  case kFS_Paused:
  case kFS_PlayerDied:
    DrawWorld(false);
    DrawGui(false);
    break;
  case kFS_CinematicSkip: {
    if (mFlowTime >= 1.f) {
      gpMain->SetMaxSpeed(true);
      return;
    }
    DrawWorld(false);
    DrawGui(false);
    const float intensity = CMath::Clamp(0.f, 1.f - mFlowTime, 1.f);
    CCameraFilterPass::DrawFilter(CCameraFilterPass::kFT_Multiply,
                                  CCameraFilterPass::kFS_Fullscreen,
                                  CColor(intensity, intensity, intensity, 1.f), nullptr, 1.f);
    break;
  }
  case kFS_LayerRestart: {
    if (mFlowTime >= 1.f / 60.f) {
      return;
    }
    DrawWorld(false);
    DrawGui(false);
    const float intensity = rstl::min_val(1.f, (1.f / 60.f - mFlowTime) / (1.f / 60.f));
    CCameraFilterPass::DrawFilter(CCameraFilterPass::kFT_Multiply,
                                  CCameraFilterPass::kFS_Fullscreen,
                                  CColor(intensity, intensity, intensity, 1.f), nullptr, 1.f);
    break;
  }
  }

  if (gpGameState->GetGameMode().GetGameType() == 'FRND') {
    gpRender->SetViewportOrtho(false, -4096.f, 4096.f);
    ScreenText::DrawString(rstl::string_l(CFrontEndUI::GetVersionInfo()), 24, -412, *gpDefaultFont);
  }
}

void CMFGame::ProcessDebugMenuCommand(int command, CArchitectureQueue& queue, int controller) {
  switch (command) {
  case 0:
    break;
  case 1:
    queue.Push(MakeMsg::CreateRemoveAllIOWins(kAMT_IOWinManager));
    break;
  case 2:
    queue.Push(MakeMsg::CreateQuitGameplay(kAMT_Game));
    break;
  case 3:
    if (gpGameState->GetGameMode().GetGameType() != 'FRND' &&
        gpGameState->GetGameMode().GetGameType() != 'SNGL' &&
        !mStateManager->mDisplayManager->IsCinematicActive()) {
      mGuiManager->GetPlayerGuiManager().PauseGame(*mStateManager, kIGGS_QuitGame);
      SetFlowState(kFS_Paused);
    }
    break;
  }
}

void CMFGame::EnterMapScreen() {
  SetFlowState(kFS_Paused);
  CPlayerGuiManager& gui = mGuiManager->GetPlayerGuiManager();
  gui.GetAutoMapper().SetMapMode(CAutoMapper::kMM_Normal);
  gui.PauseGame(*mStateManager, kIGGS_MapScreen);
  mStateManager->SetInMapScreen(true);
}

void CMFGame::PauseGame() {
  SetFlowState(kFS_Paused);
  CPlayerGuiManager& gui = mGuiManager->GetPlayerGuiManager();
  gui.GetAutoMapper().SetMapMode(CAutoMapper::kMM_Teleport);
  gui.PauseGame(*mStateManager, kIGGS_MapScreen);
  mStateManager->SetInMapScreen(true);
}

void CMFGame::EnterLogBook() {
  SetFlowState(kFS_Paused);
  mGuiManager->PauseGame(*mStateManager, kIGGS_PauseGame);
}

void CMFGame::SaveGame() {
  SetFlowState(kFS_Paused);
  mGuiManager->PauseGame(*mStateManager, kIGGS_PauseLogBook);
}

void CMFGame::EnterPauseScreenState5() {
  SetFlowState(kFS_Paused);
  mGuiManager->GetPlayerGuiManager().PauseGame(*mStateManager, kIGGS_PauseSaveGame);
}

void CMFGame::EnterMessageScreen(float time) {
  SetFlowState(kFS_Paused);
  mGuiManager->GetPlayerGuiManager().ShowPauseGameHudMessage(
      *mStateManager, mStateManager->GetPauseHUDMessage(), time);
}

void CMFGame::UnpauseGame() {
  SetFlowState(kFS_InGame);
  CAudioManager::SetVoiceContext(1);
  mStateManager->DeferStateTransition(kSMT_InGame);
}

void CMFGame::PlayerDied() {
  SetFlowState(kFS_PlayerDied);
  mPlayerAlive = false;
}

bool CMFGame::IsCameraActiveFlow() const {
  return (mFlowState == kFS_InGame || mFlowState == kFS_PlayerDied) && !gpGameDebug->IsMenuOpen();
}

void CMFGame::EndGame(CArchitectureQueue& queue) {
  if (gpGameState->GetGameMode().GetResultIndex() == 0) {
    gpMain->SetRestartMode(CMain::kRM_EndMovie2);
    queue.Push(MakeMsg::CreateQuitGameplay(kAMT_Game));
  } else {
    gpGameState->PreviousGameResults().mShowResults = true;
    CGraphics::SetIsBeginSceneClearFb(false);
    ActivateMultiplayerGui();
  }
}

void CMFGame::RecordMultiplayerResults() const {}

void CMFGame::ActivateMultiplayerGui() { mMultiplayerGuiActive = true; }
