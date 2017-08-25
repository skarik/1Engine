#ifndef _PROP_DOOR_BASE_H_
#define _PROP_DOOR_BASE_H_

#include "after/entities/props/CTerrainProp.h"

class PropDoorBase : public CTerrainProp
{
	ClassName( "PropDoorBase" );
public:
	explicit PropDoorBase ( BlockTrackInfo const& inInfo );
	~PropDoorBase ( void );

	// Update function
	void Update ( void );

	// Interact function
	void OnInteract ( CActor* interactingActor );

	//void OnInteractLookAt ( CActor* interactingActor );
	//void OnInteractLookAway ( CActor* interactingActor );

	void OnTerrainPlacement ( void );

	// On-Punched
	void OnPunched ( RaycastHit const& hitInfo );

protected:
	CModel*		doorModel;
	CCollider*	doorCollision;
	CRigidBody*	doorBody;

	ftype		mDoorAngle;
	ftype		mDoorAngularVelocity;
};


#endif _PROP_DOOR_BASE_H_