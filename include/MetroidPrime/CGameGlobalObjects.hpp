#ifndef _CGAMEGLOBALOBJECTS
#define _CGAMEGLOBALOBJECTS

#include "types.h"

#include "Kyoto/TOneStatic.hpp"

class COsContext;
class CMemorySys;

// The prototype keeps CGameGlobalObjects in main.cpp: constructor 0x8000C420, PostInitialize
// 0x8000C358, string-table load 0x8000C218 and the (deleting) destructor 0x80008AF8. It lives in
// TOneStatic storage of 0xA418 bytes. The resource factory sits at 0x8 and the simple pool at
// 0xE0 (gpResourceFactory/gpSimplePool), and 0x130 holds the CGameDebug published at
// 0x8079710C. The members are not modelled yet.
class CGameGlobalObjects : public TOneStatic< CGameGlobalObjects > {
public:
  CGameGlobalObjects(COsContext& context, CMemorySys& memorySys);
  ~CGameGlobalObjects();

  void PostInitialize(COsContext& context, CMemorySys& memorySys);

private:
  uchar x0_[0xa418];
};
CHECK_SIZEOF(CGameGlobalObjects, 0xa418)

#endif // _CGAMEGLOBALOBJECTS
