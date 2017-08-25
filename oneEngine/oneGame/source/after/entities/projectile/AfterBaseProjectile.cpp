
#include "engine-common/entities/CActor.h"
#include "AfterBaseProjectile.h"

// Constructor
AfterBaseProjectile::AfterBaseProjectile( Ray const& rnInRay, ftype fnInSpeed, ftype fnWidth )
	: CProjectile( rnInRay, fnInSpeed, fnWidth )
{
	// Initialize buffs
	effects.UpdateParent( this );
}


// == Update ==
void AfterBaseProjectile::Update ( void )
{
	// Update buffs
	effects.Update();
	// Perform projectile code
	CProjectile::Update();
}

void AfterBaseProjectile::OnHit ( CGameObject* pHitObject, Item::HitType nHitType )
{
	// Hit information is stored in rhLastHit.

	// Check if the hit object is of type actorcharacter
	//if ( pHitObject->GetBaseClassName() == "CActor_Character" )
	if ( nHitType == Item::HIT_CHARACTER || nHitType == Item::HIT_ACTOR )
	{
		std::cout << "Projectile damage: " << dDamage.amount << " (d" << dDamage.type << ")" << std::endl;

		dDamage.actor = mOwner;

		// If it is, then hurt it. Bah. HURT IT.
		CActor*	pCharacter = (CActor*)pHitObject;
		bool bDestroy = effects.OnHitEnemy( pCharacter );
		if ( mOwner ) mOwner->OnDealDamage( dDamage, pCharacter );
		pCharacter->OnDamaged( dDamage );

		if ( bDestroy ) {
			// Finally, delete ourselves.
			DeleteObject( this );
		}
	}
	else if ( nHitType == Item::HIT_TERRAIN )
	{
		bool bDestroy = effects.OnHitWorld( rhLastHit.hitPos, rhLastHit.hitNormal );

		if ( bDestroy ) {
			// Finally, delete ourselves.
			DeleteObject( this );
		}
	}
	
}
