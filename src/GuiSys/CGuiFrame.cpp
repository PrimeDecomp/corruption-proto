/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x804829D8..0x80484540; 37 retained native bodies.
 * Function and helper inventory is recorded in the external agent workflow.
 */
#include "GuiSys/CGuiFrame.hpp"

#include "GuiSys/CGuiCamera.hpp"
#include "GuiSys/CGuiFactories.hpp"
#include "GuiSys/CGuiFrameLoader.hpp"
#include "GuiSys/CGuiFrameModelDatabase.hpp"
#include "GuiSys/CGuiHeadWidget.hpp"
#include "GuiSys/CGuiLight.hpp"
#include "GuiSys/CGuiModel.hpp"
#include "GuiSys/CGuiWidget.hpp"
#include "Kyoto/CResFactory.hpp"
#include "Kyoto/CSimplePool.hpp"
#include "Kyoto/Graphics/CGraphics.hpp"
#include "Kyoto/Streams/CMemoryInStream.hpp"
#include "rstl/algorithm.hpp"
#include "rstl/list.hpp"

namespace rstl {
class CWidgetFartherFromCamera {
public:
  bool operator()(const CGuiWidget* a, const CGuiWidget* b) const {
    return a->GetWorldPosition().GetY() > b->GetWorldPosition().GetY();
  }
};
} // namespace rstl

uint CGuiFrame::ReadVersion(CInputStream& in) { return in.Get< uint >(); }

rstl::vector< CToken > CGuiFrame::LoadAssets(CInputStream& in, CSimplePool* pool, uint version) {
  rstl::vector< CToken > assets;
  if (version <= 1u) {
    in.ReadInt32();
    in.ReadInt32();
    in.ReadInt32();
    return assets;
  }

  assets.reserve(in.ReadInt32());
  for (int i = 0; i < assets.capacity(); ++i) {
    const FourCC type = in.Get< uint >();
    const CAssetId id(in);
    CToken token = pool->GetObj(SObjectTag(type, id));
    token.Lock();
    assets.push_back(token);
  }
  return assets;
}

CGuiFrame::CGuiFrame(CInputStream& in, CSimplePool* pool)
: mVersion(ReadVersion(in))
, mAssets(LoadAssets(in, pool, mVersion))
, mRootWidget(nullptr)
, mCamera(nullptr)
, mLights(rstl::vector< CGuiLight* >(8, static_cast< CGuiLight* >(nullptr)))
, mModelDatabase(rs_new CGuiFrameModelDatabase(in, pool))
, mLoaded(false) {
  LoadWidgetsInGame(in, pool, mVersion);
}

CGuiFrame::~CGuiFrame() { delete mRootWidget; }

void CGuiFrame::LoadWidgetsInGame(CInputStream& in, CSimplePool* pool, uint version) {
  const int count = in.Get< int >();
  mWidgets.reserve(count);
  mWidgetIds.Reserve(count);

  int drawCount = 0;
  int updateCount = 0;
  int inputCount = 0;
  int preDrawCount = 0;
  for (int i = 0; i < count; ++i) {
    CGuiWidget* widget = FGuiWidgetFactoryInGame(in.Get< uint >(), this, in, pool, version);
    if (widget->GetWidgetTypeID() != 'CAMR' && widget->GetWidgetTypeID() != 'LITE') {
      mWidgets.push_back(widget);
      const CGuiWidget::EWidgetUsageFlags flags = widget->GetWidgetUsageFlags();
      if (flags & CGuiWidget::kWUF_Draw) {
        ++drawCount;
      }
      if (flags & CGuiWidget::kWUF_Update) {
        ++updateCount;
      }
      if (flags & CGuiWidget::kWUF_Input) {
        ++inputCount;
      }
      if (flags & CGuiWidget::kWUF_PreDraw) {
        ++preDrawCount;
      }
    }
  }

  mDrawWidgets.reserve(drawCount);
  mUpdateWidgets.reserve(updateCount);
  mInputWidgets.reserve(inputCount);
  mPreDrawWidgets.reserve(preDrawCount);
  for (rstl::vector< CGuiWidget* >::const_iterator it = mWidgets.begin(); it != mWidgets.end();
       ++it) {
    CGuiWidget* widget = *it;
    const CGuiWidget::EWidgetUsageFlags flags = widget->GetWidgetUsageFlags();
    if (flags & CGuiWidget::kWUF_Draw) {
      mDrawWidgets.push_back(widget);
    }
    if (flags & CGuiWidget::kWUF_Update) {
      mUpdateWidgets.push_back(widget);
    }
    if (flags & CGuiWidget::kWUF_Input) {
      mInputWidgets.push_back(widget);
    }
    if (flags & CGuiWidget::kWUF_PreDraw) {
      mPreDrawWidgets.push_back(widget);
    }
  }
  Initialize();
}

