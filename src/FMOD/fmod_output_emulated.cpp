// G2MEAB prototype translation unit; complete reconstruction (5/5 native functions).
// G2MEAB .text: 0x8060EEEC..0x8060F1B8 (5 retained native functions).
// direct target filename in allocation/free body.
// Evidence: CtorEEEC calls Output ctor and installs806EE690. EF40 allocates pool14 and
// count-times-0x78 emulated voices, directly naming fmod_output_emulated.cpp806EE6A0
// lines0x47/0x53; F080 releases voices/pool atline0x7E and calls base freeEA38. Preserve retained
// helpers, thunks and inline expansions in observed native order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_output_emulated.h"
#include "fmod.h"
#include "fmod_channel_emulated.h"
#include "fmod_channelpool.h"
#include "fmod_globals.h"
#include "fmod_memory.h"

#include <string.h>

namespace FMOD {

OutputEmulated::OutputEmulated()
{
    memset(&mDescription, 0, sizeof(FMOD_OUTPUT_DESCRIPTION_EX));

    mChannel = 0;
}

FMOD_RESULT OutputEmulated::init(int maxchannels)
{
    FMOD_RESULT result;

    if (!mSystem)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    if (maxchannels)
    {
        int count;

        mChannelPool = mChannelPool3D = FMOD_Object_Alloc(ChannelPool);
        if (!mChannelPool)
        {
            return FMOD_ERR_MEMORY;
        }

        result = mChannelPool->init(mSystem, this, maxchannels);
        if (result != FMOD_OK)
        {
            return result;
        }

        mChannel = (ChannelEmulated *)FMOD_Memory_Calloc(sizeof(ChannelEmulated) * maxchannels);
        if (!mChannel)
        {
            return FMOD_ERR_MEMORY;
        }

        for (count = 0; count < maxchannels; count++)
        {
            new (&mChannel[count]) ChannelEmulated;

            mChannelPool->setChannel(count, &mChannel[count], 0);
        }
    }

    return FMOD_OK;
}

FMOD_RESULT OutputEmulated::release()
{
    if (mChannelPool)
    {
        mChannelPool->release();
        mChannelPool = 0;
    }

    if (mChannel)
    {
        FMOD_Memory_Free(mChannel);
        mChannel = 0;
    }

    return Output::release();
}

FMOD_RESULT OutputEmulated::update()
{
    return FMOD_OK;
}

} // namespace FMOD
