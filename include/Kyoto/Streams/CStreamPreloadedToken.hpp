#ifndef _CSTREAMPRELOADEDTOKEN
#define _CSTREAMPRELOADEDTOKEN

#include "types.h"
#include "rstl/string.hpp"

class CStreamPreloadedData;

// Shared file handle named by Corruption's CStreamPreloadedToken diagnostics.
class CStreamPreloadedToken {
public:
  CStreamPreloadedToken(const rstl::string& path);
  CStreamPreloadedToken(const CStreamPreloadedToken& other);
  ~CStreamPreloadedToken();

  void operator=(const CStreamPreloadedToken& other);
  bool IsReady() const;
  void Read(void* dest, int offset, int length) const;

private:
  CStreamPreloadedData* mData;
};
CHECK_SIZEOF(CStreamPreloadedToken, 0x4)

#endif // _CSTREAMPRELOADEDTOKEN
