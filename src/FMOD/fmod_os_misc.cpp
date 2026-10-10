// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80622C90..0x806236F0 (23 native functions).
// Source identity: asserted original basename. Extent confidence: high.
// Complete native inventory retained, including callbacks and emitted helpers.
// Function bodies are empty placeholders from the 4.06 reference inventory.
// 0x80622C90 +0x30: OS heap aligned allocator
// 0x80622CC0 +0x94: OS heap resize/copy/free adapter
// 0x80622D54 +0x28: OS heap free adapter
// 0x80622D7C +0x100: retained native; no unsupported symbol identity assigned
// 0x80622E7C +0x58: retained native; no unsupported symbol identity assigned
// 0x80622ED4 +0x12C: retained native; no unsupported symbol identity assigned
// 0x80623000 +0x180: retained native; no unsupported symbol identity assigned
// 0x80623180 +0x24: retained native; no unsupported symbol identity assigned
// 0x806231A4 +0x60: retained native; no unsupported symbol identity assigned
// 0x80623204 +0x24: retained native; no unsupported symbol identity assigned
// 0x80623228 +0x88: retained native; no unsupported symbol identity assigned
// 0x806232B0 +0x34: retained native; no unsupported symbol identity assigned
// 0x806232E4 +0x13C: retained native; no unsupported symbol identity assigned
// 0x80623420 +0x48: retained native; no unsupported symbol identity assigned
// 0x80623468 +0x9C: retained native; no unsupported symbol identity assigned
// 0x80623504 +0x60: retained native; no unsupported symbol identity assigned
// 0x80623564 +0x34: retained native; no unsupported symbol identity assigned
// 0x80623598 +0x34: retained native; no unsupported symbol identity assigned
// 0x806235CC +0x6C: retained native; no unsupported symbol identity assigned
// 0x80623638 +0x48: retained native; no unsupported symbol identity assigned
// 0x80623680 +0x34: retained native; no unsupported symbol identity assigned
// 0x806236B4 +0x34: retained native; no unsupported symbol identity assigned
// 0x806236E8 +0x8: retained native; no unsupported symbol identity assigned

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_os_misc.h"
#include "fmod.h"

void * FMOD_OS_Memory_Alloc(int size)
{
}

void * FMOD_OS_Memory_Realloc(void * ptr, int size)
{
}

void FMOD_OS_Memory_Free(void * ptr)
{
}

FMOD_RESULT FMOD_OS_File_Open(const char * name, char * mode, int unicode, unsigned int * filesize, void * * handle)
{
}

FMOD_RESULT FMOD_OS_File_Close(void * handle)
{
}

FMOD_RESULT FMOD_OS_File_Read(void * handle, void * buf, unsigned int count, unsigned int * read)
{
}

FMOD_RESULT FMOD_OS_File_Seek(void * handle, unsigned int offset)
{
}

FMOD_RESULT FMOD_OS_Debug_OutputStr(const char * s)
{
}

FMOD_RESULT FMOD_OS_Time_GetNs(unsigned int * ns)
{
}

FMOD_RESULT FMOD_OS_Time_Sleep(unsigned int ms)
{
}

FMOD_RESULT FMOD_OS_Thread_GetCurrentID(unsigned int * id)
{
}

FMOD_RESULT FMOD_OS_Thread_Create(const char * name, void (* func)(void *), void * param, FMOD_THREAD_PRIORITY priority, void * stack, int stacksize, void * * handle)
{
}

FMOD_RESULT FMOD_OS_Thread_Destroy(void * handle)
{
}

FMOD_RESULT FMOD_OS_CriticalSection_Create(FMOD_OS_CRITICALSECTION * * crit, bool memorycrit)
{
}

FMOD_RESULT FMOD_OS_CriticalSection_Free(FMOD_OS_CRITICALSECTION * crit)
{
}

FMOD_RESULT FMOD_OS_CriticalSection_Enter(FMOD_OS_CRITICALSECTION * crit)
{
}

FMOD_RESULT FMOD_OS_CriticalSection_Leave(FMOD_OS_CRITICALSECTION * crit)
{
}

FMOD_RESULT FMOD_OS_Semaphore_Create(FMOD_OS_SEMAPHORE * * sema)
{
}

FMOD_RESULT FMOD_OS_Semaphore_Free(FMOD_OS_SEMAPHORE * sema)
{
}

FMOD_RESULT FMOD_OS_Semaphore_Wait(FMOD_OS_SEMAPHORE * sema)
{
}

FMOD_RESULT FMOD_OS_Semaphore_Signal(FMOD_OS_SEMAPHORE * sema, bool interrupt)
{
}

FMOD_RESULT FMOD_OS_Library_Load(const char * dllname, FMOD_OS_LIBRARY * * handle)
{
}

FMOD_RESULT FMOD_OS_Library_GetProcAddress(FMOD_OS_LIBRARY * handle, const char * procname, void * * address)
{
}

FMOD_RESULT FMOD_OS_Library_Free(FMOD_OS_LIBRARY * handle)
{
}
