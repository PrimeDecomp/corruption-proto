#ifndef _ACTORCOMMON
#define _ACTORCOMMON

// Responses are named after the 4CC tags of the CRSM tables (particle/sound/decal).
enum EWeaponCollisionResponseTypes {
  kWCR_None,         // NODP
  kWCR_Default,      // DEFS
  kWCR_Crate,        // CRTS
  kWCR_Metal,        // MTLS
  kWCR_Grass,        // GRAS
  kWCR_Glass,        // GLAS
  kWCR_Ice,          // ICEE
  kWCR_Snow,         // SNEE
  kWCR_Goo,          // GOOO
  kWCR_Wood,         // WODS
  kWCR_Water,        // WATR
  kWCR_Mud,          // 1MUD
  kWCR_Wood2,        // 1WOD
  kWCR_Lava,         // 1LAV
  kWCR_Sand,         // 1SAN
  kWCR_Projectile,   // 1PRJ
  kWCR_OtherProjectile, // DCHR (Prime name)
  kWCR_Shield,       // CSHD (guessed from the tag)
  kWCR_EnemyNormal,  // DENM
  kWCR_EnemySpecial, // DSNM
  kWCR_Count,
};
enum EProjectileAttrib {
  kPA_None = 0,
  kPA_PartialCharge = (1 << 0),
  kPA_PlasmaProjectile = (1 << 1),
  kPA_Charged = (1 << 2),
  kPA_Ice = (1 << 3),
  kPA_Wave = (1 << 4),
  kPA_Plasma = (1 << 5),
  kPA_Phazon = (1 << 6),
  kPA_ComboShot = (1 << 7),
  kPA_Bombs = (1 << 8),
  kPA_PowerBombs = (1 << 9),
  kPA_BigProjectile = (1 << 10),
  kPA_ArmCannon = (1 << 11),
  kPA_BigStrike = (1 << 12),
  kPA_DamageFalloff = (1 << 13),
  kPA_StaticInterference = (1 << 14),
  kPA_PlayerUnFreeze = (1 << 15),
  kPA_ParticleOPTS = (1 << 16),
  kPA_KeepInCinematic = (1 << 17),
};
enum EUserEventType {
  kUE_Projectile = 0,
  kUE_EggLay = 1,
  kUE_LoopedSoundStop = 2,
  kUE_AlignTargetPos = 3,
  kUE_AlignTargetRot = 4,
  kUE_ChangeMaterial = 5,
  kUE_Delete = 6,
  kUE_GenerateEnd = 7,
  kUE_DamageOn = 8,
  kUE_DamageOff = 9,
  kUE_AlignTargetPosStart = 10,
  kUE_DeGenerate = 11,
  kUE_Landing = 12,
  kUE_TakeOff = 13,
  kUE_FadeIn = 14,
  kUE_FadeOut = 15,
  kUE_ScreenShake = 16,
  kUE_BeginAction = 17,
  kUE_EndAction = 18,
  kUE_BecomeRagDoll = 19,
  kUE_IkLock = 20,
  kUE_IkRelease = 21,
  kUE_BreakLockOn = 22,
  kUE_BecomeShootThrough = 23,
  kUE_RemoveCollision = 24,
  kUE_ObjectPickUp = 25,
  kUE_ObjectDrop = 26,
  kUE_EventStart = 27,
  kUE_EventStop = 28,
  kUE_Activate = 29,
  kUE_Deactivate = 30,
  kUE_SoundPlay = 31,
  kUE_SoundStop = 32,
  kUE_EffectOn = 33,
  kUE_EffectOff = 34,
};

#endif // _ACTORCOMMON
