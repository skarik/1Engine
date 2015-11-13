
#include "ItemFloraSnowLily.h"
#include "after/entities/props/flora/PropFloraSnowLily.h"

#include "engine/physics/raycast/Raycaster.h"

#include "renderer/logic/model/CModel.h"

ItemFloraSnowLily::ItemFloraSnowLily ( void )
	: CWeaponItem( ItemData() )
{
	pModel = new CModel ( string("models/flora/snowdrop_lily.FBX") );
	pModel->transform.scale = Vector3d( 1.3f,1.3f,1.3f );
	
	holdType = Item::Default;
}

ItemFloraSnowLily::~ItemFloraSnowLily ( void )
{
	;
}


// Use function
bool ItemFloraSnowLily::Use( int x )
{
	if ( x == Item::USecondary )
	{
		if ( !CanUse( x ) )
		{
			SetCooldown( x, Time::deltaTime*3.0f );
			return true;
		}
		SetCooldown( x, Time::deltaTime*3.0f );

		Vector3d normal;
		CTransform* target = WeaponItem::ComponentPlacement<PropFloraSnowLily>( pOwner, this, &normal );
		if ( target ) {
			Vector3d targetPos = target->position + normal*0.5f;
			targetPos.x = floor( targetPos.x / 2 ) * 2;
			targetPos.y = floor( targetPos.y / 2 ) * 2;
			targetPos.z = floor( targetPos.z / 2 ) * 2;
			targetPos += Vector3d( 1,1,1 );
			if ( normal.z > 0.5f ) {
				targetPos.z -= 0.4f;
			}
			else if ( normal.z < -0.5f ) {
				targetPos.z += 0.2f;
			}
			else {
				targetPos -= normal * 0.5f;
			}
			target->position = targetPos;
		}

		return true;
	}
	else
	{
		return false;
	}
}