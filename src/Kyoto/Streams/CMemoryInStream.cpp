#include "Kyoto/Streams/CMemoryInStream.hpp"

CMemoryInStream::CMemoryInStream(const void* data, unsigned long length, EOwnerShip ownership)
: CInputStream(data, length, ownership == kOS_Owned) {}
