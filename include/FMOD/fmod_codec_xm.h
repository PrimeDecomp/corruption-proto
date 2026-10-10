// Method set from a later FMOD Ex (Gormiti, Wii/MWCC) debug information, adapted to the G2MEAB natives.
// G2MEAB: sizeof(CodecXM) = 0xA20 (descriptor mSize, 0x805EB78C); mSample sits right after MusicSong
// (closeInternal 0x805EFB3C frees +0xA1C). Gormiti's mChannelPoolMemory is absent.

#ifndef _FMOD_CODEC_XM_H
#define _FMOD_CODEC_XM_H

#include "fmod.h"
#include "fmod_music.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct CodecXM;
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct MusicChannel;
    struct MusicChannelXM;
    struct MusicEnvelopeState;
    struct MusicInstrument;
    struct MusicNote;
    struct MusicSample;
    struct MusicSong;
    struct MusicVirtualChannel;
}

namespace FMOD {

// XM effect numbers (pattern effect byte; letters G..X continue after F). Guessed names (FMOD 3 spelling);
// the values are the updateNote 0x805EC58C / updateEffects 0x805ED190 switch cases.
enum
{
    FMUSIC_XM_ARPEGGIO = 0,
    FMUSIC_XM_PORTAUP = 1,
    FMUSIC_XM_PORTADOWN = 2,
    FMUSIC_XM_PORTATO = 3,
    FMUSIC_XM_VIBRATO = 4,
    FMUSIC_XM_PORTATOVOLSLIDE = 5,
    FMUSIC_XM_VIBRATOVOLSLIDE = 6,
    FMUSIC_XM_TREMOLO = 7,
    FMUSIC_XM_SETPANPOSITION = 8,
    FMUSIC_XM_SETSAMPLEOFFSET = 9,
    FMUSIC_XM_VOLUMESLIDE = 10,
    FMUSIC_XM_PATTERNJUMP = 11,
    FMUSIC_XM_SETVOLUME = 12,
    FMUSIC_XM_PATTERNBREAK = 13,
    FMUSIC_XM_SPECIAL = 14,
    FMUSIC_XM_SETSPEED = 15,
    FMUSIC_XM_SETGLOBALVOLUME = 16,
    FMUSIC_XM_GLOBALVOLSLIDE = 17,
    FMUSIC_XM_KEYOFF = 20,
    FMUSIC_XM_SETENVELOPEPOS = 21,
    FMUSIC_XM_PANSLIDE = 25,
    FMUSIC_XM_MULTIRETRIG = 27,
    FMUSIC_XM_TREMOR = 29,
    FMUSIC_XM_EXTRAFINEPORTA = 33,
    FMUSIC_XM_Z = 35 // no-op case: the updateNote/updateEffects jump tables run to 0x23
};

// XM Exy sub-effects. Guessed names (FMOD 3 spelling).
enum
{
    FMUSIC_XM_FINEPORTAUP = 1,
    FMUSIC_XM_FINEPORTADOWN = 2,
    FMUSIC_XM_SETGLISSANDO = 3,
    FMUSIC_XM_SETVIBRATOWAVE = 4,
    FMUSIC_XM_SETFINETUNE = 5,
    FMUSIC_XM_PATTERNLOOP = 6,
    FMUSIC_XM_SETTREMOLOWAVE = 7,
    FMUSIC_XM_SETPANPOSITION16 = 8,
    FMUSIC_XM_RETRIG = 9,
    FMUSIC_XM_FINEVOLUMESLIDEUP = 10,
    FMUSIC_XM_FINEVOLUMESLIDEDOWN = 11,
    FMUSIC_XM_NOTECUT = 12,
    FMUSIC_XM_NOTEDELAY = 13,
    FMUSIC_XM_PATTERNDELAY = 14,
    FMUSIC_XM_FUNKREPEAT = 15 // no-op case: the updateNote Exy jump table runs to 0xF
};

struct CodecXM : public MusicSong
{
    FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT calculateLength(); // Guessed name
    FMOD_RESULT getVirtualChannel(MusicChannel * cptr, MusicVirtualChannel * vcptr, MusicSample * sptr, MusicVirtualChannel * * newvcptr); // Guessed name
    FMOD_RESULT updateFlags(MusicVirtualChannel * vcptr, MusicSample * sptr);
    FMOD_RESULT processEnvelope(MusicEnvelopeState * env, MusicVirtualChannel * vcptr, int numpoints, unsigned short * points, int type, int loopstart, int loopend, unsigned char ISustain, unsigned char control);
    FMOD_RESULT getAmigaPeriod(int note, int finetune, int * period); // Guessed name
    FMOD_RESULT processNote(MusicNote * current, MusicChannelXM * cptr, MusicVirtualChannel * vcptr, MusicInstrument * iptr, MusicSample * sptr);
    FMOD_RESULT updateNote();
    FMOD_RESULT updateEffects();
    FMOD_RESULT update(bool audible);
    FMOD_RESULT openInternal(unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, unsigned int postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec_state, unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec_state);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec_state, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec_state, int subsound, unsigned int position, unsigned int postype);
    MusicSample * * mSample; // offset 0xA1C
};

struct MusicChannelXM : public MusicChannel
{
    FMOD_RESULT portamento(); // Guessed name
    FMOD_RESULT vibrato();
    FMOD_RESULT tremolo();
    FMOD_RESULT instrumentVibrato(MusicInstrument * iptr);
    FMOD_RESULT processVolumeByte(unsigned char volume);
};

} // namespace FMOD

#endif
