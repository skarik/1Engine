#ifndef _DEBUFF_DPS_H_
#define _DEBUFF_DPS_H_

#include "../Debuffs.h"

class CBloomShaders;

class DebuffDPS : public Debuffs::CCharacterBuff
{

public:

	explicit	DebuffDPS ( CCharacter* inTarget );
				~DebuffDPS ( void );
	
	void		Update( void ) override;		

private:
	ftype duration;
	ftype damagePerTick;
	ftype tickTime;
};


#endif