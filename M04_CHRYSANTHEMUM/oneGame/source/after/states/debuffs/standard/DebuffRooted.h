#ifndef _DEBUFF_ROOTED_H_
#define _DEBUFF_ROOTED_H_

#include "../Debuffs.h"

class CBloomShader;

class DebuffRooted : public Debuffs::CCharacterBuff
{
public:

	DebuffRooted ( CCharacter* inTarget );
	~DebuffRooted ( void );

	void Update ( void ) override;

private:
	//Determines what type of movements are allowed by the root debuff
	bool canPitch;
	bool canYaw;
	bool canRoll;
	
	//Sets a Maximum degree to each movement
	ftype maxPitch;
	ftype maxYaw;
	ftype maxRoll;

	//Set Values for Duration and how much it
	ftype duration;
	ftype degradePecent;
	ftype degradeTick;
};

#endif