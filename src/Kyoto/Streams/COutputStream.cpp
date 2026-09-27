#include "Kyoto/Streams/COutputStream.hpp"
#include "Kyoto/Alloc/CMemory.hpp"

extern "C" void* memcpy(void* destination, const void* source, unsigned long length);

void COutputStream::DoFlush() {
  if (mUnwrittenLength != 0) {
    Write(mBuffer, mUnwrittenLength);
    mUnwrittenLength = 0;
  }
}

void COutputStream::DoPut(const void* data, unsigned long length) {
  unsigned int remaining = length;
  if (remaining == 0) {
    return;
  }

  mWrittenBytes += remaining;
  if (remaining + mUnwrittenLength <= mBufferLength) {
    memcpy(static_cast< unsigned char* >(mBuffer) + mUnwrittenLength, data, remaining);
    mUnwrittenLength += remaining;
    return;
  }

  const unsigned char* const end = static_cast< const unsigned char* >(data) + remaining;
  while (remaining != 0) {
    unsigned int count = mBufferLength - mUnwrittenLength;
    if (remaining < count) {
      count = remaining;
    }
    if (count != 0) {
      memcpy(static_cast< unsigned char* >(mBuffer) + mUnwrittenLength, end - remaining, count);
      remaining -= count;
      mUnwrittenLength += count;
    } else {
      DoFlush();
    }
  }
}

COutputStream::~COutputStream() {
  if (mBufferLength > 64) {
    delete[] static_cast< unsigned char* >(mBuffer);
  }
}

COutputStream::COutputStream(int bufferLength)
: mUnwrittenLength(0)
, mBufferLength(bufferLength)
, mBuffer(bufferLength > 64
              ? new ("COutputStream.cpp(22) : ", (const char*)0) unsigned char[bufferLength]
              : &mScratch[32 - reinterpret_cast< unsigned long >(mScratch) % 31])
, mWrittenBytes(0) {}
