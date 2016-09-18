#ifndef _C_PERSONALITY_FACTORY_H_
#define _C_PERSONALITY_FACTORY_H_

#include "CPersonality.h"
#include <stdlib.h>

class CPersonalityFactory
{
public:
	CPersonalityFactory(void);
	~CPersonalityFactory(void);

	CPersonality * MakePersonality(CCloudEnemy * host, int type);

private:
	int numPersonalities;
};

extern CPersonalityFactory *pfac;
#endif