
#include "SkillMagikinesis.h"

#include "core/time/time.h"
#include "core/math/random/Random.h"

#include "engine/physics/raycast/Raycaster.h"

#include "engine/physics/motion/CRigidbody.h"

#include "after/states/inventory/CPlayerInventory.h"
#include "after/entities/item/ammo/ItemPebble.h"
#include "after/entities/projectile/projectile/ProjectilePebble.h"

#include "engine-common/entities/CParticleSystem.h"
#include "renderer/logic/particle/CParticleEmitter.h"

#include "after/entities/character/CCharacter.h"
#include "engine/behavior/CGameHandle.h"

SkillMagikinesis::SkillMagikinesis ( void ) : CSkill( ItemData() )
{
	mana_cost	= 2;
	cooldown	= 3.0f;
	passive		= false;

	cast_state	= 0;

	ps_magikinesis_active = new CParticleSystem ( ".res/particlesystems/spells/blink_target.pcf", "particle_blink" );

	currentAmmo = NULL;
}

SkillMagikinesis::~SkillMagikinesis ( void )
{
	delete_safe(ps_magikinesis_active);
}

// left click throws pebble out
// right click sends them to target

//however, others can grab pebbles

bool SkillMagikinesis::Use ( int use )
{
	if ( use == Item::UPrimary )
	{
		if ( CanUse(0) && CanCast() ) {
			if ( hasAmmo ) {
				if ( currentAmmo ) {
					currentAmmo->SetStackSize( currentAmmo->GetStackSize() - 1 );
					// Decrement mana
					CastMana();
					// Put on a short cooldown
					SetCooldown( 0,0.3f );
					// Go into primary cast state
					cast_state = 1;
					// Set power timer
					power_timer = 3.0f;

					// Get aim dir
					Ray aimDir = pOwner->GetAimRay( pOwner->GetEyeRay().pos );
					
					// Create the pebble
					ItemPebble* newPebble = new ItemPebble();
					newPebble->transform.position = aimDir.pos + aimDir.dir * 1.6f + Random.PointOnUnitSphere()*0.4f;
					newPebble->transform.SetDirty();
					newPebble->SetHoldState( Item::Hover );
					newPebble->CreatePhysics();
					newPebble->GetRigidbody()->SetMotionType( physMotion::MOTION_KEYFRAMED );
					newPebble->GetRigidbody()->SetVelocity( aimDir.dir * 0.4f );

					pebble_list.push_back( newPebble->GetHandle() );
				}
			}
		}
		return true;
	}
	else if ( use == Item::USecondary )
	{
		// First, check pebbles for validity
		for ( uint i = 0; i < pebble_list.size(); ++i )
		{
			if ( pebble_list[i].valid() )
			{
				if ( ((ItemPebble*)(CGameBehavior*)pebble_list[i])->GetHoldState() != Item::Hover )
				{
					pebble_list.erase( pebble_list.begin() + i );
					i -= 1;
				}
			}
			else
			{
				pebble_list.erase( pebble_list.begin() + i );
				i -= 1;
			}
		}

		if ( CanUse(0) && CanCast() )
		{
			// Decrement mana + set cooldown
			mana_cost = 2.0f * pebble_list.size();
			CastMana();
			SetCooldown( 0,cooldown );
			mana_cost = 2;

			if ( pOwner ) {
				((CCharacter*)pOwner)->OnRangedAttack();
			}

			// With all pebbles still in the list, delete them and turn them into projectiles going to where player is facing.

			// Calculate hit position
			Ray		aimDir;
			aimDir = pOwner->GetAimRay( pOwner->GetEyeRay().pos );
			RaycastHit hitResult;
			if ( Raycaster.Raycast( aimDir, 200.0f, &hitResult, Physics::GetCollisionFilter( Layers::PHYS_CHARACTER ) ) ) {
				target_position = hitResult.hitPos + hitResult.hitNormal*0.2f;
			}
			else {
				target_position = aimDir.pos + aimDir.dir.normal() * 200.0f;
			}

			for ( uint i = 0; i < pebble_list.size(); ++i )
			{
				Ray target;
				target.pos = ((ItemPebble*)(CGameBehavior*)pebble_list[i])->transform.position;
				target.dir = (target_position - target.pos).normal();

				Damage rockDmg;
				rockDmg.amount = 3.0f+3*skill_lvl;
				rockDmg.type = DamageType::Bullet | DamageType::Pierce | DamageType::Physical;
				rockDmg.actor = pOwner;
				rockDmg.direction = target.dir;
				rockDmg.source = target.pos;

				ProjectilePebble* newBolt = new ProjectilePebble ( target, 100.0f );
				newBolt->SetDamping( 0.05f, 0.95f, 220.0f );
				newBolt->SetOwner( pOwner );
				newBolt->SetDamage( rockDmg );

				//delete (*(pebble_list[i]));
				DeleteObject( pebble_list[i] );
			}
			pebble_list.clear();

			// Go to initial cast state
			cast_state = 0;
		}
		return true;
	}
	return false;
}
void SkillMagikinesis::EndUse ( int use )
{
	if ( use == Item::UPrimary )
	{
		if ( CanCast() && (cast_state == 1) ) {
			SetCooldown( 0,0.0f );
		}
	}
}

void SkillMagikinesis::Update ( void )
{
	CSkill::Update();

	// Search the inventory for ammo
	hasAmmo = false;
	currentAmmo = NULL;
	if ( pOwner )
	{
		CInventory* inv = pOwner->GetInventory();
		if ( inv ) {
			currentAmmo = (ItemPebble*)inv->FindItem( 12 ); 
			if ( currentAmmo ) {
				hasAmmo = true;
			}	
		}
	}

	// Check power timer and state
	if ( cast_state == 1 )
	{
		// Have pebbles in stasis, remove last one when time runs out
		power_timer -= Time::deltaTime;
		if ( power_timer < 0.0f ) {
			power_timer += 3.0f;

			// Remove pebble in front of list
			if ( pebble_list[0].valid() )
			{
				((ItemPebble*)(CGameBehavior*)pebble_list[0])->SetHoldState( Item::None );
				pebble_list.erase( pebble_list.begin() );
			}
		}
		if ( pebble_list.empty() ) {
			cast_state = 0; // Go back to neutral state when no more pebbles
		}

		// Set effect positions
		if ( pOwner ) {
			ps_magikinesis_active->transform.position = pOwner->GetHoldTransform().position;
			ps_magikinesis_active->enabled = true;
		}
	}
	else
	{
		// Set effect positions
		ps_magikinesis_active->enabled = false;
	}

	// Check pebble list size
	if ( pebble_list.size() > (ushort)(5+2*skill_lvl) )
	{
		// Remove pebble in front of list
		if ( pebble_list[0].valid() )
		{
			((ItemPebble*)(CGameBehavior*)pebble_list[0])->SetHoldState( Item::None );
			pebble_list.erase( pebble_list.begin() );
		}
	}
}