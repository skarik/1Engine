#include "CPersonalityFactory.h"
//Personality Types
#include "Personalities/CTestPersonality.h"
#include "Personalities/CPersonalityVanguard.h"
#include "Personalities/CPersonalityScout.h"
#include "Personalities/CPersonalityMob.h"

CPersonalityFactory *pfac = new CPersonalityFactory();

CPersonalityFactory::CPersonalityFactory (void) 
{
	numPersonalities = 3;
	std::random_device rd;

	mt = new std::mt19937(rd());
}

CPersonalityFactory::~CPersonalityFactory (void) 
{
	if (pfac != nullptr)
		delete pfac;

	if (mt)
		delete mt;
	mt = NULL;
}

CPersonality *CPersonalityFactory::MakePersonality (CCloudEnemy *host, int type)
{
	if (type < 0)
	{//Change the type to a random number and proceed
		type = (*mt)() % numPersonalities;
	}
	
	//Now make the personality
	//An erroneous type value will returrn the test personality, which will just spin in circles.
	switch (type)
	{
	case 0:
		return new CPersonalityVanguard(host);
	case 1:
		return new CPersonalityScout(host);
	case 2:
		return new CPersonalityMob(host);
	default:
		return new CTestPersonality(host);
	}
}