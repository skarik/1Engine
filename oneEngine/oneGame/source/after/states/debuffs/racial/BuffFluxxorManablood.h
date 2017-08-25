
#ifndef _BUFF_FLUXXOR_MANABLOOD_H_
#define _BUFF_FLUXXOR_MANABLOOD_H_

#include "../Debuffs.h"

class BuffFluxxorManablood : public Debuffs::CCharacterBuff
{
public:
	explicit			BuffFluxxorManablood ( CCharacter* inTarget );
						~BuffFluxxorManablood ( void );

	void				Update ( void ) override;

	void				PostStatsUpdate ( void ) override;

private:
	//CBloomShader*		bloomTarget;
};

#endif//_BUFF_FLUXXOR_MANABLOOD_H_