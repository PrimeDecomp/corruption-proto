#include "Kyoto/Input/CRevolutionController.hpp"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Input/CFinalInput.hpp"
#include "Kyoto/Math/CMath.hpp"

#include <dolphin/vi.h>
#include <revolution/kpad.h>
#include <revolution/wpad.h>

// KPAD hold masks per EButton. Core buttons use the Core/FS formats' bits and GameCube
// buttons the Dolphin format's; stick directions and triggers are filled from axes.
static uint sButtonMasks[kBU_MAX] = {
    0,      0x1,    0x2,    0x4,    0x8,    0x10,   0x20,   0x100,  0x200,  0x400, 0x800,
    0x1000, 0,      0,      0,      0,      0x2000, 0x4000, 0,      0,      0,     0,
    0x100,  0x200,  0x400,  0x800,  0x1000, 0x10,   0x40,   0x20,   0x8,    0x2,   0x4,
    0x1,    0x40,   0x20,   0,      0,      0,      0,      0,      0,      0,     0,
    0,      0,      0,      0,      0,      0,      0,
};

static inline void UpdateButton(CControllerButton& button, bool pressed) {
  button.SetPressEvent(!!(pressed & (pressed ^ button.GetIsPressed())));
  button.SetReleaseEvent(!!(button.GetIsPressed() & (pressed ^ button.GetIsPressed())));
  button.SetIsPressed(pressed);
}

CRevolutionController::CRevolutionController()
: mStatus(4, KPADStatus())
, mGamepadStates(4, CControllerGamepadData())
, mMotorStates(4, kMS_StopHard)
, mDeviceTypes(4, 0)
, mTrackingStates(4, 0)
, mTrackedPositions(4, CVector2f::skZeroVector)
, mTrackedDistances(4, 0.f)
, mConsecutiveValidFrames(4, 0)
, mConsecutiveInvalidFrames(4, 0) {
  Initialize();
}

CRevolutionController::~CRevolutionController() {}

uint CRevolutionController::GetDeviceCount() const { return 4; }

CControllerGamepadData& CRevolutionController::GetGamepadData(int controller) {
  return mGamepadStates[controller];
}

uint CRevolutionController::GetControllerType(int controller) const {
  switch (mStatus[controller].dev_type) {
  case WPAD_DEV_CORE:
    return kControllerTypeRevn;
  case WPAD_DEV_FREESTYLE:
    return kControllerTypeChak;
  case WPAD_DEV_DOLPHIN:
    return kControllerTypeCube;
  default:
    return kControllerTypeUnknown;
  }
}

void CRevolutionController::Poll() {
  for (int i = 0; i < 4; ++i) {
    u32 type = WPAD_DEV_UNKNOWN;
    const s32 result = WPADProbe(i, &type);
    switch (result) {
    case WPAD_ERR_NONE:
      if (type != mDeviceTypes[i]) {
        switch (type) {
        case WPAD_DEV_CORE:
          WPADSetDataFormat(i, WPAD_FMT_CORE);
          rs_debugger_printf("Controller %d changed to CORE\n", i);
          break;
        case WPAD_DEV_FREESTYLE:
          WPADSetDataFormat(i, WPAD_FMT_FREESTYLE);
          rs_debugger_printf("Controller %d changed to CORE + NUNCHAKU\n", i);
          break;
        case WPAD_DEV_DOLPHIN:
          WPADSetDataFormat(i, WPAD_FMT_DOLPHIN);
          rs_debugger_printf("Controller %d changed to GAMECUBE\n", i);
          break;
        }
        if (type != WPAD_DEV_UNKNOWN) {
          rs_debugger_printf("Recalibrating controller %d\n", i);
          WPADRecalibrate(i);
        }
      }
      mDeviceTypes[i] = type;
      break;
    case WPAD_ERR_INVALID:
      rs_debugger_printf("Controller %d is INVALID\n", i);
      mDeviceTypes[i] = 0;
      break;
    case WPAD_ERR_TRANSFER:
    case WPAD_ERR_BUSY:
      rs_debugger_printf("Controller %d is BUSY\n", i);
      break;
    case WPAD_ERR_NO_CONTROLLER:
      if (mDeviceTypes[i] != 0) {
        rs_debugger_printf("Controller %d was DISCONNECTED\n", i);
        mGamepadStates[i].SetDeviceJustDisconnected(true);
      } else {
        mGamepadStates[i].SetDeviceJustDisconnected(false);
      }
      mDeviceTypes[i] = 0;
      break;
    }
    mGamepadStates[i].SetDeviceIsPresent(result == WPAD_ERR_NONE);
  }

  for (int i = 0; i < 4; ++i) {
    KPADRead(i, &mStatus[i], 1);
  }

  ProcessInputData();
}

