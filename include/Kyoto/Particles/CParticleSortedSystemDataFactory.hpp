#ifndef _CPARTICLESORTEDSYSTEMDATAFACTORY
#define _CPARTICLESORTEDSYSTEMDATAFACTORY

#include "types.h"
#include "Kyoto/CFactoryMgr.hpp"

class CSortedParticleSystemDescription;
class CInputStream;
class CSimplePool;
class CVParamTransfer;

// Class name follows Corruption's CParticleSortedSystemDataFactory.cpp diagnostics.
class CParticleSortedSystemDataFactory {
public:
  static CSortedParticleSystemDescription* GetGeneratorDesc(CInputStream& in, CSimplePool* pool);

private:
  static CSortedParticleSystemDescription* CreateGeneratorDescription(CInputStream& in,
                                                                      CSimplePool* pool);
  static bool CreateSRSM(CSortedParticleSystemDescription* desc, CInputStream& in,
                         CSimplePool* pool);
};

// Guessed free-factory name, supported by the SRSC registration and parsed descriptor type.
CFactoryFnReturn FSortedParticleSystemDataFactory(const SObjectTag& tag, CInputStream& in,
                                                  const CVParamTransfer& transfer);

#endif // _CPARTICLESORTEDSYSTEMDATAFACTORY
