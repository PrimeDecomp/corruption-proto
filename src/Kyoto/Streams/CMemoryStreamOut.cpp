#include "Kyoto/Streams/CMemoryStreamOut.hpp"
#include "Kyoto/Alloc/CMemory.hpp"

extern "C" void* memcpy(void* destination, const void* source, unsigned long length);

CMemoryStreamOut::CMemoryStreamOut(void* buffer, unsigned long length, EOwnerShip ownership,
                                   int blockLength)
: COutputStream(blockLength)
, mOutput(buffer)
, mOutputLength(length)
, mPosition(0)
, mOwnsOutput(ownership == kOS_Owned) {}

CMemoryStreamOut::~CMemoryStreamOut() {
  Flush();
  if (mOwnsOutput) {
    delete[] static_cast< unsigned char* >(mOutput);
  }
}

void CMemoryStreamOut::Write(const void* data, unsigned long length) {
  const unsigned long available = mOutputLength - mPosition;
  length = available < length ? available : length;
  if (length != 0) {
    memcpy(static_cast< unsigned char* >(mOutput) + mPosition, data, length);
    mPosition += length;
  }
}
