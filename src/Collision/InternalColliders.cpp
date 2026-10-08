/*
 * G2MEAB Collision/InternalColliders.cpp translation-unit scaffold.
 * .text:
 * 0x80474888..0x80474A24 (2 native functions, including emitted helpers).
 * NonMatching:
 * implementation has not been reconstructed.
 * Boundary evidence: Two full registry routines:
 * AddColliders74888 installs AABox/Sphere and new
 * OrientedBox callbacks; AddTypes749D8 calls
 * their individually inspected GetType routines. Same
 * two-routine TU in both references, with
 * extra prototype registration preserved. Next74A24
 * constructs a ray and dispatches
 * CCollisionPrimitive virtual CastRayInternal.
 */
#include "Collision/InternalColliders.hpp"
#include "Collision/CCollisionPrimitive.hpp"

#include "Collision/CCollidableAABox.hpp"
#include "Collision/CCollidableCollisionSurface.hpp"
#include "Collision/CCollidableSphere.hpp"

void InternalColliders::AddTypes() {
  CCollisionPrimitive::InitAddType(CCollidableAABox::GetType());
  CCollisionPrimitive::InitAddType(CCollidableCollisionSurface::GetType());
  CCollisionPrimitive::InitAddType(CCollidableSphere::GetType());
}

void InternalColliders::AddColliders() {
  CCollisionPrimitive::InitAddCollider(Collide::AABox_AABox, "CCollidableAABox",
                                       "CCollidableAABox");
  CCollisionPrimitive::InitAddCollider(Collide::Sphere_AABox, "CCollidableSphere",
                                       "CCollidableAABox");
  CCollisionPrimitive::InitAddCollider(Collide::Sphere_Sphere, "CCollidableSphere",
                                       "CCollidableSphere");
  CCollisionPrimitive::InitAddBooleanCollider(Collide::AABox_AABox_Bool, "CCollidableAABox",
                                              "CCollidableAABox");
  CCollisionPrimitive::InitAddBooleanCollider(Collide::Sphere_AABox_Bool, "CCollidableSphere",
                                              "CCollidableAABox");
  CCollisionPrimitive::InitAddBooleanCollider(Collide::Sphere_Sphere_Bool, "CCollidableSphere",
                                              "CCollidableSphere");
  CCollisionPrimitive::InitAddMovingCollider(CCollidableAABox::CollideMovingAABox,
                                             "CCollidableAABox", "CCollidableAABox");
  CCollisionPrimitive::InitAddMovingCollider(CCollidableAABox::CollideMovingSphere,
                                             "CCollidableAABox", "CCollidableSphere");
  CCollisionPrimitive::InitAddMovingCollider(CCollidableSphere::CollideMovingAABox,
                                             "CCollidableSphere", "CCollidableAABox");
  CCollisionPrimitive::InitAddMovingCollider(CCollidableSphere::CollideMovingSphere,
                                             "CCollidableSphere", "CCollidableSphere");
}
