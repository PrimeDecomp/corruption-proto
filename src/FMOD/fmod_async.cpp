// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805B60A0..0x805B67C0 (12 retained native functions).
// Original basename directly named by target allocation/free evidence.
// Evidence: Allocation805B65F8 names fmod_async.cpp line415 and frees805B6338 with the same file
// line236. Entry805B60A0 forwards to worker805B6468; ctor805B60C0 establishes thread, nested
// intrusive list, lock and flags in0x158 bytes;805B6258 starts FMOD_NONBLOCKING thread. Shared
// global list807176E4 and mutex8079B7E0 close shutdown/reap/assign paths; final805B6764 registers
// list destructor805B6184. Next805B67C0 changes to channel-handle validation and forwarding
// wrappers. Source basename is direct target allocation evidence, not the version/thread string
// alone. Preserve every retained stub, emitted helper and adjustor thunk; full inventory and
// inlining uncertainty are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_async.h"
#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_os_misc.h"
#include "fmod_soundi.h"

namespace FMOD {

FMOD_OS_CRITICALSECTION * AsyncThread::gAsyncCrit;
LinkedListNode AsyncThread::gAsyncHead;

AsyncThread::AsyncThread()
{
}

FMOD_RESULT AsyncThread::removeCallback(FMOD_ASYNC_CALLBACK callback)
{
}

FMOD_RESULT AsyncThread::init(bool owned)
{
}

FMOD_RESULT AsyncThread::getAsyncThread(SoundI * sound)
{
}

FMOD_RESULT AsyncThread::addCallback(FMOD_ASYNC_CALLBACK callback, AsyncThread * * asyncthread)
{
}

FMOD_RESULT AsyncThread::release()
{
}

FMOD_RESULT AsyncThread::reallyRelease()
{
}

FMOD_RESULT AsyncThread::shutDown()
{
}

FMOD_RESULT AsyncThread::wakeupThread()
{
}

FMOD_RESULT AsyncThread::update()
{
}

FMOD_RESULT AsyncThread::threadFunc()
{
}

void asyncThreadFunc(void * data)
{
}

} // namespace FMOD
