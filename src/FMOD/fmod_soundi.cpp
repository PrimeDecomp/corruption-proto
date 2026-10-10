// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x8061606C..0x80619D8C (52 retained native functions).
// direct target filename in allocation/free body.
// Evidence: Leading1606C validates public handle/out pointer (errors20/21), called all four public
// Sound wrappers; SoundI ctor16098 installs806EF4DC, shared list+2C8 and sync-point list+1A8.
// Subsound creation16938, codec read/seek16B5C/17138, PCM skip171D4, clear175A8, full
// defaults/3D/subsound/sentence/name/format/tag/sync-point/mode/loop/userdata APIs share this
// layout. Preserve retained helpers, thunks and inline expansions in observed native order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_soundi.h"
#include "fmod.h"
#include "fmod.hpp"

namespace FMOD {

FMOD_RESULT SoundI::validate(Sound * sound, SoundI * * soundi)
{
}

SoundI::SoundI()
{
}

FMOD_RESULT SoundI::setPositionInternal(unsigned int pcm)
{
}

FMOD_RESULT SoundI::read(unsigned int offset, unsigned int numsamples, unsigned int * read)
{
}

FMOD_RESULT SoundI::loadSubSound(int index, FMOD_MODE mode)
{
}

FMOD_RESULT SoundI::seek(int subsound, unsigned int position)
{
}

FMOD_RESULT SoundI::clear(unsigned int offset, unsigned int numsamples)
{
}

FMOD_RESULT SoundI::release(bool freethis)
{
}

FMOD_RESULT SoundI::getSystemObject(System * * system)
{
}

FMOD_RESULT SoundI::lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
}

FMOD_RESULT SoundI::unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2)
{
}

FMOD_RESULT SoundI::setDefaults(float frequency, float volume, float pan, int priority)
{
}

FMOD_RESULT SoundI::getDefaults(float * frequency, float * volume, float * pan, int * priority)
{
}

FMOD_RESULT SoundI::setVariations(float frequencyvar, float volumevar, float panvar)
{
}

FMOD_RESULT SoundI::getVariations(float * frequencyvar, float * volumevar, float * panvar)
{
}

FMOD_RESULT SoundI::set3DMinMaxDistance(float min, float max)
{
}

FMOD_RESULT SoundI::get3DMinMaxDistance(float * min, float * max)
{
}

FMOD_RESULT SoundI::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
}

FMOD_RESULT SoundI::get3DConeSettings(float * insideconeangle, float * outsideconeangle, float * outsidevolume)
{
}

FMOD_RESULT SoundI::set3DCustomRolloff(FMOD_VECTOR * points, int numpoints)
{
}

FMOD_RESULT SoundI::get3DCustomRolloff(FMOD_VECTOR * * points, int * numpoints)
{
}

FMOD_RESULT SoundI::setSubSound(int index, SoundI * subsound)
{
}

FMOD_RESULT SoundI::getSubSound(int index, SoundI * * subsound)
{
}

FMOD_RESULT SoundI::setSubSoundSentence(int * subsoundlist, int numsubsounds)
{
}

FMOD_RESULT SoundI::getName(char * name, int namelen)
{
}

FMOD_RESULT SoundI::getLength(unsigned int * length, FMOD_TIMEUNIT lengthtype)
{
}

FMOD_RESULT SoundI::getFormat(FMOD_SOUND_TYPE * type, FMOD_SOUND_FORMAT * format, int * channels, int * bits)
{
}

FMOD_RESULT SoundI::getNumSubSounds(int * numsubsounds)
{
}

FMOD_RESULT SoundI::getNumTags(int * numtags, int * numtagsupdated)
{
}

FMOD_RESULT SoundI::getTag(const char * name, int index, FMOD_TAG * tag)
{
}

FMOD_RESULT SoundI::getOpenState(FMOD_OPENSTATE * openstate, unsigned int * percentbuffered, bool * starving)
{
}

FMOD_RESULT SoundI::readData(void * buffer, unsigned int numbytes, unsigned int * read)
{
}

FMOD_RESULT SoundI::seekData(unsigned int position)
{
}

FMOD_RESULT SoundI::getNumSyncPoints(int * numsyncpoints)
{
}

FMOD_RESULT SoundI::getSyncPoint(int index, FMOD_SYNCPOINT * * point)
{
}

FMOD_RESULT SoundI::getSyncPointInfo(FMOD_SYNCPOINT * point, char * name, int namelen, unsigned int * offset, FMOD_TIMEUNIT offsettype)
{
}

FMOD_RESULT SoundI::addSyncPoint(unsigned int offset, FMOD_TIMEUNIT offsettype, const char * name, FMOD_SYNCPOINT * * syncpoint)
{
}

FMOD_RESULT SoundI::deleteSyncPoint(FMOD_SYNCPOINT * point)
{
}

FMOD_RESULT SoundI::setMode(FMOD_MODE mode)
{
}

FMOD_RESULT SoundI::getMode(FMOD_MODE * mode)
{
}

FMOD_RESULT SoundI::setLoopCount(int loopcount)
{
}

FMOD_RESULT SoundI::getLoopCount(int * loopcount)
{
}

FMOD_RESULT SoundI::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
}

FMOD_RESULT SoundI::getLoopPoints(unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype)
{
}

FMOD_RESULT SoundI::setPosition(unsigned int pos)
{
}

FMOD_RESULT SoundI::getPosition(unsigned int * pcm)
{
}

FMOD_RESULT SoundI::setUserData(void * userdata)
{
}

FMOD_RESULT SoundI::getUserData(void * * userdata)
{
}

} // namespace FMOD
