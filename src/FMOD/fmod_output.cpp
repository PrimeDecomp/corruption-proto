// G2MEAB prototype translation unit; all native functions reconstructed.
// G2MEAB .text: 0x8060E934..0x8060EEEC (8 retained native functions).
// direct target filename in allocation/free body.
// Evidence: Allocation/free wrapperEA38 directly names fmod_output.cpp at806EE680 line0x42;
// constructorE934 installs Output vtable806EE658, captures globals+18/+1C, and initializes embedded
// plugin descriptor+3C/list+70. EA70 is Output::mix (pulls the DSP soundcard unit under the DSP crit), ED74 chooses
// emulated/software voice pools. Preserve retained helpers, thunks and inline expansions in
// observed native order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_output.h"
#include "fmod.h"
#include "fmod_channel_real.h"
#include "fmod_outputi.h"
#include "fmod_channelpool.h"
#include "fmod_globals.h"
#include "fmod_memory.h"
#include "fmod_dspi.h"
#include "fmod_localcriticalsection.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

Output::Output()
{
    mEnumerated = false;
    mPolling = false;
    mChannelPool = 0;
    mChannelPool3D = 0;
    mMusicChannelGroup = 0;
}

FMOD_RESULT Output::release()
{
    FMOD_Memory_Free(this);

    return FMOD_OK;
}

FMOD_RESULT Output::mix(void * buffer, unsigned int numsamples)
{
    FMOD_RESULT result;
    unsigned int offset = 0;
    unsigned int blockalign;
    DSPI * dsphead;
    FMOD_SOUND_FORMAT outputformat;
    int outputchannels;
    LocalCriticalSection crit(mSystem->mDSPCrit);

    if (!buffer || !numsamples)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = mSystem->getSoftwareFormat(0, &outputformat, &outputchannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = SoundI::getBytesFromSamples(1, &blockalign, outputchannels, outputformat);
    if (result != FMOD_OK)
    {
        return result;
    }

    dsphead = mSystem->mDSPSoundCard;
    if (!dsphead)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    crit.enter();

    do
    {
        void * destbuffer = (char *)buffer + (offset * blockalign);
        void * outbuffer = destbuffer;
        unsigned int length = numsamples;

        mSystem->mDSPActive = true;
        mSystem->mDSPReadBuffIndex = 0;

        dsphead->execute(outbuffer, &outbuffer, &length, outputchannels, &outputchannels, mSystem->mSpeakerMode);

        mSystem->mDSPActive = false;

        if (outbuffer != destbuffer)
        {
            memcpy(destbuffer, outbuffer, length * blockalign);
        }

        dsphead->resetVisited();

        numsamples -= length;
        offset += length;
    } while (numsamples);

    crit.leave();

    return FMOD_OK;
}

FMOD_RESULT Output::getFreeChannel(FMOD_MODE mode, ChannelReal * * realchannel, int numchannels, int * found)
{
    FMOD_RESULT result;

    if (!realchannel)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mode & FMOD_3D)
    {
        if (!mChannelPool3D)
        {
            return FMOD_ERR_CHANNEL_ALLOC;
        }

        result = mChannelPool3D->allocateChannel(realchannel, -1, numchannels, found);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else
    {
        if (!mChannelPool)
        {
            return FMOD_ERR_CHANNEL_ALLOC;
        }

        result = mChannelPool->allocateChannel(realchannel, -1, numchannels, found);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT Output::mixCallback(FMOD_OUTPUT_STATE * output, void * buffer, unsigned int length)
{
    Output * out = (Output *)output;

    return out->mix(buffer, length);
}

} // namespace FMOD
