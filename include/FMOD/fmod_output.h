// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_OUTPUT_H
#define _FMOD_OUTPUT_H

#include "fmod.h"

struct FMOD_OUTPUT_DESCRIPTION;
struct FMOD_OUTPUT_STATE;

typedef FMOD_OUTPUT_STATE FMOD_OUTPUT_STATE;
typedef FMOD_RESULT (* FMOD_OUTPUT_GETNUMDRIVERSCALLBACK)(FMOD_OUTPUT_STATE *, int *);
typedef FMOD_RESULT (* FMOD_OUTPUT_GETDRIVERNAMECALLBACK)(FMOD_OUTPUT_STATE *, int, char *, int);
typedef FMOD_RESULT (* FMOD_OUTPUT_GETDRIVERCAPSCALLBACK)(FMOD_OUTPUT_STATE *, int, FMOD_CAPS *);
typedef FMOD_RESULT (* FMOD_OUTPUT_INITCALLBACK)(FMOD_OUTPUT_STATE *, int, FMOD_INITFLAGS, int *, int, FMOD_SOUND_FORMAT *, int, int, void *);
typedef FMOD_RESULT (* FMOD_OUTPUT_CLOSECALLBACK)(FMOD_OUTPUT_STATE *);
typedef FMOD_RESULT (* FMOD_OUTPUT_UPDATECALLBACK)(FMOD_OUTPUT_STATE *);
typedef FMOD_RESULT (* FMOD_OUTPUT_GETHANDLECALLBACK)(FMOD_OUTPUT_STATE *, void * *);
typedef FMOD_RESULT (* FMOD_OUTPUT_GETPOSITIONCALLBACK)(FMOD_OUTPUT_STATE *, unsigned int *);
typedef FMOD_RESULT (* FMOD_OUTPUT_LOCKCALLBACK)(FMOD_OUTPUT_STATE *, unsigned int, unsigned int, void * *, void * *, unsigned int *, unsigned int *);
typedef FMOD_RESULT (* FMOD_OUTPUT_UNLOCKCALLBACK)(FMOD_OUTPUT_STATE *, void *, void *, unsigned int, unsigned int);
typedef FMOD_RESULT (* FMOD_OUTPUT_READFROMMIXER)(FMOD_OUTPUT_STATE *, void *, unsigned int);
struct FMOD_OUTPUT_DESCRIPTION
{
    const char * name; // offset 0x0
    unsigned int version; // offset 0x4
    int polling; // offset 0x8
    FMOD_OUTPUT_GETNUMDRIVERSCALLBACK getnumdrivers; // offset 0xC
    FMOD_OUTPUT_GETDRIVERNAMECALLBACK getdrivername; // offset 0x10
    FMOD_OUTPUT_GETDRIVERCAPSCALLBACK getdrivercaps; // offset 0x14
    FMOD_OUTPUT_INITCALLBACK init; // offset 0x18
    FMOD_OUTPUT_CLOSECALLBACK close; // offset 0x1C
    FMOD_OUTPUT_UPDATECALLBACK update; // offset 0x20
    FMOD_OUTPUT_GETHANDLECALLBACK gethandle; // offset 0x24
    FMOD_OUTPUT_GETPOSITIONCALLBACK getposition; // offset 0x28
    FMOD_OUTPUT_LOCKCALLBACK lock; // offset 0x2C
    FMOD_OUTPUT_UNLOCKCALLBACK unlock; // offset 0x30
};

typedef FMOD_OUTPUT_DESCRIPTION FMOD_OUTPUT_DESCRIPTION;
struct FMOD_OUTPUT_STATE
{
    void * plugindata; // offset 0x0
    FMOD_OUTPUT_READFROMMIXER readfrommixer; // offset 0x4
};

#endif
