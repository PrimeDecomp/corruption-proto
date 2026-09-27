#ifndef KYOTO_STREAMS_CINPUTSTREAM_HPP
#define KYOTO_STREAMS_CINPUTSTREAM_HPP

class CInputStream {
public:
  CInputStream(const void* data, unsigned long length, bool owned);
  virtual ~CInputStream();

  char ReadInt8() { return *mPosition++; }

private:
  unsigned char* mBuffer;
  unsigned char* mPosition;
  unsigned long mLength;
  bool mOwned;
};

#endif
