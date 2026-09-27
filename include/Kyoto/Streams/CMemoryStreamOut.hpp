#ifndef KYOTO_STREAMS_CMEMORYSTREAMOUT_HPP
#define KYOTO_STREAMS_CMEMORYSTREAMOUT_HPP

#include "Kyoto/Streams/COutputStream.hpp"

class CMemoryStreamOut : public COutputStream {
public:
  enum EOwnerShip { kOS_Owned, kOS_NotOwned };

  CMemoryStreamOut(void* buffer, unsigned long length, EOwnerShip ownership, int blockLength);
  virtual ~CMemoryStreamOut();
  virtual void Write(const void* data, unsigned long length);

private:
  void* mOutput;
  unsigned long mOutputLength;
  unsigned long mPosition;
  bool mOwnsOutput;
};

#endif
