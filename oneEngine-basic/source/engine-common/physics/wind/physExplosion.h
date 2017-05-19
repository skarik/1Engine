
#ifndef _PHYS_EXPLOSION_H_
#define _PHYS_EXPLOSION_H_
/*
#include "CPhysicsWindManager.h"

class physExplosion : public hkpWind
{
public:
HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
	/// Constructor.
	Explosion( const Vector3d& center, const Real magnitude, const Real range, const Real speed );

	// hkpWind implementation.
	virtual void getWindVector( const hkVector4& pos, hkVector4& windOut ) const;
private:

	hkReal m_range;
	hkReal m_speed;
	hkReal m_magnitude;
	
};
*/
#include "physical/physics/wind/CPhysWindBase.h"

class physExplosion : public CPhysWindBase
{
public:
	physExplosion ( const Vector3d& center, const Real magnitude, const Real range, const Real speed );

	virtual void Update ( Real deltaTime );
	virtual void GetWindVector ( const Vector3d& pos, Vector3d& windOut ) const;
	virtual bool IsActive ( void ) const;

private:
	Vector3d	m_center;
	Real		m_magnitude;
	Real		m_range;
	Real		m_speed;

	Real		currentRadius;
};


#endif


