#include "Kyoto/CRandom16.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Alloc/Assert.hpp"

CRandom16* CRandom16::gRandomNumber = 0;
CGlobalRandom* CGlobalRandom::gCurrentGlobalRandom = 0;

CGlobalRandom::CGlobalRandom(CRandom16& rnd) : mRandom(rnd) {
  mIsFirst = true;
  mPrev = gCurrentGlobalRandom;
  if (mPrev != 0) {
    mPrev->mIsFirst = false;
  }

  gCurrentGlobalRandom = this;
  CRandom16::_SetRandomNumber(&mRandom);
}

CGlobalRandom::~CGlobalRandom() {
  gCurrentGlobalRandom = mPrev;
  if (gCurrentGlobalRandom != 0) {
    gCurrentGlobalRandom->mIsFirst = true;
    CRandom16::_SetRandomNumber(&gCurrentGlobalRandom->mRandom);
  } else {
    CRandom16::_SetRandomNumber(0);
  }
}

CRandom16* CRandom16::GetRandomNumber() {
  if (gRandomNumber == 0) {
    CCallStack stack(0, "CRandom16.cpp(60) : ", kUnknownType);
    rs_log_assert_failure(&stack, "CRandom16.cpp", 60, "Verify", "gRandomNumber != NULL",
                          "CRandom16::gRandomNumber was set to NULL");
    rs_debugger_printf("Would have thrown exception: %s\n", "false");
    fn_80491108();
  }
  return gRandomNumber;
}

void CRandom16::_SetRandomNumber(CRandom16* rnd) { gRandomNumber = rnd; }

CRandom16::CRandom16(const uint seed) : mSeed(seed) {}

void CRandom16::SetSeed(const uint seed) { mSeed = seed; }

int CRandom16::Range(const int min, const int max) { return min + (Next() % ((max - min) + 1)); }

float CRandom16::Range(const float min, const float max) { return ((max - min) * Float()) + min; }

int CRandom16::Next() {
  mSeed = (mSeed * 0x41c64e6d) + 0x00003039;
  return (mSeed >> 16) & 0xffff;
}

float CRandom16::Float() {
  mSeed = (mSeed * 0x41c64e6d) + 0x00003039;
  return 3.7252903e-9f * (mSeed & 0x0fffffff);
}
