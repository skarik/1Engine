
#ifndef _PROP_FLORA_SNOW_LILY_H_
#define _PROP_FLORA_SNOW_LILY_H_

#include "after/entities/props/CTerrainProp.h"

class PropFloraSnowLily : public CTerrainProp
{
	ClassName( "PropFloraSnowLily" );
public:
	explicit PropFloraSnowLily ( BlockTrackInfo const& inInfo );
	~PropFloraSnowLily ( void );

	void OnTerrainPlacement ( void );

	// Lookat calls
	void	OnInteractLookAt	( CActor* interactingActor );
	void	OnInteractLookAway	( CActor* interactingActor );
	// Interact call
	void	OnInteract ( CActor* interactingActor );
	// On-Punched
	void	OnPunched ( RaycastHit const& hitInfo );
};

#endif//_PROP_FLORA_SNOW_LILY_H_