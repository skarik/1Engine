
#include "WeaponCrossbow.h"
#include "after/entities/projectile/projectile/ProjectileCBolt.h"
#include "renderer/logic/model/CModel.h"

WeaponCrossbow::WeaponCrossbow ( void )
	: CWeaponItem( ItemData() )
{
	holdType = Item::DefaultRanged;

	if ( pModel == NULL )
		pModel = new CModel( "models/weapons/crossbow_beta.FBX" );
}

bool WeaponCrossbow::Use ( int x )
{
	if ( x == Item::UPrimary )
	{
		if ( CanUse( x ) )
		{
			SetCooldown( x, 0.6f );

			ProjectileCBolt*	tempBolt;
			Ray		aimDir;
			ftype	fireSpeed;

			//aimDir.dir = pOwner->GetEyeRay().dir;
			//aimDir.pos = transform.position;
			aimDir = pOwner->GetAimRay( transform.position );
			fireSpeed = 6.0f;
			
			tempBolt = new ProjectileCBolt( aimDir, fireSpeed );
			tempBolt->SetDamping( 0.04f, 0.94f );
			tempBolt->SetOwner( pOwner );
		}
		else
		{
			SetCooldown( x, 0.6f );
		}

		return true;
	}
	else
	{
		return false;
	}
}