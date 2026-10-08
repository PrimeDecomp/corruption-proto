#ifndef _CCOLLISIONRESPONSEDATA
#define _CCOLLISIONRESPONSEDATA

#include "Kyoto/TToken.hpp"
#include "MetroidPrime/ActorCommon.hpp"
#include "rstl/optional_object.hpp"
#include "rstl/vector.hpp"

class CInputStream;
class CSimplePool;
class CGenDescription;
class CDecalDescription;
class CAudioGroupSet;

class CCollisionResponseData {
public:
  typedef rstl::vector< rstl::optional_object< TLockedToken< CGenDescription > > >
      GeneratorListType;
  typedef rstl::vector< rstl::optional_object< TLockedToken< CAudioGroupSet > > > SoundListType;
  typedef rstl::vector< rstl::optional_object< TLockedToken< CDecalDescription > > > DecalListType;

  CCollisionResponseData(CInputStream& in, CSimplePool* pool);
  ~CCollisionResponseData();

  bool CheckAndAddResourcesToResponse(const FourCC clsId, CInputStream& in, CSimplePool* pool);
  bool CheckAndAddParticleSystemToResponse(const FourCC clsId, CInputStream& in, CSimplePool* pool);
  bool CheckAndAddSoundFXToResponse(const FourCC clsId, CInputStream& in, CSimplePool* pool);
  bool CheckAndAddDecalToResponse(const FourCC clsId, CInputStream& in, CSimplePool* pool);
  bool AddParticleSystemToResponse(EWeaponCollisionResponseTypes type, CInputStream& in,
                                   CSimplePool* pool);

  rstl::optional_object< TLockedToken< CGenDescription > >
  GetParticleDescription(EWeaponCollisionResponseTypes type) const;
  rstl::optional_object< TLockedToken< CAudioGroupSet > >
  GetSoundDescription(EWeaponCollisionResponseTypes type) const;
  rstl::optional_object< TLockedToken< CDecalDescription > >
  GetDecalDescription(EWeaponCollisionResponseTypes type) const;
  static EWeaponCollisionResponseTypes GetWorldCollisionResponseType(uint materialFlags);
  static bool ResponseTypeIsEnemyNormal(EWeaponCollisionResponseTypes type);
  static bool ResponseTypeIsEnemySpecial(EWeaponCollisionResponseTypes type);

private:
  GeneratorListType mGeneratorTokens;
  SoundListType mSoundTokens;
  DecalListType mDecalTokens;

  static const int skWorldMaterialTable[];
};
CHECK_SIZEOF(CCollisionResponseData, 0x30)

#endif // _CCOLLISIONRESPONSEDATA