void CGuiFrame::Initialize() {
  SortDrawOrder();
  mRootWidget->RecalcWidgetColor(kTM_ChildrenAndSiblings);
  mRootWidget->DispatchInitialize();
}

void CGuiFrame::Draw(const CGuiWidgetDrawParms& parms) const {
  CGraphics::SetCullMode(kCM_None);
  CGraphics::ResetGfxStates();
  CGraphics::SetAmbientColor(CColor::White());
  DisableLights();
  mCamera->Draw(parms);
  CGraphics::SetTevOp(kTS_Stage0, CGraphics::kEnvModulate);
  CGraphics::SetBlendMode(kBM_Blend, kBF_SrcAlpha, kBF_InvSrcAlpha, kLO_Clear);
  ApplyLights();

  CGuiModel::BeginDraw();
  for (rstl::vector< CGuiWidget* >::const_iterator it = mPreDrawWidgets.begin();
       it != mPreDrawWidgets.end(); ++it) {
    CGuiWidget* widget = *it;
    if (widget->GetIsVisible()) {
      widget->Draw(parms);
    }
  }
  CGuiModel::EndDraw();
  DisableLights();

  for (rstl::vector< CGuiWidget* >::const_iterator it = mDrawWidgets.begin();
       it != mDrawWidgets.end(); ++it) {
    CGuiWidget* widget = *it;
    if (widget->GetIsVisible()) {
      widget->Draw(parms);
    }
  }
  CGraphics::SetCullMode(kCM_Front);
  CGraphics::SetDepthWriteMode(true, kE_LEqual, true);
}

void CGuiFrame::Update(float dt) {
  for (rstl::vector< CGuiWidget* >::iterator it = mUpdateWidgets.begin();
       it != mUpdateWidgets.end(); ++it) {
    (*it)->Update(dt);
  }

  mActiveLights.clear();
  mAmbientColor = CColor::Black();
  // The original compares against size() after clear(), not the fixed capacity.
  for (int i = 0; i < mLights.size() && i < mActiveLights.size(); ++i) {
    bool added = false;
    CGuiLight* light = mLights[i];
    if (light != nullptr && light->GetIsVisible()) {
      const CColor& color = light->GetModifiedColor();
      if (color.GetRedu8() != 0 || color.GetGreenu8() != 0 || color.GetBlueu8() != 0) {
        mActiveLights.push_back(light->BuildLight());
        added = true;
      }
      mAmbientColor = CColor::Add(mAmbientColor, light->GetAmbientContribution());
    }
    if (!added) {
      mActiveLights.push_back(CLight::BuildPoint(CVector3f::Zero(), CColor::Black()));
    }
  }
}

void CGuiFrame::ProcessUserInput(const CFinalInput& input) {
  rstl::list< CGuiWidget* > activeWidgets;
  for (rstl::vector< CGuiWidget* >::const_iterator it = mInputWidgets.begin();
       it != mInputWidgets.end(); ++it) {
    if ((*it)->GetIsActive()) {
      activeWidgets.push_back(*it);
    }
  }
  for (rstl::list< CGuiWidget* >::iterator it = activeWidgets.begin(); it != activeWidgets.end();
       ++it) {
    (*it)->ProcessUserInput(input);
  }
}

bool CGuiFrame::GetIsFinishedLoading() const {
  if (mLoaded) {
    return true;
  }
  for (rstl::vector< CToken >::const_iterator it = mAssets.begin(); it != mAssets.end(); ++it) {
    if (!it->IsLoaded()) {
      return false;
    }
  }
  mLoaded = true;
  return true;
}

void CGuiFrame::AddLight(CGuiLight* light) { mLights[light->GetLightIndex()] = light; }

void CGuiFrame::RemoveLight(CGuiLight* light) {
  if (mLights[light->GetLightIndex()] == light) {
    mLights[light->GetLightIndex()] = nullptr;
  }
}

void CGuiFrame::DisableLights() const { EnableLights(0); }

void CGuiFrame::ApplyLights() const {
  for (int i = 0; i < mActiveLights.size(); ++i) {
    CGraphics::LoadLight(static_cast< ERglLight >(i), mActiveLights[i]);
  }
}

