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
  float GetGrappleDistance() const;
  float GetGrappleBeamLength() const;
  float GetGrappleSwingTime() const;
  float GetGrappleMaxVelocity() const;
  float GetGrapplePullCloseDistance() const;
  float GetGrapplePullDampenDistance() const;
  float GetGrapplePullVelocity() const;
  float GetGrappleTurnRate() const;
  float GetGrappleJumpForce() const;
  int GetGrappleControlScheme() const;
  bool GetGrappleHoldOrbitButton() const;
  bool GetGrappleTurnControlsReversed() const;
  int GetHyperModeType() const;

  void SetNormalTurnFactor(float value);
  void SetFreeLookTurnFactor(float value);
  void SetRevFreeLookGun(bool value);
  void SetRevOrbitLockGun(bool value);
  void SetRevOrbitTagObjects(bool value);
  void SetRevLockCursor(bool value);
  void SetOrbitDash(bool value);
  void SetOrbitDashUsesTap(bool value);
  void SetOrbitDashTapTime(float value);
  void SetOrbitDashStickThreshold(float value);
  void SetOrbitDashDoubleJumpImpulse(float value);
  void SetOrbitDashVerticalDoubleJumpAccel(float value);
  void SetOrbitDashHorizontalDoubleJumpAccel(float value);
  void SetScanFreezesGame(bool value);
  void SetScanLineOfSight(bool value);
  void SetShieldAllowsMotion(bool value);
  void SetGrappleDistance(float value);
  void SetGrappleBeamLength(float value);
  void SetGrappleSwingTime(float value);
  void SetGrappleMaxVelocity(float value);
  void SetGrapplePullCloseDistance(float value);
  void SetGrapplePullDampenDistance(float value);
  void SetGrapplePullVelocity(float value);
  void SetGrappleTurnRate(float value);
  void SetGrappleJumpForce(float value);
  void SetGrappleControlScheme(int value);
  void SetGrappleHoldOrbitButton(bool value);
  void SetGrappleTurnControlsReversed(bool value);
  void SetHyperModeType(int value);

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
  void SetHyperModeInvulnerablePhazonLoss(int type, bool value);
  void SetHyperModeInvulnerableTime(int type, float value);
  void SetHyperModeCorruptionTime(int type, float value);
  void SetHyperModeConstantCorruptionRate(int type, bool value);
  void SetHyperModeCorruptionRate(int type, float value);
  void SetHyperModePhazonLevel(int type, float value);
  void SetHyperModePhazonCapacity(int type, float value);
  void SetHyperModeDangerPercentage(int type, float value);
  void SetHyperModeBeamLossAmount(int type, float value);
  void SetHyperModeMissileLossAmount(int type, float value);
  void SetHyperModePhazonBallRate(int type, float value);
  void SetHyperModeDamageMultiplier(int type, float value);

private:
  const SLdrTweakPlayer* mData;
};
CHECK_SIZEOF(CTweakPlayer, 0x4)

extern rstl::single_ptr< CTweakPlayer > gpTweakPlayer;

#endif // _CTWEAKPLAYER
