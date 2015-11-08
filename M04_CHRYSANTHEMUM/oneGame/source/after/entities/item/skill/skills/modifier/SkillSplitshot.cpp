

#include "SkillSplitshot.h"

#include "after/entities/projectile/projectile/ProjectileArrow.h"

#include "after/entities/item/weapon/CBaseRandomBow.h"
#include "core/math/random/Random.h"

SkillSplitshot::SkillSplitshot ( void ) : CSkill( ItemData() )
{
	mana_cost	= 5;
	stamina_cost= 5;
	cooldown	= 1.8f;
	passive		= false;

	holdType = Item::HoldType::EmptyHanded;

}

SkillSplitshot::~SkillSplitshot ( void )
{

}

// Item cast
bool SkillSplitshot::Use ( int x )
{
	if ( x == Item::UModifier ) {
		// Cast from mana
		CastMana();
		// Set cooldown
		SetCooldown( x, cooldown );

		// Create projectile
		ProjectileArrow*	tempArrow;
		Ray		aimDir;
		ftype	fireSpeed;
		Damage	arrowDamage;
	
		ftype tension = 1.0f;

		arrowDamage.amount	= mBow->weapon_stats.damage * ((0.2f+tension)/1.2f) * 0.65f; //65% damage
		arrowDamage.type	= DamageType::Pierce;
						
		aimDir = pOwner->GetAimRay( transform.position );
		//aimDir.dir = ( aimDir.dir+(Random.PointOnUnitSphere()*0.15f*(1.03f-tension)) ).normal(); // Add randomness
		aimDir.dir = pOwner->GetAimRotator() * Vector3d(1,0.06f,0).normal();
		fireSpeed = mBow->weapon_stats.pspeed * 0.4f * ((0.1f+tension)/1.1f) * 0.6f; // Slightly slower arrow than normal shot

		tempArrow = new ProjectileArrow( aimDir, fireSpeed*28.0f );
		tempArrow->SetDamping( 0.04f + (1-tension)*0.16f, 0.95f + (1-tension)*0.15f );
		tempArrow->SetOwner( pOwner );
		tempArrow->SetDamage( arrowDamage );

		aimDir.dir = pOwner->GetAimRotator() * Vector3d(1,-0.06f,0).normal();
		tempArrow = new ProjectileArrow( aimDir, fireSpeed*28.0f );
		tempArrow->SetDamping( 0.04f + (1-tension)*0.16f, 0.95f + (1-tension)*0.15f );
		tempArrow->SetOwner( pOwner );
		tempArrow->SetDamage( arrowDamage );

		return true;
	}
	return false;
}
// Is modifier for bow items
bool SkillSplitshot::IsModifier ( const CWeaponItem* item )
{
	if ( item->GetItemData()->eTopType == WeaponItem::ItemBow ) {
		mBow = (CBaseRandomBow*)item;
		return true;
	}
	else {
		return false;
	}
}