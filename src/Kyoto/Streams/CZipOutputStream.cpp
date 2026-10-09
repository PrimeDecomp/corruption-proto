#include "Kyoto/Streams/CZipOutputStream.hpp"
#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Streams/CZipSupport.hpp"

CZipOutputStream::CZipOutputStream(COutputStream* output, int level)
: COutputStream(1024)
, mOutput(output)
, mCompressedBytesWritten(0)
, mStream(rs_new(14) z_stream)
, mFinished(false) {
  mStream->zalloc = CZipSupport::Alloc;
  mStream->zfree = CZipSupport::Free;
  mStream->opaque = 0;

  int useLevel = 9;
  if (level <= 9) {
    useLevel = level;
  }
  useLevel = useLevel < 0 ? 0 : useLevel;
  deflateInit_(mStream.get(), useLevel, "1.1.3", sizeof(z_stream));
}

bool CZipOutputStream::Process(bool finish) {
  unsigned char output[1024];
  mStream->avail_out = sizeof(output);
  mStream->next_out = output;
  const int result = deflate(mStream.get(), finish ? 4 : 0);
  if (result != 0 && result != 1) {
    RS_VERIFY_THROW(40, false, kException_OutputError, "kException_OutputError");
  }
  const unsigned int remaining = mStream->avail_out;
  if (sizeof(output) - remaining != 0) {
    mOutput->DoPut(output, sizeof(output) - mStream->avail_out);
    mCompressedBytesWritten += sizeof(output) - mStream->avail_out;
  }
  if (result == 1) {
    return true;
  }
  return false;
}

CZipOutputStream::~CZipOutputStream() {
  Finish();
  deflateEnd(mStream.get());
}

void CZipOutputStream::Finish() {
  DoFlush();
  mStream->next_in = 0;
  mStream->avail_in = 0;
  while (!Process(true)) {
  }
}

void CZipOutputStream::Write(const void* data, unsigned long length) {
  mStream->next_in = static_cast< unsigned char* >(const_cast< void* >(data));
  mStream->avail_in = length;
  while (mStream->avail_in != 0) {
    Process(false);
  }
}

int CZipOutputStream::GetCompressedBytesWritten() {
  if (!mFinished) {
    Finish();
    mFinished = true;
  }
  return mCompressedBytesWritten;
}
