/*
 * G2MEAB CDecalDataFactory.cpp translation-unit scaffold.
 * .text: 0x80597290..0x80598024, end exclusive; 12 native functions.
 * Includes emitted description, quad, token and owning-pointer helpers.
 * NonMatching: implementation and declarations remain to be reconstructed.
 */

#include "Weapons/CDecalDataFactory.hpp"

#include "Kyoto/CRandom16.hpp"
#include "Kyoto/CSimplePool.hpp"
#include "Kyoto/CVParamTransfer.hpp"
#include "Kyoto/IObj.hpp"
#include "Kyoto/Particles/CParticleDataFactory.hpp"
#include "Kyoto/Streams/CInputStream.hpp"

CFactoryFnReturn FDecalDataFactory(const SObjectTag& tag, CInputStream& in,
                                   const CVParamTransfer& transfer) {
  rstl::rc_ptr< IVParamObj > obj = transfer.GetObj();
  CSimplePool* pool = static_cast< TObjOwnerParam< CSimplePool* >* >(obj.GetPtr())->GetData();
  CDecalDescription* ret = CDecalDataFactory::GetGeneratorDesc(in, pool);
  return ret;
}

static void hack() { TObjOwnerDerivedFromIObj< CDecalDescription >::GetNewDerivedObject(nullptr); }

CDecalDescription* CDecalDataFactory::GetGeneratorDesc(CInputStream& in, CSimplePool* pool) {
  return CreateGeneratorDescription(in, pool);
}

CDecalDescription* CDecalDataFactory::CreateGeneratorDescription(CInputStream& in,
                                                                 CSimplePool* pool) {
  if (CParticleDataFactory::GetClassID(in) != 'DPSM') {
    return nullptr;
  }

  CDecalDescription* desc = rs_new CDecalDescription();
  CreateDPSM(desc, in, pool);
  return desc;
}

void CDecalDataFactory::GetQuadDecalInfo(CInputStream& in, CSimplePool* pool, uint classId,
                                         CDecalDescription::SQuadDescr& quad) {
  switch (classId) {
  case '1LFT':
  case '2LFT':
    quad.mLFT = CParticleDataFactory::GetIntElement(in);
    break;
  case '1SZE':
  case '2SZE':
    quad.mSZE = CParticleDataFactory::GetRealElement(in);
    break;
  case '1ROT':
  case '2ROT':
    quad.mROT = CParticleDataFactory::GetRealElement(in);
    break;
  case '1OFF':
  case '2OFF':
    quad.mOFF = CParticleDataFactory::GetVectorElement(in);
    break;
  case '1CLR':
  case '2CLR':
    quad.mCLR = CParticleDataFactory::GetColorElement(in);
    break;
  case '1TEX':
  case '2TEX':
    quad.mTEX = CParticleDataFactory::GetTextureElement(in, pool);
    break;
  case '1ADD':
  case '2ADD':
    quad.mADD = CParticleDataFactory::GetBool(in);
    break;
  }
}

