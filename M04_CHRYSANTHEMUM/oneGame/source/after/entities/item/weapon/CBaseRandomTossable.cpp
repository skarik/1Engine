
#include "after/entities/item/weapon/CBaseRandomTossable.h"

CBaseRandomTossable::CBaseRandomTossable( tTossableProperties &newProps, const WItemData &wdat )
	: CBaseRandomItem( wdat )
{
	tossable_stats = newProps;
	weaponItemData.iHands = tossable_stats.hands;
	lastTossed = NULL;
}
CBaseRandomTossable::~CBaseRandomTossable ( void )
{
	;
}
CWeaponItem& CBaseRandomTossable::operator= ( const CWeaponItem * original )
{
	CBaseRandomTossable* oitem = (CBaseRandomTossable*)original;
	tossable_stats = oitem->tossable_stats;
	weaponItemData.iHands = tossable_stats.hands;

	return CBaseRandomItem::operator= ( original );
}

// Serialization
void CBaseRandomTossable::serialize ( Serializer & ser, const uint ver )
{
	CBaseRandomItem::serialize(ser,ver);
	ser & tossable_stats.damage;
	ser & tossable_stats.hands;
	ser & tossable_stats.damagetype;
	ser & tossable_stats.speed;
	weaponItemData.iHands = tossable_stats.hands;
}


bool CBaseRandomTossable::Use( int x )
{
	if ( !CanUse( x ) )
		return true;
	if ( x == Item::UPrimary )
	{
		// Throw it
		SetCooldown( x, 1.0f );
		transform.position += pOwner->GetEyeRay().dir * 0.5f;
		lastTossed = (CBaseRandomTossable*)TossFromStack( pOwner->GetEyeRay().dir * 140.0f );
	}
	else if ( x == Item::USecondary )
	{
		// Nothing much....
	}
	return true;
}