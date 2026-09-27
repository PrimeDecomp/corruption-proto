#ifndef KYOTO_STREAMS_ZIPTYPES_HPP
#define KYOTO_STREAMS_ZIPTYPES_HPP

struct z_stream {
  unsigned char* next_in;
  unsigned int avail_in;
  unsigned long total_in;
  unsigned char* next_out;
  unsigned int avail_out;
  unsigned long total_out;
  char* msg;
  void* state;
  void* (*zalloc)(void*, unsigned int, unsigned int);
  void (*zfree)(void*, void*);
  void* opaque;
  int data_type;
  unsigned long adler;
  unsigned long reserved;
};

extern "C" int deflateInit_(z_stream* stream, int level, const char* version, int streamSize);
extern "C" int deflate(z_stream* stream, int flush);
extern "C" int deflateEnd(z_stream* stream);
extern "C" int inflateInit_(z_stream* stream, const char* version, int streamSize);
extern "C" int inflate(z_stream* stream, int flush);
extern "C" int inflateEnd(z_stream* stream);

#endif
