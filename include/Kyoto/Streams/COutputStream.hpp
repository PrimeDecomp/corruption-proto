#ifndef KYOTO_STREAMS_COUTPUTSTREAM_HPP
#define KYOTO_STREAMS_COUTPUTSTREAM_HPP

class COutputStream {
public:
  explicit COutputStream(int bufferLength);
  virtual ~COutputStream();
  virtual void Write(const void* data, unsigned long length) = 0;

  void DoPut(const void* data, unsigned long length);
  void DoFlush();
  void Flush() { DoFlush(); }
  void WriteChar(unsigned char value) {
    if (mUnwrittenLength >= mBufferLength) {
      DoFlush();
    }
    ++mWrittenBytes;
    *(static_cast< unsigned char* >(mBuffer) + mUnwrittenLength++) = value;
  }

private:
  unsigned int mUnwrittenLength;
  unsigned int mBufferLength;
  void* mBuffer;
  unsigned int mWrittenBytes;
  unsigned char mScratch[96];
};

#endif
