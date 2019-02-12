#ifndef PHYSICAL_PHYSICS_WIND_I_PR_WIND_H_
#define PHYSICAL_PHYSICS_WIND_I_PR_WIND_H_

#include "core/types/types.h"
#include "core/math/Vector3.h"

class IPrWind
{
public:
	PHYS_API explicit		IPrWind ( void );
	PHYS_API virtual		~IPrWind ( void );

public:

	//	Update (dt) : Overridable update.
	// Called on physics update at a constant framerate.
	PHYS_API virtual void	Update ( Real deltaTime ) =0;
	//	GetWindVector (in pos, out wind) : Overridable wind sampling.
	// Called on wind update.
	PHYS_API virtual void	GetWindVector ( const Vector3f& in_position, Vector3f& out_windforce ) const =0;

public:
	// Is this wind object included in the current simulation
	bool	m_active;
};

#endif//PHYSICAL_PHYSICS_WIND_I_PR_WIND_H_