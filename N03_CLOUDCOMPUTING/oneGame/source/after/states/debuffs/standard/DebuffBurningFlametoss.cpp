
#include "DebuffBurningFlametoss.h"
#include "after/entities/character/CCharacter.h"
#include "core/time/time.h"

#include "engine-common/entities/CParticleSystem.h"
#include "renderer/logic/particle/CParticleEmitterSkeleton.h"
//#include "CSkinnedModel.h"
#include "after/entities/CCharacterModel.h"

DebuffBurningFlametoss::DebuffBurningFlametoss ( CCharacter* inTarget )
	: CCharacterBuff( TYPE_ACTIVE, inTarget ), m_system(NULL)
{
	can_stack = false;
	use_new_stack = true; // replace self
}
DebuffBurningFlametoss::~DebuffBurningFlametoss ( void )
{
	//delete_safe( m_system );
	if ( m_system ) {
		m_system->bAutoDestroy = true;
		m_system->enabled = false;
	}
}

void DebuffBurningFlametoss::Initialize ( void )
{
	//tick = 0.0f;
	//ticks = 0;
	time = 3.0f;

	if ( !m_system && this->pTarget )
	{
		m_system = new CParticleSystem( "particlesystems/flame01_ff.pcf", true );
		CParticleEmitterSkeleton* t_emitter = (CParticleEmitterSkeleton*) m_system->GetEmitter();
		// give that emitter some bones to emit on
		if ( t_emitter )
		{
			t_emitter->AddSkeleton( pTarget->model->GetModelLowLevel() );
		}
	}
}
void DebuffBurningFlametoss::Update ( void )
{
	//tick -= Time::deltaTime;
	
	/*while ( tick < 0 ) {
		tick += 0.25f;
		ticks += 1;

		Damage fireDamage;
		fireDamage.amount = 4;
		fireDamage.type = DamageType::Burn | DamageType::Magical;
		fireDamage.stagger_chance = 0;

		pTarget->OnDamaged( fireDamage );
	}

	if ( ticks == 16 ) {
		Delete();
	}*/
	time -= Time::deltaTime;

	Damage fireDamage;
	fireDamage.amount = Time::deltaTime * 4.0f/3.0f;
	fireDamage.type = DamageType::Burn | DamageType::Magical;
	fireDamage.stagger_chance = 0;

	pTarget->OnDamaged( fireDamage );

	if ( time <= 0 ) {
		Delete();
	}
}