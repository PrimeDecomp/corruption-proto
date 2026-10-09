#ifndef _CCECHARACTERFACTORYBUILDER
#define _CCECHARACTERFACTORYBUILDER

#include "types.h"

#include "Kyoto/CResFactory.hpp"
#include "Kyoto/CSimplePool.hpp"

class IObjectStore;

// Minimal declaration. The class name comes from the CCECharacterFactoryBuilder.cpp allocation
// asserts; it is the prototype's CCharacterFactoryBuilder. Unlike Echoes the dummy factory keeps
// the object store it was built with (it fetches the CHAR and SAND resources through it), so the
// builder is 0x2C bytes.
class CCECharacterFactoryBuilder {
public:
  // Echoes' name. Constructor 0x8058E038, destructor 0x8058E068.
  class CDummyFactory : public IFactory {
  public:
    CDummyFactory(IObjectStore& store);
    ~CDummyFactory();
    rstl::auto_ptr< IObj > Build(const SObjectTag& tag, const CVParamTransfer& params) override;
    void BuildAsync(const SObjectTag& tag, const CVParamTransfer& params, IObj** out) override;
    void CancelBuild(const SObjectTag& tag) override;
    bool CanBuild(const SObjectTag& tag) override;
    const SObjectTag* GetResourceIdByName(const char* name) const override;

  private:
    IObjectStore* mStore; // Guessed name
  };

  CCECharacterFactoryBuilder(IObjectStore& store);
  ~CCECharacterFactoryBuilder();

private:
  CDummyFactory mDummyFactory; // Echoes' name
  CSimplePool mDummyStore;     // Echoes' name
};
CHECK_SIZEOF(CCECharacterFactoryBuilder, 0x2c)

extern CCECharacterFactoryBuilder* gpCharacterFactoryBuilder; // Echoes' name

#endif // _CCECHARACTERFACTORYBUILDER
