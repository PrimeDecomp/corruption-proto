#ifndef KYOTO_STREAMS_CZIPOUTPUTSTREAM_HPP
#define KYOTO_STREAMS_CZIPOUTPUTSTREAM_HPP

#include "Kyoto/Streams/COutputStream.hpp"
#include "Kyoto/Streams/ZipTypes.hpp"
#include "rstl/auto_ptr.hpp"

class CZipOutputStream : public COutputStream {
public:
  CZipOutputStream(COutputStream* output, int level);
  virtual ~CZipOutputStream();
  virtual void Write(const void* data, unsigned long length);

  void Finish();
  bool Process(bool finish);
  int GetCompressedBytesWritten();

private:
  COutputStream* mOutput;
  int mCompressedBytesWritten;
  rstl::auto_ptr<z_stream> mStream;
  bool mFinished;
};

#endif
