/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x8048F1CC..0x8048FF38.
 */
#include "GuiSys/CGuiFrameModelDatabase.hpp"

#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Basics/CBasics.hpp"
#include "Kyoto/CFrameDelayedKiller.hpp"
#include "Kyoto/CSimplePool.hpp"
#include "Kyoto/Graphics/CCubeModel.hpp"
#include "Kyoto/Graphics/CModel.hpp"
#include "Kyoto/Math/CAABox.hpp"
#include "Kyoto/Streams/CInputStream.hpp"

#include <dolphin/os.h>

// Guessed local helper name; serialized lengths determine the section boundaries.
static uchar* MemoryFromPartData(uchar*& dataCur, int*& sectionSizeCur) {
  uchar* result;
  if (*sectionSizeCur != 0) {
    result = dataCur;
    dataCur += *sectionSizeCur;
  } else {
    result = nullptr;
  }
  ++sectionSizeCur;
  return result;
}

CGuiFrameModelDatabase::CGuiFrameModelDatabase(CInputStream& in, CSimplePool* pool)
: mBufferSize(in.ReadInt32())
, mBuffer(mBufferSize != 0
              ? static_cast< uchar* >(CMemory::Alloc(mBufferSize, IAllocator::kHI_RoundUpLen))
              : nullptr)
, mModels(in.ReadInt32(), rstl::auto_ptr< CCubeModel >())
, mSurfaces(mModels.size(), rstl::vector< void* >()) {
  CModel::AddToTotal(mBufferSize);
  rstl::vector< int > sectionSizes(in);
  if (!sectionSizes.empty()) {
    in.Get(mBuffer.get(), mBufferSize);

    int* sectionSizeCur = sectionSizes.data();
    const int modelCount = mModels.size();
    uchar* dataCur = mBuffer.get();
    const void* materialData = MemoryFromPartData(dataCur, sectionSizeCur);

    for (int i = 0; i < modelCount; ++i) {
      const void* positions = MemoryFromPartData(dataCur, sectionSizeCur);
      const void* normals = MemoryFromPartData(dataCur, sectionSizeCur);
      const void* colors = MemoryFromPartData(dataCur, sectionSizeCur);
      const void* uvs = MemoryFromPartData(dataCur, sectionSizeCur);
      const uint surfaceCount = CBasics::SwapBytes(
          *reinterpret_cast< const uint* >(MemoryFromPartData(dataCur, sectionSizeCur)));
      rstl::vector< void* >& surfaces = mSurfaces[i];
      surfaces.reserve(surfaceCount);
      for (uint j = 0; j < surfaceCount; ++j) {
        surfaces.push_back_unsafe(MemoryFromPartData(dataCur, sectionSizeCur));
      }

      const rstl::auto_ptr< CCubeModel > model(rs_new CCubeModel(pool, &surfaces, materialData,
                                                                 positions, normals, colors, uvs,
                                                                 CAABox::Identity(), 0, true, i));
      mModels[i] = model;
    }
  }

  DCFlushRange(mBuffer.get(), mBufferSize);
}

CGuiFrameModelDatabase::~CGuiFrameModelDatabase() {
  CModel::RemoveFromTotal(mBufferSize);
  CFrameDelayedKiller::ScheduleDeletion(CFrameDelayedKiller::kWhichFrame_NextFrame,
                                        mBuffer.release());
}

void CGuiFrameModelDatabase::Draw(int index, const CModelFlags& flags) const {
  mModels[index]->Draw(flags);
}

const CCubeModel* CGuiFrameModelDatabase::GetModel(int index) const {
  return mModels[index].get();
}

// Remaining native helpers (CCubeModel member destruction and stream-cursor reads) kept as raw
// drafts until the prototype CCubeModel layout is reconstructed.
// mwdec-drafted
extern "C" void fn_8048F3D8();
extern "C" void fn_8048F3B8();
extern "C" void fn_8048F3B8() {
    fn_8048F3D8();
}

extern "C" void fn_8048F770();
extern "C" void fn_8048F750();
extern "C" void fn_8048F750() {
    fn_8048F770();
}

extern "C" void fn_8048F460(int, int);
extern "C" int fn_8048FCDC(int obj, int obj2);
extern "C" int fn_8048FCDC(int obj, int obj2) {
    if (obj2 != (unsigned int)obj) {
        if (*(unsigned char*)obj) {
            fn_8048F460(*(int*)((char*)obj + 0x4), 1);
        }
        *(unsigned char*)obj = *(unsigned char*)obj2;
        *(int*)((char*)obj + 0x4) = *(int*)((char*)obj2 + 0x4);
        *(unsigned char*)obj2 = 0;
    }
    return obj;
}

extern "C" int fn_8048FF00(int obj, int obj2);
extern "C" int fn_8048FF00(int obj, int obj2) {
    int result;
    int val = *(int*)*(int*)obj2;
    if (val) {
        result = *(int*)obj;
        *(int*)obj = result + val;
    } else {
        result = 0;
    }
    *(int*)obj2 = *(int*)obj2 + 4;
    return result;
}
