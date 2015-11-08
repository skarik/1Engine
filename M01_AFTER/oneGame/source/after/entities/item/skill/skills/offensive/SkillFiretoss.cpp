
#include "SkillFiretoss.h"

#include "core/time/time.h"
#include "engine/physics/raycast/Raycaster.h"
#include "core/math/random/Random.h"

//#include "CPlayerInventory.h"
//#include "ItemPebble.h"
//#include "ProjectilePebble.h"
#include "after/entities/projectile/magical/ProjectileFiretoss.h"

#include "engine-common/entities/CParticleSystem.h"
#include "renderer/logic/particle/CParticleEmitter.h"

#include "after/entities/character/CCharacter.h"

#include "renderer/light/CLight.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/material/glMaterial.h"

SkillFiretoss::SkillFiretoss ( void ) : CSkill( ItemData() )
{
	mana_cost	= 10;
	cooldown	= 3.0f;
	passive		= false;

	cast_state	= 0;

	ps_firetoss_active = new CParticleSystem ( ".res/particlesystems/flame03_b.pcf", "particle_fluuxflame" );
	ps_firetoss_active->enabled = false;

	mMagicLight = new CLight();
	mMagicLight->range = 0.1f;
	mMagicLight->diffuseColor = Color( 0.71f, 0.55f, 0.24f );
	mMagicLight->transform.Get( transform );
	mMagicLight->transform.SetParent( &transform );

	mMagicModel = new CModel( ".res/models/effects/magic_ball.fbx" );
	glMaterial* newMat = new glMaterial;
	newMat->loadFromFile( "effects/spell_fireball" );
	mMagicModel->SetMaterial( newMat );
	newMat->removeReference();
	mMagicModel->SetVisibility( false );
	mMagicModel->transform.Get( transform );
	mMagicModel->transform.SetParent( &transform );

	ps_firetoss_active->transform.Get( transform );
	ps_firetoss_active->transform.SetParent( &transform );
		
	holdType = Item::SpellHanded;
	iAnimationSubset = 2;
}

SkillFiretoss::~SkillFiretoss ( void )
{
	delete_safe(ps_firetoss_active);
	delete_safe(mMagicModel);
	delete_safe(mMagicLight);
}

bool SkillFiretoss::Use ( int use )
{
	if ( use == Item::UPrimary )
	{
		if ( CanUse(0) && CanCast() )
		{
			// Decrement mana
			CastMana();
			// Put on a short cooldown
			SetCooldown( 0,cooldown );

			if ( pOwner ) {
				((CCharacter*)pOwner)->OnRangedAttack();
			}

			if ( pOwner ) {
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Toss, iAnimationSubset, mHand, 0 );
			}
		}
	}
	return false;
}

// Attack function
void SkillFiretoss::Attack ( XTransform& )
{
	// Create projectile here
	if ( isCasting )
	{
		ProjectileFiretoss*	tempArrow;
		Ray		aimDir;
		ftype	fireSpeed;
		Damage	arrowDamage;
	
		aimDir = pOwner->GetAimRay( transform.position );
		//aimDir.dir = ( aimDir.dir+(Random.PointOnUnitSphere()*0.15f*(1.03f-tension)) ).normal(); // Add randomness
		//fireSpeed = weapon_stats.pspeed * 0.7f * ((0.1f+tension)/1.1f);
		fireSpeed = 40.0f;

		//arrowDamage.amount	= weapon_stats.damage * ((0.2f+tension)/1.2f);
		arrowDamage.amount	= 10.0f + skill_lvl * 5.0f;
		arrowDamage.type	= DamageType::Fire | DamageType::Burn | DamageType::Magical;
						
		tempArrow = new ProjectileFiretoss( aimDir, fireSpeed );
		tempArrow->SetDamping( 0.0f );
		tempArrow->SetOwner( pOwner );
		tempArrow->SetDamage( arrowDamage );

		// Has casted, so turn that shit off
		isCasting = false;
	}
}

