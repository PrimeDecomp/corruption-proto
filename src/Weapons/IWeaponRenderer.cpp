/*
 * G2MEAB Weapons/IWeaponRenderer.cpp translation-unit scaffold.
 * .text: 0x805971B8..0x80597290, end exclusive; 3 native functions.
 * Complete source/native inventories compared in both reference games.
 * NonMatching: implementation and declarations remain to be reconstructed.
 */

#include "Weapons/IWeaponRenderer.hpp"

#include "Kyoto/Particles/CParticleGen.hpp"

class CDefaultWeaponRenderer : public IWeaponRenderer {
public:
  ~CDefaultWeaponRenderer() {}
  void AddParticleGen(const CParticleGen& gen) { const_cast< CParticleGen& >(gen).Render(); }
};

static CDefaultWeaponRenderer sDefaultRenderer;
IWeaponRenderer* IWeaponRenderer::sWeaponRenderer = &sDefaultRenderer;
