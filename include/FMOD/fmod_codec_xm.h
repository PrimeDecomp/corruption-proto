// Reconstructed from a later FMOD Ex (Gormiti, Wii/MWCC) debug information. Member layout and offsets are the Gormiti reference, not yet verified against G2MEAB.

#ifndef _FMOD_CODEC_XM_H
#define _FMOD_CODEC_XM_H

#include "fmod.h"
#include "fmod_music.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    class ChannelPool;
    struct CodecXM;
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct MusicChannel;
    struct MusicChannelXM;
    struct MusicInstrument;
    struct MusicNote;
    struct MusicSample;
    struct MusicSong;
    struct MusicVirtualChannel;
}

namespace FMOD {

struct CodecXM : public MusicSong
{
    FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT updateFlags(MusicChannel * cptr, MusicVirtualChannel * vcptr);
    FMOD_RESULT processEnvelope(unsigned char ISustain, unsigned char control);
    FMOD_RESULT processNote(MusicNote * current, MusicVirtualChannel * vcptr, MusicInstrument * iptr);
    FMOD_RESULT updateNote();
    FMOD_RESULT updateEffects();
    FMOD_RESULT update();
    FMOD_RESULT openInternal(unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(unsigned int position);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec_state, unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec_state);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec_state, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec_state, int subsound, unsigned int position, unsigned int postype);
    static FMOD_RESULT updateCallback(FMOD_CODEC_STATE * codec);
    MusicSample * * mSample; // offset 0x8A0
    ChannelPool * mChannelPoolMemory; // offset 0x8A4
};

struct MusicChannelXM : public MusicChannel
{
    FMOD_RESULT vibrato();
    FMOD_RESULT tremolo();
    FMOD_RESULT instrumentVibrato();
    FMOD_RESULT processVolumeByte();
};

} // namespace FMOD

#endif
