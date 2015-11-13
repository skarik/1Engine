#ifndef _DEBUFF_SLOW_H_
#define _DEBUFF_SLOW_H_

#include "../Debuffs.h"

class CBloomShader;

class DebuffSlow : public Debuffs::CCharacterBuff
{

public:

	explicit		DebuffSlow ( CCharacter *inTarget );
					~DebuffSlow ( void );

	void			Update (void) override;


private:
	ftype slowPercent;			// What starting percentage the slow has
	ftype duration;				// How long the slow lasts in total
	ftype timeLeft;				// How long the debuff effect still has to last.
	ftype pctReductionPerRick;	// If the slow tappers off over the duration use this, set to zero otherwise
	ftype pctReducTickTime;		// Defines the interval in which the slow tappers off. 0 if it doesn't reduce over the life time of the slow. Can't be greater then duration.
	//CBloomShader*	bloomTarget;

};

#endif
