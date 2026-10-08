#include "Kyoto/Streams/CZipSupport.hpp"
#include "Kyoto/Streams/ZipTypes.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Alloc/Assert.hpp"

void* CZipSupport::Alloc(void*, unsigned int count, unsigned int size) {
  return new ("CZipSupport.cpp(19) : ", (const char*)0) unsigned char[count * size];
}

void CZipSupport::Free(void*, void* memory) {
  if (memory != 0) {
    delete[] static_cast< unsigned char* >(memory);
  }
}

bool CZipSupport::Decompress(const void* source, unsigned int sourceLength, void* output,
                             unsigned int outputLength) {
  z_stream stream;
  stream.zalloc = Alloc;
  stream.zfree = Free;
  stream.opaque = 0;
  stream.next_in = static_cast< unsigned char* >(const_cast< void* >(source));
  stream.avail_in = sourceLength;

  int err = inflateInit_(&stream, "1.1.3", sizeof(z_stream));
  RS_VERIFY_THROW(67, err == Z_OK, false, "Error in inflateInit");

  stream.next_out = static_cast< unsigned char* >(output);
  stream.avail_out = outputLength;

  while (err != Z_STREAM_END) {
    err = inflate(&stream, 0);
    RS_VERIFY_THROW(76, err == Z_OK || err == Z_STREAM_END, false,
                    "CInputStream::kException_StreamError");
  }
  inflateEnd(&stream);
  return true;
}
