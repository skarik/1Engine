
// class CItemJunkBase
// junk


#include "CItemJunkBase.h"
#include "engine/physics/raycast/Raycaster.h"
#include "engine/physics/motion/CRigidbody.h"

CItemJunkBase::CItemJunkBase ( tJunkStats& newProps, const WItemData & wdat )
	: CWeaponItem( wdat )
{
	junk_stats = newProps;
	bWasTossed = false;
}

CItemJunkBase::~CItemJunkBase ( void )
{
	//
}

CWeaponItem& CItemJunkBase::operator= ( const CWeaponItem * original ) 
{
	CItemJunkBase* oitem = (CItemJunkBase*)original;
	junk_stats = oitem->junk_stats;

	return CWeaponItem::operator= ( original );
}

// Serialization
void CItemJunkBase::serialize ( Serializer & ser, const uint ver )
{
	CWeaponItem::serialize(ser,ver);
	ser & junk_stats.breaksOnThrow;
	ser & junk_stats.morbid;
	ser & junk_stats.totallyUseless;
	ser & junk_stats.plotRelevant;
	ser & junk_stats.cursed;
	ser & junk_stats.hasFeelings;
	ser & junk_stats.wantsToKillYou;
	ser & junk_stats.notInaminate;
}


// Use function
bool CItemJunkBase::Use ( int x )
{
	if ( !CanUse( x ) )
		return true;
	if ( x == Item::UPrimary )
	{	// Toss it
		bWasTossed = true;
		SetCooldown( x, 1.0f );
		transform.position += pOwner->GetEyeRay().dir * 0.5f;
		Toss( pOwner->GetEyeRay().dir * 210.0f );
		return true;
	}
	else if ( x == Item::USecondary )
	{	// Look at it
		bWasTossed = false;
		SetCooldown( x, 1.0f );
		return true;
	}
	return false;
}

// Update, for check what's going on when flying through the air
void CItemJunkBase::Update ( void )
{
	// If was tossed and breaks on throw, then check for collision
	if ( bWasTossed && junk_stats.breaksOnThrow )
	{
		if ( pBody ) {
			Ray targetFlight;
			RaycastHit hitResult;

			// Get direction of flight
			targetFlight.pos = transform.position;
			targetFlight.dir = pBody->GetVelocity().normal();
			// Raytrace in that direction
			if ( Raycaster.Raycast( targetFlight, 1.2f, &hitResult, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31) ) )
			{
				// Delete object with a delay
				DeleteObjectDelayed( this, 0.1f );
			}
		}
	}
}