
#ifndef _BUFF_RACIAL_KITTEN_H_
#define _BUFF_RACIAL_KITTEN_H_

#include "../Debuffs.h"

class CBloomShader;

class BuffRacialKitten : public Debuffs::CCharacterBuff
{
public:
	explicit			BuffRacialKitten ( CCharacter* inTarget );
						~BuffRacialKitten ( void );

	void				Update ( void ) override;
private:
	ftype				currentStrength;
	CBloomShader*		bloomTarget;
};

#endif//_BUFF_RACIAL_KITTEN_H_