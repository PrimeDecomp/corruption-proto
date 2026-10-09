#include "Kyoto/Particles/CParticleDataFactory.hpp"

#include "Kyoto/CFactoryMgr.hpp"
#include "Kyoto/CRandom16.hpp"
#include "Kyoto/CSimplePool.hpp"
#include "Kyoto/CVParamTransfer.hpp"
#include "Kyoto/Graphics/CTexture.hpp"
#include "Kyoto/Particles/CColorElement.hpp"
#include "Kyoto/Particles/CElementAllocationChunk.hpp"
#include "Kyoto/Particles/CEmitterElement.hpp"
#include "Kyoto/Particles/CGenDescription.hpp"
#include "Kyoto/Particles/CIntElement.hpp"
#include "Kyoto/Particles/CModVectorElement.hpp"
#include "Kyoto/Particles/CParticleGen.hpp"
#include "Kyoto/Particles/CRealElement.hpp"
#include "Kyoto/Particles/CSpawnSystemKeyframeData.hpp"
#include "Kyoto/Particles/CSwooshDescription.hpp"
#include "Kyoto/Particles/CUVElement.hpp"
#include "Kyoto/Particles/CVectorElement.hpp"
#include "Kyoto/Particles/IElement.hpp"
#include "Kyoto/SObjectTag.hpp"
#include "Kyoto/Streams/CInputStream.hpp"
#include "Kyoto/TToken.hpp"
#include "dolphin/types.h"
#include "rstl/algorithm.hpp"
#include "rstl/list.hpp"
#include "rstl/optional_object.hpp"
#define SBIG(v) v

CTexture* CreateTexture(int value);

rstl::list< CElementAllocationChunk > sElementAllocationChunks;
CElementAllocationChunk* IElement::CElementAllocator::sCurrentChunk = nullptr;
CElementAllocationChunk* IElement::CElementAllocator::sFreeChunk = nullptr;

void* IElement::CElementAllocator::Alloc(size_t size, const char*, const char*) {
  if (sCurrentChunk == nullptr || !sCurrentChunk->CanAllocate(size)) {
    sElementAllocationChunks.push_back(CElementAllocationChunk());
    sCurrentChunk = &sElementAllocationChunks.back();
  }

  return sCurrentChunk->Allocate(size);
}

void IElement::CElementAllocator::Free(void* ptr, size_t) {
  if (ptr == nullptr) {
    return;
  }

  if (sFreeChunk == nullptr || !sFreeChunk->Contains(ptr)) {
    sFreeChunk = nullptr;
    for (rstl::list< CElementAllocationChunk >::iterator it = sElementAllocationChunks.begin();
         it != sElementAllocationChunks.end(); ++it) {
      if (it->Contains(ptr)) {
        sFreeChunk = &*it;
        break;
      }
    }
  }

  sFreeChunk->Free(ptr);
  if (sFreeChunk->GetAllocationCount() == 0) {
    for (rstl::list< CElementAllocationChunk >::iterator it = sElementAllocationChunks.begin();
         it != sElementAllocationChunks.end(); ++it) {
      if (&*it == sFreeChunk) {
        sElementAllocationChunks.erase(it);
        if (sCurrentChunk == sFreeChunk) {
          sCurrentChunk = nullptr;
        }
        break;
      }
    }
    sFreeChunk = nullptr;
  }
}

CFactoryFnReturn FParticleFactory(const SObjectTag& tag, CInputStream& in,
                                  const CVParamTransfer& xfer) {
  rstl::rc_ptr< IVParamObj > obj = xfer.GetObj();
  CSimplePool* pool = static_cast< TObjOwnerParam< CSimplePool* >* >(obj.GetPtr())->GetData();
  CGenDescription* desc = CParticleDataFactory::GetGeneratorDesc(in, pool, tag.id);
  return desc;
}

CGenDescription* CParticleDataFactory::GetGeneratorDesc(CInputStream& in, CSimplePool* pool,
                                                        const CAssetId& id) {
  rstl::vector< CAssetId > assets;
  assets.reserve(8);
  return CParticleDataFactory::CreateGeneratorDescription(in, assets, id, pool);
}

CGenDescription* CParticleDataFactory::CreateGeneratorDescription(CInputStream& in,
                                                                  rstl::vector< CAssetId >& assets,
                                                                  const CAssetId& id,
                                                                  CSimplePool* pool) {
  if (rstl::count(assets.begin(), assets.end(), id) != 0) {
    return nullptr;
  }
  assets.push_back_unsafe(id);
  FourCC clsId = GetClassID(in);
  if (clsId != SBIG('GPSM')) {
    return nullptr;
  }
  CGenDescription* desc = NEW CGenDescription;
  CreateGPSM(desc, in, assets, pool);
  LoadGPSMTokens(desc);
  return desc;
}

void CParticleDataFactory::LoadGPSMTokens(CGenDescription* desc) {
  if (desc->mPMDL) {
    desc->mPMDL->ForceCache();
  }
  if (desc->mICTS) {
    desc->mICTS->ForceCache();
  }
  if (desc->mIDTS) {
    desc->mIDTS->ForceCache();
  }
  if (desc->mIITS) {
    desc->mIITS->ForceCache();
  }
  if (desc->mSSWH) {
    desc->mSSWH->ForceCache();
  }
}

