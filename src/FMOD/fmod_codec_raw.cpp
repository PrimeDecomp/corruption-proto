// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805E308C..0x805E3420 (10 native functions).
// Inferred basename; original source filename is unproven.
// CodecRaw adds no members (descriptor mSize 0x1F4) and decodes into the embedded Codec waveformat.
// canPointInternal/canPointCallback do not exist in this FMOD version.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference.

#include "fmod_codec_raw.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_file.h"

#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX rawcodec;

FMOD_CODEC_DESCRIPTION_EX * CodecRaw::getDescriptionEx()
{
    memset(&rawcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    rawcodec.name = "FMOD Raw Codec";
    rawcodec.version = 0x00010100;
    rawcodec.timeunits = FMOD_TIMEUNIT_PCM | FMOD_TIMEUNIT_RAWBYTES;
    rawcodec.open = &CodecRaw::openCallback;
    rawcodec.close = &CodecRaw::closeCallback;
    rawcodec.read = &CodecRaw::readCallback;
    rawcodec.setposition = &CodecRaw::setPositionCallback;

    rawcodec.mType = FMOD_SOUND_TYPE_RAW;
    rawcodec.mSize = sizeof(CodecRaw);

    return &rawcodec;
}

FMOD_RESULT CodecRaw::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;

    init(FMOD_SOUND_TYPE_RAW);

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = 0;
    waveformat = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getSize(&mWaveFormat.lengthbytes);
    if (result != FMOD_OK)
    {
        return result;
    }

    mSrcDataOffset = 0;

    mWaveFormat.format = FMOD_SOUND_FORMAT_PCM16;
    mWaveFormat.channels = 1;
    mWaveFormat.frequency = 44100;
    mWaveFormat.lengthpcm = 0;
    mWaveFormat.blockalign = mWaveFormat.channels * 16 / 8;

    numsubsounds = 0;
    waveformat = &mWaveFormat;

    return result;
}

FMOD_RESULT CodecRaw::closeInternal()
{
    return FMOD_OK;
}

FMOD_RESULT CodecRaw::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    FMOD_RESULT result;

    if (mWaveFormat.format == FMOD_SOUND_FORMAT_PCM16)
    {
        result = mFile->read(buffer, 2, sizebytes / 2, bytesread);
        *bytesread *= 2;
    }
    else
    {
        result = mFile->read(buffer, 1, sizebytes, bytesread);
    }

    return result;
}

FMOD_RESULT CodecRaw::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    unsigned int raw;

    if (postype == FMOD_TIMEUNIT_RAWBYTES)
    {
        raw = position;
    }
    else
    {
        raw = (unsigned int)((unsigned long long)position * mWaveFormat.lengthbytes / mWaveFormat.lengthpcm);
    }

    result = mFile->seek(mSrcDataOffset + raw, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecRaw::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    return ((CodecRaw *)codec)->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecRaw::closeCallback(FMOD_CODEC_STATE * codec)
{
    return ((CodecRaw *)codec)->closeInternal();
}

FMOD_RESULT CodecRaw::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    return ((CodecRaw *)codec)->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecRaw::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    return ((CodecRaw *)codec)->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
