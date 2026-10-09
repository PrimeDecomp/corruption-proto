#include "Kyoto/Streams/COutputStream.hpp"
#include "Kyoto/Alloc/CMemory.hpp"

extern "C" void* memcpy(void* destination, const void* source, unsigned long length);

COutputStream::COutputStream(int bufferLength)
: mUnwrittenLength(0)
, mBufferLength(bufferLength)
, mBuffer(bufferLength > 64 ? rs_new_line(22) unsigned char[bufferLength]
                            : &mScratch[32 - reinterpret_cast< unsigned long >(mScratch) % 31])
, mWrittenBytes(0) {}

COutputStream::~COutputStream() {
  if (mBufferLength > 64) {
    delete[] static_cast< unsigned char* >(mBuffer);
  }
}

void COutputStream::DoPut(const void* data, unsigned long length) {
  unsigned int offset = 0;
  const unsigned char* end = 0;
  unsigned int remaining = length;
  if (remaining != 0) {
    mWrittenBytes += remaining;
    if (remaining + mUnwrittenLength <= mBufferLength) {
      memcpy(static_cast< unsigned char* >(mBuffer) + mUnwrittenLength, data, remaining);
      mUnwrittenLength += remaining;
      return;
    }

    end = static_cast< const unsigned char* >(data) + remaining;
    while (remaining != 0) {
      const unsigned int count = mBufferLength - mUnwrittenLength;
      offset = count;
      if (remaining < count) {
        offset = remaining;
      }
      if (offset != 0) {
        memcpy(static_cast< unsigned char* >(mBuffer) + mUnwrittenLength, end - remaining, offset);
        remaining -= offset;
        mUnwrittenLength += offset;
      } else {
        DoFlush();
      }
    }
  }
}

void COutputStream::DoFlush() {
  if (mUnwrittenLength != 0) {
    Write(mBuffer, mUnwrittenLength);
    mUnwrittenLength = 0;
  }
}