bool CParticleDataFactory::CreateGPSM(CGenDescription* desc, CInputStream& in,
                                      rstl::vector< CAssetId >& resources, CSimplePool* pool) {
  bool done = false;
  CRandom16 random(99);
  CGlobalRandom context(random);
  while (!done) {
    FourCC clsId = GetClassID(in);
    switch (clsId) {
    case SBIG('PSIV'):
      delete GetVectorElement(in);
      break;
    case SBIG('PSVM'):
      delete GetModVectorElement(in);
      break;
    case SBIG('PSOV'):
      delete GetVectorElement(in);
      break;
    case SBIG('PSTS'):
      desc->mPSTS = GetRealElement(in);
      break;
    case SBIG('PSLT'):
      desc->mPSLT = GetIntElement(in);
      break;
    case SBIG('PSWT'):
      desc->mPSWT = GetIntElement(in);
      break;
    case SBIG('LIT_'):
      desc->mLIT_ = GetBool(in);
      break;
    case SBIG('ORNT'):
      desc->mORNT = GetBool(in);
      break;
    case SBIG('RSOP'):
      desc->mRSOP = GetBool(in);
      break;
    case SBIG('AAPH'):
      desc->mAAPH = GetBool(in);
      break;
    case SBIG('XTAD'):
      desc->mXTAD = GetIntElement(in);
      break;
    case SBIG('ZBUF'):
      desc->mZBUF = GetBool(in);
      break;
    case SBIG('SORT'):
      desc->mSORT = GetBool(in);
      break;
    case SBIG('MBLR'):
      desc->mMBLR = GetBool(in);
      break;
    case SBIG('MBSP'):
      desc->mMBSP = GetIntElement(in);
      break;
    case SBIG('MAXP'):
      desc->mMAXP = GetIntElement(in);
      break;
    case SBIG('GRTE'):
      desc->mGRTE = GetRealElement(in);
      break;
    case SBIG('ILOC'):
      delete GetVectorElement(in);
      break;
    case SBIG('IVEC'):
      delete GetVectorElement(in);
      break;
    case SBIG('EMTR'):
      desc->mEMTR = GetEmitterElement(in);
      break;
    case SBIG('SIZE'):
      desc->mSIZE = GetRealElement(in);
      break;
    case SBIG('COLR'):
      desc->mCOLR = GetColorElement(in);
      break;
    case SBIG('POFS'):
      desc->mPOFS = GetVectorElement(in);
      break;
    case SBIG('VMD1'):
      desc->mVMD1 = GetBool(in);
      break;
    case SBIG('VMD2'):
      desc->mVMD2 = GetBool(in);
      break;
    case SBIG('VMD3'):
      desc->mVMD3 = GetBool(in);
      break;
    case SBIG('VMD4'):
      desc->mVMD4 = GetBool(in);
      break;
    case SBIG('VEL1'):
      desc->mVEL1 = GetModVectorElement(in);
      break;
    case SBIG('VEL2'):
      desc->mVEL2 = GetModVectorElement(in);
      break;
    case SBIG('VEL3'):
      desc->mVEL3 = GetModVectorElement(in);
      break;
    case SBIG('VEL4'):
      desc->mVEL4 = GetModVectorElement(in);
      break;
    case SBIG('LTME'):
      desc->mLTME = GetIntElement(in);
      break;
    case SBIG('ROTA'):
      desc->mROTA = GetRealElement(in);
      break;
    case SBIG('LENG'):
      desc->mLENG = GetRealElement(in);
      break;
    case SBIG('WIDT'):
      desc->mWIDT = GetRealElement(in);
      break;
    case SBIG('TEXR'):
      desc->mTEXR = GetTextureElement(in, pool);
      break;
    case SBIG('TIND'):
      desc->mTIND = GetTextureElement(in, pool);
      break;
    case SBIG('CIND'):
      desc->mCIND = GetBool(in);
      break;
    case SBIG('INDM'):
      desc->mINDM = GetBool(in);
      break;
    case SBIG('PMDL'): {
      rstl::optional_object< TToken< CModel > > model(GetModel(in, pool));
      if (model) {
        desc->mPMDL = TCachedToken< CModel >(*model);
      } else {
        desc->mPMDL = CGenDescription::TParticleModel();
      }
    } break;
    case SBIG('PMOP'):
      desc->mPMOP = GetVectorElement(in);
      break;
    case SBIG('PMRT'):
      desc->mPMRT = GetVectorElement(in);
      break;
    case SBIG('PMSC'):
      desc->mPMSC = GetVectorElement(in);
      break;
    case SBIG('PMCL'):
      desc->mPMCL = GetColorElement(in);
      break;
    case SBIG('PMAB'):
      desc->mPMAB = GetBool(in);
      break;
    case SBIG('PMUS'):
      desc->mPMUS = GetBool(in);
      break;
    case SBIG('PMOO'):
      desc->mPMOO = GetBool(in);
      break;
    case SBIG('VMPC'):
      desc->mVMPC = GetBool(in);
      break;
    case SBIG('PMOV'):
      desc->mPMOV = GetVectorElement(in);
      break;
    case SBIG('SEED'):
      desc->mSEED = GetIntElement(in);
      break;
    case SBIG('ICTS'): {
      rstl::optional_object< TToken< CGenDescription > > child(
          GetChildGeneratorDesc(in, pool, resources));
      if (child) {
        desc->mICTS = TCachedToken< CGenDescription >(*child);
      } else {
        desc->mICTS = CGenDescription::TChildGeneratorDesc();
      }
      break;
    }
    case SBIG('NCSY'):
      desc->mNCSY = GetIntElement(in);
      break;
    case SBIG('CSSD'):
      desc->mCSSD = GetIntElement(in);
      break;
    case SBIG('IDTS'): {
      rstl::optional_object< TToken< CGenDescription > > child(
          GetChildGeneratorDesc(in, pool, resources));
      if (child) {
        desc->mIDTS = TCachedToken< CGenDescription >(*child);
      } else {
        desc->mIDTS = CGenDescription::TChildGeneratorDesc();
      }
      break;
    }
    case SBIG('NDSY'):
      desc->mNDSY = GetIntElement(in);
      break;
    case SBIG('IITS'): {
      rstl::optional_object< TToken< CGenDescription > > child(
          GetChildGeneratorDesc(in, pool, resources));
      if (child) {
        desc->mIITS = TCachedToken< CGenDescription >(*child);
      } else {
        desc->mIITS = CGenDescription::TChildGeneratorDesc();
      }
      break;
    }
    case SBIG('PISY'):
      desc->mPISY = GetIntElement(in);
      break;
    case SBIG('SISY'):
      desc->mSISY = GetIntElement(in);
      break;
    case SBIG('SSWH'): {
      rstl::optional_object< TToken< CSwooshDescription > > child(GetSwooshGeneratorDesc(in, pool));
      if (child) {
        desc->mSSWH = TCachedToken< CSwooshDescription >(*child);
      } else {
        desc->mSSWH = CGenDescription::TSwooshGeneratorDesc();
      }
      break;
    }
    case SBIG('SSSD'):
      desc->mSSSD = GetIntElement(in);
      break;
    case SBIG('SSPO'):
      desc->mSSPO = GetVectorElement(in);
      break;
    case SBIG('SELC'): {
      rstl::optional_object< TToken< CElectricDescription > > electric =
          GetElectricGeneratorDesc(in, pool);
      if (electric) {
        desc->mSELC = *electric;
      } else {
        desc->mSELC = CGenDescription::TElectricGeneratorDesc();
      }
      break;
    }
    case SBIG('SESD'):
      desc->mSESD = GetIntElement(in);
      break;
    case SBIG('SEPO'):
      desc->mSEPO = GetVectorElement(in);
      break;
    case SBIG('KSSM'):
      if (GetClassID(in) == SBIG('CNST')) {
        desc->mKSSM = NEW CSpawnSystemKeyframeData(in);
        desc->mKSSM->LoadAllSpawnedSystemTokens(pool);
      }
      break;
    case SBIG('LINE'):
      desc->mLINE = GetBool(in);
      break;
    case SBIG('FXLL'):
      desc->mFXLL = GetBool(in);
      break;
    case SBIG('LTYP'):
      desc->mLTYP = GetIntElement(in);
      break;
    case SBIG('LCLR'):
      desc->mLCLR = GetColorElement(in);
      break;
    case SBIG('LINT'):
      desc->mLINT = GetRealElement(in);
      break;
    case SBIG('LOFF'):
      desc->mLOFF = GetVectorElement(in);
      break;
    case SBIG('LDIR'):
      desc->mLDIR = GetVectorElement(in);
      break;
    case SBIG('LFOT'):
      desc->mLFOT = GetIntElement(in);
      break;
    case SBIG('LFOR'):
      desc->mLFOR = GetRealElement(in);
      break;
    case SBIG('LSLA'):
      desc->mLSLA = GetRealElement(in);
      break;
    case SBIG('OPTS'):
      desc->mOPTS = GetBool(in);
      break;
    case SBIG('ADV1'):
      desc->mADV1 = GetRealElement(in);
      break;
    case SBIG('ADV2'):
      desc->mADV2 = GetRealElement(in);
      break;
    case SBIG('ADV3'):
      desc->mADV3 = GetRealElement(in);
      break;
    case SBIG('ADV4'):
      desc->mADV4 = GetRealElement(in);
      break;
    case SBIG('ADV5'):
      desc->mADV5 = GetRealElement(in);
      break;
    case SBIG('ADV6'):
      desc->mADV6 = GetRealElement(in);
      break;
    case SBIG('ADV7'):
      desc->mADV7 = GetRealElement(in);
      break;
    case SBIG('ADV8'):
      desc->mADV8 = GetRealElement(in);
      break;
    case SBIG('ADV9'):
      desc->mADV9 = GetRealElement(in);
      break;
    case SBIG('VAV1'):
      desc->mVAV1 = GetVectorElement(in);
      break;
    case SBIG('VAV2'):
      desc->mVAV2 = GetVectorElement(in);
      break;
    case SBIG('VAV3'):
      desc->mVAV3 = GetVectorElement(in);
      break;
    case SBIG('DFLG'):
      desc->mDFLG = GetBitflag(in);
      break;
    case SBIG('FXBR'):
      desc->mFXBR = GetRealElement(in);
      break;
    case SBIG('FXBO'):
      desc->mFXBO = GetVectorElement(in);
      break;
    case SBIG('RDOP'):
      desc->mRDOP = GetBool(in);
      break;
    case SBIG('_END'):
      done = true;
      break;
    default:
      return false;
    }
  }

  return true;
}

