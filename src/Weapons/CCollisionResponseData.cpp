/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x80595558..0x805971B8; 49 retained native bodies.
 * Function and helper inventory is recorded in the external agent workflow.
 */

#include "Weapons/CCollisionResponseData.hpp"

#include "Kyoto/CFactoryMgr.hpp"
#include "Kyoto/CRandom16.hpp"
#include "Kyoto/CVParamTransfer.hpp"
#include "Kyoto/Particles/CParticleDataFactory.hpp"
#include "Kyoto/Streams/CInputStream.hpp"
#include "Kyoto/Streams/CMemoryInStream.hpp"
#include "Weapons/CDecalDescription.hpp"

const int CCollisionResponseData::skWorldMaterialTable[] = {
    -2, kWCR_Crate, kWCR_Metal, kWCR_Grass, kWCR_Ice, -2, kWCR_Metal, kWCR_Wood,
    kWCR_Grass, kWCR_Metal, kWCR_Glass, kWCR_Snow, kWCR_Default, -2, kWCR_Default, kWCR_Default,
    kWCR_Goo, kWCR_Sand, kWCR_Default, kWCR_Default, -2, -2, kWCR_Wood2, kWCR_Mud,
    -2, kWCR_Default, -2, -2, -2, -2, -2, -2,
};

CCollisionResponseData::CCollisionResponseData(CInputStream& in, CSimplePool* sp) {
  bool done = false;
  mGeneratorTokens.assign(kWCR_Count);
  mSoundTokens.assign(kWCR_Count);
  mDecalTokens.assign(kWCR_Count);

  if (CParticleDataFactory::GetClassID(in) != 'CRSM') {
    return;
  }

  CRandom16 random(99);
  CGlobalRandom globalRandom(random);

  while (!done) {
    const FourCC clsId = CParticleDataFactory::GetClassID(in);
    if (CheckAndAddResourcesToResponse(clsId, in, sp)) {
      continue;
    }

    if (clsId == '_END') {
      done = true;
    } else {
      return;
    }
  }
}

bool CCollisionResponseData::CheckAndAddResourcesToResponse(const FourCC clsId, CInputStream& in,
                                                            CSimplePool* sp) {
  if (CheckAndAddParticleSystemToResponse(clsId, in, sp)) {
    return true;
  }

  if (CheckAndAddSoundFXToResponse(clsId, in, sp)) {
    return true;
  }

  if (CheckAndAddDecalToResponse(clsId, in, sp)) {
    return true;
  }

  return false;
}

bool CCollisionResponseData::CheckAndAddParticleSystemToResponse(const FourCC clsId,
                                                                 CInputStream& in,
                                                                 CSimplePool* sp) {
  static const FourCC kWCRTIDs[] = {'NODP', 'DEFS', 'CRTS', 'MTLS', 'GRAS', 'GLAS', 'ICEE',
                                    'SNEE', 'GOOO', 'WODS', 'WATR', '1MUD', '1WOD', '1LAV',
                                    '1SAN', '1PRJ', 'DCHR', 'CSHD', 'DENM', 'DSNM'};

  for (int i = 0; i < ARRAY_SIZE(kWCRTIDs); i++) {
    if (clsId != kWCRTIDs[i]) {
      continue;
    }
    AddParticleSystemToResponse(static_cast< EWeaponCollisionResponseTypes >(i), in, sp);
    return true;
  }

  return false;
}

bool CCollisionResponseData::CheckAndAddSoundFXToResponse(const FourCC clsId, CInputStream& in,
                                                          CSimplePool* sp) {
  static const FourCC kCRTSFXIDs[] = {'NSFX', 'DSND', 'CSND', 'MSND', 'GSND', 'GLND', 'ISND',
                                      'SNND', 'GOSD', 'WOSD', 'WTSD', '4MUD', '1WOD', '4LAV',
                                      '4SAN', '3PRJ', 'DCSD', 'CSSD', 'DESD', 'DSSD'};

  for (int i = 0; i < ARRAY_SIZE(kCRTSFXIDs); i++) {
    if (clsId != kCRTSFXIDs[i]) {
      continue;
    }

    if (CParticleDataFactory::GetClassID(in) != 'NONE') {
      mSoundTokens[i] =
          TLockedToken< CAudioGroupSet >(sp->GetObj(SObjectTag('CAUD', CAssetId(in))));
    }

    return true;
  }

  return false;
}

