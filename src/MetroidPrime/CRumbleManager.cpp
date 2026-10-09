// NonMatching translation-unit scaffold.
// G2MEAB .text 0x80189EF4..0x8018A0F0 (end exclusive).
// Echoes' CRumbleManager.cpp without the player index or the destructor. Listed below are the
// functions not implemented yet.
// 0x8018A038 +0x50: Rumble(CStateManager&, ERumbleFxId, float, ERumblePriority); needs the rumble
//   option (bit 0x20 of CGameOptions' byte at 0x44), which CGameOptions.hpp does not model yet
// 0x8018A0C0 +0x30: static initializer for TGameTypes.hpp's seven SDA constants

#include "MetroidPrime/CRumbleManager.hpp"

#include "MetroidPrime/CStateManager.hpp"
#include "MetroidPrime/CStateManagerObject.hpp"
#include "MetroidPrime/Player/CPlayer.hpp"

#include "Kyoto/Math/CloseEnough.hpp"

CRumbleManager::CRumbleManager(EIOPort port) : mPort(port), mRumbleGenerator(port) {}

// Unlike Echoes, the distance is measured to the one player.
short CRumbleManager::Rumble(CStateManager& mgr, const CVector3f& pos, ERumbleFxId fx, float dist,
                             ERumblePriority priority) {
  if (!close_enough(dist, 0.f)) {
    CVector3f delta = mgr.ObjectManager().GetPlayer()->GetTranslation() - pos;
    if (delta.MagSquared() < dist * dist) {
      return Rumble(mgr, fx, 1.f - delta.Magnitude() / dist, priority);
    }
  }
  return -1;
}

void CRumbleManager::StopRumble(short id) {
  if (id == -1) {
    return;
  }
  mRumbleGenerator.Stop(id);
}

void CRumbleManager::Update(float dt) { mRumbleGenerator.Update(dt); }