FourCC CParticleDataFactory::GetClassID(CInputStream& in) { return in.ReadInt32(); }

bool CParticleDataFactory::GetBool(CInputStream& in) {
  GetClassID(in);
  return in.ReadBool();
}

int CParticleDataFactory::GetBitflag(CInputStream& in) {
  GetClassID(in);
  FourCC type = GetClassID(in);
  uint count = in.ReadInt32();
  switch (type) {
  case SBIG('BITF'):
    return in.ReadInt32();
  default:
    for (uint i = 0; i < count; ++i) {
      in.ReadUint8();
    }
    return 0;
  }
}

int CParticleDataFactory::GetInt(CInputStream& in) { return in.ReadInt32(); }

float CParticleDataFactory::GetReal(CInputStream& in) { return in.ReadFloat(); }

CIntElement* CParticleDataFactory::GetIntElement(CInputStream& in) {
  FourCC clsId = GetClassID(in);
  switch (clsId) {
  case SBIG('CNST'): {
    return NEW CIEConstant(GetInt(in));
  }
  case SBIG('KEYE'):
  case SBIG('KEYP'): {
    return NEW CIEKeyframeEmitter(in);
  }
  case SBIG('KEYF'): {
    return NEW CIEKeyframeInput(in);
  }
  case SBIG('TSCL'): {
    return NEW CIETimescale(GetRealElement(in));
  }
  case SBIG('DETH'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    return NEW CIEDeath(a, b);
  }
  case SBIG('CHAN'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    CIntElement* c = GetIntElement(in);
    return NEW CIETimeChain(a, b, c);
  }
  case SBIG('ADD_'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    return NEW CIEAdd(a, b);
  }
  case SBIG('MULT'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    return NEW CIEMultiply(a, b);
  }
  case SBIG('DIVD'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    return NEW CIEDivide(a, b);
  }
  case SBIG('MODU'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    return NEW CIEModulo(a, b);
  }
  case SBIG('RAND'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    return NEW CIERandom(a, b);
  }
  case SBIG('IMPL'): {
    return NEW CIEImpulse(GetIntElement(in));
  }
  case SBIG('ILPT'): {
    return NEW CIELifetimePercent(GetIntElement(in));
  }
  case SBIG('SPAH'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    CIntElement* c = GetIntElement(in);
    return NEW CIESampleAndHold(c, a, b);
  }
  case SBIG('IRND'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    return NEW CIEInitialRandom(a, b);
  }
  case SBIG('CLMP'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    CIntElement* c = GetIntElement(in);
    return NEW CIEClamp(a, b, c);
  }
  case SBIG('PULS'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    CIntElement* c = GetIntElement(in);
    CIntElement* d = GetIntElement(in);
    return NEW CIEPulse(a, b, c, d);
  }
  case SBIG('NONE'): {
    return nullptr;
  }
  case SBIG('RTOI'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    return NEW CIERealToInt(a, b);
  }
  case SBIG('SUB_'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    return NEW CIESubtract(a, b);
  }
  case SBIG('GTCP'): {
    return NEW CIEGetCumulativeParticleCount();
  }
  case SBIG('GAPC'): {
    return NEW CIEGetActiveParticleCount();
  }
  case SBIG('GEMT'): {
    return NEW CIEGetEmitterTime();
  }
  case SBIG('ISWT'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    return NEW CIEInitialSwitch(a, b);
  }
  case SBIG('KPIN'): {
    CIntElement* a = GetIntElement(in);
    return NEW CIEKeepInitial(a);
  }
  case SBIG('PCRT'): {
    return NEW CIEParticleCreationTime();
  }
  case SBIG('PDET'): {
    return NEW CIEParticleCreationTime();
  }
  }
  return nullptr;
}

