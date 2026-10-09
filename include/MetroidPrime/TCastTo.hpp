#ifndef _TCASTTO
#define _TCASTTO

class CEntity;

// Echoes' casts. In G2MEAB TypesMatch.cpp holds the null-safe dispatch (0x801D9AFC, Echoes'
// TryCast) and one wrapper per class; TCastToPtr<CEntity> (0x801D7DE8) passes type id 0.
template < class T >
T* TCastToPtr(CEntity* p);

template < class T >
const T* TCastToConstPtr(const CEntity* p);

// Echoes' reference cast. Each class's pointer cast is followed by this one, which skips the
// null check (the cast-flag classes test the flag, the others call TypesMatch directly).
// CStateManagerCollision's sorted-list update writes through its result.
template < class T >
T* TCastToPtr(CEntity& p);

// Echoes' inline wrapper.
template < class T >
static inline const T* TCastToConstPtr(const CEntity& p) {
  return TCastToPtr< T >(const_cast< CEntity& >(p));
}

#endif // _TCASTTO
