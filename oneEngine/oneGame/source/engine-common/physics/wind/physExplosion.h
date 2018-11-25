#ifndef ENGINE_COMMON_PHYSICS_EXPLOSION_H_
#define ENGINE_COMMON_PHYSICS_EXPLOSION_H_
/*
#include "CPhysicsWindManager.h"

class physExplosion : public hkpWind
{
public:
HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
	/// Constructor.
	Explosion( const Vector3f& center, const Real magnitude, const Real range, const Real speed );

	// hkpWind implementation.
	virtual void getWindVector( const hkVector4& pos, hkVector4& windOut ) const;
private:

	hkReal m_range;
	hkReal m_speed;
	hkReal m_magnitude;
	
};
*/
//#include "physical/physics/wind/CPhysWindBase.h"
#include "physical/physics/fluid/IPrWind.h"

class physExplosion : public IPrWind
{
public:
	explicit	physExplosion ( const Vector3f& center, const Real magnitude, const Real range, const Real speed );
				~physExplosion ( void );

	virtual void Update ( Real deltaTime ) override;
	virtual void GetWindVector ( const Vector3f& pos, Vector3f& windOut ) const override;

private:
	Vector3f	m_center;
	Real		m_magnitude;
	Real		m_range;
	Real		m_speed;

	Real		currentRadius;
};


#endif//ENGINE_COMMON_PHYSICS_EXPLOSION_H_
