
#include "SkillBlink.h"
#include "core/time/time.h"

#include "engine/physics/raycast/Raycaster.h"

#include "renderer/logic/particle/CParticleEmitter.h"
#include "renderer/material/glMaterial.h"

#include "engine-common/entities/CParticleSystem.h"

#include "after/entities/character/CCharacter.h"

SkillBlink::SkillBlink ( void ) : CSkill( ItemData() )
{
	mana_cost	= 20;
	cooldown	= 3.0f;
	passive		= false;

	cast_state	= 0;

	ps_blink_target = new CParticleSystem ( "particlesystems/spells/blink_target.pcf", "particle_blink" );
}

SkillBlink::~SkillBlink ( void )
{
	delete_safe(ps_blink_target);
}

bool SkillBlink::Use ( int use )
{
	if ( use == Item::UPrimary || use == Item::UPrimarySprint )
	{
		switch ( cast_state )
		{
		case 0: // Ready to cast
			if ( CanUse(0) && CanCast() ) {
				release_timer = 0;
				cast_state = 1;
			}
			break;
		case 1: // Channelling for target
			if ( CanUse(0) && CanCast() ) {
				release_timer = 0; // Wait for release before cast
			}
			else {
				cast_state = 0; // Go back to waiting if can't cast anymore
			}
			break;
		case -1:
			release_timer = 0; // Wait for release before reset
			break;
		}
	}
	else if ( use == Item::USecondary || use == Item::USecondarySprint )
	{
		if ( cast_state == 1 ) {
			cast_state = -1;	// Go to wait release state
			release_timer = 0;
		}
	}
	return true;
}

void SkillBlink::Update ( void )
{
	CSkill::Update();

	ps_blink_target->enabled = false;
	if ( cast_state == -1 )
	{
		// Work off the release casts
		if ( release_timer > 0.1f ) {
			cast_state = 0;
			SetCooldown( 0,0.2f );
		}
		release_timer += Time::deltaTime;

		((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Holster, iAnimationSubset, mHand, 0 );
	}
	else if ( cast_state == 1 )
	{
		((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Precast, iAnimationSubset, mHand, 0 );

		// Calculate blink position
		Ray		aimDir;
		aimDir = pOwner->GetAimRay( pOwner->GetEyeRay().pos );
		RaycastHit hitResult;

		if ( Raycaster.Raycast( aimDir, 24.0f, &hitResult, Physics::GetCollisionFilter( Layers::PHYS_CHARACTER ), pOwner ) )
		{
			blink_position = hitResult.hitPos + hitResult.hitNormal*0.6f;
		}
		else
		{
			blink_position = aimDir.pos + aimDir.dir.normal() * 24.0f;
		}

		// Set target position
		ps_blink_target->transform.position = blink_position;
		ps_blink_target->enabled = true;
		ps_blink_target->GetRenderable(0)->GetMaterial()->m_diffuse = GetFocusColor();
		ps_blink_target->GetRenderable(1)->GetMaterial()->m_diffuse = GetFocusColor();

		// Work off the release casts
		if ( release_timer > 0.1f ) {
			cast_state = 2;
			SetCooldown( 0,cooldown );
			CastMana();
		}
		release_timer += Time::deltaTime;
	}
	else if ( cast_state == 2 )
	{
		// Reset cast state
		/*cast_state = 0;

		// Blink to the target position
		pOwner->transform.position = blink_position;
		pOwner->transform.SetDirty();*/
		((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Cast, iAnimationSubset, mHand, 0 );

		blink_start = pOwner->transform.position;

		// Create blink effects
		CParticleSystem* ps_blink_effect;
		ps_blink_effect = new CParticleSystem ( "particlesystems/spells/blink_poof.pcf", "particle_blink" );
		ps_blink_effect->transform.position = blink_start + Vector3d( 0,0,3.0f );
		ps_blink_effect->GetEmitter()->rvVelocity.mMinVal += (blink_position-blink_start)*0.6f;
		ps_blink_effect->GetEmitter()->rvVelocity.mMaxVal += (blink_position-blink_start);
		ps_blink_effect->PostUpdate();
		ps_blink_effect->GetRenderable(0)->GetMaterial()->m_diffuse = GetFocusColor();
		ps_blink_effect->GetRenderable(1)->GetMaterial()->m_diffuse = GetFocusColor();
		ps_blink_effect->RemoveReference();
		ps_blink_effect = new CParticleSystem ( "particlesystems/spells/blink_poof.pcf", "particle_blink" );
		ps_blink_effect->transform.position = blink_position + Vector3d( 0,0,3.0f );
		ps_blink_effect->GetEmitter()->rvVelocity.mMinVal += (blink_position-blink_start)*0.6f;
		ps_blink_effect->GetEmitter()->rvVelocity.mMaxVal += (blink_position-blink_start);
		ps_blink_effect->PostUpdate();
		ps_blink_effect->GetRenderable(0)->GetMaterial()->m_diffuse = GetFocusColor();
		ps_blink_effect->GetRenderable(1)->GetMaterial()->m_diffuse = GetFocusColor();
		ps_blink_effect->RemoveReference();

		// Go to move state
		blink_timer = 0;
		cast_state = 3;
	}
	else if ( cast_state == 3 )
	{
		ftype dist = (blink_start-blink_position).magnitude();
		blink_timer += Time::deltaTime / ( 0.12f * (dist/24.0f) * 3.0f );

		pOwner->transform.position = blink_start.lerp( blink_position, sqrt(blink_timer) );
		pOwner->transform.SetDirty();

		SetViewAngleOffset( 50*(1-blink_timer) );

		if ( blink_timer > 1 )
		{
			pOwner->transform.position = blink_position;
			SetViewAngleOffset( 0 );
			// Reset cast state
			cast_state = 0;
		}

		((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Holster, iAnimationSubset, mHand, 0 );
	}
}