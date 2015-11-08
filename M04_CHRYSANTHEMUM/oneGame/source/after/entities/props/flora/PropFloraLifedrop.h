
#ifndef _PROP_FLORA_LIFEDROP_H_
#define _PROP_FLORA_LIFEDROP_H_

#include "after/entities/props/CTerrainProp.h"

class PropFloraLifedrop : public CTerrainProp
{
	ClassName( "PropFloraLifedrop" );
public:
	explicit PropFloraLifedrop ( BlockTrackInfo const& inInfo );
	~PropFloraLifedrop ( void );

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