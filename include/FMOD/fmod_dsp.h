// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod.h"

#ifndef _FMOD_DSP_H
#define _FMOD_DSP_H

struct FMOD_DSP_DESCRIPTION;
struct FMOD_DSP_PARAMETERDESC;
struct FMOD_DSP_STATE;

typedef FMOD_DSP_STATE FMOD_DSP_STATE;
typedef FMOD_RESULT (* FMOD_DSP_CREATECALLBACK)(FMOD_DSP_STATE *);
typedef FMOD_RESULT (* FMOD_DSP_RELEASECALLBACK)(FMOD_DSP_STATE *);
typedef FMOD_RESULT (* FMOD_DSP_RESETCALLBACK)(FMOD_DSP_STATE *);
typedef FMOD_RESULT (* FMOD_DSP_READCALLBACK)(FMOD_DSP_STATE *, float *, float *, unsigned int, int, int);
typedef FMOD_RESULT (* FMOD_DSP_SETPOSITIONCALLBACK)(FMOD_DSP_STATE *, unsigned int);
typedef FMOD_RESULT (* FMOD_DSP_SETPARAMCALLBACK)(FMOD_DSP_STATE *, int, float);
typedef FMOD_RESULT (* FMOD_DSP_GETPARAMCALLBACK)(FMOD_DSP_STATE *, int, float *, char *);
typedef FMOD_RESULT (* FMOD_DSP_DIALOGCALLBACK)(FMOD_DSP_STATE *, void *, int);
enum FMOD_DSP_TYPE {
    FMOD_DSP_TYPE_UNKNOWN = 0,
    FMOD_DSP_TYPE_MIXER = 1,
    FMOD_DSP_TYPE_OSCILLATOR = 2,
    FMOD_DSP_TYPE_LOWPASS = 3,
    FMOD_DSP_TYPE_ITLOWPASS = 4,
    FMOD_DSP_TYPE_HIGHPASS = 5,
    FMOD_DSP_TYPE_ECHO = 6,
    FMOD_DSP_TYPE_FLANGE = 7,
    FMOD_DSP_TYPE_DISTORTION = 8,
    FMOD_DSP_TYPE_NORMALIZE = 9,
    FMOD_DSP_TYPE_PARAMEQ = 10,
    FMOD_DSP_TYPE_PITCHSHIFT = 11,
    FMOD_DSP_TYPE_CHORUS = 12,
    FMOD_DSP_TYPE_REVERB = 13,
    FMOD_DSP_TYPE_VSTPLUGIN = 14,
    FMOD_DSP_TYPE_WINAMPPLUGIN = 15,
    FMOD_DSP_TYPE_ITECHO = 16,
    FMOD_DSP_TYPE_COMPRESSOR = 17,
    FMOD_DSP_TYPE_SFXREVERB = 18,
    FMOD_DSP_TYPE_LOWPASS_SIMPLE = 19,
    FMOD_DSP_TYPE_FORCEINT = 65536
};

struct FMOD_DSP_PARAMETERDESC
{
    float min; // offset 0x0
    float max; // offset 0x4
    float defaultval; // offset 0x8
    char name[16]; // offset 0xC
    char label[16]; // offset 0x1C
    const char * description; // offset 0x2C
};

typedef FMOD_DSP_PARAMETERDESC FMOD_DSP_PARAMETERDESC;
struct FMOD_DSP_DESCRIPTION
{
    char name[32]; // offset 0x0
    unsigned int version; // offset 0x20
    int channels; // offset 0x24
    FMOD_DSP_CREATECALLBACK create; // offset 0x28
    FMOD_DSP_RELEASECALLBACK release; // offset 0x2C
    FMOD_DSP_RESETCALLBACK reset; // offset 0x30
    FMOD_DSP_READCALLBACK read; // offset 0x34
    FMOD_DSP_SETPOSITIONCALLBACK setposition; // offset 0x38
    int numparameters; // offset 0x3C
    FMOD_DSP_PARAMETERDESC * paramdesc; // offset 0x40
    FMOD_DSP_SETPARAMCALLBACK setparameter; // offset 0x44
    FMOD_DSP_GETPARAMCALLBACK getparameter; // offset 0x48
    FMOD_DSP_DIALOGCALLBACK config; // offset 0x4C
    int configwidth; // offset 0x50
    int configheight; // offset 0x54
    void * userdata; // offset 0x58
};

typedef FMOD_DSP_DESCRIPTION FMOD_DSP_DESCRIPTION;
struct FMOD_DSP_STATE
{
    FMOD_DSP * instance; // offset 0x0
    void * plugindata; // offset 0x4
};

#endif
