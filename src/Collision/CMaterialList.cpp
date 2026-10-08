/*
 * G2MEAB Collision/CMaterialList.cpp
 * .text: 0x80476108..0x804762A8
 * Debug name helpers for material masks plus the stream constructor.
 */
#include "Collision/CMaterialList.hpp"

#include "Kyoto/Streams/CInputStream.hpp"
#include "rstl/string.hpp"

static const char* const sMaterialNames[64] = {
    "Unknown",
    "Stone",
    "Metal",
    "Grass",
    "Ice",
    "Pillar",
    "MetalGrating",
    "Phazon",
    "Dirt",
    "SP_Metal",
    "Glass",
    "Snow",
    "Fabric",
    "Halfpipe",
    "Unused3",
    "Unused4",
    "Shield",
    "Sand",
    "Seed_Organics",
    "Web",
    "ShootThru",
    "CameraThru",
    "Wood",
    "Organic",
    "RedundantEdge/FlippedTri",
    "Rubber",
    "SeeThru",
    "ScanThru",
    "AiWalkThru",
    "Ceiling",
    "Wall",
    "Floor",
    "Player",
    "Character",
    "Trigger",
    "Projectile",
    "Bomb",
    "Ground Collider",
    "NoStaticWorldCollision",
    "Scannable",
    "Target",
    "Orbit",
    "Occluder",
    "Immovable",
    "Debris",
    "PowerBomb",
    "TargetableProjectile",
    "CollisionOnlyActor",
    "AiBlock",
    "Platform",
    "NonSolidDamageable",
    "ShowOnRadar",
    "PlatformSlave",
    "NoIceSpread",
    "GrappleThrough",
    "CanJumpOnCharacter",
    "ExcludeFromLineOfSightTest",
    "DontShowOnRadar",
    "JumpNotAllowed",
    "Solid",
    "Complex",
    "SpiderBall",
    "ScrewAttackWallJump",
    "Seek",
};

// Guessed names (target-derived): look up the debug name of one material bit.
rstl::string CMaterialList::GetMaterialName(const EMaterialTypes& material) {
  int index = material;
  if (index >= 0 && index <= 63) {
    return rstl::string_l(sMaterialNames[index]);
  }
  return rstl::string_l("Invalid");
}

// Guessed name (target-derived): names of the materials set in both lists.
rstl::string CMaterialList::GetSharedMaterialString(const CMaterialList& other) const {
  rstl::string result;
  u64 shared = mValue & other.mValue;
  for (u64 i = 0; i < 64; ++i) {
    EMaterialTypes material = EMaterialTypes(i);
    if ((shared & (u64(1) << i)) != 0) {
      result.append(GetMaterialName(material) + " ");
    }
  }
  return result;
}

CMaterialList::CMaterialList(CInputStream& in) : mValue(in.ReadInt64()) {}

int CMaterialList::BitPosition(u64 flags) {
  for (int ret = 0, i = 0; i < 32; ++i) {
    if ((flags & 1) != 0) {
      return ret;
    }
    flags >>= 1;
    ++ret;
  }
  return -1;
}
