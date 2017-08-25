#ifndef PHYSICAL_PHYSICS_FLUID_I_PR_WIND_VOLUME_H_
#define PHYSICAL_PHYSICS_FLUID_I_PR_WIND_VOLUME_H_

#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "physical/physics/motion/IPrVolume.h"

#include <vector>

class IPrWind;

class IPrWindVolume : public IPrVolume
{
public:
	PHYS_API explicit		IPrWindVolume ( PrWorld* world ) : IPrVolume(world) {}
	PHYS_API virtual		~IPrWindVolume ( void ) {}

public:

	//	Update (dt) : Overridable update.
	// Called on physics update at a constant framerate.
	PHYS_API virtual void	Update ( Real deltaTime ) =0;
	//	GetWindVector (in pos, out wind) : Overridable wind sampling.
	// Called on wind update.
	PHYS_API virtual void	GetWindVector ( const Vector3f& in_position, Vector3f& out_windforce ) const =0;

	//	Add (wind) : Adds a wind to the volume sim
	PHYS_API virtual void	Add ( IPrWind* wind )
	{
		auto l_windFind = std::find(m_winds.begin(), m_winds.end(), wind);
		if (l_windFind == m_winds.end())
		{
			m_winds.push_back(wind);
		}
	}
	//	Remove (wind) : Removes a wind from the volume sim.
	PHYS_API virtual void	Remove ( IPrWind* wind )
	{
		auto l_windFind = std::find(m_winds.begin(), m_winds.end(), wind);
		if (l_windFind != m_winds.end())
		{
			m_winds.erase(l_windFind);
		}
	}

protected:
	// List of current winds.
	std::vector<IPrWind*>	m_winds;
};

#endif//PHYSICAL_PHYSICS_FLUID_I_PR_WIND_VOLUME_H_