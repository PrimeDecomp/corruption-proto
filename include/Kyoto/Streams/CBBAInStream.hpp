#ifndef _CBBAINSTREAM
#define _CBBAINSTREAM

#include "types.h"

#include "Kyoto/Streams/CInputStream.hpp"

// An input stream over a whole file read from the development host through the broadband adapter
// (CBBAInStream.cpp). The constructor (0x80544A04) loads the file into an owned buffer.
class CBBAInStream : public CInputStream {
public:
  explicit CBBAInStream(const char* path);
  ~CBBAInStream();

  // Guessed name. 0x8054498C: the bytes of the file not read yet.
  uint GetRemainingBytes() const;
};

#endif // _CBBAINSTREAM
