// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805E8310..0x805E85D4 (10 native functions).
// Inferred basename; original source filename is unproven.
// CodecUser adds no members (descriptor mSize 0x1F4); open fills the embedded Codec waveformat.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference.

#include "fmod_codec_user.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_file.h"

#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX usercodec;

FMOD_CODEC_DESCRIPTION_EX * CodecUser::getDescriptionEx()
{
    memset(&usercodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    usercodec.name = "FMOD User Reader Codec";
    usercodec.version = 0x00010100;
    usercodec.timeunits = FMOD_TIMEUNIT_PCM;
    usercodec.open = &CodecUser::openCallback;
    usercodec.close = &CodecUser::closeCallback;
    usercodec.read = &CodecUser::readCallback;
    usercodec.setposition = &CodecUser::setPositionCallback;

    usercodec.mType = FMOD_SOUND_TYPE_USER;
    usercodec.mSize = sizeof(CodecUser);

    return &usercodec;
}

FMOD_RESULT CodecUser::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;

    init(FMOD_SOUND_TYPE_USER);

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

FMOD_RESULT CodecUser::closeInternal()
{
    return FMOD_OK;
}

FMOD_RESULT CodecUser::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    *bytesread = sizebytes;

    return FMOD_OK;
}

FMOD_RESULT CodecUser::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    return FMOD_OK;
}

FMOD_RESULT CodecUser::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    return ((CodecUser *)codec)->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecUser::closeCallback(FMOD_CODEC_STATE * codec)
{
    return ((CodecUser *)codec)->closeInternal();
}

FMOD_RESULT CodecUser::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    return ((CodecUser *)codec)->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecUser::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    return ((CodecUser *)codec)->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
