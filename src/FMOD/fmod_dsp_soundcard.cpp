// G2MEAB fmod_dsp_soundcard.cpp: complete reconstruction (group D).
// .text: 0x80606150..0x80606428 (alloc, release, execute and the implicit deleting dtor 0x80606328).
// alloc reads the DSP block size from SystemI 0x624 (fmod_systemi.h still has the 4.06 offset).

#include "fmod_dsp_soundcard.h"
#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_memory.h"
#include "fmod_systemi.h"

namespace FMOD {

FMOD_RESULT DSPSoundCard::alloc(FMOD_DSP_DESCRIPTION_EX * description)
{
    FMOD_RESULT result;

    result = DSPI::alloc(description);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (description->mFormat == FMOD_SOUND_FORMAT_PCMFLOAT)
    {
        mConversionBuffer = 0;
    }
    else
    {
        mConversionBuffer = (float *)FMOD_Memory_Calloc(mSystem->mDSPBlockSize * description->channels * sizeof(float));
        if (!mConversionBuffer)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    updateTreeLevel(0);

    return FMOD_OK;
}

FMOD_RESULT DSPSoundCard::release(bool freethis)
{
    if (mConversionBuffer)
    {
        FMOD_Memory_Free(mConversionBuffer);
        mConversionBuffer = 0;
    }

    return DSPFilter::release(freethis);
}

FMOD_RESULT DSPSoundCard::execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode)
{
    FMOD_RESULT result;

    if (mConversionBuffer)
    {
        float * buffer = 0;

        result = DSPFilter::execute(mConversionBuffer, &buffer, length, inchannels, outchannels, speakermode);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = DSPI::convert(*outbuffer, buffer, mDescription.mFormat, FMOD_SOUND_FORMAT_PCMFLOAT, *length * *outchannels, 1, 1, 1.0f);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else
    {
        result = DSPFilter::execute(inbuffer, outbuffer, length, inchannels, outchannels, speakermode);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return FMOD_OK;
}

} // namespace FMOD
