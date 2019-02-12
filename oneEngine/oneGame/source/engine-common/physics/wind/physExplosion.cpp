#include "physExplosion.h"
#include "physical/physics/fluid/IPrWindVolume.h"

physExplosion::physExplosion ( const Vector3f& center, const Real magnitude, const Real range, const Real speed )
	: IPrWind()
{
	m_center	= center;
	m_magnitude	= magnitude;
	m_range		= range;
	m_speed		= speed;
	currentRadius = 0;

	// Add to the active wind sim (as a helper)
	IPrWindVolume* active_volume = NULL;
	if (active_volume != NULL)
	{
		active_volume->Add(this);
	}
}
physExplosion::~physExplosion ( void )
{
	// Remove from the active wind sim (as a helper)
	IPrWindVolume* active_volume = NULL;
	if (active_volume != NULL)
	{
		active_volume->Remove(this);
	}
}


void physExplosion::Update ( Real deltaTime )
{
	currentRadius += m_speed * deltaTime;
}

void physExplosion::GetWindVector ( const Vector3f& pos, Vector3f& windOut ) const
{
	Vector3f difVect = pos - m_center;

	Real dist = difVect.magnitude();
	if ( dist < sqr(currentRadius) )
	{
		windOut = (difVect / dist) * ( 1 - sqr( dist/m_range ) ) * m_magnitude;
	}
	else
	{
		windOut = Vector3f( 0,0,0 );
	}
}
