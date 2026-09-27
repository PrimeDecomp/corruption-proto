#include "Kyoto/Streams/CZipOutputStream.hpp"
#include "Kyoto/Streams/CZipSupport.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Alloc/Assert.hpp"

int CZipOutputStream::GetCompressedBytesWritten() {
  if (!mFinished) {
    Finish();
    mFinished = true;
  }
  return mCompressedBytesWritten;
}

void CZipOutputStream::Write(const void* data, unsigned long length) {
  mStream->next_in = static_cast< unsigned char* >(const_cast< void* >(data));
  mStream->avail_in = length;
  while (mStream->avail_in != 0) {
    Process(false);
  }
}

void CZipOutputStream::Finish() {
  DoFlush();
  mStream->next_in = 0;
  mStream->avail_in = 0;
  while (!Process(true)) {}
}

CZipOutputStream::~CZipOutputStream() {
  Finish();
  deflateEnd(mStream);
  if (mOwnsStream) {
    delete mStream;
  }
}

bool CZipOutputStream::Process(bool finish) {
  unsigned char output[1024];
  mStream->avail_out = sizeof(output);
  mStream->next_out = output;
  const int result = deflate(mStream, finish ? 4 : 0);
  if (result != 0 && result != 1) {
    CCallStack stack(0, "CZipOutputStream.cpp(40) : ", "UnknownType");
    rs_log_assert_failure(&stack, "CZipOutputStream.cpp", 40, "Verify", "false",
                          "kException_OutputError");
    rs_debugger_printf("Would have thrown exception: %s\n", "kException_OutputError");
    fn_80491108();
  }
  if (mStream->avail_out != sizeof(output)) {
    mOutput->DoPut(output, sizeof(output) - mStream->avail_out);
    mCompressedBytesWritten += sizeof(output) - mStream->avail_out;
  }
  return result == 1;
}

CZipOutputStream::CZipOutputStream(COutputStream* output, int level)
: COutputStream(1024)
, mOutput(output)
, mCompressedBytesWritten(0)
, mOwnsStream(false)
, mStream(new ("CZipOutputStream.cpp(14) : ", (const char*)0) z_stream)
, mFinished(false) {
  mOwnsStream = mStream != 0;
  mStream->zalloc = CZipSupport::Alloc;
  mStream->zfree = CZipSupport::Free;
  mStream->opaque = 0;

  int useLevel = 9;
  if (level < 10) {
    useLevel = level;
  }
  if (useLevel < 0) {
    useLevel = 0;
  }
  deflateInit_(mStream, useLevel, "1.1.3", sizeof(z_stream));
}
