// G2MEAB S3M codec. No reference debug information names this class; the method set follows CodecXM.
// sizeof(CodecS3M) = 0x1CAC (descriptor mSize, 0x805E3420): MusicSong (0xA1C) plus 99 inline MusicSamples
// (closeInternal 0x805E6AE4 releases +0xA1C with a 0x30 stride).

#ifndef _FMOD_CODEC_S3M_H
#define _FMOD_CODEC_S3M_H

#include "fmod.h"
#include "fmod_music.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct CodecS3M;
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct MusicChannelS3M;
}

namespace FMOD {

// S3M effect letters (A = 1 .. Z = 26) and Sxy sub-effects. Guessed names (FMOD 3 spelling); the values are
// the updateNote 0x805E3A20 / updateEffects 0x805E45F8 switch cases.
enum
{
    FMUSIC_S3M_SETSPEED = 1,
    FMUSIC_S3M_PATTERNJUMP = 2,
    FMUSIC_S3M_PATTERNBREAK = 3,
    FMUSIC_S3M_VOLUMESLIDE = 4,
    FMUSIC_S3M_PORTADOWN = 5,
    FMUSIC_S3M_PORTAUP = 6,
    FMUSIC_S3M_PORTATO = 7,
    FMUSIC_S3M_VIBRATO = 8,
    FMUSIC_S3M_TREMOR = 9,
    FMUSIC_S3M_ARPEGGIO = 10,
    FMUSIC_S3M_VIBRATOVOLSLIDE = 11,
    FMUSIC_S3M_PORTATOVOLSLIDE = 12,
    FMUSIC_S3M_M = 13,
    FMUSIC_S3M_N = 14,
    FMUSIC_S3M_SETSAMPLEOFFSET = 15,
    FMUSIC_S3M_P = 16,
    FMUSIC_S3M_RETRIGVOLSLIDE = 17,
    FMUSIC_S3M_TREMOLO = 18,
    FMUSIC_S3M_SPECIAL = 19,
    FMUSIC_S3M_SETTEMPO = 20,
    FMUSIC_S3M_FINEVIBRATO = 21,
    FMUSIC_S3M_GLOBALVOLUME = 22,
    FMUSIC_S3M_W = 23,
    FMUSIC_S3M_SETPAN = 24,
    FMUSIC_S3M_Y = 25,
    FMUSIC_S3M_Z = 26
};

enum
{
    FMUSIC_S3M_SETFILTER = 0,
    FMUSIC_S3M_SETGLISSANDO = 1,
    FMUSIC_S3M_SETFINETUNE = 2,
    FMUSIC_S3M_SETVIBRATOWAVE = 3,
    FMUSIC_S3M_SETTREMOLOWAVE = 4,
    FMUSIC_S3M_SETPANPOSITION16 = 8,
    FMUSIC_S3M_STEREOCONTROL = 10,
    FMUSIC_S3M_PATTERNLOOP = 11,
    FMUSIC_S3M_NOTECUT = 12,
    FMUSIC_S3M_NOTEDELAY = 13,
    FMUSIC_S3M_PATTERNDELAY = 14,
    FMUSIC_S3M_FUNKREPEAT = 15
};

struct CodecS3M : public MusicSong // Guessed name
{
    FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT calculateLength(); // Guessed name
    FMOD_RESULT updateNote(bool audible); // Guessed name; parameter from update 0x805E4FC0 (r4 passed through)
    FMOD_RESULT updateEffects(); // Guessed name
    FMOD_RESULT update(bool audible);
    FMOD_RESULT openInternal(unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, unsigned int postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec_state, unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec_state);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec_state, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec_state, int subsound, unsigned int position, unsigned int postype);
    MusicSample mSample[99]; // offset 0xA1C
};

struct MusicChannelS3M : public MusicChannel // Guessed name
{
    FMOD_RESULT volumeSlide(); // Guessed name
    FMOD_RESULT portamento(); // Guessed name
    FMOD_RESULT vibrato();
    FMOD_RESULT tremolo();
    FMOD_RESULT fineVibrato(); // Guessed name
};

} // namespace FMOD

#endif
