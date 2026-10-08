/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x8048E7EC..0x8048E860; 2 retained native bodies.
 * Function and helper inventory is recorded in the external agent workflow.
 */
#include "GuiSys/CRepeatState.hpp"

CRepeatState::CRepeatState() : mTimer(0.f) {}

const bool CRepeatState::Update(float dt, bool pressed) {
  bool repeat = false;
  if (mTimer == 0.f) {
    if (pressed) {
      mTimer = 0.6f;
      repeat = true;
    }
  } else {
    if (pressed) {
      mTimer -= dt;
      if (mTimer <= 0.f) {
        mTimer = 0.05f;
        repeat = true;
      }
    } else {
      mTimer = 0.f;
    }
  }

  return repeat;
}
