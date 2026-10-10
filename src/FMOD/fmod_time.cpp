// G2MEAB prototype translation unit; complete reconstruction (NonMatching only by sdata2 constant labels).
// .text: 0x80621568..0x80621870 (7 native functions).
// Split out of the former fmod_systemi scaffold; original basename from the 4.06 reference library object.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; TimeStamp keeps the 4.06 layout.

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
    unsigned int val;
    FMOD_UFLOAT total;
    FMOD_UFLOAT smoothedtotal;
    FMOD_UFLOAT dampratio = (FMOD_UFLOAT)damppercentage / 100.0f;

    FMOD_OS_Time_GetNs(&val);
    mOut = val;
    mTotalOut = val;

    if (mTotalOut < mTotalIn)
    {
        total = 0.0f;
    }
    else
    {
        total = (FMOD_UFLOAT)(mTotalOut - mTotalIn);
    }

    mAvTotal *= dampratio;
    mAvTotal += total;
    smoothedtotal = mAvTotal * (1.0f - dampratio);

    mPercent *= dampratio;
    if (mOut > mIn)
    {
        unsigned int delta = mOut - mIn - mPausedTotal;

        mPercent += 100.0f * (FMOD_UFLOAT)delta / smoothedtotal;
    }

    mCPUUsage = mPercent * (1.0f - dampratio);
    mTotalIn = val;
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
