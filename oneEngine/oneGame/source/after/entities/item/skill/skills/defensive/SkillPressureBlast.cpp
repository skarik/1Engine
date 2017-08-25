#include "SkillPressureBlast.h"

#include "core/time/time.h"
#include "engine/physics/raycast/Raycaster.h"
#include "core/math/random/Random.h"

#include "after/entities/projectile/magical/ProjectileFrostball.h"

#include "engine-common/entities/CParticleSystem.h"
#include "renderer/logic/particle/CParticleEmitter.h"

#include "after/entities/character/CCharacter.h"
#include "after/entities/CCharacterModel.h"
#include "after/physics/Caster.h"

//HitType?

SkillPressureBlast::SkillPressureBlast ( void ) : CSkill( ItemData() )
{
	mana_cost	= 1;
	cooldown	= 0.0;
	passive		= false;

	ps_waterblast_active = new CParticleSystem ( ".res/particlesystems/waterfade.pcf");
	//ps_firetoss_active = new CParticleSystem( ".res/particlesystems/spells/magicbolt_projectile.pcf" );
	ps_waterblast_active->enabled = false;
	ps_waterblast_active->bAutoDestroy = false;

	/*mMagicModel = new CModel( "models/effects/magic_ball.fbx" );
	glMaterial* newMat = new glMaterial;
	newMat->loadFromFile( "effects/spell_fireball" );
	mMagicModel->SetMaterial( newMat );
	newMat->removeReference();
	mMagicModel->visible = false;
	mMagicModel->transform.Get( transform );
	mMagicModel->transform.SetParent( &transform );*/
		
	holdType = Item::SpellHanded;
	iAnimationSubset = 2;
}

SkillPressureBlast::~SkillPressureBlast ( void )
{
	delete_safe(ps_waterblast_active);
}

bool SkillPressureBlast::Use ( int use )
{
	if ( use == Item::UPrimary )
	{
		mana_cost = 6 * Time::deltaTime;
		if ( CanUse(0) && CanCast() )
		{
			
			// Decrement mana
			CastMana();
			// Put on a short cooldown
			//SetCooldown( 0,cooldown );
			Attack( XTransform(transform.position,transform.rotation) );
			if ( pOwner ) {
				((CCharacter*)pOwner)->OnRangedAttack();
			}

			if ( pOwner && pOwner->ActorType() != ACTOR_TYPE_GENERAL ) {
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Toss, iAnimationSubset, mHand, 3.0f );
			}
		}
	}
	return false;
}

// Attack function
void SkillPressureBlast::Attack ( XTransform& )
{
	// Create projectile here
	if ( isCasting )
	{
		Ray		aimDir;
		ftype	waterSpeed;
		Damage	arrowDamage;
		RaycastHit hitresult;
		CGameBehavior* hitobject;

		aimDir = pOwner->GetAimRay( transform.position );
		Item::HitType htype = Caster::Raycast(aimDir, 10, &hitresult, &hitobject, NULL, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE));
		ps_waterblast_active->transform.rotation.RotationTo(Vector3d::forward, aimDir.dir);
		if(htype == Item::HIT_CHARACTER)
		{
			Vector3d test = aimDir.dir * 15;
			test.z = 0;
			std::swap(test.x, test.y);
			test.x *= -1;
			((CCharacter*)hitobject)->ApplyLinearMotion(((CCharacter*)hitobject)->model->GetModelRotation().transpose() * test , Time::deltaTime * 2);
		}
		ps_waterblast_active->enabled = true;
		waterSpeed = 25.0f;



		//arrowDamage.amount	= 0.0f;// + skill_lvl * 1.0f;
		//arrowDamage.type	= DamageType::Magical | DamageType::Drown;
						
		/*tempArrow = new ProjectileFrostball( aimDir, waterSpeed );
		tempArrow->SetDamping( 0.1f );
		tempArrow->SetOwner( pOwner );
		tempArrow->SetDamage( arrowDamage );*/



		// Has casted, so turn that shit off
		isCasting = false;
	}
}

// CastCombine. Combines two casting skills for an effect. Is called when both skills are casted and not channelling.
// Return true if something happens. Generally, objects look through all the spells, and stop the skills by setting isCasting to false.
// Returning true will normally stop non-instant skills.
bool SkillPressureBlast::CastCombine ( CSkill* combineSkill )
{
	// Same skill here
	if ( combineSkill->GetID() == this->GetID() )
	{
		return true;
	}
	// Nothing matches
	return false;
}

void SkillPressureBlast::EndUse ( int use )
{
	
	ps_waterblast_active->enabled = false;
}

void SkillPressureBlast::Update ( void )
{
	CSkill::Update();

	if ( holdState == Item::Holding && pOwner && CanUse(0) ) {
		ps_waterblast_active->transform.position = transform.position;
		
	}
	else {
		ps_waterblast_active->enabled = false;
		
	}

	cooldown	= 4.5f - skill_lvl * 0.5f;
	mana_cost	= 1.0f * skill_lvl * skill_lvl;
}