// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x806267E0..0x80626F40 (10 native functions).
// Source identity: inferred from the Retro GCADPCM codec descriptor ("Retro GCADPCM Codec").
// This codec has no 4.06 counterpart; the class and method names follow the other codecs and are guessed.

#include "fmod_codec_gcadpcm.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_file.h"
#include "fmod_soundi.h"

#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX gcadpcmcodec; // Guessed name

FMOD_CODEC_DESCRIPTION_EX * CodecGCADPCM::getDescriptionEx()
{
    memset(&gcadpcmcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    gcadpcmcodec.name = "Retro GCADPCM Codec";
    gcadpcmcodec.version = 0x00010100;
    gcadpcmcodec.timeunits = FMOD_TIMEUNIT_PCM;
    gcadpcmcodec.open = &CodecGCADPCM::openCallback;
    gcadpcmcodec.close = &CodecGCADPCM::closeCallback;
    gcadpcmcodec.read = &CodecGCADPCM::readCallback;
    gcadpcmcodec.setposition = &CodecGCADPCM::setPositionCallback;

    gcadpcmcodec.mType = FMOD_SOUND_TYPE_GCADPCM;
    gcadpcmcodec.mSize = sizeof(CodecGCADPCM);

    return &gcadpcmcodec;
}

FMOD_RESULT CodecGCADPCM::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;
    unsigned int rd;
    FMOD_MODE mode;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(&mHeader, 1, sizeof(GCADPCM_HEADER), &rd);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mHeader.format && mHeader.loopflag != 0 && mHeader.loopflag != 1 && mHeader.currentaddress)
    {
        return FMOD_ERR_FORMAT;
    }

    result = mFile->tell(&mSrcDataOffset);
    if (result != FMOD_OK)
    {
        return result;
    }

    mWaveFormat.format = FMOD_SOUND_FORMAT_GCADPCM;
    mWaveFormat.channels = 1;
    mWaveFormat.frequency = mHeader.samplerate;
    mWaveFormat.lengthpcm = mHeader.numsamples;
    mWaveFormat.blockalign = 0;

    SoundI::getBytesFromSamples(mHeader.numsamples, &mWaveFormat.lengthbytes, mWaveFormat.channels, mWaveFormat.format);
    SoundI::getBytesFromSamples(1, (unsigned int *)&mWaveFormat.blockalign, mWaveFormat.channels, mWaveFormat.format);

    mWaveFormat.loopstart = mHeader.loopstart;
    mWaveFormat.loopend = mHeader.loopend;

    mode = 0;
    if (mHeader.loopflag)
    {
        mode |= FMOD_LOOP_NORMAL;
    }
    else
    {
        mode |= FMOD_LOOP_OFF;
    }
    mWaveFormat.mode = mode | FMOD_2D | FMOD_HARDWARE;

    mCoefficients = mHeader.coefficients;
    plugindata = &mCoefficients;

    numsubsounds = 0;
    waveformat = &mWaveFormat;

    return FMOD_OK;
}

FMOD_RESULT CodecGCADPCM::closeInternal()
{
    return FMOD_OK;
}

FMOD_RESULT CodecGCADPCM::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    unsigned int bytes;

    if (mWaveFormat.format >= FMOD_SOUND_FORMAT_MAX || mWaveFormat.format < FMOD_SOUND_FORMAT_NONE)
    {
        result = FMOD_ERR_FORMAT;
    }
    else
    {
        result = FMOD_OK;
    }
    if (result != FMOD_OK)
    {
        return result;
    }

    SoundI::getBytesFromSamples(position, &bytes, mWaveFormat.channels, mWaveFormat.format);

    return mFile->seek(mSrcDataOffset + bytes, 0);
}

FMOD_RESULT CodecGCADPCM::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    return mFile->read(buffer, 1, sizebytes, bytesread);
}

FMOD_RESULT CodecGCADPCM::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    return ((CodecGCADPCM *)codec)->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecGCADPCM::closeCallback(FMOD_CODEC_STATE * codec)
{
    return ((CodecGCADPCM *)codec)->closeInternal();
}

FMOD_RESULT CodecGCADPCM::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    return ((CodecGCADPCM *)codec)->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecGCADPCM::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    return ((CodecGCADPCM *)codec)->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
