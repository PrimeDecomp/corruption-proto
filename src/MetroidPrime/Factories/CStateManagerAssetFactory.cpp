// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x802FE800..0x802FEF4C (14 native functions).
// Guessed file and class name; see the header. Listed below are the functions not implemented
// yet, both called from CRigidBody, CPhazonPuddle, CScriptActorMorph and CScriptCrossAreaRelay:
// 0x802FEA5C +0x60: returns mPool's token for (type, id), with the id passed by value
// 0x802FEABC +0xA0: registers a builder for a type unless one is registered already

#include "MetroidPrime/CStateManagerAssetFactory.hpp"

CStateManagerAssetFactory::CStateManagerAssetFactory(CStateManager& mgr)
: mPool(*this), mStateMgr(&mgr) {}

CStateManagerAssetFactory::~CStateManagerAssetFactory() {}

rstl::auto_ptr< IObj > CStateManagerAssetFactory::Build(const SObjectTag& tag,
                                                        const CVParamTransfer& xfer) {
  return mFactories.find(tag.type)->second(*mStateMgr, tag, xfer);
}

void CStateManagerAssetFactory::BuildAsync(const SObjectTag& tag, const CVParamTransfer& xfer,
                                           IObj** objOut) {
  *objOut = Build(tag, xfer).release();
}

bool CStateManagerAssetFactory::CanBuild(const SObjectTag& tag) {
  return mFactories.find(tag.type) != mFactories.end();
}

const SObjectTag* CStateManagerAssetFactory::GetResourceIdByName(const char* name) const {
  return nullptr;
}

void CStateManagerAssetFactory::CancelBuild(const SObjectTag& tag) {}
