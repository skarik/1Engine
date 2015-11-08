
#ifndef _ELECTRIC_LAMP_COMPONENT_
#define _ELECTRIC_LAMP_COMPONENT_

#include "after/entities/props/CTerrainProp.h"
//#include "CLight.h"
class CLight;

class ElectricLampComponent : public CTerrainProp
{
	ClassName( "ElectricLampComponent" );
public:
	ElectricLampComponent ( BlockTrackInfo const& inInfo );
	~ElectricLampComponent ( void );

	// Create common
	void InitializeCommon ( void );

	// Update function
	void Update ( void );

	// Use function
	//bool Use( int x );

	// Lookat calls
	void	OnInteractLookAt	( CActor* interactingActor );
	void	OnInteractLookAway	( CActor* interactingActor );

	// Pre-simulation. Is executed after the electric current map clears
	void PreSimulation ( void );
	// Post-simulation. Is executed after all simulation has finished
	void PostSimulation ( void );

	// On-Punched
	void OnPunched ( RaycastHit const& hitInfo );

private:
	CLight*	pLight;
	bool	bLightOn;

};

#endif