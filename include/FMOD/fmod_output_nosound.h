// Synthesized: no DWARF declarations exist for this header in the 4.06 data; contents are prototypes/classes rebuilt from the definitions in the matching 4.06 PS3 object.

#ifndef _FMOD_OUTPUT_NOSOUND_H
#define _FMOD_OUTPUT_NOSOUND_H

#include "fmod.h"
#include "fmod_outputi.h"

struct FMOD_OUTPUT_STATE;
namespace FMOD {
    struct FMOD_OUTPUT_DESCRIPTION_EX;
}

namespace FMOD {

// Synthesized from the definitions in fmod_output_nosound.cpp: the 4.06 DWARF has no type entry,
// so the base class is guessed and members are unknown.
struct OutputNoSound : public Output
{
    static FMOD_OUTPUT_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT getNumDrivers(int * numdrivers);
    FMOD_RESULT getDriverName(int driver, char * name, int namelen);
    FMOD_RESULT getDriverCaps(int id, FMOD_CAPS * caps);
    FMOD_RESULT init(int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata);
    FMOD_RESULT close();
    FMOD_RESULT getPosition(unsigned int * pcm);
    FMOD_RESULT lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
    static FMOD_RESULT getNumDriversCallback(FMOD_OUTPUT_STATE * output, int * numdrivers);
    static FMOD_RESULT getDriverNameCallback(FMOD_OUTPUT_STATE * output, int id, char * name, int namelen);
    static FMOD_RESULT getDriverCapsCallback(FMOD_OUTPUT_STATE * output, int id, FMOD_CAPS * caps);
    static FMOD_RESULT initCallback(FMOD_OUTPUT_STATE * output, int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata);
    static FMOD_RESULT closeCallback(FMOD_OUTPUT_STATE * output);
    static FMOD_RESULT getPositionCallback(FMOD_OUTPUT_STATE * output, unsigned int * pcm);
    static FMOD_RESULT lockCallback(FMOD_OUTPUT_STATE * output, unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
};

} // namespace FMOD

#endif
