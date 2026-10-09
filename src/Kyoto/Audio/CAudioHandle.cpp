#include "Kyoto/Audio/CAudioHandle.hpp"

// NonMatching reference implementation; Corruption uses a different slot/generation encoding.
uint CAudioHandle::mRefCount = 0;

CAudioHandle::CAudioHandle(uint value) : mID((++mRefCount << 14) | (value & 0xfff)) {}

CAudioHandle::CAudioHandle() : mID(0) {}
