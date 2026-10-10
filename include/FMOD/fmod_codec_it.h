// G2MEAB Impulse Tracker codec. No FMOD Ex 4.06 or Gormiti reference names this class; class, member and
// method names are descriptive.
// G2MEAB: sizeof(CodecIT) = 0x3C04 (descriptor mSize, 0x805C84F4); MusicSong ends at +0xA1C.
// The IT channel is a 0x248-byte MusicChannel with a back pointer to the song at +0x244 (open 0x805CE198).

#ifndef _FMOD_CODEC_IT_H
#define _FMOD_CODEC_IT_H

#include "fmod.h"
#include "fmod_music.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct CodecIT;
    class DSPI;
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct MusicEnvelopeState;
    struct MusicInstrument;
    struct MusicNote;
    struct MusicChannel;
    struct MusicSample;
    struct MusicVirtualChannel;
    struct _SNDMIXPLUGIN;
}

namespace FMOD {

// IT header flags kept in MusicSong::mMusicFlags. Guessed names; the values are the IT file flag bits
// (vibrato 0x805C8E98 tests 0x10, portamento 0x805C8DA8 tests 0x20).
enum
{
    FMUSIC_ITFLAGS_STEREO = 0x01, // Guessed name
    FMUSIC_ITFLAGS_VOL0OPTIMIZATION = 0x02, // Guessed name
    FMUSIC_ITFLAGS_INSTRUMENTS = 0x04, // Guessed name
    FMUSIC_ITFLAGS_LINEARFREQUENCY = 0x08, // Guessed name
    FMUSIC_ITFLAGS_OLD_EFFECTS = 0x10, // Guessed name
    FMUSIC_ITFLAGS_EFFECT_G = 0x20 // Guessed name
};

// Envelope type bits (MusicInstrument::mVolumeType/mPanType as tested by processEnvelope 0x805C93F0). Guessed names.
enum
{
    FMUSIC_ENVELOPE_ON = 0x01, // Guessed name
    FMUSIC_ENVELOPE_SUSTAIN = 0x02, // Guessed name
    FMUSIC_ENVELOPE_LOOP = 0x04, // Guessed name
    FMUSIC_ENVELOPE_FILTER = 0x10 // Guessed name; pitch envelope used as filter envelope (0x805C95FC)
};

// IT envelope node as open 0x805CE660 stores it into MusicInstrument::mVolumePoints/mPanPoints (byte value at +0,
// tick word at +2, 25 nodes) and processEnvelope 0x805C93F0 reads it. Guessed name.
struct MusicEnvelopeNode
{
    signed char mValue; // offset 0x0
    unsigned short mTick; // offset 0x2
};

struct MusicChannelIT : public MusicChannel // Guessed name
{
    FMOD_RESULT volumeSlide(); // Guessed name
    FMOD_RESULT portamento(); // Guessed name
    FMOD_RESULT vibrato(); // Guessed name
    FMOD_RESULT fineVibrato(); // Guessed name
    FMOD_RESULT tremolo(); // Guessed name
    FMOD_RESULT panbrello(); // Guessed name
    FMOD_RESULT processVolumeByte(MusicNote * current, bool firsttick); // Guessed name

    CodecIT * mModule; // offset 0x244, Guessed name
};

struct CodecIT : public MusicSong // Guessed name
{
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT calculateLength(); // Guessed name
    FMOD_RESULT readBits(unsigned char bitwidth, unsigned int * result); // Guessed name
    FMOD_RESULT readBlock(char * * buff); // Guessed name
    FMOD_RESULT freeBlock(); // Guessed name
    FMOD_RESULT decompress8(char * * src, void * dst, int len, bool it215, int channels); // Guessed name
    FMOD_RESULT decompress16(char * * src, void * dst, int len, bool it215, int channels); // Guessed name
    FMOD_RESULT processEnvelope(MusicEnvelopeState * env, MusicVirtualChannel * vcptr, int numpoints, MusicEnvelopeNode * points, int type, int loopstart, int loopend, int susloopstart, int susloopend, unsigned char control); // Guessed name
    FMOD_RESULT processPitchEnvelope(MusicVirtualChannel * vcptr, MusicInstrument * iptr, int note); // Guessed name
    FMOD_RESULT sampleVibrato(MusicVirtualChannel * vcptr); // Guessed name
    FMOD_RESULT unpackRow(); // Guessed name
    FMOD_RESULT updateNote(bool audible); // Guessed name
    FMOD_RESULT update(bool audible); // Guessed name
    FMOD_RESULT play(); // Guessed name
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec_state, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec_state);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec_state, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec_state, int subsound, unsigned int position, FMOD_TIMEUNIT postype);

    MusicSample * * mSample; // offset 0xA1C (closeInternal 0x805D08D4 frees it)
    MusicSample mSampleMem[256]; // offset 0xA20, Guessed name
    unsigned char * mPatternData; // offset 0x3A20, packed pattern read position (play 0x805CCFC0), Guessed name
    char * mSourceBuffer; // offset 0x3A24, packed sample block (readBlock 0x805C8798), Guessed name
    unsigned int * mSourcePos; // offset 0x3A28, Guessed name
    unsigned char mRemBits; // offset 0x3A2C, bits left in *mSourcePos, Guessed name
    int mUnk3A30; // offset 0x3A30, count of the mLowPass units released by closeInternal
    _SNDMIXPLUGIN * mMixPlugin[50]; // offset 0x3A34, Guessed name
    unsigned char mUnk3AFC[256]; // offset 0x3AFC
    DSPI * mDSPFinalHead; // offset 0x3BFC, "FMOD IT final mixdown unit", Guessed name
    DSPI * mDSPEffectHead; // offset 0x3C00, "FMOD IT global effect head unit", Guessed name
};

} // namespace FMOD

#endif