bool CDecalDataFactory::CreateDPSM(CDecalDescription* desc, CInputStream& in, CSimplePool* pool) {
  bool done = false;
  CRandom16 random;

  while (!done) {
    CGlobalRandom globalRandom(random);
    const FourCC clsId = CParticleDataFactory::GetClassID(in);
    bool loadFirstDesc = false;
    switch (clsId) {
    case '1SZE':
    case '1LFT':
    case '1ROT':
    case '1OFF':
    case '1CLR':
    case '1TEX':
    case '1ADD':
      loadFirstDesc = true;
    case '2LFT':
    case '2SZE':
    case '2ROT':
    case '2OFF':
    case '2CLR':
    case '2TEX':
    case '2ADD': {
      CDecalDescription::SQuadDescr& quad = loadFirstDesc ? desc->mQuad1 : desc->mQuad2;
      GetQuadDecalInfo(in, pool, clsId, quad);
    } break;
    case 'DMDL': {
      rstl::optional_object< TToken< CModel > > model = CParticleDataFactory::GetModel(in, pool);
      if (model.valid()) {
        desc->mDMDL = TLockedToken< CModel >(*model);
      } else {
        desc->mDMDL = rstl::optional_object_null();
      }
    } break;
    case 'DLFT':
      desc->mDLFT = CParticleDataFactory::GetIntElement(in);
      break;
    case 'DMOP':
      desc->mDMOP = CParticleDataFactory::GetVectorElement(in);
      break;
    case 'DMRT':
      desc->mDMRT = CParticleDataFactory::GetVectorElement(in);
      break;
    case 'DMSC':
      desc->mDMSC = CParticleDataFactory::GetVectorElement(in);
      break;
    case 'DMCL':
      desc->mDMCL = CParticleDataFactory::GetColorElement(in);
      break;
    case 'DMAB':
      desc->mDMAB = CParticleDataFactory::GetBool(in);
      break;
    case 'DMOO':
      desc->mDMOO = CParticleDataFactory::GetBool(in);
      break;
    case '_END':
      done = true;
      break;
    default:
      return false;
    }
  }

  return true;
}

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern unsigned char lbl_806B1B80[12];
extern unsigned char lbl_806B1B8C[12];
extern unsigned char lbl_806E2430[16];
extern "C" void fn_80597B98(int, int);
extern "C" int fn_80597B08(int obj, int val);
extern "C" int fn_80597B08(int obj, int val) {
    if (obj) {
        *(int*)obj = (int)lbl_806E2430;
        if ((unsigned int)*(int*)((char*)obj + 0x4) != 0) {
            fn_80597B98(*(int*)((char*)obj + 0x4), 1);
        }
        if ((unsigned int)obj != 0) {
            *(int*)obj = (int)lbl_806B1B8C;
            if ((unsigned int)obj != 0) {
                *(int*)obj = (int)lbl_806B1B80;
            }
        }
        if ((short)val > 0) {
            CMemory::Free((const void*)obj);
        }
    }
    return obj;
}

struct __mwdec_vt_0 { virtual void _0(int); };
struct __mwdec_vt_1 { virtual void _0(int); };
struct __mwdec_vt_2 { virtual void _0(int); };
struct __mwdec_vt_3 { virtual void _0(int); };
struct __mwdec_vt_4 { virtual void _0(int); };
struct __mwdec_vt_5 { virtual void _0(int); };
extern "C" int fn_80597CF0(int obj, int val);
extern "C" int fn_80597CF0(int obj, int val) {
    if (obj) {
        if (obj + 20) {
            unsigned int val2 = *(int*)((char*)obj + 0x14);
            if (val2 != 0) {
                ((__mwdec_vt_0*)val2)->_0(1);
            }
        }
        if (obj + 16) {
            unsigned int val3 = *(int*)((char*)obj + 0x10);
            if (val3 != 0) {
                ((__mwdec_vt_1*)val3)->_0(1);
            }
        }
        if (obj + 12) {
            unsigned int val4 = *(int*)((char*)obj + 0xc);
            if (val4 != 0) {
                ((__mwdec_vt_2*)val4)->_0(1);
            }
        }
        if (obj + 8) {
            unsigned int val5 = *(int*)((char*)obj + 0x8);
            if (val5 != 0) {
                ((__mwdec_vt_3*)val5)->_0(1);
            }
        }
        if (obj + 4) {
            unsigned int val6 = *(int*)((char*)obj + 0x4);
            if (val6 != 0) {
                ((__mwdec_vt_4*)val6)->_0(1);
            }
        }
        if ((unsigned int)obj != 0) {
            unsigned int val7 = *(int*)obj;
            if (val7 != 0) {
                ((__mwdec_vt_5*)val7)->_0(1);
            }
        }
        if ((short)val > 0) {
            CMemory::Free((const void*)obj);
        }
    }
    return obj;
}