int CRevolutionController::GetTrackingState(int controller) const {
  if (!mGamepadStates[controller].DeviceIsPresent()) {
    return 1;
  }
  return mTrackingStates[controller];
}

uint CRevolutionController::GetConsecutiveValidTrackingFrames(int controller) const {
  return mConsecutiveValidFrames[controller];
}

uint CRevolutionController::GetConsecutiveInvalidTrackingFrames(int controller) const {
  return mConsecutiveInvalidFrames[controller];
}

CVector2f CRevolutionController::GetTrackedPosition(int controller) const {
  return mTrackedPositions[controller];
}

void CRevolutionController::AdjustTrackedPositionScale(int controller) {
  KPADStatus& status = mStatus[controller];
  status.pos.x *= 1.33f;
  if (status.dpd_valid_fg > 0) {
    mTrackedDistances[controller] = status.dist;
  }

  const float t = CMath::Clamp(0.f, (mTrackedDistances[controller] - 0.5f) / 1.5f, 1.f);
  const float scale = -0.5f * t + 1.5f;
  status.pos.x = CMath::Limit(scale * status.pos.x, 1.f);
  status.pos.y = CMath::Limit(scale * status.pos.y, 1.f);
}

void CRevolutionController::UpdateTrackedPosition(int controller) {
  KPADStatus& status = mStatus[controller];
  bool valid = status.dpd_valid_fg > 0;
  if (status.dev_type == WPAD_DEV_DOLPHIN) {
    valid = true;
  }

  if (valid) {
    ++mConsecutiveValidFrames[controller];
    mConsecutiveInvalidFrames[controller] = 0;
  } else {
    ++mConsecutiveInvalidFrames[controller];
    mConsecutiveValidFrames[controller] = 0;
  }

  switch (mTrackingStates[controller]) {
  case 0:
    if (!valid) {
      mTrackingStates[controller] = 1;
      status.pos.x = mTrackedPositions[controller].GetX();
      status.pos.y = mTrackedPositions[controller].GetY();
    } else {
      mTrackedPositions[controller].SetX(status.pos.x);
      mTrackedPositions[controller].SetY(status.pos.y);
      if (status.dev_type == WPAD_DEV_DOLPHIN) {
        mTrackedPositions[controller].SetX(status.ex_status.gc.substick.x);
        mTrackedPositions[controller].SetY(status.ex_status.gc.substick.y);
      }
    }
    break;

  case 1:
    if (!valid) {
      if (mConsecutiveInvalidFrames[controller] >= 90) {
        mTrackingStates[controller] = 2;
      }
    } else if (mConsecutiveValidFrames[controller] >= 10) {
      mTrackingStates[controller] = 3;
      mConsecutiveValidFrames[controller] = 0;
    }
    status.pos.x = mTrackedPositions[controller].GetX();
    status.pos.y = mTrackedPositions[controller].GetY();
    break;

  case 2: {
    bool decay = true;
    if (valid && mConsecutiveValidFrames[controller] > 10) {
      mTrackingStates[controller] = 3;
      decay = false;
      mConsecutiveValidFrames[controller] = 0;
    }
    if (decay) {
      CVector2f& pos = mTrackedPositions[controller];
      const float mag = pos.Magnitude() * 0.95f;
      if (pos.CanBeNormalized()) {
        pos = pos.AsNormalized() * mag;
      }
    }
    status.pos.x = mTrackedPositions[controller].GetX();
    status.pos.y = mTrackedPositions[controller].GetY();
    break;
  }

  case 3:
    if (!valid) {
      mTrackingStates[controller] = 2;
    } else {
      CVector2f& tracked = mTrackedPositions[controller];
      CVector2f delta(tracked.GetX() - status.pos.x, tracked.GetY() - status.pos.y);
      if (delta.MagSquared() < 0.01f) {
        mTrackingStates[controller] = 0;
      } else {
        delta *= 0.9f;
        status.pos.x += delta.GetX();
        status.pos.y += delta.GetY();
        tracked.SetX(status.pos.x);
        tracked.SetY(status.pos.y);
      }
    }
    break;
  }
}

