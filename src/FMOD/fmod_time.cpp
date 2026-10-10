// G2MEAB prototype NonMatching translation-unit scaffold; function bodies are empty placeholders.
// .text: 0x80621568..0x80621870 (7 native functions).
// Split out of the former fmod_systemi scaffold; original basename from the 4.06 reference library object.
// 0x80621568 +0x30: profiling state constructor emitted with system
// 0x80621598 +0x38: retained native; no unsupported symbol identity assigned
// 0x806215D0 +0x150: retained native; no unsupported symbol identity assigned
// 0x80621720 +0x20: retained native; no unsupported symbol identity assigned
// 0x80621740 +0xC4: retained native; no unsupported symbol identity assigned
// 0x80621804 +0x48: retained native; no unsupported symbol identity assigned
// 0x8062184C +0x24: thread sleep adapter to OS623228

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_time.h"
#include "fmod.h"
#include "fmod_types.h"
#include "fmod_os_misc.h"

namespace FMOD {

TimeStamp::TimeStamp()
{
    mIn = 0;
    mOut = 0;
    mTotalIn = 0;
    mTotalOut = 0;
    mPaused = false;
    mPausedIn = 0;
    mPausedOut = 0;
    mPausedTotal = 0;
    mPausedRefCount = 0;
    mTiming = false;
}

FMOD_RESULT TimeStamp::stampIn()
{
    FMOD_OS_Time_GetNs(&mIn);
    mTiming = true;
    return FMOD_OK;
}

FMOD_RESULT TimeStamp::stampOut(int damppercentage)
{
    unsigned int now;
    FMOD_UFLOAT damp = (FMOD_UFLOAT)damppercentage / 100.0f;
    FMOD_UFLOAT total;
    FMOD_UFLOAT avtotal;

    FMOD_OS_Time_GetNs(&now);
    mOut = now;
    mTotalOut = now;

    if (mTotalOut < mTotalIn)
    {
        total = 0.0f;
    }
    else
    {
        total = (FMOD_UFLOAT)(mTotalOut - mTotalIn);
    }

    mAvTotal *= damp;
    mAvTotal += total;
    avtotal = mAvTotal * (1.0f - damp);

    mPercent *= damp;
    if (mOut > mIn)
    {
        mPercent += 100.0f * (FMOD_UFLOAT)(mOut - mIn - mPausedTotal) / avtotal;
    }

    mCPUUsage = mPercent * (1.0f - damp);
    mTotalIn = now;
    mPausedTotal = 0;
    mPausedRefCount = 0;
    mTiming = false;
    return FMOD_OK;
}

FMOD_RESULT TimeStamp::getCPUUsage(FMOD_UFLOAT * cpuusage)
{
    if (!cpuusage)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *cpuusage = mCPUUsage;
    return FMOD_OK;
}

FMOD_RESULT TimeStamp::setPaused(bool paused)
{
    if (!mTiming)
    {
        return FMOD_OK;
    }

    if (paused)
    {
        if (!mPausedRefCount)
        {
            FMOD_OS_Time_GetNs(&mPausedIn);
        }
        mPausedRefCount++;
    }
    else if (!paused)
    {
        mPausedRefCount--;
        if (!mPausedRefCount)
        {
            FMOD_OS_Time_GetNs(&mPausedOut);
            if (mPausedOut > mPausedIn)
            {
                mPausedTotal += mPausedOut - mPausedIn;
            }
        }
    }

    mPaused = paused;
    return FMOD_OK;
}

} // namespace FMOD

FMOD_RESULT FMOD_Time_Get(unsigned int * ms)
{
    FMOD_OS_Time_GetNs(ms);
    *ms /= 1000;
    return FMOD_OK;
}

FMOD_RESULT FMOD_Time_Sleep(unsigned int sleeptime)
{
    FMOD_OS_Time_Sleep(sleeptime);
    return FMOD_OK;
}
