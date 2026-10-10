// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. G2MEAB: sizeof(CodecRaw) == sizeof(Codec) == 0x1F4
// (descriptor mSize 0x805E310C); the 4.06 mWaveFormat member is Codec::mWaveFormat here and canPoint is absent.

#ifndef _FMOD_CODEC_RAW_H
#define _FMOD_CODEC_RAW_H

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
}

namespace FMOD {

class CodecRaw : public Codec
{
public:
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
};

} // namespace FMOD

#endif
