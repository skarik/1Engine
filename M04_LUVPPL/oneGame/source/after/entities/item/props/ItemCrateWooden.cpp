
#include "ItemCrateWooden.h"
#include "after/entities/props/props/PropCrateWooden.h"
#include "after/entities/item/CWeaponItemComponents.h"
#include "renderer/logic/model/CModel.h"

// Constructor (on ground)
ItemCrateWooden::ItemCrateWooden ( void )
	: CWeaponItem( ItemData() )
{
	transform.scale = Vector3d( 0.1f, 0.1f, 0.1f );

	if ( pModel == NULL ) {
		pModel = new CModel( string("models/props/crate0.FBX") );
		pModel->transform.scale = Vector3d( 0.1f, 0.1f, 0.1f );
	}
}

// Destructor
ItemCrateWooden::~ItemCrateWooden ( void )
{

}

// Use places the object on a terrain bit
bool ItemCrateWooden::Use ( int x )
{
	if ( x == Item::USecondary )
	{
		if ( !CanUse( x ) )
		{
			SetCooldown( x, Time::deltaTime*3.0f );
			return true;
		}
		SetCooldown( x, Time::deltaTime*3.0f );

		CTransform* target = WeaponItem::ComponentPlacement<PropCrateWooden>( pOwner, this );
		if ( target ) {
			// Snap?
		}

		return true;
	}
	else
	{
		return false;
	}
}