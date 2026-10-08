/*
 * G2MEAB Collision/InternalColliders.cpp
 * .text: 0x80474888..0x80474A24
 * AddColliders installs the AABox/Sphere/OrientedBox collision callbacks, AddTypes registers the
 * primitive types.
 */
#include "Collision/InternalColliders.hpp"
#include "Collision/CCollisionPrimitive.hpp"

#include "Collision/CCollidableAABox.hpp"
#include "Collision/CCollidableOrientedBox.hpp"
#include "Collision/CCollidableSphere.hpp"

void InternalColliders::AddTypes() {
  CCollisionPrimitive::InitAddType(CCollidableAABox::GetType());
  CCollisionPrimitive::InitAddType(CCollidableSphere::GetType());
  CCollisionPrimitive::InitAddType(CCollidableOrientedBox::GetType());
}

void InternalColliders::AddColliders() {
  CCollisionPrimitive::InitAddCollider(Collide::AABox_AABox, "CCollidableAABox",
                                       "CCollidableAABox");
  CCollisionPrimitive::InitAddCollider(Collide::Sphere_AABox, "CCollidableSphere",
                                       "CCollidableAABox");
  CCollisionPrimitive::InitAddCollider(Collide::Sphere_Sphere, "CCollidableSphere",
                                       "CCollidableSphere");
  CCollisionPrimitive::InitAddCollider(Collide::OBBox_OBBox, "CCollidableOrientedBox",
                                       "CCollidableOrientedBox");
  CCollisionPrimitive::InitAddBooleanCollider(Collide::AABox_AABox_Bool, "CCollidableAABox",
                                              "CCollidableAABox");
  CCollisionPrimitive::InitAddBooleanCollider(Collide::Sphere_AABox_Bool, "CCollidableSphere",
                                              "CCollidableAABox");
  CCollisionPrimitive::InitAddBooleanCollider(Collide::Sphere_Sphere_Bool, "CCollidableSphere",
                                              "CCollidableSphere");
  CCollisionPrimitive::InitAddBooleanCollider(Collide::OBBox_OBBox_Bool, "CCollidableOrientedBox",
                                              "CCollidableOrientedBox");
  CCollisionPrimitive::InitAddMovingCollider(CCollidableAABox::CollideMovingAABox,
                                             "CCollidableAABox", "CCollidableAABox");
  CCollisionPrimitive::InitAddMovingCollider(CCollidableAABox::CollideMovingSphere,
                                             "CCollidableAABox", "CCollidableSphere");
  CCollisionPrimitive::InitAddMovingCollider(CCollidableSphere::CollideMovingAABox,
                                             "CCollidableSphere", "CCollidableAABox");
  CCollisionPrimitive::InitAddMovingCollider(CCollidableSphere::CollideMovingSphere,
                                             "CCollidableSphere", "CCollidableSphere");
}
