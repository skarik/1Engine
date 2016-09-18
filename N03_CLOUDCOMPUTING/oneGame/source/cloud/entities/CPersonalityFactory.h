#ifndef _C_PERSONALITY_FACTORY_H_
#define _C_PERSONALITY_FACTORY_H_

#include "CPersonality.h"
#include <random>

class CPersonalityFactory
{
public:
	CPersonalityFactory(void);
	~CPersonalityFactory(void);

	CPersonality * MakePersonality(CCloudEnemy * host, int type);

	int GetRand(void);
private:
	int numPersonalities;
	std::mt19937 *mt;
};

extern CPersonalityFactory *pfac;
#endif