#ifndef _CSTATEMANAGERASSETFACTORY
#define _CSTATEMANAGERASSETFACTORY

#include "types.h"

#include "Kyoto/CResFactory.hpp"
#include "Kyoto/CSimplePool.hpp"

#include "rstl/map.hpp"

class CStateManager;

// Guessed name, after Factories/CStateManagerAssetFactory.cpp, which holds its constructor
// (0x802FEC54). CStateManager news one (0x40 bytes) and keeps it at 0x13C.
//
// It is an IFactory whose builders are registered per resource type, like Prime's CFactoryMgr,
// and it owns a CSimplePool built over itself. Each builder receives the state manager. Objects
// that need runtime-built resources (CRigidBody's 'RDBY', for example) register a builder for a
// type and then fetch a token for that type with id 0 from the pool (0x802FEABC, 0x802FEA5C;
// their names are unknown and they are not implemented yet).
class CStateManagerAssetFactory : public IFactory {
public:
  // Guessed name. The builder is called with the factory's state manager.
  typedef rstl::auto_ptr< IObj > (*FFactoryFunc)(CStateManager& mgr, const SObjectTag& tag,
                                                 const CVParamTransfer& xfer);

  explicit CStateManagerAssetFactory(CStateManager& mgr);
  ~CStateManagerAssetFactory() override;

  // IFactory's interface. Build calls the builder registered for the tag's type without checking
  // that one exists; there is no resource name lookup, and nothing is built asynchronously.
  rstl::auto_ptr< IObj > Build(const SObjectTag& tag, const CVParamTransfer& xfer) override;
  void BuildAsync(const SObjectTag& tag, const CVParamTransfer& xfer, IObj** objOut) override;
  void CancelBuild(const SObjectTag& tag) override;
  bool CanBuild(const SObjectTag& tag) override;
  const SObjectTag* GetResourceIdByName(const char* name) const override;

private:
  // Guessed names.
  rstl::map< FourCC, FFactoryFunc > mFactories;
  CSimplePool mPool;
  CStateManager* mStateMgr;
};
CHECK_SIZEOF(CStateManagerAssetFactory, 0x40)

#endif // _CSTATEMANAGERASSETFACTORY
