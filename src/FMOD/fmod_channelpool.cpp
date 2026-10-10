// G2MEAB prototype translation unit; complete reconstruction of the 8 native functions.
// .text: 0x805C11FC..0x805C168C. Basename asserted by the pool allocation/free file arguments.
// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; ChannelPool keeps the
// 4.06 layout (0x14: ctor 0x805C11FC, init 0x805C1218), ChannelReal uses the G2MEAB layout.

#include "fmod_channelpool.h"
#include "fmod.h"
#include "fmod_channel_real.h"
#include "fmod_memory.h"

namespace FMOD {

ChannelPool::ChannelPool()
{
    mChannel = 0;
    mNumChannels = 0;
    mChannelsUsed = 0;
    mSystem = 0;
    mOutput = 0;
}

FMOD_RESULT ChannelPool::init(SystemI * system, Output * output, int numchannels)
{
    if (numchannels < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mChannel = (ChannelReal **)FMOD_Memory_Calloc(sizeof(ChannelReal *) * numchannels);
    if (!mChannel)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mNumChannels = numchannels;
    mSystem = system;
    mOutput = output;
    return FMOD_OK;
}

FMOD_RESULT ChannelPool::release()
{
    if (mChannel)
    {
        int count;

        for (count = 0; count < mNumChannels; count++)
        {
            if (mChannel[count])
            {
                mChannel[count]->close();
            }
        }

        FMOD_Memory_Free(mChannel);
    }

    FMOD_Memory_Free(this);
    return FMOD_OK;
}

FMOD_RESULT ChannelPool::allocateChannel(ChannelReal * * realchannel, int index, int numchannels, int * found_out)
{
    int count;
    int found = 0;

    if (!realchannel)
    {
        if (found_out)
        {
            *found_out = 0;
        }
        return FMOD_ERR_INVALID_PARAM;
    }

    if (index == FMOD_CHANNEL_FREE)
    {
        for (count = 0; count < mNumChannels; count++)
        {
            if (!(mChannel[count]->mFlags & CHANNELREAL_FLAG_ALLOCATED) &&
                !(mChannel[count]->mFlags & CHANNELREAL_FLAG_RESERVED) &&
                !(mChannel[count]->mFlags & CHANNELREAL_FLAG_ENDDELAY))
            {
                bool isplaying;

                if (mChannel[count]->isPlaying(&isplaying) == FMOD_OK && !isplaying)
                {
                    mChannel[count]->mFlags |= CHANNELREAL_FLAG_ALLOCATED;
                    mChannel[count]->mFlags |= CHANNELREAL_FLAG_RESERVED;
                    mChannel[count]->mFlags &= ~CHANNELREAL_FLAG_STOPPED;
                    realchannel[found] = mChannel[count];
                    found++;

                    if (found == numchannels)
                    {
                        if (found_out)
                        {
                            *found_out = found;
                        }
                        return FMOD_OK;
                    }
                }
            }
        }
    }
    else if (index >= 0 && index < mNumChannels)
    {
        if (numchannels > 1)
        {
            return FMOD_ERR_CHANNEL_ALLOC;
        }

        mChannel[index]->mFlags |= CHANNELREAL_FLAG_ALLOCATED;
        mChannel[index]->mFlags |= CHANNELREAL_FLAG_RESERVED;
        mChannel[index]->mFlags &= ~CHANNELREAL_FLAG_STOPPED;
        *realchannel = mChannel[index];
        return FMOD_OK;
    }

    for (count = 0; count < found; count++)
    {
        if (realchannel[count])
        {
            realchannel[count]->mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
            realchannel[count]->mFlags &= ~CHANNELREAL_FLAG_RESERVED;
            realchannel[count]->mFlags |= CHANNELREAL_FLAG_STOPPED;
        }
    }

    if (found_out)
    {
        *found_out = found;
    }
    return FMOD_ERR_CHANNEL_ALLOC;
}

FMOD_RESULT ChannelPool::getNumChannels(int * numchannels)
{
    if (!numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numchannels = mNumChannels;
    return FMOD_OK;
}

FMOD_RESULT ChannelPool::getChannelsUsed(int * numchannels)
{
    if (!numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numchannels = mChannelsUsed;
    return FMOD_OK;
}

FMOD_RESULT ChannelPool::setChannel(int index, ChannelReal * channel, DSPI * dspmixtarget)
{
    if (!channel || index < 0 || index >= mNumChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mChannel[index] = channel;
    mChannel[index]->mPool = this;
    return mChannel[index]->init(index, mSystem, mOutput, dspmixtarget);
}

FMOD_RESULT ChannelPool::getChannel(int index, ChannelReal * * channel)
{
    if (!channel || index < 0 || index >= mNumChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *channel = mChannel[index];
    return FMOD_OK;
}

} // namespace FMOD