CRealElement* CParticleDataFactory::GetRealElement(CInputStream& in) {
  FourCC clsId = GetClassID(in);
  switch (clsId) {
  case SBIG('CNST'): {
    return NEW CREConstant(GetReal(in));
  }
  case SBIG('NONE'): {
    return nullptr;
  }
  case SBIG('KEYE'):
  case SBIG('KEYP'): {
    return NEW CREKeyframeEmitter(in);
  }
  case SBIG('KEYF'): {
    return NEW CREKeyframeInput(in);
  }
  case SBIG('SCAL'): {
    return NEW CRETimeScale(GetRealElement(in));
  }
  case SBIG('SINE'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    return NEW CRESineWave(c, a, b);
  }
  case SBIG('ADD_'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    return NEW CREAdd(a, b);
  }
  case SBIG('MULT'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    return NEW CREMultiply(a, b);
  }
  case SBIG('DOTP'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    return NEW CREDotProduct(a, b);
  }
  case SBIG('RAND'): {
    CRealElement* min = GetRealElement(in);
    CRealElement* max = GetRealElement(in);
    return NEW CRERandom(min, max);
  }
  case SBIG('IRND'): {
    CRealElement* min = GetRealElement(in);
    CRealElement* max = GetRealElement(in);
    return NEW CREInitialRandom(min, max);
  }
  case SBIG('CHAN'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    CIntElement* c = GetIntElement(in);
    return NEW CRETimeChain(a, b, c);
  }
  case SBIG('CLMP'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    return NEW CREClamp(a, b, c);
  }
  case SBIG('PULS'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    return NEW CREPulse(a, b, c, d);
  }
  case SBIG('RLPT'): {
    CRealElement* a = GetRealElement(in);
    return NEW CRELifetimePercent(a);
  }
  case SBIG('LFTW'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    return NEW CRELifetimeTween(a, b);
  }
  case SBIG('PRLW'): {
    return NEW CREParticleRotationOrLineWidth();
  }
  case SBIG('PSLL'): {
    return NEW CREParticleSizeOrLineLength();
  }
  case SBIG('PAP1'): {
    return NEW CREParticleAccessParameter1();
  }
  case SBIG('PAP2'): {
    return NEW CREParticleAccessParameter2();
  }
  case SBIG('PAP3'): {
    return NEW CREParticleAccessParameter3();
  }
  case SBIG('PAP4'): {
    return NEW CREParticleAccessParameter4();
  }
  case SBIG('PAP5'): {
    return NEW CREParticleAccessParameter5();
  }
  case SBIG('PAP6'): {
    return NEW CREParticleAccessParameter6();
  }
  case SBIG('PAP7'): {
    return NEW CREParticleAccessParameter7();
  }
  case SBIG('PAP8'): {
    return NEW CREParticleAccessParameter8();
  }
  case SBIG('PAP9'): {
    return NEW CREParticleAccessParameter9();
  }
  case SBIG('VXTR'): {
    CVectorElement* a = GetVectorElement(in);
    return NEW CREVectorXToReal(a);
  }
  case SBIG('VYTR'): {
    CVectorElement* a = GetVectorElement(in);
    return NEW CREVectorYToReal(a);
  }
  case SBIG('VZTR'): {
    CVectorElement* a = GetVectorElement(in);
    return NEW CREVectorZToReal(a);
  }
  case SBIG('VMAG'): {
    CVectorElement* a = GetVectorElement(in);
    return NEW CREVectorMagnitude(a);
  }
  case SBIG('ISWT'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    return NEW CREInitialSwitch(a, b);
  }
  case SBIG('CLTN'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    return NEW CRECompareLessThan(a, b, c, d);
  }
  case SBIG('CEQL'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    return NEW CRECompareEqual(a, b, c, d);
  }
  case SBIG('CRNG'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    CRealElement* e = GetRealElement(in);
    return NEW CREConstantRange(a, b, c, d, e);
  }
  case SBIG('CEXT'): {
    CIntElement* a = GetIntElement(in);
    return NEW CREExternalVar(a);
  }
  case SBIG('ITRL'): {
    CIntElement* a = GetIntElement(in);
    CRealElement* b = GetRealElement(in);
    return NEW CREIntTimesReal(a, b);
  }
  case SBIG('SUB_'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    return NEW CRESubtract(a, b);
  }
  case SBIG('GTCR'): {
    CColorElement* alpha = GetColorElement(in);
    return NEW CREGetComponentRed(alpha);
  }
  case SBIG('GTCG'): {
    CColorElement* alpha = GetColorElement(in);
    return NEW CREGetComponentGreen(alpha);
  }
  case SBIG('GTCB'): {
    CColorElement* alpha = GetColorElement(in);
    return NEW CREGetComponentBlue(alpha);
  }
  case SBIG('GTCA'): {
    CColorElement* alpha = GetColorElement(in);
    return NEW CREGetComponentAlpha(alpha);
  }
  case SBIG('GTCP'): {
    return NEW CREGetCumulativeParticleCount();
  }
  case SBIG('KPIN'): {
    CRealElement* a = GetRealElement(in);
    return NEW CREKeepInitial(a);
  }
  case SBIG('OCSP'): {
    CIntElement* a = GetIntElement(in);
    return NEW CREOscillatingSweep(a);
  }
  case SBIG('TOCS'): {
    bool a = GetBool(in);
    CIntElement* b = GetIntElement(in);
    CIntElement* c = GetIntElement(in);
    CIntElement* d = GetIntElement(in);
    return NEW CRETimeOscillatingSweep(a, b, c, d);
  }
  case SBIG('PRN1'): {
    CRealElement* a = GetRealElement(in);
    return NEW CREPerlinNoise1d(a);
  }
  case SBIG('PRN2'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    return NEW CREPerlinNoise2d(a, b);
  }
  case SBIG('PRN3'): {
    CVectorElement* a = GetVectorElement(in);
    return NEW CREPerlinNoise3d(a);
  }
  case SBIG('PRN4'): {
    CVectorElement* a = GetVectorElement(in);
    CRealElement* b = GetRealElement(in);
    return NEW CREPerlinNoise4d(a, b);
  }
  case SBIG('PNO1'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    CIntElement* d = GetIntElement(in);
    return NEW CREPerlinNoiseOctave1d(a, b, c, d);
  }
  case SBIG('PNO2'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    CIntElement* e = GetIntElement(in);
    return NEW CREPerlinNoiseOctave2d(a, b, c, d, e);
  }
  case SBIG('PNO3'): {
    CVectorElement* a = GetVectorElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    CIntElement* d = GetIntElement(in);
    return NEW CREPerlinNoiseOctave3d(a, b, c, d);
  }
  case SBIG('PNO4'): {
    CVectorElement* a = GetVectorElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    CIntElement* e = GetIntElement(in);
    return NEW CREPerlinNoiseOctave4d(a, b, c, d, e);
  }
  }
  return nullptr;
}

