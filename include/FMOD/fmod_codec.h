// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CODEC_H
#define _FMOD_CODEC_H

#include "fmod.h"

struct FMOD_CODEC_DESCRIPTION;
struct FMOD_CODEC_STATE;
struct FMOD_CODEC_WAVEFORMAT;
struct FMOD_CREATESOUNDEXINFO;

typedef FMOD_CODEC_STATE FMOD_CODEC_STATE;
typedef FMOD_CODEC_WAVEFORMAT FMOD_CODEC_WAVEFORMAT;
typedef FMOD_RESULT (* FMOD_CODEC_OPENCALLBACK)(FMOD_CODEC_STATE *, FMOD_MODE, FMOD_CREATESOUNDEXINFO *);
typedef FMOD_RESULT (* FMOD_CODEC_CLOSECALLBACK)(FMOD_CODEC_STATE *);
typedef FMOD_RESULT (* FMOD_CODEC_READCALLBACK)(FMOD_CODEC_STATE *, void *, unsigned int, unsigned int *);
typedef FMOD_RESULT (* FMOD_CODEC_GETLENGTHCALLBACK)(FMOD_CODEC_STATE *, unsigned int *, FMOD_TIMEUNIT);
typedef FMOD_RESULT (* FMOD_CODEC_SETPOSITIONCALLBACK)(FMOD_CODEC_STATE *, int, unsigned int, FMOD_TIMEUNIT);
typedef FMOD_RESULT (* FMOD_CODEC_GETPOSITIONCALLBACK)(FMOD_CODEC_STATE *, unsigned int *, FMOD_TIMEUNIT);
typedef FMOD_RESULT (* FMOD_CODEC_SOUNDCREATECALLBACK)(FMOD_CODEC_STATE *, int, FMOD_SOUND *);
typedef FMOD_RESULT (* FMOD_CODEC_METADATACALLBACK)(FMOD_CODEC_STATE *, FMOD_TAGTYPE, char *, void *, unsigned int, FMOD_TAGDATATYPE, int);
typedef FMOD_RESULT (* FMOD_CODEC_GETWAVEFORMAT)(FMOD_CODEC_STATE *, int, FMOD_CODEC_WAVEFORMAT *);
// G2MEAB: 0x2C bytes. The description EX node starts at +0x2C (__sinit_fmod_codec_wav_cpp), so the
// 4.06 getwaveformat member is absent; no builder stores past soundcreate.
struct FMOD_CODEC_DESCRIPTION
{
    const char * name; // offset 0x0
    unsigned int version; // offset 0x4
    int defaultasstream; // offset 0x8
    FMOD_TIMEUNIT timeunits; // offset 0xC
    FMOD_CODEC_OPENCALLBACK open; // offset 0x10
    FMOD_CODEC_CLOSECALLBACK close; // offset 0x14
    FMOD_CODEC_READCALLBACK read; // offset 0x18
    FMOD_CODEC_GETLENGTHCALLBACK getlength; // offset 0x1C
    FMOD_CODEC_SETPOSITIONCALLBACK setposition; // offset 0x20
    FMOD_CODEC_GETPOSITIONCALLBACK getposition; // offset 0x24
    FMOD_CODEC_SOUNDCREATECALLBACK soundcreate; // offset 0x28
};

typedef FMOD_CODEC_DESCRIPTION FMOD_CODEC_DESCRIPTION;
struct FMOD_CODEC_WAVEFORMAT
{
    char name[256]; // offset 0x0
    FMOD_SOUND_FORMAT format; // offset 0x100
    int channels; // offset 0x104
    int frequency; // offset 0x108
    unsigned int lengthbytes; // offset 0x10C
    unsigned int lengthpcm; // offset 0x110
    int blockalign; // offset 0x114
    int loopstart; // offset 0x118
    int loopend; // offset 0x11C
    FMOD_MODE mode; // offset 0x120
    unsigned int channelmask; // offset 0x124
};

struct FMOD_CODEC_STATE
{
    int numsubsounds; // offset 0x0
    FMOD_CODEC_WAVEFORMAT * waveformat; // offset 0x4
    void * plugindata; // offset 0x8
    void * filehandle; // offset 0xC
    unsigned int filesize; // offset 0x10
    FMOD_FILE_READCALLBACK fileread; // offset 0x14
    FMOD_FILE_SEEKCALLBACK fileseek; // offset 0x18
    FMOD_CODEC_METADATACALLBACK metadata; // offset 0x1C
};

#endif
