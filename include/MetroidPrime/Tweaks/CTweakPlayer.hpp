#ifndef _CTWEAKPLAYER
#define _CTWEAKPLAYER

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakPlayer;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakPlayer {
public:
  explicit CTweakPlayer(const SLdrTweakPlayer& data) : mData(&data) {}

  // Echoes names; out of line in TweaksAccessors.cpp (0x8024AE58 and 0x8024AE4C).
  float GetLeftAnalogMax() const;
  float GetRightAnalogMax() const;

  // Named after the SLdrTweakPlayer fields they return. Defined in TweaksAccessors.cpp.
  float GetNormalTurnFactor() const;
  float GetFreeLookTurnFactor() const;
  bool GetRevFreeLookGun() const;
  bool GetRevOrbitLockGun() const;
  bool GetRevOrbitTagObjects() const;
  bool GetRevLockCursor() const;
  bool GetOrbitDash() const;
  bool GetOrbitDashUsesTap() const;
  float GetOrbitDashTapTime() const;
  float GetOrbitDashStickThreshold() const;
  float GetOrbitDashDoubleJumpImpulse() const;
  float GetOrbitDashVerticalDoubleJumpAccel() const;
  float GetOrbitDashHorizontalDoubleJumpAccel() const;
  bool GetScanFreezesGame() const;
  bool GetScanLineOfSight() const;
  bool GetShieldAllowsMotion() const;

  // The hyper mode tuning of a hyper mode type: 0 reads hyperModeTimer, anything else
  // hyperModePhazonLevel.
  bool GetHyperModeInvulnerablePhazonLoss(int type) const;
  float GetHyperModeInvulnerableTime(int type) const;
  float GetHyperModeCorruptionTime(int type) const;
  bool GetHyperModeConstantCorruptionRate(int type) const;
  float GetHyperModeCorruptionRate(int type) const;
  float GetHyperModePhazonLevel(int type) const;
  float GetHyperModePhazonCapacity(int type) const;
  float GetHyperModeDangerPercentage(int type) const;
  float GetHyperModeBeamLossAmount(int type) const;
  float GetHyperModeMissileLossAmount(int type) const;
  float GetHyperModePhazonBallRate(int type) const;
  float GetHyperModeDamageMultiplier(int type) const;

private:
  const SLdrTweakPlayer* mData;
};
CHECK_SIZEOF(CTweakPlayer, 0x4)

extern rstl::single_ptr< CTweakPlayer > gpTweakPlayer;

#endif // _CTWEAKPLAYER
