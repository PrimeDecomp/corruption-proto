// G2MEAB MOD codec. No reference debug information names this class; the method set follows CodecXM.
// sizeof(CodecMOD) = 0xFEC (descriptor mSize, 0x805D4E84): MusicSong (0xA1C) plus 31 inline MusicSamples
// (closeInternal 0x805D7908 releases +0xA1C with a 0x30 stride).

#ifndef _FMOD_CODEC_MOD_H
#define _FMOD_CODEC_MOD_H

#include "fmod.h"
#include "fmod_music.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct CodecMOD;
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct MusicChannelMOD;
}

namespace FMOD {

// Protracker effect numbers and Exy sub-effects. Guessed names (FMOD 3 spelling); the values are the
// updateNote 0x805D529C / updateEffects 0x805D5B94 switch cases.
enum
{
    FMUSIC_MOD_ARPEGGIO = 0,
    FMUSIC_MOD_PORTAUP = 1,
    FMUSIC_MOD_PORTADOWN = 2,
    FMUSIC_MOD_PORTATO = 3,
    FMUSIC_MOD_VIBRATO = 4,
    FMUSIC_MOD_PORTATOVOLSLIDE = 5,
    FMUSIC_MOD_VIBRATOVOLSLIDE = 6,
    FMUSIC_MOD_TREMOLO = 7,
    FMUSIC_MOD_SETPANPOSITION = 8,
    FMUSIC_MOD_SETSAMPLEOFFSET = 9,
    FMUSIC_MOD_VOLUMESLIDE = 10,
    FMUSIC_MOD_PATTERNJUMP = 11,
    FMUSIC_MOD_SETVOLUME = 12,
    FMUSIC_MOD_PATTERNBREAK = 13,
    FMUSIC_MOD_SPECIAL = 14,
    FMUSIC_MOD_SETSPEED = 15
};

enum
{
    FMUSIC_MOD_SETFILTER = 0,
    FMUSIC_MOD_FINEPORTAUP = 1,
    FMUSIC_MOD_FINEPORTADOWN = 2,
    FMUSIC_MOD_SETGLISSANDO = 3,
    FMUSIC_MOD_SETVIBRATOWAVE = 4,
    FMUSIC_MOD_SETFINETUNE = 5,
    FMUSIC_MOD_PATTERNLOOP = 6,
    FMUSIC_MOD_SETTREMOLOWAVE = 7,
    FMUSIC_MOD_SETPANPOSITION16 = 8,
    FMUSIC_MOD_RETRIG = 9,
    FMUSIC_MOD_FINEVOLUMESLIDEUP = 10,
    FMUSIC_MOD_FINEVOLUMESLIDEDOWN = 11,
    FMUSIC_MOD_NOTECUT = 12,
    FMUSIC_MOD_NOTEDELAY = 13,
    FMUSIC_MOD_PATTERNDELAY = 14,
    FMUSIC_MOD_FUNKREPEAT = 15
};

struct CodecMOD : public MusicSong // Guessed name
{
    FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT calculateLength(); // Guessed name
    FMOD_RESULT updateNote(bool audible); // Guessed name; parameter from update 0x805D63A4 (r4 passed through)
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
    MusicSample mSample[31]; // offset 0xA1C
};

struct MusicChannelMOD : public MusicChannel // Guessed name
{
    FMOD_RESULT portamento(); // Guessed name
    FMOD_RESULT vibrato();
    FMOD_RESULT tremolo();
};

} // namespace FMOD

#endif
