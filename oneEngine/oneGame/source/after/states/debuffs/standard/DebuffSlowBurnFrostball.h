
#ifndef _DEBUFF_SLOW_BURN_FROSTBALL_H_
#define _DEBUFF_SLOW_BURN_FROSTBALL_H_

#include "../Debuffs.h"


class DebuffSlowBurnFrostball : public Debuffs::CCharacterBuff
{

public:
	explicit			DebuffSlowBurnFrostball ( CCharacter* inTarget );
						~DebuffSlowBurnFrostball ( void );

	void				Initialize ( void ) override;
	void				Update ( void ) override;

private:
	ftype				time;
	//int					ticks;
	//ftype				tick;
};

#endif//_DEBUFF_SLOW_BURN_FROSTBALL_H_