// CastCombine. Combines two casting skills for an effect. Is called when both skills are casted and not channelling.
// Return true if something happens. Generally, objects look through all the spells, and stop the skills by setting isCasting to false.
// Returning true will normally stop non-instant skills.
bool SkillFiretoss::CastCombine ( CSkill* combineSkill )
{
	// Same skill here
	if ( combineSkill->GetID() == this->GetID() )
	{
		ProjectileFiretoss*	tempArrow;
		Ray		aimDir;
		ftype	fireSpeed;
		Damage	arrowDamage;
	
		aimDir = pOwner->GetAimRay( transform.position );
		//aimDir.dir = ( aimDir.dir+(Random.PointOnUnitSphere()*0.15f*(1.03f-tension)) ).normal(); // Add randomness
		//fireSpeed = weapon_stats.pspeed * 0.7f * ((0.1f+tension)/1.1f);
		fireSpeed = 40.0f;

		//arrowDamage.amount	= weapon_stats.damage * ((0.2f+tension)/1.2f);
		arrowDamage.amount	= 10.0f + skill_lvl * 5.0f;
		arrowDamage.type	= DamageType::Fire | DamageType::Burn | DamageType::Magical;
						
		tempArrow = new ProjectileFiretoss( aimDir, fireSpeed );
		tempArrow->SetDamping( 0.0f );
		tempArrow->SetOwner( pOwner );
		tempArrow->SetDamage( arrowDamage );

		aimDir.dir = (aimDir.dir*3 + Random.PointOnUnitSphere()*0.2f).normal();
		tempArrow = new ProjectileFiretoss( aimDir, fireSpeed );
		tempArrow->SetDamping( 0.1f );
		tempArrow->SetOwner( pOwner );
		tempArrow->SetDamage( arrowDamage );

		aimDir.dir = (aimDir.dir*3 + Random.PointOnUnitSphere()*0.2f).normal();
		tempArrow = new ProjectileFiretoss( aimDir, fireSpeed );
		tempArrow->SetDamping( 0.2f );
		tempArrow->SetOwner( pOwner );
		tempArrow->SetDamage( arrowDamage );

		return true;
	}
	// Nothing matches
	return false;
}

void SkillFiretoss::EndUse ( int use )
{
	//return false;

}

void SkillFiretoss::Update ( void )
{
	CSkill::Update();

	if ( holdState == Item::Holding && pOwner && CanUse(0) ) {
		//ps_firetoss_active->transform.position = transform.position;
		ps_firetoss_active->enabled = true;
		ps_firetoss_active->transform.localRotation = !transform.rotation;
		
		//mMagicModel->transform.position = transform.position;
		mMagicModel->transform.localScale = Vector3d(1,1,1) * 1.1f;
		mMagicModel->SetVisibility( true );

		mMagicLight->SetActive( true );
		if ( mMagicLight->range < 3.0f ) {
			mMagicLight->range += Time::deltaTime*4;
		}
		else {
			mMagicLight->range = 3.0f;
		}
	}
	else {
		ps_firetoss_active->enabled = false;
		if ( holdState == Item::Holding ) {
			//mMagicModel->transform.position = transform.position;
			mMagicModel->transform.localScale = Vector3d(1,1,1) * sqrt(fabs(std::max<ftype>( 0,  (0.6f - GetCooldown(0)/cooldown)/0.6f ))) * 1.1f;
			mMagicModel->SetVisibility( true );
			mMagicLight->SetActive( true );
			mMagicLight->range = 3.0f - (GetCooldown(0)/cooldown)*2.0f;
		}
		else {
			mMagicModel->SetVisibility( false );
			mMagicLight->SetActive( false );
			mMagicLight->range = 0;
		}
	}

	cooldown	= 3.3f - skill_lvl * 0.3f;
	mana_cost	= 8.0f + skill_lvl * 6.0f;
}