#include "Kyoto/Streams/CLZOInputStream.hpp"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Streams/CLZOSupport.hpp"

static const CInputStream::SBufferAndSize
get_buffer_and_size(CInputStream& in, unsigned long compressedLen, unsigned long decompressedLen) {
  rstl::auto_ptr< uchar > buffer(static_cast< uchar* >(
      CMemory::Alloc(decompressedLen, IAllocator::kHI_RoundUpLen, IAllocator::kSC_Unk1,
                     IAllocator::kTP_Heap,
                     CCallStack(-1, "DolphinCLZOInputStream.cpp(13) : ", kUnknownType))));
  unsigned long written = 0;
  uchar* dest = buffer.get();
  while (written != decompressedLen) {
    uint blockLen = 0x4000;
    uint compressedBlockLen = in.ReadUint16();
    const uchar* source = static_cast< const uchar* >(in.Get(compressedBlockLen));
    int decompressResult = CLZOSupport::Inflate(source, compressedBlockLen, dest, blockLen);
    if (decompressResult < 0) {
      CCallStack stack(0, "DolphinCLZOInputStream.cpp(28) : ", kUnknownType);
      rs_log_assert_failure(&stack, "DolphinCLZOInputStream.cpp", 28, "Verify",
                            "decompressResult >= 0",
                            "CLZOSupport::Inflate failed in CInputStream::SBufferAndSize const "
                            "get_buffer_and_size.");
      rs_debugger_printf("Would have thrown exception: %s\n", "false");
      RAssert_TriggerIllegalInstruction();
    }
    dest += blockLen;
    written += blockLen;
  }
  return CInputStream::SBufferAndSize(buffer.release(), decompressedLen);
}

CLZOInputStream::CLZOInputStream(const rstl::auto_ptr< CInputStream >& in,
                                 unsigned long compressedLen, unsigned long decompressedLen)
: CInputStream(get_buffer_and_size(*in, compressedLen, decompressedLen), true) {
  // Consume ownership after decompression without retaining the source stream.
  rstl::auto_ptr< CInputStream > stream(in);
}

CLZOInputStream::~CLZOInputStream() {}
