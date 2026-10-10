// G2MEAB prototype translation unit; all retained native functions reconstructed.
// G2MEAB .text: 0x80610154..0x80610930 (7 retained native functions).
// direct target filename in allocation/free body.
// Evidence: Ctor10154 installs806EE94C and descriptor named FMOD Software Output. 101E4 allocates
// pool14/count-times0x90 software voices, naming fmod_output_software.cpp806EE975
// lines0x53/0x5F;10334 frees them line0x89. 103A8 creates 0x484 SoftwareSample objects via11C3C and
// constructs format/sample buffers (line0xD2). Preserve retained helpers, thunks and inline
// expansions in observed native order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_output_software.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_output.h"
#include "fmod_sound_sample.h"
#include "fmod_channel_software.h"
#include "fmod_channelgroupi.h"
#include "fmod_channelpool.h"
#include "fmod_dsp_resampler.h"
#include "fmod_memory.h"
#include "fmod_sample_software.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

OutputSoftware::OutputSoftware()
{
    memset(&mDescription, 0, sizeof(FMOD_OUTPUT_DESCRIPTION_EX));

    mDescription.name = "FMOD Software Output";
    mDescription.version = 0x00010100;
    mDescription.polling = 0;
    mDescription.getsamplemaxchannels = &OutputSoftware::getSampleMaxChannelsCallback;
    mDescription.mType = FMOD_OUTPUTTYPE_SOFTWARE;
    mDescription.mSize = sizeof(OutputSoftware);

    mChannel = 0;
    mChannelPool = 0;
}

FMOD_RESULT OutputSoftware::init(int maxchannels)
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

        mChannel = (ChannelSoftware *)FMOD_Memory_Calloc(sizeof(ChannelSoftware) * maxchannels);
        if (!mChannel)
        {
            return FMOD_ERR_MEMORY;
        }

        for (count = 0; count < maxchannels; count++)
        {
            new (&mChannel[count]) ChannelSoftware;

            mChannelPool->setChannel(count, &mChannel[count], mSystem->mChannelGroup->mDSPHead);
        }
    }

    return FMOD_OK;
}

FMOD_RESULT OutputSoftware::release()
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

FMOD_RESULT OutputSoftware::createSample(FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT * waveformat, Sample * * sample)
{
    FMOD_RESULT result;
    SampleSoftware * newsample;
    unsigned int overflowbytes;

    if (!sample)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (waveformat)
    {
        int bits;

        result = SoundI::getBitsFromFormat(waveformat->format, &bits);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (!bits && waveformat->format != FMOD_SOUND_FORMAT_NONE && waveformat->format != FMOD_SOUND_FORMAT_IMAADPCM && waveformat->format != FMOD_SOUND_FORMAT_MPEG)
        {
            return FMOD_ERR_FORMAT;
        }
    }

    if (*sample)
    {
        newsample = (SampleSoftware *)*sample;
    }
    else
    {
        newsample = new (FMOD_Memory_Calloc(sizeof(SampleSoftware))) SampleSoftware;
        if (!newsample)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    if (!waveformat)
    {
        *sample = newsample;
        return FMOD_OK;
    }

    newsample->mFormat = waveformat->format;

    if (waveformat->format == FMOD_SOUND_FORMAT_IMAADPCM || waveformat->format == FMOD_SOUND_FORMAT_XMA || waveformat->format == FMOD_SOUND_FORMAT_MPEG)
    {
        newsample->mLengthBytes = waveformat->lengthbytes;
        overflowbytes = 0;
    }
    else
    {
        result = SoundI::getBytesFromSamples(waveformat->lengthpcm, &newsample->mLengthBytes, waveformat->channels, waveformat->format);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = SoundI::getBytesFromSamples(FMOD_DSP_RESAMPLER_OVERFLOWLENGTH, &overflowbytes, waveformat->channels, waveformat->format);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    newsample->mBufferMemory = FMOD_Memory_Calloc(newsample->mLengthBytes + (overflowbytes * 2));
    if (!newsample->mBufferMemory)
    {
        return FMOD_ERR_MEMORY;
    }

    newsample->mBuffer = (char *)newsample->mBufferMemory + overflowbytes;
    newsample->mFormat = waveformat->format;
    newsample->mLength = waveformat->lengthpcm;

    *sample = newsample;

    return FMOD_OK;
}

int OutputSoftware::getSampleMaxChannels(FMOD_MODE mode, FMOD_SOUND_FORMAT format)
{
    return 8;
}

int OutputSoftware::getSampleMaxChannelsCallback(FMOD_OUTPUT_STATE * output, FMOD_MODE mode, FMOD_SOUND_FORMAT format)
{
    OutputSoftware * outputsoftware = (OutputSoftware *)output;

    return outputsoftware->getSampleMaxChannels(mode, format);
}

} // namespace FMOD
