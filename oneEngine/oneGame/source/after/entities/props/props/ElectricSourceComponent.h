
#ifndef _ELECTRIC_SOURCE_COMPONENT_
#define _ELECTRIC_SOURCE_COMPONENT_

#include "after/entities/props/CTerrainProp.h"

class ElectricSourceComponent : public CTerrainProp
{
	ClassName( "ElectricSourceComponent" );
public:
	ElectricSourceComponent ( BlockTrackInfo const& inInfo );

	// Update function
	void Update ( void );

	// Lookat calls
	void	OnInteractLookAt	( CActor* interactingActor );
	void	OnInteractLookAway	( CActor* interactingActor );

	// Pre-simulation. Is executed after the electric current map clears
	void PreSimulation ( void );
	// Post-simulation. Is executed after all simulation has finished
	void PostSimulation ( void );

	// On-Punched
	void OnPunched ( RaycastHit const& hitInfo );

};

#endif