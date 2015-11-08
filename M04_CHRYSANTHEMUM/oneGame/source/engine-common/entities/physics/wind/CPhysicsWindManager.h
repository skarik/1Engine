
#ifndef _C_PHYSICS_WIND_MANAGER_H_
#define _C_PHYSICS_WIND_MANAGER_H_

#include "engine/behavior/CGameBehavior.h"
#include "physical/physics/CPhysics.h"

//class CPhysicsWindManager : public CGameBehavior 
/*class CPhysicsWindManager : public hkpWind
{
	public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		CPhysicsWindManager();
		virtual ~CPhysicsWindManager();

		void update ( hkReal delta );
		virtual void getWindVector ( const hkVector4& pos, hkVector4& windOut ) const;

		//void addExplosion( const hkVector4& center, const ExplosionInfo& info );
	private:
		//hkArray< Explosion* > m_explosions;
};*/

class CPhysicsWindManager;	// Game-side wind manager, controls phantom and wind class
class physWindManager;		// Havok-side implementation of the hkpWind interface.
class CPhysWindBase;		// Abstract game-side wind interface


class CPhysicsWindManager : public CGameBehavior
{
	ClassName( "PhysicsWindManager" );
public:
	ENGCOM_API CPhysicsWindManager ( void );
	ENGCOM_API ~CPhysicsWindManager ( void );

	// Empty Update
	ENGCOM_API void Update ( void ) override {}

	// Update wind during physics step
	ENGCOM_API void FixedUpdate ( void ) override;

	// Add wind object to list
	//ENGCOM_API void AddWind ( CPhysWindBase* windToAdd );

private:
	static CPhysicsWindManager*	Active;
public:
	ENGCOM_API static CPhysicsWindManager* GetActive ( void );

private:
	static bool	bReady;
	static physWindManager *	m_physWindManager;
	static hkpWindRegion *		m_region;
	static hkpAabbPhantom*		m_phantom;
	static hkAabb				m_aabb;
};

#endif