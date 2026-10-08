#ifndef _CWORLDLIGHT
#define _CWORLDLIGHT

#include "Kyoto/Graphics/CLight.hpp"
#include "Kyoto/Math/CVector3f.hpp"
#include "rstl/construct.hpp"

class CInputStream;
class CWorldLight {
  static const CVector3f kDefaultPosition;
  static const CVector3f kDefaultDirection;
  static const CVector3f kDefaultUp;

public:
  enum EWorldLightType {
    kWLT_LocalAmbient,
    kWLT_Directional,
    kWLT_Custom,
    kWLT_Spot,
    kWLT_Spot2,
    kWLT_LocalAmbient2
  };

  explicit CWorldLight(CInputStream& in);
  CLight GetAsCGraphicsLight() const;
  const CVector3f& GetPosition() const { return mPosition; }
  bool DoesCastShadows() const { return mCastShadows; }

private:
  EWorldLightType mType;
  CColor mColor;
  CVector3f mPosition;
  CVector3f mDirection;
  CVector3f mUp;
  float mQ;
  float mCutoffAngle;
  float x34_;
  bool mCastShadows;
  float x3c_;
  EFalloffType mFalloff;
  float x44_;
  float mAlphaOffset;
  float mAlphaScale;
  uint x50_;
  float x54_;
  float x58_;
  uint x5c_;
};
CHECK_SIZEOF(CWorldLight, 0x60)

namespace rstl {
RSTL_DECLARE_TRIVIALLY_DESTRUCTIBLE(CWorldLight)
}

#endif // _CWORLDLIGHT
