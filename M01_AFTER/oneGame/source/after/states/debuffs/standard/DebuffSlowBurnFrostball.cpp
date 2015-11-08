
#include "DebuffSlowBurnFrostball.h"

#include "after/entities/character/CCharacter.h"
#include "after/states/CRacialStats.h"
#include "core/time/time.h"

DebuffSlowBurnFrostball::DebuffSlowBurnFrostball ( CCharacter* inTarget )
	: CCharacterBuff( TYPE_ACTIVE, inTarget )
{
	can_stack = false;
	use_new_stack = true; // replace self
}
DebuffSlowBurnFrostball::~DebuffSlowBurnFrostball ( void )
{
	;
}

void DebuffSlowBurnFrostball::Initialize ( void )
{
	time = 3.0f;
}
void DebuffSlowBurnFrostball::Update ( void )
{
	time -= Time::deltaTime;

	// Damage over time
	Damage fireDamage;
	fireDamage.amount = Time::deltaTime * 4.0f/3.0f;
	fireDamage.type = DamageType::Ice | DamageType::Magical;
	fireDamage.stagger_chance = 0;

	pTarget->OnDamaged( fireDamage );

	// 50% slow wow
	pTarget->GetRacialStats()->fRunSpeed *= 0.5f;

	if ( time <= 0 ) {
		Delete();
	}
}