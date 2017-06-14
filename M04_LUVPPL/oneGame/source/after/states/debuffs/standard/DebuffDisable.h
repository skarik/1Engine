#ifndef _DEBUFF_DISABLE_H_
#define _DEBUFF_DISABLE_H_

#include "../Debuffs.h"

class CBloomShaders;

class DebuffDisable : public Debuffs::CCharacterBuff
{
public:

	DebuffDisable(CCharacter* inTarget);
	~DebuffDisable(void);

	void Update(void) override;

private:
	bool magicDisable;
	bool physDisable;
	bool manaDisable;
	bool stamDisable;

	ftype duration;
	

};

#endif