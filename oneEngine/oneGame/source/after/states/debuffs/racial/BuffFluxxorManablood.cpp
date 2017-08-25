
#include "BuffFluxxorManablood.h"
#include "after/entities/character/CCharacter.h"
#include "core/math/Math.h"

BuffFluxxorManablood::BuffFluxxorManablood ( CCharacter* inTarget )
	: CCharacterBuff( TYPE_ACTIVE, inTarget )
{
	can_stack = false;
	use_new_stack = true; // replace self
	positive = 2;
}
BuffFluxxorManablood::~BuffFluxxorManablood ( void )
{
	;
}
/*
void BuffFluxxorManablood::Initialize ( void )
{
	
}*/
void BuffFluxxorManablood::Update ( void )
{
	// Get target difference
	ftype fManaDelta = pTarget->stats.fMana - pTarget->stats.fManaPrev;
	ftype fStamDelta = pTarget->stats.fStamina - pTarget->stats.fStaminaPrev;

	ftype fTotalDelta = fManaDelta + fStamDelta;
	if ( fTotalDelta > 0 ) {
		fTotalDelta = std::max<ftype>( fManaDelta,fStamDelta );
	}
	// Reset max stamina to mana
	pTarget->stats.fStaminaMax = pTarget->stats.fManaMax;
	// Apply mana difference
	pTarget->stats.fMana = pTarget->stats.fManaPrev + fTotalDelta;
	// Set stamina equal to mana
	pTarget->stats.fStamina = pTarget->stats.fMana;
}
void BuffFluxxorManablood::PostStatsUpdate ( void )
{
	
}