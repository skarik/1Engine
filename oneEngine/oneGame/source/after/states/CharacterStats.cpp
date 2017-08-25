
#include "CharacterStats.h"
#include "core/time/time.h"
#include "core/math/Math.h"
#include "core-ext/system/io/serializer.h"

// == Public Calculation Routines ==

// UpdateStats()
//  Happens at beginning of the character update, before buffs have been performed
void CharacterStats::UpdateStats ( void )
{
	// Update max points
	PointStats_SetMaxPointsFromStats();


}
// UpdatePostDebuff()
//  Happens after the debuffs have had their primary stat-editing calculations
void CharacterStats::UpdatePostDebuff ( void )
{
	// Update regen and other point maths
	PointStats_DoPointMath();
}


// == Private Calculation Routines ==

// PointStats_SetDefault()
//  Set defaults stats
void CharacterStats::PointStats_SetDefault ( void )
{
	// Set init health
	fHealthMax	= 100;
	fHealth		= fHealthMax;
	fHealthPrev	= fHealth;

	// Set init stamina and mana
	fManaMax	= 100;
	fMana		= fManaMax;
	fManaPrev	= fMana;
	fStaminaMax = 100;
	fStamina	= fStaminaMax;
	fStaminaPrev= fStamina;

	// Set init level
	iLevel		= 1;
	fExperience	= 0;

	// Set 15/15/15 stats
	iStrength		= 15;
	iAgility		= 15;
	iIntelligence	= 15;

	// Set 1/1/1 offset stats
	iOffsetStrength		= 1;
	iOffsetAgility		= 1;
	iOffsetIntelligence = 1;
}
// PointStats_SetMaxPointsFromStats()
//  Sets max health, stamina, and mana based on the current stats.
//  Also, calculates the target regen based on the stats
void CharacterStats::PointStats_SetMaxPointsFromStats ( void )
{
	// Calculate health
	fHealthMax	= (ftype)(( 4 )*iStrength + 40);
	fStaminaMax = (ftype)(( 2 )*iAgility + 20);
	fManaMax	= (ftype)(( 3 )*iIntelligence + 5);

	// Calculate regens
	fHealthRegen	= ( fTimeSinceHealthDrop < 0.3f ) ? 0.0f : (( fTimeSinceHealthDrop < 20.0f ) ? ( iStrength * 0.1f * 0.2f ) : ( iStrength * 0.5f ));
	fStaminaRegen	= ( fTimeSinceStaminaDrop < 1.5f ) ? ( iAgility * 0.02f ) : ( iAgility * 0.4f ); // *0.5 is too high
	fManaRegen		= ( iIntelligence / 30.0f ) * std::min( iIntelligence * 0.8f, fTimeSinceManaDrop );

	// Calculate experience
	fExperienceMax	= (0.8f*Math.Sqr(iLevel+10)) - iLevel;
}

// PointStats_DoPointMath()
//  Does a lot of point math stuff
void CharacterStats::PointStats_DoPointMath ( void )
{
	// Do negative point limiting
	// health isn't here because it is allowed for go below zero ( think like death )
	if ( fStamina < 0 )
		fStamina = 0;
	if ( fMana < 0 )
		fMana = 0;

	// Work timers based on previous point values
	fTimeSinceHealthDrop	+= Time::smoothDeltaTime;
	fTimeSinceStaminaDrop	+= Time::smoothDeltaTime;
	fTimeSinceManaDrop		+= Time::smoothDeltaTime;
	if ( fHealth < fHealthPrev )
		fTimeSinceHealthDrop = 0;
	if ( fStamina < fStaminaPrev )
		fTimeSinceStaminaDrop = 0;
	if ( fMana < fManaPrev )
		fTimeSinceManaDrop = 0;

	// Do regen (only if alive)
	if ( fHealth > 0 )
	{
		fHealth		+= fHealthRegen * Time::smoothDeltaTime;
		fStamina	+= fStaminaRegen * Time::smoothDeltaTime;
		fMana		+= fManaRegen * Time::smoothDeltaTime;
	}

	// Do positive point limiting
	if ( fHealth > fHealthMax )
		fHealth = fHealthMax;
	if ( fStamina > fStaminaMax )
		fStamina = fStaminaMax;
	if ( fMana > fManaMax )
		fMana = fManaMax;

	// Set delta values
	fHealthDelta = fHealthPrev-fHealth;
	fStaminaDelta = fStaminaPrev-fStamina;
	fManaDelta = fManaPrev-fMana;

	// Set previous values
	fHealthPrev	= fHealth;
	fStaminaPrev= fStamina;
	fManaPrev	= fMana;
}


void CharacterStats::serialize ( Serializer & ser, const uint ver )
{
	// Point stats
	ser & fHealth;
	ser & fHealthMax;

	ser & fStamina;
	ser & fStaminaMax;

	ser & fMana;
	ser & fManaMax;

	// Primary stats
	ser & (uint32_t&)iLevel;
	ser & fExperience;

	ser & iStrength;
	ser & iAgility;
	ser & iIntelligence;
	ser & iCharisma;

	ser & iOffsetStrength;
	ser & iOffsetAgility;
	ser & iOffsetIntelligence;


	if ( iLevel <= 0 ) {
		iLevel = 1;
	}
}