void CRevolutionController::ProcessInputData() {
  for (int i = 0; i < 4; ++i) {
    if (mGamepadStates[i].DeviceIsPresent()) {
      const u32 devType = mStatus[i].dev_type;
      if (devType == WPAD_DEV_CORE || devType == WPAD_DEV_FREESTYLE) {
        AdjustTrackedPositionScale(i);
      }
      UpdateTrackedPosition(i);
    }
  }

  for (int i = 0; i < 4; ++i) {
    CControllerGamepadData& data = mGamepadStates[i];
    if (data.DeviceIsPresent()) {
      ProcessAxis(i, kJA_GcLeftX, data.GetButton(kBU_GcLeftStickLeft),
                  data.GetButton(kBU_GcLeftStickRight));
      ProcessAxis(i, kJA_GcLeftY, data.GetButton(kBU_GcLeftStickUp),
                  data.GetButton(kBU_GcLeftStickDown));
      ProcessAxis(i, kJA_GcRightX, data.GetButton(kBU_GcRightStickLeft),
                  data.GetButton(kBU_GcRightStickRight));
      ProcessAxis(i, kJA_GcRightY, data.GetButton(kBU_GcRightStickUp),
                  data.GetButton(kBU_GcRightStickDown));
      // These two pairs are the reverse of the directions CFinalInput reads from the axes.
      ProcessAxis(i, kJA_LeftX, data.GetButton(kBU_LeftStickRight),
                  data.GetButton(kBU_LeftStickLeft));
      ProcessAxis(i, kJA_LeftY, data.GetButton(kBU_LeftStickDown),
                  data.GetButton(kBU_LeftStickUp));
      ProcessAxis(i, kJA_RightX, data.GetButton(kBU_RightStickLeft),
                  data.GetButton(kBU_RightStickRight));
      ProcessAxis(i, kJA_RightY, data.GetButton(kBU_RightStickUp),
                  data.GetButton(kBU_RightStickDown));
      ProcessButtons(i);
    }
  }
}

void CRevolutionController::ProcessAxis(int controller, EJoyAxis axis, CControllerButton& negative,
                                        CControllerButton& positive) {
  CControllerGamepadData& data = mGamepadStates[controller];
  if (!data.DeviceIsPresent()) {
    return;
  }

  const float scale = 1.f / GetAnalogStickMaxValue(axis);
  CControllerAxis& ctrlAxis = data.GetAxis(axis);
  const KPADStatus& status = mStatus[controller];
  float value = 0.f;
  switch (status.dev_type) {
  case WPAD_DEV_DOLPHIN:
    switch (axis) {
    case kJA_GcLeftX:
      value = status.ex_status.gc.stick.x;
      break;
    case kJA_GcLeftY:
      value = -status.ex_status.gc.stick.y;
      break;
    case kJA_RightX:
    case kJA_GcRightX:
      value = status.ex_status.gc.substick.x;
      break;
    case kJA_GcRightY:
      value = -status.ex_status.gc.substick.y;
      break;
    case kJA_RightY:
      value = status.ex_status.gc.substick.y;
      break;
    }
    break;
  case WPAD_DEV_FREESTYLE:
    switch (axis) {
    case kJA_LeftX:
      value = status.ex_status.fs.stick.x;
      break;
    case kJA_LeftY:
      value = status.ex_status.fs.stick.y;
      break;
    }
  case WPAD_DEV_CORE:
    switch (axis) {
    case kJA_RightX:
      value = status.pos.x;
      break;
    case kJA_RightY:
      value = status.pos.y;
      break;
    }
    break;
  }

  float absolute = value * scale;
  if (absolute < kAbsoluteMinimum) {
    absolute = kAbsoluteMinimum;
  } else if (absolute > kAbsoluteMaximum) {
    absolute = kAbsoluteMaximum;
  }
  float relative = absolute - ctrlAxis.GetAbsoluteValue();
  if (relative < kRelativeMinimum) {
    relative = kRelativeMinimum;
  } else if (relative > kRelativeMaximum) {
    relative = kRelativeMaximum;
  }
  ctrlAxis.SetRelativeValue(relative);
  ctrlAxis.SetAbsoluteValue(absolute);
  const bool negativePressed = absolute <= -CFinalInput::kInput_AnalogOnThreshhold;
  UpdateButton(negative, negativePressed);
  const bool positivePressed = absolute >= CFinalInput::kInput_AnalogOnThreshhold;
  UpdateButton(positive, positivePressed);
}

void CRevolutionController::ProcessButtons(int controller) {
  CControllerGamepadData& data = mGamepadStates[controller];
  if (!data.DeviceIsPresent()) {
    return;
  }

  for (int i = 1; i < kBU_MAX; ++i) {
    if (sButtonMasks[i] != 0) {
      ProcessDigitalButton(controller, data.GetButton(static_cast< EButton >(i)), sButtonMasks[i]);
    }
  }

  // Both triggers update the same button.
  const KPADStatus& status = mStatus[controller];
  ProcessAnalogButton(status.ex_status.gc.ltrigger, data.GetAnalogButton(kBA_Left),
                      data.GetButton(kBU_GcLeftStickLeft));
  ProcessAnalogButton(status.ex_status.gc.rtrigger, data.GetAnalogButton(kBA_Right),
                      data.GetButton(kBU_GcLeftStickLeft));
}

