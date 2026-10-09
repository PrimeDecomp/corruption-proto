#ifndef _TCASTTO
#define _TCASTTO

class CEntity;

// Echoes' casts. In G2MEAB TypesMatch.cpp holds the null-safe dispatch (0x801D9AFC, Echoes'
// TryCast) and one wrapper per class; TCastToPtr<CEntity> (0x801D7DE8) passes type id 0.
template < class T >
T* TCastToPtr(CEntity* p);

template < class T >
const T* TCastToConstPtr(const CEntity* p);

#endif // _TCASTTO
