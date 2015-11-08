

#include "SkillFireArrow.h"

//#include "ProjectileFiretoss.h"
#include "after/entities/projectile/projectile/ProjectileArrow.h"

#include "after/entities/item/weapon/CBaseRandomBow.h"
#include "core/math/random/Random.h"

#include "after/states/projectileeffect/effects/PEffectBurnOnHit.h"

SkillFireArrow::SkillFireArrow ( void ) : CSkill( ItemData() )
{
	mana_cost	= 10;
	cooldown	= 3.0f;
	passive		= false;

	holdType = Item::HoldType::EmptyHanded;

	//holdType = SpellHanded;
	//iAnimationSubset = 2;
}

SkillFireArrow::~SkillFireArrow ( void )
{

}

// Item cast
bool SkillFireArrow::Use ( int x )
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

		aimDir = pOwner->GetAimRay( transform.position );
		aimDir.dir = ( aimDir.dir+(Random.PointOnUnitSphere()*0.15f*(1.03f-tension)) ).normal(); // Add randomness
		fireSpeed = mBow->weapon_stats.pspeed * 0.4f * ((0.1f+tension)/1.1f) * 0.3f; // Much slower arrow than normal shot.

		arrowDamage.amount	= mBow->weapon_stats.damage * ((0.2f+tension)/1.2f) * 0.8f; //slightly reduced damage, but has a DoT
		arrowDamage.type	= DamageType::Pierce | DamageType::Fire | DamageType::Burn | DamageType::Magical;
						
		tempArrow = new ProjectileArrow( aimDir, fireSpeed*28.0f );
		tempArrow->SetDamping( 0.04f + (1-tension)*0.16f, 0.95f + (1-tension)*0.15f );
		tempArrow->SetOwner( pOwner );
		tempArrow->SetDamage( arrowDamage );
		tempArrow->Effects()->Add( new PEffectBurnOnHit );

		return true;
	}
	return false;
}
// Is modifier for bow items
bool SkillFireArrow::IsModifier ( const CWeaponItem* item )
{
	if ( item->GetItemData()->eTopType == WeaponItem::ItemBow ) {
		mBow = (CBaseRandomBow*)item;
		return true;
	}
	else {
		return false;
	}
}