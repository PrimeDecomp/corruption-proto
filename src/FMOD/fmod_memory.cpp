/*
 * G2MEAB FMOD/fmod_memory.cpp translation-unit scaffold (NonMatching).
 * .text: 0x8060B560..0x8060C2B4 (16 native functions).
 * Allocation/release paths name the file; complete helper boundaries are inferred.
 * Native implementations, declarations and data ownership remain unreconstructed.
 */

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_memory.h"
#include "fmod.h"

namespace FMOD {

void * Memory_DefaultMalloc(unsigned int size, FMOD_MEMORY_TYPE type)
{
}

void * Memory_DefaultRealloc(void * data, unsigned int size, FMOD_MEMORY_TYPE type)
{
}

void Memory_DefaultFree(void * ptr, FMOD_MEMORY_TYPE type)
{
}

MemPool::MemPool()
{
}

void MemPool::set(int blockoffset, int value, int numblocks)
{
}

int MemPool::getSize(void * ptr, const char * file, int line)
{
}

unsigned int MemPool::getAvailable()
{
}

void MemPool::removeCounter(unsigned int * ptr, const char * file, const int line)
{
}

void LocalMemoryCounter::remove()
{
}

LocalMemoryCounter::~LocalMemoryCounter()
{
}

void MemPool::addCounter(unsigned int * ptr, const char * file, const int line)
{
}

LocalMemoryCounter::LocalMemoryCounter()
{
}

void MemPool::free(void * ptr, const char * file, const int line, FMOD_MEMORY_TYPE type)
{
}

void MemPool::close()
{
}

FMOD_RESULT MemPool::init(void * poolmem, int poolsize, int blocksize)
{
}

void MemSingleton::free(const char * file, const int line)
{
}

} // namespace FMOD

void FMOD_Memory_freeC(void * ptr, const char * file, const int line)
{
}

namespace FMOD {

void * MemPool::alloc(int len, const char * file, const int line, FMOD_MEMORY_TYPE type)
{
}

void * MemPool::realloc(void * ptr, int len, const char * file, const int line, FMOD_MEMORY_TYPE type)
{
}

} // namespace FMOD

void * FMOD_Memory_reallocC(void * ptr, int len, const char * file, const int line)
{
}

namespace FMOD {

void * MemPool::calloc(int len, const char * file, const int line, FMOD_MEMORY_TYPE type)
{
}

} // namespace FMOD

void * FMOD_Memory_callocC(int len, const char * file, const int line)
{
}

namespace FMOD {

FMOD_RESULT MemPool::initCustom(void * poolmem, int poolsize, int blocksize)
{
}

void * MemSingleton::alloc(int len, const char * file, const int line)
{
}

} // namespace FMOD

void * FMOD_Memory_allocC(int len, const char * file, const int line)
{
}

namespace FMOD {

void MemPool::pauseCounter(unsigned int * ptr, bool pause, const char * file, const int line)
{
}

} // namespace FMOD