CVectorElement* CParticleDataFactory::GetVectorElement(CInputStream& in) {
  CVectorElement* ret;
  FourCC clsId = GetClassID(in);
  switch (clsId) {
  case SBIG('NONE'):
    ret = nullptr;
    break;
  case SBIG('CNST'): {
    CElementAllocationChunk* allocationContext = IElement::CElementAllocator::GetCurrentChunk();
    uint initialSize = IElement::CElementAllocator::GetCurrentAllocatedSize();
    CRealElement* x = GetRealElement(in);
    CRealElement* y = GetRealElement(in);
    CRealElement* z = GetRealElement(in);
    if (x && y && z) {
      if (x->IsConstant() && y->IsConstant() && z->IsConstant()) {
        float xf, yf, zf;
        x->GetValue(0, xf);
        y->GetValue(0, yf);
        z->GetValue(0, zf);

        delete x;
        delete y;
        delete z;

        if (allocationContext != nullptr &&
            allocationContext == IElement::CElementAllocator::GetCurrentChunk()) {
          allocationContext->Rewind(allocationContext->GetAllocatedSize() - initialSize);
        }
        ret = NEW CVEFastConstant(xf, yf, zf);
        break;
      }
    }
    ret = NEW CVEConstant(x, y, z);
    break;
  }
  case SBIG('KEYE'):
  case SBIG('KEYP'):
    ret = NEW CVEKeyframeEmitter(in);
    break;
  case SBIG('KEYF'): {
    ret = NEW CVEKeyframeInput(in);
    break;
  }
  case SBIG('ANGC'): {
    CRealElement* angleXBias = GetRealElement(in);
    CRealElement* angleYBias = GetRealElement(in);
    CRealElement* angleXRange = GetRealElement(in);
    CRealElement* angleYRange = GetRealElement(in);
    CRealElement* magnitude = GetRealElement(in);
    ret = NEW CVEAngleCone(angleXBias, angleYBias, angleXRange, angleYRange, magnitude);
    break;
  }
  case SBIG('CONE'): {
    CVectorElement* direction = GetVectorElement(in);
    CRealElement* baseRadius = GetRealElement(in);
    ret = NEW CVECone(direction, baseRadius);
    break;
  }
  case SBIG('CIRC'): {
    CVectorElement* circleOffset = GetVectorElement(in);
    CVectorElement* circleNormal = GetVectorElement(in);
    CRealElement* angleConstant = GetRealElement(in);
    CRealElement* angleLinear = GetRealElement(in);
    CRealElement* radius = GetRealElement(in);
    ret = NEW CVECircle(circleOffset, circleNormal, angleConstant, angleLinear, radius);
    break;
  }
  case SBIG('RNDV'): {
    CRealElement* a = GetRealElement(in);
    ret = NEW CVERandomVector(a);
    break;
  }
  case SBIG('CCLU'): {
    CVectorElement* circleOffset = GetVectorElement(in);
    CVectorElement* circleNormal = GetVectorElement(in);
    CIntElement* cycleFrames = GetIntElement(in);
    CRealElement* randomFactor = GetRealElement(in);

    ret = NEW CVECircleCluster(circleOffset, circleNormal, cycleFrames, randomFactor);
    break;
  }
  case SBIG('ADD_'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    ret = NEW CVEAdd(a, b);
    break;
  }
  case SBIG('MULT'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    ret = NEW CVEMultiply(a, b);
    break;
  }
  case SBIG('CHAN'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    CIntElement* switchFrame = GetIntElement(in);

    ret = NEW CVETimeChain(a, b, switchFrame);
    break;
  }
  case SBIG('PULS'): {
    CIntElement* durationA = GetIntElement(in);
    CIntElement* durationB = GetIntElement(in);
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    ret = NEW CVEPulse(durationA, durationB, a, b);
    break;
  }
  case SBIG('RTOV'): {
    CRealElement* value = GetRealElement(in);
    ret = NEW CVERealToVector(value);
    break;
  }
  case SBIG('PLOC'): {
    ret = NEW CVEParticleLocation();
    break;
  }
  case SBIG('PLCO'): {
    ret = NEW CVEParticlePreviousLocation();
    break;
  }
  case SBIG('PVEL'): {
    ret = NEW CVEParticleVelocity();
    break;
  }
  case SBIG('PSOF'): {
    ret = NEW CVEParticleSystemOrientationFront();
    break;
  }
  case SBIG('PSOU'): {
    ret = NEW CVEParticleSystemOrientationUp();
    break;
  }
  case SBIG('PSOR'): {
    ret = NEW CVEParticleSystemOrientationRight();
    break;
  }
  case SBIG('PSTR'): {
    ret = NEW CVEParticleSystemTranslation();
    break;
  }
  case SBIG('SUB_'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    ret = NEW CVESubtract(a, b);
    break;
  }
  case SBIG('CTVC'): {
    CColorElement* value = GetColorElement(in);
    ret = NEW CVEColorToVector(value);
    break;
  }
  case SBIG('PNCV'): {
    ret = NEW CVENormalizedCompensatedVelocity();
    break;
  }
  case SBIG('NORM'): {
    CVectorElement* a = GetVectorElement(in);
    ret = NEW CVENormalize(a);
    break;
  }
  case SBIG('PAP1'): {
    ret = NEW CVEParticleAccessParameter1();
    break;
  }
  case SBIG('PAP2'): {
    ret = NEW CVEParticleAccessParameter2();
    break;
  }
  case SBIG('PAP3'): {
    ret = NEW CVEParticleAccessParameter3();
    break;
  }
  case SBIG('ISWT'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    ret = NEW CVEInitialSwitch(a, b);
    break;
  }
  case SBIG('KPIN'): {
    CVectorElement* a = GetVectorElement(in);
    ret = NEW CVEKeepInitial(a);
    break;
  }
  case SBIG('PIVL'): {
    ret = NEW CVEParticleInitialVelocity();
    break;
  }
  case SBIG('PINV'): {
    ret = NEW CVEParticleInitialNormalizedVelocity();
    break;
  }
  case SBIG('PITR'): {
    ret = NEW CVEParticleInitialTranslation();
    break;
  }
  case SBIG('PEVL'): {
    ret = NEW CVEParticleEndVelocity();
    break;
  }
  case SBIG('PENV'): {
    ret = NEW CVEParticleEndNormalizedVelocity();
    break;
  }
  case SBIG('PETR'): {
    ret = NEW CVEParticleEndTranslation();
    break;
  }
  default:
    ret = nullptr;
    break;
  }
  return ret;
}

