
#include "PEffectBurnOnHit.h"
#include "after/entities/character/CCharacter.h"
#include "engine-common/entities/CParticleSystem.h"
#include "engine-common/entities/CProjectile.h"
#include "after/states/debuffs/standard/DebuffBurningFlametoss.h"

void PEffectBurnOnHit::Initialize ( void )
{
	ps_trail_effect = new CParticleSystem( ".res/particlesystems/spells/firetoss_projectile.pcf" );
	ps_trail_effect->transform.position = pTarget->transform.position;
	ps_trail_effect->transform.SetParent( &pTarget->transform );
}

PEffectBurnOnHit::~PEffectBurnOnHit ( void )
{
	//delete_safe( ps_trail_effect );
	ps_trail_effect->enabled = false;
	ps_trail_effect->RemoveReference();
}

bool PEffectBurnOnHit::OnHitEnemy ( CActor* enemy )
{
	if ( enemy->ActorType() == ACTOR_TYPE_PLAYER || enemy->ActorType() == ACTOR_TYPE_NPC )
	{
		((CCharacter*)enemy)->AddBuff<DebuffBurningFlametoss>();
	}

	return true;
}