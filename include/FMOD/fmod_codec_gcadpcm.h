// G2MEAB-only Retro GCADPCM codec ("Retro GCADPCM Codec", descriptor 0x80756E84); no 4.06 or Gormiti
// counterpart exists, so the class and member names are guessed. sizeof 0x258 (descriptor mSize, 0x8062685C).

#ifndef _FMOD_CODEC_GCADPCM_H
#define _FMOD_CODEC_GCADPCM_H

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
}

namespace FMOD {

// Guessed name: the 0x60-byte Nintendo DSP ADPCM header read whole by openInternal 0x806268B0.
struct GCADPCM_HEADER
{
    unsigned int numsamples;         // 0x00 -> lengthpcm (0x80626938)
    unsigned int numnibbles;         // 0x04
    unsigned int samplerate;         // 0x08 -> frequency (0x80626930)
    unsigned short loopflag;         // 0x0C -> loop mode (0x80626BC8)
    unsigned short format;           // 0x0E, must be 0 (0x806268D4)
    unsigned int loopstart;          // 0x10 -> loopstart (0x80626BBC)
    unsigned int loopend;            // 0x14 -> loopend (0x80626BC4)
    unsigned int currentaddress;     // 0x18 (0x806268F4)
    short coefficients[16];          // 0x1C, exposed through plugindata
    unsigned short gain;             // 0x3C
    unsigned short predscale;        // 0x3E
    short history[2];                // 0x40
    unsigned short looppredscale;    // 0x44
    short loophistory[2];            // 0x46
    unsigned short pad[11];          // 0x4A
};

// Guessed name
class CodecGCADPCM : public Codec
{
public:
    GCADPCM_HEADER mHeader;          // 0x1F4 // Guessed name
    short * mCoefficients;           // 0x254, set to mHeader.coefficients, plugindata points here // Guessed name

    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
};

} // namespace FMOD

#endif