CEmitterElement* CParticleDataFactory::GetEmitterElement(CInputStream& in) {
  CEmitterElement* ret;
  FourCC clsId = GetClassID(in);
  switch (clsId) {
  case SBIG('NONE'):
    ret = nullptr;
    break;
  case SBIG('SETR'): {
    FourCC prop = GetClassID(in);
    CVectorElement* pos = nullptr;
    CVectorElement* vel = nullptr;
    bool valid = false;
    if (prop == SBIG('ILOC')) {
      pos = GetVectorElement(in);
      prop = GetClassID(in);
      if (prop == SBIG('IVEC')) {
        vel = GetVectorElement(in);
        valid = true;
      }
    }
    ret = valid ? NEW CEESimpleEmitter(pos, vel) : nullptr;
    break;
  }
  case SBIG('SEMR'): {
    CVectorElement* pos = GetVectorElement(in);
    CVectorElement* vel = GetVectorElement(in);
    ret = NEW CEESimpleEmitter(pos, vel);
    break;
  }
  case SBIG('SPHE'): {
    CVectorElement* origin = GetVectorElement(in);
    CRealElement* radius = GetRealElement(in);
    CRealElement* velocity = GetRealElement(in);
    ret = NEW CVESphere(origin, radius, velocity);
    break;
  }
  case SBIG('ELPS'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    CVectorElement* c = GetVectorElement(in);
    CRealElement* d = GetRealElement(in);
    bool e = GetBool(in);
    ret = NEW CVEEllipsoid(a, b, c, d, e);
    break;
  }
  case SBIG('ASPH'): {
    CVectorElement* origin = GetVectorElement(in);
    CRealElement* angleXBias = GetRealElement(in);
    CRealElement* angleYBias = GetRealElement(in);
    CRealElement* angleXRange = GetRealElement(in);
    CRealElement* angleYRange = GetRealElement(in);
    CRealElement* radius = GetRealElement(in);
    CRealElement* velocity = GetRealElement(in);
    ret = NEW CVEAngleSphere(origin, radius, velocity, angleXBias, angleYBias, angleXRange,
                                angleYRange);
    break;
  }
  case SBIG('PLNE'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    CVectorElement* c = GetVectorElement(in);
    CRealElement* d = GetRealElement(in);
    CRealElement* e = GetRealElement(in);
    CRealElement* f = GetRealElement(in);
    ret = NEW CEEPlaneEmitter(a, b, c, d, e, f);
    break;
  }
  default:
    ret = nullptr;
    break;
  }
  return ret;
}

