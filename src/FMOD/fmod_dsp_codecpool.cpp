// G2MEAB fmod_dsp_codecpool.cpp: complete reconstruction (group D).
// .text: 0x80628C88..0x806293B4 (4 native functions): init 0x80628C88, close 0x806291B4, alloc 0x80629284 and
// the implicit CodecMPEG deleting dtor 0x806292F4 (its vtable 0x806F0180 is emitted here).

#include "fmod_dsp_codecpool.h"
#include "fmod.h"
#include "fmod_codec_mpeg.h"
#include "fmod_codec_wav.h"
#include "fmod_codeci.h"
#include "fmod_dsp_codec.h"
#include "fmod_dspi.h"
#include "fmod_memory.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

FMOD_RESULT DSPCodecPool::init(Codec * codec, int numdspcodecs)
{
    FMOD_RESULT result;
    FMOD_DSP_DESCRIPTION_EX descriptionex;
    int count;

    mNumDSPCodecs = numdspcodecs;

    mPool = (DSPI * *)FMOD_Memory_Calloc(numdspcodecs * sizeof(DSPI *));
    if (!mPool)
    {
        return FMOD_ERR_MEMORY;
    }

    descriptionex = *DSPCodec::getDescriptionEx();

    for (count = 0; count < numdspcodecs; count++)
    {
        DSPI * dsp;
        DSPCodec * dspcodec;

        descriptionex.mResamplerBlockLength = codec->mPCMBufferLength;
        descriptionex.channels = 2;

        result = mSystem->createDSP(&descriptionex, &dsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        dspcodec = (DSPCodec *)dsp;

        if (codec->mWaveFormat.format == FMOD_SOUND_FORMAT_MPEG)
        {
            CodecMPEG * mpeg = FMOD_Object_Alloc(CodecMPEG);

            memcpy(mpeg, codec, codec->mDescription.mSize);

            mpeg->mPCMBuffer = mpeg->mPCMBufferMemory;
            mpeg->mPCMBuffer = (unsigned char *)(((unsigned int)mpeg->mPCMBuffer + 15) & ~15);

            dspcodec->mCodec = mpeg;
        }
        else if (codec->mWaveFormat.format == FMOD_SOUND_FORMAT_IMAADPCM)
        {
            CodecWav * wav = FMOD_Object_Alloc(CodecWav);

            memcpy(wav, codec, codec->mDescription.mSize);

            wav->mPCMBuffer = (unsigned char *)FMOD_Memory_Calloc(codec->mPCMBufferLengthBytes);
            if (!wav->mPCMBuffer)
            {
                return FMOD_ERR_MEMORY;
            }

            dspcodec->mCodec = wav;
        }
        else
        {
            return FMOD_ERR_FORMAT;
        }

        dspcodec->mCodec->mFile = &dspcodec->mFileMemory;
        dspcodec->mCodec->mSrcDataOffset = 0;
        dspcodec->mCodec->mAccurateLength = true;
        dspcodec->mPool = this;

        mPool[count] = dsp;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPCodecPool::close()
{
    int count;

    if (mPool)
    {
        for (count = 0; count < mNumDSPCodecs; count++)
        {
            FMOD_Memory_Free(((DSPCodec *)mPool[count])->mCodec);
            mPool[count]->release(true);
        }

        FMOD_Memory_Free(mPool);
        mPool = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPCodecPool::alloc(DSPI * * dspcodec)
{
    int count;

    for (count = 0; count < mNumDSPCodecs; count++)
    {
        if (!mPool[count]->mAllocated)
        {
            mPool[count]->mAllocated = true;
            *dspcodec = mPool[count];
            mUnk8++;
            return FMOD_OK;
        }
    }

    return FMOD_ERR_CHANNEL_ALLOC;
}

} // namespace FMOD
