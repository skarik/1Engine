
#ifndef _BUFF_RACIAL_DARK_ELF_H_
#define _BUFF_RACIAL_DARK_ELF_H_

#include "../Debuffs.h"

class CBloomShader;

class BuffRacialDarkElf : public Debuffs::CCharacterBuff
{
public:
	explicit			BuffRacialDarkElf ( CCharacter* inTarget );
						~BuffRacialDarkElf ( void );

	void				Update ( void ) override;

private:
	ftype				currentStrength;
	CBloomShader*		bloomTarget;
};

#endif//_BUFF_RACIAL_DARK_ELF_H_