void CRevolutionController::ProcessDigitalButton(int controller, CControllerButton& button,
                                                 uint mask) {
  if (!mGamepadStates[controller].DeviceIsPresent()) {
    return;
  }
  UpdateButton(button, (mStatus[controller].hold & mask) != 0);
}

void CRevolutionController::ProcessAnalogButton(float value, CControllerAxis& axis,
                                                CControllerButton& button) {
  float absolute = value * (1.f / 150.f);
  if (absolute > kAbsoluteMaximum) {
    absolute = kAbsoluteMaximum;
  }
  float relative = absolute - axis.GetAbsoluteValue();
  if (relative > kRelativeMaximum) {
    relative = kRelativeMaximum;
  }
  axis.SetRelativeValue(relative);
  axis.SetAbsoluteValue(absolute);
  const bool pressed = CMath::AbsF(absolute) > CFinalInput::kInput_AnalogTriggerOnThreshhold;
  UpdateButton(button, pressed);
}

void CRevolutionController::SetMotorState(EIOPort port, EMotorState state) {
  if (!mGamepadStates[port].DeviceIsPresent()) {
    return;
  }

  mMotorStates[port] = state;
  switch (state) {
  case kMS_Rumble:
    WPADControlMotor(port, WPAD_MOTOR_RUMBLE);
    break;
  case kMS_Stop:
  case kMS_StopHard:
    WPADControlMotor(port, WPAD_MOTOR_STOP);
    break;
  }
}

float CRevolutionController::GetAnalogStickMaxValue(EJoyAxis axis) const {
  switch (axis) {
  case kJA_LeftX:
  case kJA_LeftY:
  case kJA_RightX:
  case kJA_RightY:
    return 1.f;
  case kJA_GcLeftX:
  case kJA_GcLeftY:
    return 1.f;
  case kJA_GcRightX:
  case kJA_GcRightY:
    return 1.f;
  default:
    return 0.f;
  }
}

bool CRevolutionController::Initialize() {
  for (int i = 0; i < 4; ++i) {
    mGamepadStates[i].SetDeviceIsPresent(false);
    mMotorStates[i] = kMS_StopHard;
    mTrackingStates[i] = 0;
  }

  KPADInit();
  KPADSetPosParam(0, 0.05f, 1.f);
  KPADSetObjInterval(0.15f);
  VIWaitForRetrace();
  VIWaitForRetrace();
  VIWaitForRetrace();
  VIWaitForRetrace();

  for (int i = 0; i < 4; ++i) {
    u32 type = WPAD_DEV_UNKNOWN;
    switch (WPADProbe(i, &type)) {
    case WPAD_ERR_NONE:
      switch (type) {
      case WPAD_DEV_CORE:
        WPADSetDataFormat(i, WPAD_FMT_CORE);
        rs_debugger_printf("Controller %d is a CORE controller\n", i);
        break;
      case WPAD_DEV_FREESTYLE:
        WPADSetDataFormat(i, WPAD_FMT_FREESTYLE);
        rs_debugger_printf("Controller %d is a CORE + NUNCHAKU controller\n", i);
        break;
      case WPAD_DEV_DOLPHIN:
        WPADSetDataFormat(i, WPAD_FMT_DOLPHIN);
        rs_debugger_printf("Controller %d is a GAMECUBE controller\n", i);
        break;
      case WPAD_DEV_UNKNOWN:
        rs_debugger_printf("Controller %d is an UNKNOWN controller\n", i);
        break;
      default:
        rs_debugger_printf("Controller %d is an '%d' controller\n", i, type);
        break;
      }
      if (type != WPAD_DEV_UNKNOWN) {
        VIWaitForRetrace();
        VIWaitForRetrace();
        rs_debugger_printf("Recalibrating controller %d\n", i);
        WPADRecalibrate(i);
        mDeviceTypes[i] = type;
      }
      break;
    case WPAD_ERR_INVALID:
      rs_debugger_printf("Controller %d is INVALID\n", i);
      break;
    case WPAD_ERR_TRANSFER:
    case WPAD_ERR_BUSY:
      rs_debugger_printf("Controller %d is BUSY\n", i);
      break;
    case WPAD_ERR_NO_CONTROLLER:
      rs_debugger_printf("Controller %d is NOT ATTACHED\n", i);
      break;
    }
    mDeviceTypes[i] = type;
  }

  return true;
}

const uint kControllerTypeUnknown = 'UNKN';
const uint kControllerTypeCube = 'CUBE';
const uint kControllerTypeRevn = 'REVN';
const uint kControllerTypeChak = 'CHAK';
