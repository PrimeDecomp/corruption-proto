// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

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
    FMOD_CODEC_WAVEFORMAT mWaveFormat; // offset 0xD8
public:
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT canPointInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT canPointCallback(FMOD_CODEC_STATE * codec);
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
};

} // namespace FMOD

#endif
