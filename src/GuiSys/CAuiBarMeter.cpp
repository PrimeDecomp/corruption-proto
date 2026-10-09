/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x8048E860..0x8048F1CC.
 */
#include "GuiSys/CAuiBarMeter.hpp"

#include "GuiSys/CGuiWidgetDrawParms.hpp"
#include "Kyoto/Graphics/CGX.hpp"
#include "Kyoto/Graphics/CGraphics.hpp"
#include "Kyoto/Graphics/CTexture.hpp"
#include "Kyoto/Math/CloseEnough.hpp"
#include "Kyoto/Streams/CInputStream.hpp"
#include "dolphin/gx/GXVert.h"
#include "rstl/math.hpp"


CAuiBarMeter::CAuiBarMeter(const CGuiWidgetParms& parms, CSimplePool* pool,
                                 CAssetId textureId,
                                 const rstl::reserved_vector< CVector3f, 4 >& coords,
                                 const rstl::reserved_vector< CVector2f, 4 >& uvs, bool loadTexture)
: CGuiWidget(parms)
, mCoords(coords)
, mUvs(uvs)
, mTextureId(textureId)
, mShadowColor(CColor::White())
, mTargetFraction(0.f)
, mCurrentFraction(0.f)
, mShadowFraction(0.f)
, mIncreaseSpeed(1.f)
, mDecreaseSpeed(1.f)
, mShadowDrainSpeed(0.75f) {
  if (loadTexture) {
    mTexture = TCachedToken< CTexture >(pool->GetObj(SObjectTag('TXTR', mTextureId)));
    mTexture->Lock();
  }
}

CAuiBarMeter::~CAuiBarMeter() {}

CGuiWidget* CAuiBarMeter::Create(CGuiFrame* frame, CInputStream& in, CSimplePool* pool,
                                    uint version) {
  CGuiWidgetParms parms = ReadWidgetHeader(frame, in);
  rstl::reserved_vector< CVector3f, 4 > coords(in);
  rstl::reserved_vector< CVector2f, 4 > uvs(in);
  CAssetId textureId(in);
  CAuiBarMeter* widget = NEW CAuiBarMeter(parms, pool, textureId, coords, uvs, true);
  widget->ParseBaseInfo(frame, in, parms, version);
  return widget;
}

void CAuiBarMeter::SetTargetFraction(float fraction) { mTargetFraction = fraction; }

void CAuiBarMeter::SetCurrentFraction(float fraction) { mCurrentFraction = fraction; }

float CAuiBarMeter::GetCurrentFraction() const { return mCurrentFraction; }

float CAuiBarMeter::GetShadowFraction() const { return mShadowFraction; }

void CAuiBarMeter::SetShadowColor(const CColor& color) { mShadowColor = color; }

void CAuiBarMeter::SetIncreaseSpeed(float speed) { mIncreaseSpeed = speed; }

void CAuiBarMeter::SetDecreaseSpeed(float speed) { mDecreaseSpeed = speed; }

void CAuiBarMeter::Update(float dt) {
  if (mTexture) {
    mTexture->TryCache();
  }

  if (mTargetFraction < mCurrentFraction) {
    mCurrentFraction = rstl::max_val(mTargetFraction, mCurrentFraction - mDecreaseSpeed * dt);
  } else {
    mCurrentFraction = rstl::min_val(mTargetFraction, mCurrentFraction + mIncreaseSpeed * dt);
  }

  if (mCurrentFraction < mShadowFraction) {
    mShadowFraction = rstl::max_val(mCurrentFraction, mShadowFraction - mShadowDrainSpeed * dt);
  } else {
    mShadowFraction = mCurrentFraction;
  }

  CGuiWidget::Update(dt);
}