CModVectorElement* CParticleDataFactory::GetModVectorElement(CInputStream& in) {
  CModVectorElement* ret;
  FourCC clsId = GetClassID(in);
  switch (clsId) {
  case SBIG('NONE'): {
    ret = nullptr;
    break;
  }
  case SBIG('CNST'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    if (a && b && c && a->IsConstant() && b->IsConstant() && c->IsConstant()) {
      float af, bf, cf;
#if NONMATCHING
      a->GetValue(0, af);
      b->GetValue(0, bf);
      c->GetValue(0, cf);
#else
      // BUG: Fetching from the same element for each
      a->GetValue(0, af);
      a->GetValue(0, bf);
      a->GetValue(0, cf);
#endif
      ret = NEW CMVEFastConstant(af, bf, cf);
      delete a;
      delete b;
      delete c;
    } else {
      ret = NEW CMVEConstant(a, b, c);
    }
    break;
  }
  case SBIG('GRAV'): {
    ret = NEW CMVEGravity(GetVectorElement(in));
    break;
  }
  case SBIG('WIND'): {
    CVectorElement* a = GetVectorElement(in);
    CRealElement* b = GetRealElement(in);
    ret = NEW CMVEWind(a, b);
    break;
  }
  case SBIG('EXPL'): {
    CRealElement* a = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    ret = NEW CMVEExplode(a, b);
    break;
  }
  case SBIG('CHAN'): {
    CModVectorElement* a = GetModVectorElement(in);
    CModVectorElement* b = GetModVectorElement(in);
    CIntElement* c = GetIntElement(in);
    ret = NEW CMVETimeChain(a, b, c);
    break;
  }
  case SBIG('PULS'): {
    CIntElement* a = GetIntElement(in);
    CIntElement* b = GetIntElement(in);
    CModVectorElement* c = GetModVectorElement(in);
    CModVectorElement* d = GetModVectorElement(in);
    ret = NEW CMVEPulse(a, b, c, d);
    break;
  }
  case SBIG('IMPL'): {
    CVectorElement* a = GetVectorElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    bool e = GetBool(in);
    ret = NEW CMVEImplosion(a, b, c, d, e);
    break;
  }
  case SBIG('LMPL'): {
    CVectorElement* a = GetVectorElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    bool e = GetBool(in);
    ret = NEW CMVELinearImplosion(a, b, c, d, e);
    break;
  }
  case SBIG('EMPL'): {
    CVectorElement* a = GetVectorElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    bool e = GetBool(in);
    ret = NEW CMVEExponentialImplosion(a, b, c, d, e);
    break;
  }
  case SBIG('SWRL'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    ret = NEW CMVESwirl(a, b, c, d);
    break;
  }
  case SBIG('BNCE'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    CRealElement* c = GetRealElement(in);
    CRealElement* d = GetRealElement(in);
    bool e = GetBool(in);
    ret = NEW CMVEBounce(a, b, c, d, e);
    break;
  }
  case SBIG('SPOS'): {
    ret = NEW CMVESetPosition(GetVectorElement(in));
    break;
  }
  case SBIG('SPHV'): {
    CVectorElement* a = GetVectorElement(in);
    CRealElement* b = GetRealElement(in);
    CModVectorElement* c = GetModVectorElement(in);
    ret = NEW CMVESphereVolume(a, b, c);
    break;
  }
  case SBIG('BOXV'): {
    CVectorElement* a = GetVectorElement(in);
    CVectorElement* b = GetVectorElement(in);
    CModVectorElement* c = GetModVectorElement(in);
    ret = NEW CMVEBoxVolume(a, b, c);
    break;
  }
  default:
    return nullptr;
    break;
  }
  return ret;
}

CColorElement* CParticleDataFactory::GetColorElement(CInputStream& in) {
  CColorElement* ret;
  FourCC clsId = GetClassID(in);
  switch (clsId) {
  case SBIG('CNST'): {
    CElementAllocationChunk* allocationContext = IElement::CElementAllocator::GetCurrentChunk();
    uint initialSize = IElement::CElementAllocator::GetCurrentAllocatedSize();
    CRealElement* r = GetRealElement(in);
    CRealElement* g = GetRealElement(in);
    CRealElement* b = GetRealElement(in);
    CRealElement* a = GetRealElement(in);
    if (r && g && b && a) {
      if (r->IsConstant() && g->IsConstant() && b->IsConstant() && a->IsConstant()) {
        float rf, gf, bf, af;
        r->GetValue(0, rf);
        g->GetValue(0, gf);
        b->GetValue(0, bf);
        a->GetValue(0, af);

        delete r;
        delete g;
        delete b;
        delete a;

        if (allocationContext != nullptr &&
            allocationContext == IElement::CElementAllocator::GetCurrentChunk()) {
          allocationContext->Rewind(allocationContext->GetAllocatedSize() - initialSize);
        }
        ret = NEW CCEFastConstant(rf, gf, bf, af);
        break;
      }
    }
    ret = NEW CCEConstant(r, g, b, a);
    break;
  }
  case SBIG('KEYE'):
  case SBIG('KEYP'):
    ret = NEW CCEKeyframeEmitter(in);
    break;
  case SBIG('KEYF'): {
    ret = NEW CCEKeyframeInput(in);
    break;
  }
  case SBIG('FADE'): {
    CColorElement* a = GetColorElement(in);
    CColorElement* b = GetColorElement(in);
    CRealElement* end = GetRealElement(in);
    ret = NEW CCEFade(a, b, end);
    break;
  }
  case SBIG('CFDE'): {
    CColorElement* a = GetColorElement(in);
    CColorElement* b = GetColorElement(in);
    CRealElement* start = GetRealElement(in);
    CRealElement* end = GetRealElement(in);
    ret = NEW CCEFadeEnd(a, b, start, end);
    break;
  }
  case SBIG('CHAN'): {
    CColorElement* a = GetColorElement(in);
    CColorElement* b = GetColorElement(in);
    CIntElement* frame = GetIntElement(in);
    ret = NEW CCETimeChain(a, b, frame);
    break;
  }
  case SBIG('PULS'): {
    CIntElement* aDuration = GetIntElement(in);
    CIntElement* bDuration = GetIntElement(in);
    CColorElement* a = GetColorElement(in);
    CColorElement* b = GetColorElement(in);
    ret = NEW CCEPulse(aDuration, bDuration, a, b);
    break;
  }
  case SBIG('PCOL'):
    ret = NEW CCEParticleColor();
    break;
  case SBIG('MULT'): {
    CColorElement* a = GetColorElement(in);
    CColorElement* b = GetColorElement(in);
    ret = NEW CCEMultiply(a, b);
    break;
  }
  case SBIG('VRTC'): {
    CVectorElement* a = GetVectorElement(in);
    CRealElement* b = GetRealElement(in);
    ret = NEW CCEVectorAndRealToColor(a, b);
    break;
  }
  case SBIG('ISWT'): {
    CColorElement* a = GetColorElement(in);
    CColorElement* b = GetColorElement(in);
    ret = NEW CCEInitialSwitch(a, b);
    break;
  }
  case SBIG('KPIN'): {
    CColorElement* a = GetColorElement(in);
    ret = NEW CCEKeepInitial(a);
    break;
  }
  case SBIG('MDAO'): {
    CColorElement* a = GetColorElement(in);
    CRealElement* b = GetRealElement(in);
    ret = NEW CCEModifyAlphaOnly(a, b);
    break;
  }
  case SBIG('NONE'):
    ret = nullptr;
    break;
  default:
    ret = nullptr;
    break;
  }
  return ret;
}

