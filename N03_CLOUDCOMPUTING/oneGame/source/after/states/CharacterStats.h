
#ifndef _CHARACTER_STATS_H_
#define _CHARACTER_STATS_H_

#include <algorithm>
#include <limits.h>
#include "core/types/types.h"
#include "core/types/float.h"

#include "after/types/character/Attributes.h"

class CBaseSerializer;
typedef CBaseSerializer Serializer;

class CharacterStats
{
public:
	void	ResetStats ( void ) { PointStats_SetDefault(); };

	void	UpdateStats ( void );
	void	UpdatePostDebuff ( void );

			// Unified saving and loading
	void	serialize ( Serializer &, const uint );

public:
	// Point stats
	float	fHealthPrev;
	float	fHealth;		// Only used for saving and loading
	float	fHealthMax;
	float	fHealthRegen;
	float	fHealthDelta;

	float	fStaminaPrev;
	float	fStamina;
	float	fStaminaMax;
	float	fStaminaRegen;
	float	fStaminaDelta;

	float	fManaPrev;
	float	fMana;
	float	fManaMax;
	float	fManaRegen;
	float	fManaDelta;

	// Primary stats
	unsigned int	iLevel;
	float	fExperience;
	float	fExperienceMax;

	unsigned int	iStrength;
	unsigned int	iAgility;
	unsigned int	iIntelligence;
	unsigned int	iCharisma;

	unsigned int	iOffsetStrength;
	unsigned int	iOffsetAgility;
	unsigned int	iOffsetIntelligence;

	// Time stats (used for buffs)
	float	fTimeSinceHealthDrop;
	float	fTimeSinceStaminaDrop;
	float	fTimeSinceManaDrop;

private:
	void PointStats_SetDefault ( void );
	void PointStats_SetMaxPointsFromStats ( void );
	void PointStats_DoPointMath ( void );
};

typedef CharacterStats CharStats;

#endif//_CHARACTER_STATS_H_