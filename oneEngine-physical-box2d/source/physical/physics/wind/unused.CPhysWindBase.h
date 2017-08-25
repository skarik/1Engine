
#ifndef _C_PHYSICS_WIND_BASE_
#define _C_PHYSICS_WIND_BASE_

//#include "CPhysics.h"
#include "core/types.h"
#include "core/math/Vector3d.h"
//#include "physical/physics/CPhysics.h"
#include <vector>

//class CPhysWindBase
//{
//public:
//	//=========================================//
//	// Creation and destruction
//
//	PHYS_API explicit CPhysWindBase ( void );
//	PHYS_API virtual ~CPhysWindBase ( void );
//
//public:
//	//=========================================//
//	// Overrideable behaviors
//
//	PHYS_API virtual void Update ( Real deltaTime ) =0;
//	PHYS_API virtual void GetWindVector ( const Vector3d& pos, Vector3d& windOut ) const =0;
//
//	PHYS_API virtual bool IsActive ( void ) const =0;
//
//	//=========================================//
//	// Static state
//private:
//	static std::vector<CPhysWindBase*>	_wind_listing;
//public:
//	PHYS_API static std::vector<CPhysWindBase*>& Listing ( void );
//};
//
////===============================================================================================//
//// Havok Wind system that works off of CPhysWindBase
////===============================================================================================//
//
//// Havok-side implementation of hkpWind interface
//class physWindManager : public physWind //public hkpWind
//{
//public:
//	//HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
//	PHYS_CLASS_ALLOCATOR;
//	PHYS_API			physWindManager();
//	PHYS_API virtual	~physWindManager();
//
//	//PHYS_API void update ( hkReal delta );
//	PHYS_API void update ( physReal delta );
//	//PHYS_API virtual void getWindVector ( const hkVector4& pos, hkVector4& windOut ) const;
//	PHYS_API virtual void getWindVector ( const physVector4& pos, physVector4& windOut ) const;
//
//	//void addExplosion( const hkVector4& center, const ExplosionInfo& info );
//	//void addWind ( CPhysWindBase* newWind );
//private:
//	//hkArray< Explosion* > m_explosions;
//	//vector<CPhysWindBase*> m_physForces;
//	//hkArray< CPhysWindBase* > m_winds;
//
//public:
//	PHYS_API static physWindManager*	Instantiate ( void );
//	//PHYS_API static hkpWindRegion*		InstantiateWindRegion( hkpAabbPhantom* phantom, const hkpWind* wind, hkReal resistanceFactor, hkReal obbFactor = 0.0f );
//	PHYS_API static physWindRegion*		InstantiateWindRegion( physAabbPhantom* phantom, const physWind* wind, physReal resistanceFactor, physReal obbFactor = 0.0f );
//};

#endif