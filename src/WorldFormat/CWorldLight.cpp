/*
 * G2MEAB WorldFormat/CWorldLight.cpp translation-unit scaffold.
 * .text: 0x805A6358..0x805A687C (3 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Three natives: GetAsCGraphicsLight5A6358 switches on stream-loaded light type
 * and calls light factories/setters; streamctor5A6714 reads type/color/position/direction and
 * prototype extra scalars; initializer5A6840+3C initializes three default vectors used by the
 * family. Both reference source/header and full three-native inventories consulted; prototype
 * initializer is3C rather than broad candidate2C, and constructor is larger. Prior5A6134 is
 * individually inspected locked-allocator helper called by collider reserve, so not absorbed.
 * Next5A687C is COBBTree allocator Alloc.
 */

#include "WorldFormat/CWorldLight.hpp"

#include "Kyoto/Streams/CInputStream.hpp"
#include "rstl/math.hpp"

CLight CWorldLight::GetAsCGraphicsLight() const {
  float q = mQ;
  if (mQ < FLT_EPSILON) {
    q = 10.f * FLT_EPSILON;
  }

  float r = q * mColor.GetRed();
  float g = q * mColor.GetGreen();
  float b = q * mColor.GetBlue();
  float maxColor = rstl::max_val(r, rstl::max_val(g, b));
  float intensity = (maxColor * q - mAlphaOffset) * mAlphaScale;
  float alpha = rstl::min_val(1.f, rstl::max_val(0.f, intensity));
  CColor color = mColor.WithAlphaOf(alpha);

  if (mType == kWLT_LocalAmbient || mType == kWLT_LocalAmbient2) {
    CColor ambientColor(rstl::min_val(1.f, r), rstl::min_val(1.f, g), rstl::min_val(1.f, b),
                        color.GetAlpha());
    CLight light = CLight::BuildLocalAmbient(mPosition, ambientColor);
    light.SetColorProcessingMode(true);
    return light;
  }

  if (mType == kWLT_Directional) {
    CLight light = CLight::BuildDirectional(mDirection, color);
    light.SetColorProcessingMode(true);
    return light;
  }

  if (mType == kWLT_Spot) {
    CLight light =
        CLight::BuildSpot(mPosition, mDirection.AsNormalized(), color, mCutoffAngle * 0.5f);
    float quadratic = mFalloff == kFT_Quadratic ? 25000.f / q : 0.f;
    float linear = mFalloff == kFT_Linear ? (1.f / 0.004f) / q : 0.f;
    float constant = mFalloff == kFT_Constant ? 2.f / q : 0.f;
    light.SetAttenuation(constant, linear, quadratic);
    light.SetColorProcessingMode(true);
    return light;
  }

  if (mType == kWLT_Spot2) {
    CLight light = CLight::BuildSpot(mPosition, mDirection.AsNormalized(), color, 180.f);
    float quadratic = mFalloff == kFT_Quadratic ? 25000.f / q : 0.f;
    float linear = mFalloff == kFT_Linear ? (1.f / 0.004f) / q : 0.f;
    float constant = mFalloff == kFT_Constant ? 2.f / q : 0.f;
    light.SetAttenuation(constant, linear, quadratic);
    light.SetColorProcessingMode(true);
    return light;
  }

  float quadratic = mFalloff == kFT_Quadratic ? 25000.f / q : 0.f;
  float linear = mFalloff == kFT_Linear ? (1.f / 0.004f) / q : 0.f;
  float constant = mFalloff == kFT_Constant ? 2.f / q : 0.f;
  CLight light = CLight::BuildCustom(mPosition, CVector3f(1.f, 0.f, 0.f), color, constant, linear,
                                     quadratic, 1.f, 0.f, 0.f);
  light.SetColorProcessingMode(true);
  return light;
}

CWorldLight::CWorldLight(CInputStream& in)
: mType(static_cast< EWorldLightType >(in.Get< uint >()))
, mColor(in)
, mPosition(in)
, mDirection(in)
, mUp(in)
, mQ(in.Get< float >())
, mCutoffAngle(in.Get< float >())
, x34_(in.Get< float >())
, mCastShadows(in.Get< bool >())
, x3c_(in.Get< float >())
, mFalloff(static_cast< EFalloffType >(in.Get< uint >()))
, x44_(in.Get< float >())
, mAlphaOffset(in.Get< float >())
, mAlphaScale(in.Get< float >())
, x54_(in.Get< float >())
, x58_(in.Get< float >())
, x5c_(in.Get< uint >()) {}

const CVector3f CWorldLight::kDefaultPosition(0.f, 0.f, 0.f);
const CVector3f CWorldLight::kDefaultDirection(0.f, 1.f, 0.f);
const CVector3f CWorldLight::kDefaultUp(0.f, 1.f, 0.f);
