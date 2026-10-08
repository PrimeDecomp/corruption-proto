#ifndef _CACTOR
#define _CACTOR

#include "MetroidPrime/CEntity.hpp"

#include "Collision/CMaterialList.hpp"
#include "Kyoto/Math/CTransform4f.hpp"
#include "Kyoto/Math/CVector3f.hpp"

class CVisorParameters;

// Layout from the constructor (0x80036BAC) and destructor (0x80036AE0). The vtable
// (lbl_806B22A8) adds 24 virtuals after CEntity's; they are not declared yet.
class CActor : public CEntity {
public:
  CActor(TUniqueId uid, const rstl::string& name, const CEntityInfo& info, uint castFlags,
         const CTransform4f& xf, const CMaterialList& materialList,
         const CVisorParameters& visorParams);

  ~CActor();
  CEntity* TypesMatch(int typeId) const;
  void Think(float dt, CStateManager& mgr);
  void AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg);
  void SetActive(bool active);

  const CTransform4f& GetTransform() const { return mTransform; }

private:
  CTransform4f mTransform;
  CVector3f x8c_; // Copy of the transform's translation.
  CMaterialList mMaterialList;
  u64 xa0_;
  int xa8_;
  int xac_;
  int xb0_;
  int xb4_;
  uint xb8_; // First word of the visor parameters.
  int xbc_;
  uchar xc0_[0x10];
  int xd0_;
  uchar xd4_[0x10];
  uint xe4_;
  // Two owned polymorphic objects (bool + pointer), deleted through their vtables.
  uchar xe8_[0x8];
  uchar xf0_[0x8];
};
CHECK_SIZEOF(CActor, 0xF8)

#endif // _CACTOR
