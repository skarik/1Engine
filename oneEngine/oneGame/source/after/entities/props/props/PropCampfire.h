
#ifndef _PROP_CAMPFIRE_H_
#define _PROP_CAMPFIRE_H_

#include "after/entities/props/CTerrainProp.h"

class CLight;
class CParticleSystem;

class PropCampfire : public CTerrainProp
{
	ClassName( "PropCampfire" );
public:
	explicit PropCampfire ( BlockTrackInfo const& inInfo );
	~PropCampfire ( void );

	// Update function
	void Update ( void );

	// Use function
	bool Use( int x );

	// Lookat calls
	void	OnInteract	( CActor* interactingActor ) override;
	void	OnInteractLookAt	( CActor* interactingActor ) override;
	void	OnInteractLookAway	( CActor* interactingActor ) override;

	// Save data
	uint64_t	GetUserdata ( void ) override;
	// Load data
	void		SetUserdata ( const uint64_t& ) override;

	// On-Punched
	void OnPunched ( RaycastHit const& hitInfo );

private:
	CLight*				pLight;
	CParticleSystem*	pFireSystem;
	bool				bIsLit;

	ftype				mHitValue;

	// Create and destroy fire effects
	void CreateEffects ( void );
	void FreeEffects ( void );

};

#endif//_PROP_CAMPFIRE_H_