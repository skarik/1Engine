
#ifndef _DEBUFF_BURNING_FLAMETOSS_H_
#define _DEBUFF_BURNING_FLAMETOSS_H_

#include "../Debuffs.h"

class CParticleSystem;

class DebuffBurningFlametoss : public Debuffs::CCharacterBuff
{

public:
	explicit			DebuffBurningFlametoss ( CCharacter* inTarget );
						~DebuffBurningFlametoss ( void );

	void				Initialize ( void ) override;
	void				Update ( void ) override;

private:
	ftype				time;
	//int					ticks;
	//ftype				tick;
	CParticleSystem*	m_system;
};

#endif//_DEBUFF_BURNING_FLAMETOSS_H_