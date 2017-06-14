
#ifndef _PROP_TORCH_H_
#define _PROP_TORCH_H_

#include "after/entities/props/CTerrainProp.h"

class CLight;
class CParticleSystem;

class PropTorch : public CTerrainProp
{
	ClassName( "PropTorch" );
public:
	explicit PropTorch ( BlockTrackInfo const& inInfo );
	~PropTorch ( void );

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

#endif//_PROP_TORCH_H_