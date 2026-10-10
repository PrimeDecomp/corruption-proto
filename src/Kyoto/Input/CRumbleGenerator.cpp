#include "Kyoto/Input/CRumbleGenerator.hpp"

#include <revolution/wpad.h>

CRumbleGenerator::CRumbleGenerator(EIOPort inputIndex) : mInputIndex(inputIndex), mDisabled(false) {
  HardStopAll();
}

CRumbleGenerator::~CRumbleGenerator() { HardStopAll(); }

short CRumbleGenerator::Rumble(const SAdsrData& adsr, float gain, ERumblePriority prio) {
  ushort freeChan = mVoice.GetFreeChannel();
  if (prio >= mVoice.GetPriority(freeChan)) {
    mPeriodTime = 0.f;
    mOnTime = 0.f;
    return mVoice.Activate(adsr, freeChan, gain, prio);
  }
  return -1;
}

void CRumbleGenerator::Update(float dt) {
  if (!mDisabled) {
    bool updated = false;
    const float intensity = mVoice.GetIntensity();
    if (!mVoice.Update(dt) || intensity <= 0.f) {
      mPeriodTime = 0.f;
      mOnTime = 0.f;
      if (mCommand != kMS_Stop) {
        mCommand = kMS_Stop;
        updated = true;
      }
    } else {
      mPeriodTime += dt;
      if (mPeriodTime >= 1.f / (30.f * intensity)) {
        mPeriodTime = 0.f;
        if (mCommand != kMS_Rumble) {
          mCommand = kMS_Rumble;
          updated = true;
        }
      } else {
        mOnTime += dt;
        if (mOnTime >= (1.f / 30.f)) {
          mOnTime = 0.f;
          if (mCommand != kMS_Stop) {
            mCommand = kMS_Stop;
            updated = true;
          }
        }
      }
    }
    if (updated) {
      if (mCommand == kMS_Stop || mCommand == kMS_StopHard) {
        WPADControlMotor(0, WPAD_MOTOR_STOP);
      } else {
        WPADControlMotor(0, WPAD_MOTOR_RUMBLE);
      }
    }
  }
}

void CRumbleGenerator::HardStopAll() {
  mPeriodTime = 0.f;
  mOnTime = 0.f;
  mCommand = kMS_Stop;
  mVoice.HardReset();
  WPADControlMotor(0, WPAD_MOTOR_STOP);
}

void CRumbleGenerator::SetDisabled(const bool disabled) {
  if (disabled) {
    HardStopAll();
  }

  mDisabled = disabled;
}

void CRumbleGenerator::Stop(short id) {
  if (id != -1)
    mVoice.Deactivate(id, false);
}