void CAuiBarMeter::Draw(const CGuiWidgetDrawParms& parms) const {
  if (!GetIsVisible() || !mTexture || !mTexture->IsLoaded()) {
    return;
  }
  if (close_enough(mCurrentFraction, 0.f) && close_enough(mShadowFraction, 0.f)) {
    return;
  }
  const CTexture* texture = mTexture->GetObject();
  if (!texture) {
    return;
  }

  CGraphics::SetDepthWriteMode(true, kE_LEqual, false);
  CGraphics::SetLightingMode(0);
  texture->Load(GX_TEXMAP0, CTexture::kCM_Repeat);
  switch (mDrawFlags) {
  case kGMDF_Shadeless:
  case kGMDF_Opaque:
    CGraphics::SetBlendMode(kBM_Blend, kBF_One, kBF_Zero, kLO_Clear);
    break;
  case kGMDF_Alpha:
    CGraphics::SetBlendMode(kBM_Blend, kBF_SrcAlpha, kBF_InvSrcAlpha, kLO_Clear);
    break;
  case kGMDF_Additive:
    CGraphics::SetBlendMode(kBM_Blend, kBF_SrcAlpha, kBF_One, kLO_Clear);
    break;
  default:
    break;
  }

  const CColor color = GetModifiedColor();
  const CColor currentColor =
      CColor::Modulate(color, GetColor().WithAlphaModulatedBy(parms.GetAlpha()));
  const CColor shadowColor =
      CColor::Modulate(color, mShadowColor.WithAlphaModulatedBy(parms.GetAlpha()));
  CGraphics::SetModelMatrix(GetWorldTransform());
  CGX::SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
  CGX::SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_ZERO);
  CGX::SetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
  CGX::SetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
  CGX::SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
  CGX::SetNumTevStages(1);
  CGX::SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, false, GX_PTIDENTITY);
  CGX::SetNumTexGens(1);
  CGX::SetNumChans(0);
  static const GXVtxDescList desc[] = {{GX_VA_POS, GX_DIRECT}, {GX_VA_TEX0, GX_DIRECT}, {GX_VA_NULL, GX_NONE}};
  CGX::SetVtxDescv(desc);

  for (int bar = 0; bar < 2; ++bar) {
    if (bar == 0) {
      CGX::SetTevKColor(GX_KCOLOR0, shadowColor.GetGXColor());
    } else {
      CGX::SetTevKColor(GX_KCOLOR0, currentColor.GetGXColor());
    }
    CGX::Begin(GX_TRIANGLESTRIP, GX_VTXFMT0, 4);
    float fraction;
    if (bar == 0) {
      fraction = mShadowFraction;
    } else {
      fraction = mCurrentFraction;
    }
    const float remainder = 1.f - fraction;
    {
      const CVector2f uv(mUvs[0].GetX() * fraction + mUvs[1].GetX() * remainder,
                         mUvs[0].GetY() * fraction + mUvs[1].GetY() * remainder);
      const CVector3f pos(mCoords[0].GetX() * fraction + mCoords[1].GetX() * remainder,
                          mCoords[0].GetY() * fraction + mCoords[1].GetY() * remainder,
                          mCoords[0].GetZ() * fraction + mCoords[1].GetZ() * remainder);
      GXPosition3f32(pos.GetX(), pos.GetY(), pos.GetZ());
      GXTexCoord2f32(uv.GetX(), uv.GetY());
    }
    GXPosition3f32(mCoords[1].GetX(), mCoords[1].GetY(), mCoords[1].GetZ());
    GXTexCoord2f32(mUvs[1].GetX(), mUvs[1].GetY());
    {
      const CVector2f uv(mUvs[2].GetX() * fraction + mUvs[3].GetX() * remainder,
                         mUvs[2].GetY() * fraction + mUvs[3].GetY() * remainder);
      const CVector3f pos(mCoords[2].GetX() * fraction + mCoords[3].GetX() * remainder,
                          mCoords[2].GetY() * fraction + mCoords[3].GetY() * remainder,
                          mCoords[2].GetZ() * fraction + mCoords[3].GetZ() * remainder);
      GXPosition3f32(pos.GetX(), pos.GetY(), pos.GetZ());
      GXTexCoord2f32(uv.GetX(), uv.GetY());
    }
    GXPosition3f32(mCoords[3].GetX(), mCoords[3].GetY(), mCoords[3].GetZ());
    GXTexCoord2f32(mUvs[3].GetX(), mUvs[3].GetY());
    CGX::End();
  }

  CGraphics::SetBlendMode(kBM_Blend, kBF_SrcAlpha, kBF_InvSrcAlpha, kLO_Clear);
  CGraphics::SetDepthWriteMode(true, kE_LEqual, true);
}

CGuiWidget::EWidgetUsageFlags CAuiBarMeter::GetWidgetUsageFlags() const {
  return static_cast< EWidgetUsageFlags >(kWUF_Draw | kWUF_Update);
}

FourCC CAuiBarMeter::GetWidgetTypeID() const { return 'BMTR'; }