bool CCollisionResponseData::CheckAndAddDecalToResponse(const FourCC clsId, CInputStream& in,
                                                        CSimplePool* sp) {
  static const FourCC kCRTDecalIDs[] = {'NDCL', 'DDCL', 'CODL', 'MEDL', 'GRDL', 'GLDL', 'ICDL',
                                        'SNDL', 'GODL', 'WODL', 'WTDL', '3MUD', '1WOD', '3LAV',
                                        '3SAN', '....', 'CHDL', 'CSDL', 'ENDL', 'ESDL'};

  for (int i = 0; i < ARRAY_SIZE(kCRTDecalIDs); i++) {
    if (clsId != kCRTDecalIDs[i]) {
      continue;
    }

    if (CParticleDataFactory::GetClassID(in) != 'NONE') {
      mDecalTokens[i] = TToken< CDecalDescription >(sp->GetObj(SObjectTag('DPSC', CAssetId(in))));
    }

    return true;
  }

  return false;
}

bool CCollisionResponseData::AddParticleSystemToResponse(EWeaponCollisionResponseTypes type,
                                                         CInputStream& in, CSimplePool* sp) {
  rstl::vector< CAssetId > tracker;
  rstl::optional_object< TToken< CGenDescription > > tok =
      CParticleDataFactory::GetChildGeneratorDesc(in, sp, tracker);
  if (tok) {
    mGeneratorTokens[type] = *tok;
    return true;
  }
  return false;
}
CCollisionResponseData::~CCollisionResponseData() {}

rstl::optional_object< TLockedToken< CGenDescription > >
CCollisionResponseData::GetParticleDescription(const EWeaponCollisionResponseTypes type) const {
  int idx = type;
  if (!mGeneratorTokens.at(idx).valid()) {
    bool found = false;
    if (ResponseTypeIsEnemyNormal(type)) {
      idx = kWCR_EnemyNormal;
      found = true;
    } else if (ResponseTypeIsEnemySpecial(type)) {
      idx = kWCR_EnemySpecial;
      found = true;
    }

    if (found && !mGeneratorTokens.at(idx).valid()) {
      idx = kWCR_EnemyNormal;
    }

    if (!mGeneratorTokens.at(idx).valid() && idx != kWCR_None) {
      idx = kWCR_Default;
    }
  }

  return mGeneratorTokens.at(idx);
}

rstl::optional_object< TLockedToken< CAudioGroupSet > >
CCollisionResponseData::GetSoundDescription(const EWeaponCollisionResponseTypes type) const {
  int idx = type;
  if (!mSoundTokens.at(idx).valid()) {
    bool found = false;
    if (ResponseTypeIsEnemyNormal(type)) {
      idx = kWCR_EnemyNormal;
      found = true;
    } else if (ResponseTypeIsEnemySpecial(type)) {
      idx = kWCR_EnemySpecial;
      found = true;
    }

    if (found && !mSoundTokens.at(idx).valid()) {
      idx = kWCR_EnemyNormal;
    }

    if (!mSoundTokens.at(idx).valid()) {
      idx = kWCR_Default;
    }
  }

  return mSoundTokens.at(idx);
}

rstl::optional_object< TLockedToken< CDecalDescription > >
CCollisionResponseData::GetDecalDescription(const EWeaponCollisionResponseTypes type) const {
  return mDecalTokens.at(type);
}

bool CCollisionResponseData::ResponseTypeIsEnemySpecial(const EWeaponCollisionResponseTypes type) {
  if (type >= kWCR_EnemySpecial && type <= kWCR_EnemySpecial) {
    return true;
  }

  return false;
}

bool CCollisionResponseData::ResponseTypeIsEnemyNormal(const EWeaponCollisionResponseTypes type) {
  if (type >= kWCR_EnemyNormal && type <= kWCR_EnemyNormal) {
    return true;
  }

  return false;
}

EWeaponCollisionResponseTypes CCollisionResponseData::GetWorldCollisionResponseType(uint flags) {
  for (int i = 0; i < 32; i++) {
    if ((flags & (1 << i)) != 0 && skWorldMaterialTable[i] != -2) {
      return static_cast< EWeaponCollisionResponseTypes >(skWorldMaterialTable[i]);
    }
  }
  return kWCR_Default;
}

CFactoryFnReturn FCollisionResponseDataFactory(const SObjectTag& tag,
                                               const rstl::auto_ptr< uchar >& data, int length,
                                               const CVParamTransfer& xfer) {
  CMemoryInStream in(data.get(), length);
  rstl::rc_ptr< IVParamObj > obj = xfer.GetObj();
  CSimplePool* pool = static_cast< TObjOwnerParam< CSimplePool* >* >(obj.GetPtr())->GetData();
  return NEW CCollisionResponseData(in, pool);
}