void CGuiFrame::EnableLights(uint mask) const {
  uint enabled = 0;
  for (int i = 0; i < mLights.size(); ++i) {
    if ((mask & (1 << i)) != 0) {
      CGuiLight* light = mLights[i];
      if (light != nullptr && light->GetIsVisible()) {
        enabled |= 1 << i;
      }
    }
  }
  if (enabled == 0) {
    CGraphics::DisableAllLights();
    CGraphics::SetAmbientColor(CColor::White());
  } else {
    CGraphics::SetLightState(enabled);
    CGraphics::SetAmbientColor(mAmbientColor);
  }
}

void CGuiFrame::SortDrawOrder() {
  rstl::sort(mDrawWidgets.begin(), mDrawWidgets.end(), rstl::CWidgetFartherFromCamera());
  rstl::sort(mPreDrawWidgets.begin(), mPreDrawWidgets.end(), rstl::CWidgetFartherFromCamera());
}

CGuiWidget* CGuiFrame::FindWidget(const rstl::string& name) const {
  const short id = mWidgetIds.FindWidgetID(name);
  return id != -1 ? FindWidget(id) : nullptr;
}

CGuiWidget* CGuiFrame::FindWidget(short id) const {
  return mRootWidget != nullptr ? mRootWidget->FindWidget(id) : nullptr;
}

void CGuiFrame::SetHeadWidget(CGuiHeadWidget* widget) { mRootWidget = widget; }

void CGuiFrame::SetFrameCamera(CGuiCamera* camera) { mCamera = camera; }

CGuiWidget* CGuiFrame::FindWidget(const char* name) const {
  return FindWidget(rstl::string_l(name));
}

CGuiFrameLoader::CGuiFrameLoader(CAssetId asset, CResFactory& factory, CSimplePool& pool)
: mTag('FRME', asset)
, mPool(&pool)
, mBufferLength(factory.ResourceSize(mTag))
, mBuffer(static_cast< uchar* >(CMemory::Alloc(mBufferLength, IAllocator::kHI_RoundUpLen)))
, mRequest(
      factory.GetResLoader().LoadResourceAsync(mTag, reinterpret_cast< char* >(mBuffer.get()))) {}

CGuiFrameLoader::~CGuiFrameLoader() {}

bool CGuiFrameLoader::IsFinishedLoading() const { return mRequest->IsComplete(); }

CGuiFrame* CGuiFrameLoader::CreateFrame() {
  if (!mRequest->IsComplete()) {
    return nullptr;
  }
  CMemoryInStream in(mBuffer.get(), mBufferLength);
  return rs_new CGuiFrame(in, mPool);
}

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
struct __mwdec_vt_0 { virtual void _0(); virtual void _1(); virtual void _2(); virtual void _3(); virtual int _4(); };
struct __mwdec_vt_1 { virtual void _0(); virtual void _1(); virtual void _2(); virtual void _3(); virtual void _4(); virtual void _5(); virtual void _6(); virtual void _7(); virtual void _8(int); };
extern "C" void fn_80483064(unsigned char*, int);
extern "C" void fn_80482F78(int obj, int val);
extern "C" void fn_80482F78(int obj, int val) {
    unsigned int val2;
    unsigned char val4[12];
    unsigned char val5[12];
    *(int*)((char*)val5 + 0x4) = (int)val4;
    *(int*)((char*)val5 + 0x8) = (int)val4;
    *(int*)val4 = (int)val4;
    *(int*)((char*)val4 + 0x4) = (int)val4;
    *(int*)((char*)val4 + 0x8) = 0;
    for (int i = *(int*)((char*)obj + 0x8c); (unsigned int)i != (*(int*)((char*)obj + 0x8c) + (*(int*)((char*)obj + 0x84) << 2)); i += 4) {
        int val3 = *(int*)i;
        if ((unsigned char)((__mwdec_vt_0*)val3)->_4()) {
            fn_80483064(val5, val3);
        }
    }
    for (val2 = *(int*)((char*)val5 + 0x4); val2 != (*(int*)((char*)val5 + 0x8)); val2 = *(int*)((char*)val2 + 0x4)) {
        ((__mwdec_vt_1*)*(int*)((char*)val2 + 0x8))->_8(val);
    }
    ((rstl::list<CGuiWidget*, rstl::rmemory_allocator>*)val5)->~list();
}

extern "C" int fn_80483FC0();
extern "C" int fn_80483FC0() {
    return 0x42574947;
}

