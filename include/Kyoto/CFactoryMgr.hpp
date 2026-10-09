#ifndef _CFACTORYMGR
#define _CFACTORYMGR

#include "types.h"

#include "Kyoto/IObjectStore.hpp"
#include "Kyoto/Streams/CInputStream.hpp"
#include "Kyoto/TToken.hpp"

#include "rstl/map.hpp"

class CFactoryFnReturn {
public:
  template < typename T >
  CFactoryFnReturn(T* ptr) : obj(TToken< T >::GetIObjObjectFor(ptr).release()) {}

  const rstl::auto_ptr< IObj >& GetObjForTransfer() const { return obj; }

private:
  rstl::auto_ptr< IObj > obj;
};

typedef CFactoryFnReturn (*FFactoryFunc)(const SObjectTag& tag, CInputStream& in,
                                         const CVParamTransfer& xfer);
typedef CFactoryFnReturn (*FMemFactoryFunc)(const SObjectTag& tag,
                                            const rstl::auto_ptr< uchar >& buffer, int size,
                                            const CVParamTransfer& xfer);

class CFactoryMgr {
public:
  CFactoryMgr();
  ~CFactoryMgr();

  void AddFactory(FourCC type, FFactoryFunc factory);
  rstl::auto_ptr< IObj > MakeObject(const SObjectTag& tag, CInputStream& in,
                                    const CVParamTransfer& params);
  rstl::auto_ptr< IObj > MakeObjectFromMemory(const SObjectTag& tag,
                                              const rstl::auto_ptr< uchar >& buffer, int size,
                                              bool compressed, const CVParamTransfer& params);

  static uint FourCCToTypeIdx(uint fourCC);
  static uint TypeIdxToFourCC(uint typeIdx);

private:
  // Unlike Echoes the prototype has no separate memory-factory map: its constructor (0x804FE414)
  // initializes a single map, CResFactory places its load list right after it at 0x94, and
  // MakeObjectFromMemory always reads through a stream.
  rstl::map< int, FFactoryFunc > mFactories;
};
CHECK_SIZEOF(CFactoryMgr, 0x14)

CFactoryFnReturn FStringTableFactory(const SObjectTag& tag, CInputStream& in,
                                     const CVParamTransfer& xfer);

CFactoryFnReturn FDependencyGroupFactory(const SObjectTag& tag, CInputStream& in,
                                         const CVParamTransfer& xfer);

#endif // _CFACTORYMGR
