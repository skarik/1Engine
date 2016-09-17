#ifndef _C_PERSONALITY_FACTORY_H_
#define _C_PERSONALITY_FACTORY_H_

#include "CPersonality.h"

class CPersonalityFactory
{
public:
	CPersonalityFactory(void);
	~CPersonalityFactory(void);

	CPersonality * MakePersonality(int type);

private:
};

#endif