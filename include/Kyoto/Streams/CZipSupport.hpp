#ifndef KYOTO_STREAMS_CZIPSUPPORT_HPP
#define KYOTO_STREAMS_CZIPSUPPORT_HPP

class CZipSupport {
public:
  static bool Decompress(const void* source, unsigned int sourceLength, void* output,
                         unsigned int outputLength);
  static void* Alloc(void* context, unsigned int count, unsigned int size);
  static void Free(void* context, void* memory);
};

#endif
