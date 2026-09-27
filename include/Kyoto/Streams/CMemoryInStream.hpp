#ifndef KYOTO_STREAMS_CMEMORYINSTREAM_HPP
#define KYOTO_STREAMS_CMEMORYINSTREAM_HPP

#include "Kyoto/Streams/CInputStream.hpp"

class CMemoryInStream : public CInputStream {
public:
  enum EOwnerShip { kOS_Owned, kOS_NotOwned };

  CMemoryInStream(const void* data, unsigned long length, EOwnerShip ownership);
  virtual ~CMemoryInStream();
};

#endif
