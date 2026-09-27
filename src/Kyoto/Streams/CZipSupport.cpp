#include "Kyoto/Streams/CZipSupport.hpp"
#include "Kyoto/Streams/ZipTypes.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Alloc/Assert.hpp"

void CZipSupport::Decompress(const void* source, unsigned int sourceLength, void* output,
                             unsigned int outputLength) {
  z_stream stream;
  stream.zalloc = Alloc;
  stream.zfree = Free;
  stream.opaque = 0;
  stream.next_in = static_cast< unsigned char* >(const_cast< void* >(source));
  stream.avail_in = sourceLength;

  int result = inflateInit_(&stream, "1.1.3", sizeof(z_stream));
  stream.next_out = static_cast< unsigned char* >(output);
  stream.avail_out = outputLength;
  if (result != 0) {
    CCallStack stack(0, "CZipSupport.cpp(67) : ", "UnknownType");
    rs_log_assert_failure(&stack, "CZipSupport.cpp", 67, "Verify", "err == Z_OK",
                          "Error in inflateInit");
    rs_debugger_printf("Would have thrown exception: %s\n", "false");
    fn_80491108();
  }

  while (result != 1) {
    result = inflate(&stream, 0);
    if (result != 0 && result != 1) {
      CCallStack stack(0, "CZipSupport.cpp(76) : ", "UnknownType");
      rs_log_assert_failure(&stack, "CZipSupport.cpp", 76, "Verify",
                            "err == Z_OK || err == Z_STREAM_END",
                            "CInputStream::kException_StreamError");
      rs_debugger_printf("Would have thrown exception: %s\n", "false");
      fn_80491108();
    }
  }
  inflateEnd(&stream);
}

void CZipSupport::Free(void*, void* memory) {
  if (memory != 0) {
    delete[] static_cast< unsigned char* >(memory);
  }
}

void* CZipSupport::Alloc(void*, unsigned int count, unsigned int size) {
  return new ("CZipSupport.cpp(19) : ", (const char*)0) unsigned char[count * size];
}
