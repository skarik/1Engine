
#ifndef _PROP_LANTERN_H_
#define _PROP_LANTERN_H_

#include "after/entities/props/CTerrainProp.h"

class CLight;
class CParticleSystem;

class PropLantern : public CTerrainProp
{
	ClassName( "PropLantern" );
public:
	explicit PropLantern ( BlockTrackInfo const& inInfo );
	~PropLantern ( void );

	// Update function
	void Update ( void );

	// Use function
	//bool Use( int x );

	// Lookat calls
	void	OnInteractLookAt	( CActor* interactingActor );
	void	OnInteractLookAway	( CActor* interactingActor );

	// Pre-simulation. Is executed after the electric current map clears
	//void PreSimulation ( void );
	// Post-simulation. Is executed after all simulation has finished
	//void PostSimulation ( void );

	// On-Punched
	void OnPunched ( RaycastHit const& hitInfo );

private:
	CLight*				pLight;
	CParticleSystem*	pFireSystem;

};

#endif//_PROP_LANTERN_H_