CUVElement* CParticleDataFactory::GetTextureElement(CInputStream& in, CSimplePool* resPool) {
  CUVElement* ret;
  FourCC clsId = GetClassID(in);
  switch (clsId) {
  case SBIG('NONE'):
    ret = nullptr;
    break;
  case SBIG('CNST'): {
    CAssetId id = kInvalidAssetId;
    FourCC subId = GetClassID(in);
    if (subId != SBIG('NONE')) {
      id = CAssetId(in);
    }
    if (id == kInvalidAssetId) {
      TToken< CTexture > tex = CreateTexture(-1);
      ret = NEW CUVEConstant(tex);
    } else {
      TToken< CTexture > tex = resPool->GetObj(SObjectTag(SBIG('TXTR'), id));
      ret = NEW CUVEConstant(tex);
    }
    break;
  }
  case SBIG('ATEX'): {
    CAssetId id = kInvalidAssetId;
    FourCC subId = GetClassID(in);
    if (subId != SBIG('NONE')) {
      id = CAssetId(in);
    }
    CIntElement* tileW = GetIntElement(in);
    CIntElement* tileH = GetIntElement(in);
    CIntElement* strideW = GetIntElement(in);
    CIntElement* strideH = GetIntElement(in);
    CIntElement* cycleFrames = GetIntElement(in);
    bool loop = GetBool(in);
    if (id == kInvalidAssetId) {
      TToken< CTexture > tex = CreateTexture(-1);
      ret = NEW CUVEAnimTexture(tex, tileW, tileH, strideW, strideH, cycleFrames, loop);
    } else {
      TToken< CTexture > tex = resPool->GetObj(SObjectTag(SBIG('TXTR'), id));
      ret = NEW CUVEAnimTexture(tex, tileW, tileH, strideW, strideH, cycleFrames, loop);
    }
    break;
  }
  default:
    return nullptr;
  }
  return ret;
}

rstl::optional_object< TToken< CGenDescription > >
CParticleDataFactory::GetChildGeneratorDesc(CInputStream& in, CSimplePool* pool,
                                            const rstl::vector< CAssetId >& resources) {
  FourCC clsId = GetClassID(in);
  CAssetId id;
  if (clsId != SBIG('NONE')) {
    id = CAssetId(in);
  } else {
    return rstl::optional_object< TToken< CGenDescription > >();
  }
  if (id == kInvalidAssetId) {
    return rstl::optional_object< TToken< CGenDescription > >();
  }
  return GetChildGeneratorDesc(id, pool, resources);
}

rstl::optional_object< TToken< CGenDescription > >
CParticleDataFactory::GetChildGeneratorDesc(CAssetId id, CSimplePool* pool,
                                            const rstl::vector< CAssetId >& resources) {
  if (rstl::count(resources.begin(), resources.end(), id) == 0) {
    return TToken< CGenDescription >(pool->GetObj(SObjectTag('PART', id)));
  }
  return rstl::optional_object< TToken< CGenDescription > >();
}

rstl::optional_object< TToken< CSwooshDescription > >
CParticleDataFactory::GetSwooshGeneratorDesc(CInputStream& in, CSimplePool* pool) {
  FourCC clsId = GetClassID(in);
  CAssetId id;
  if (clsId != SBIG('NONE')) {
    id = CAssetId(in);
  } else {
    return rstl::optional_object< TToken< CSwooshDescription > >();
  }
  if (id == kInvalidAssetId) {
    return rstl::optional_object< TToken< CSwooshDescription > >();
  }
  return TToken< CSwooshDescription >(pool->GetObj(SObjectTag(SBIG('SWHC'), id)));
}

rstl::optional_object< TToken< CElectricDescription > >
CParticleDataFactory::GetElectricGeneratorDesc(CInputStream& in, CSimplePool* pool) {
  FourCC clsId = GetClassID(in);
  CAssetId id;
  if (clsId != SBIG('NONE')) {
    id = CAssetId(in);
  } else {
    return rstl::optional_object< TToken< CElectricDescription > >();
  }
  if (id == kInvalidAssetId) {
    return rstl::optional_object< TToken< CElectricDescription > >();
  }
  return TToken< CElectricDescription >(pool->GetObj(SObjectTag(SBIG('ELSC'), id)));
}

rstl::optional_object< TToken< CModel > > CParticleDataFactory::GetModel(CInputStream& in,
                                                                         CSimplePool* pool) {
  FourCC clsId = GetClassID(in);
  CAssetId id;
  if (clsId != SBIG('NONE')) {
    id = CAssetId(in);
  } else {
    return rstl::optional_object< TToken< CModel > >();
  }
  if (id == kInvalidAssetId) {
    return rstl::optional_object< TToken< CModel > >();
  }
  return TToken< CModel >(pool->GetObj(SObjectTag(SBIG('CMDL'), id)));
}

CTexture* CreateTexture(int value) {
  CTexture* texture = NEW CTexture(kTF_RGBA8, 4, 4, 1);
  int* data = static_cast< int* >(texture->Lock());
  for (int i = 1; i <= 16; ++i) {
    data[i - 1] = value;
  }
  texture->UnLock();
  return texture;
}
