// G2MEAB prototype NonMatching translation-unit scaffold; function bodies are empty placeholders.
// .text: 0x80621208..0x80621568 (5 native functions).
// Split out of the former fmod_systemi scaffold; original basename from the 4.06 reference library object.
// 0x806212B4 +0x8: retained native; no unsupported symbol identity assigned
// 0x806212BC +0x34: thread object constructor; externally reused, retain emitted definition
// 0x806212F0 +0x134: retained native; no unsupported symbol identity assigned
// 0x80621424 +0x110: retained native; no unsupported symbol identity assigned
// 0x80621534 +0x34: retained native; no unsupported symbol identity assigned

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_thread.h"
#include "fmod.h"

namespace FMOD {

void Thread::callback(void * data)
{
}

FMOD_RESULT Thread::threadFunc()
{
}

Thread::Thread()
{
}

FMOD_RESULT Thread::initThread(const char * name, void (* func)(void *), void * userdata, PRIORITY priority, void * stack, int stacksize, bool usesemaphore, int sleepperiod)
{
}

FMOD_RESULT Thread::closeThread()
{
}

FMOD_RESULT Thread::getCurrentThreadID(unsigned int * id)
{
}

FMOD_RESULT Thread::wakeupThread(bool frominterrupt)
{
}

} // namespace FMOD
