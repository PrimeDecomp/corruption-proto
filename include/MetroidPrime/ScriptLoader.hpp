#ifndef _SCRIPTLOADER
#define _SCRIPTLOADER

#include "Kyoto/SObjectTag.hpp"

class CEntity;
class CEntityInfo;
class CInputStream;
class CStateManager;

// Guessed name. G2MEAB passes script object types as a 4-byte class by value: the
// registry initializer stores each literal twice (temporary and parameter copy) and the
// lookup receives the type through a hidden pointer. Stored registry entries keep a plain
// FourCC (a class member makes the sort helpers spill through the stack), so the class
// converts back to FourCC. Echoes uses the plain FourCC typedef throughout.
class CFourCC {
public:
  CFourCC(FourCC fourCC) : mFourCC(fourCC) {}

  operator FourCC() const { return mFourCC; }

private:
  FourCC mFourCC;
};

typedef CEntity* (*FScriptLoader)(CStateManager& mgr, CInputStream& input, CEntityInfo& info);

FScriptLoader GetScriptLoaderForType(CFourCC type);

// Every G2MEAB loader is linked into the DOL; there are no REL forwarding stubs.
// Names follow each loader's "Unknown property ... in X loader." diagnostic.
CEntity* LoadAIHint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadAIKeyframe(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadAITaskPoint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadAIWaypoint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadActor(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadActorKeyframe(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadActorMorph(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadActorTransform(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadAiJumpPoint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadAmbientAI(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadAreaAttributes(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadAreaDamage(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadAtomicAlpha(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadBallTrigger(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadBeastRider(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadBerserker(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadBlinkWolf(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCable(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCameraBlurKeyframe(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCameraFilterKeyframe(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCameraHint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCameraPitch(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCameraShaker(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCannonBall(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCinematicCamera(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadColorModulate(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadConditionalRelay(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadContextSensitiveAction(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadContextSensitiveActivator(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadControlHint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadControllerAction(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCounter(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCoverPoint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadCrossAreaRelay(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadDamageActor(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadDamageableTrigger(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadDamageableTriggerOrientated(CStateManager& mgr, CInputStream& input,
                                         CEntityInfo& info);
CEntity* LoadDarkSamus(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadDebris(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadDefenseMechanoid(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadDestructibleBarrier(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadDistanceFog(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadDock(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadDoor(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadDynamicLight(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadEffect(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadEffectRepulsor(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadElectroMagneticPulse(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadEnvFxDensityController(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadEyePod(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFalsePerspective(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFargullHatcher(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFargullHatcherSwarm(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFishCloud(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFishCloudModifier(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFlyerSwarm(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFlyingPirate(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFogOverlay(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFogVolume(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFriendly(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadFrontEndDataNetwork(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGeneratedObjectDeleter(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGenerator(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGragnolFlyer(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGrapplePoint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGuiMenu(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGuiPlayerJoinManager(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGuiScreen(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGuiSlider(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGuiWidget(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGunTurretBase(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadGunTurretTop(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadHUDHint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadHUDMemo(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadKorakk(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadKorbaMaw(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadKorbaSnatcherSwarm(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadLUAScript(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadLayerController(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadMantha(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadMemoryRelay(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadMetroidHatcher(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadMetroidHopper(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadMysteryFlyer(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadOptionalAreaAsset(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPTCNoseTurret(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPathControl(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPathMeshCtrl(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPhazonFlyerSwarm(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPhazonLeech(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPhazonPuddle(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPhysicsDebris(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPickup(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPirateDrone(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPlantScarabSwarm(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPlatform(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPlayerActor(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPlayerController(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPlayerGravityScalar(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPlayerHint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPlayerTurret(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPlayerUserAnimPoint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPointOfInterest(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadPositionRelay(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadRadialDamage(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadRelay(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadRelayRandom(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadReptilicusHunter(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadRepulsor(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadRidley1(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadRipple(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadRoomAcoustics(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadRumbleEffect(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadRundas(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadScrewAttackWallJumpTarget(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSeedBoss1(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSeedBoss1Orb(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSequenceTimer(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadShadowProjector(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadShip(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadShipCommandIcon(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadShipCommandPath(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadShipProxy(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSkyRipple(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSound(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSoundModifier(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSpacePirate(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSpawnPoint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSpecialFunction(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSpiderBallAttractionSurface(CStateManager& mgr, CInputStream& input,
                                         CEntityInfo& info);
CEntity* LoadSpiderBallWaypoint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSpinner(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSteam(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSteamBot(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSteamLord(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadStreamedAudio(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadStreamedMovie(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSubtitles(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSurfaceControl(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSwarmBot(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadSwitch(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadTargetingPoint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadTeamAiMgr(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadTextPane(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadTimeKeyframe(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadTimer(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadTrigger(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadVisorFlare(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadVisorGoo(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadWater(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadWaypoint(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadWeaponGenerator(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadWorldLightFader(CStateManager& mgr, CInputStream& input, CEntityInfo& info);
CEntity* LoadWorldTeleporter(CStateManager& mgr, CInputStream& input, CEntityInfo& info);

#endif // _SCRIPTLOADER
