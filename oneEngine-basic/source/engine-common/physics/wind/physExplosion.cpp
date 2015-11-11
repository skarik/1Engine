
#include "physExplosion.h"

physExplosion::physExplosion ( const Vector3d& center, const ftype magnitude, const ftype range, const ftype speed )
	: CPhysWindBase()
{
	m_center	= center;
	m_magnitude	= magnitude;
	m_range		= range;
	m_speed		= speed;
	currentRadius = 0;
}

void physExplosion::Update ( ftype deltaTime )
{
	currentRadius += m_speed * deltaTime;
}

void physExplosion::GetWindVector ( const Vector3d& pos, Vector3d& windOut ) const
{
	Vector3d difVect = pos - m_center;

	ftype dist = difVect.magnitude();
	if ( dist < sqr(currentRadius) )
	{
		windOut = (difVect / dist) * ( 1 - sqr( dist/m_range ) ) * m_magnitude;
	}
	else
	{
		windOut = Vector3d( 0,0,0 );
	}
}

bool physExplosion::IsActive ( void ) const
{
	return (currentRadius < m_range);
}