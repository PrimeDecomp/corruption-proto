#ifndef _CRENDERACTOR
#define _CRENDERACTOR

#include "types.h"

#include "MetroidPrime/CActor.hpp"

class CActorParameters;
class CModelData;

// Minimal declaration of the actor that renders a model (CRenderActor.cpp). Its constructor
// (0x8029F944) takes Echoes' CActor arguments: the prototype split Echoes' CActor into CActor
// (transform, materials, fluids) and this class, which builds the model from the CModelData and
// keeps the rest of CActorParameters. It adds cast flag 2; CPhysicsActor derives from it.
// Vtable 0x806BAB8C; the layout past CActor (0xF8..0x170) is not modelled.
class CRenderActor : public CActor {
public:
  CRenderActor(TUniqueId uid, const rstl::string& name, const CEntityInfo& info, uint castFlags,
               const CTransform4f& xf, const CModelData& mData, const CMaterialList& matList,
               const CActorParameters& params, TUniqueId nextDrawNode);

  // CEntity
  ~CRenderActor();                       // 0x8029F840
  CEntity* TypesMatch(int typeId) const; // Emitted in TypesMatch.cpp.
  void Think(float dt, CStateManager& mgr);
  void AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg);
  void SetActive(bool active);

  // CActor
  void SetTransformDirty();
  void DrawCollisionBoxes(const CStateManager& mgr); // 0x8029D2A8
  void UpdateCollisionBoxDrawing(CStateManager& mgr);

  // CRenderActor. Placeholders (NN = slot offset) for the slots it adds.
  virtual void Virtual80();
  virtual void Virtual84();
  virtual void Virtual88();
  virtual void Virtual8C();
  virtual void Virtual90();
  virtual void Virtual94();
  virtual void Virtual98();
  virtual void Virtual9C();

private:
  uchar xF8_[0x170 - 0xF8];
};
CHECK_SIZEOF(CRenderActor, 0x170)

#endif // _CRENDERACTOR
