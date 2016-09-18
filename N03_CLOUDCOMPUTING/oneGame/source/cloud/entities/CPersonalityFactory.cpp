#include "CPersonalityFactory.h"
#include <time.h>
//Personality Types
#include "Personalities/CTestPersonality.h"
#include "Personalities/CPersonalityVanguard.h"

CPersonalityFactory *pfac = new CPersonalityFactory();

CPersonalityFactory::CPersonalityFactory (void) 
{
	numPersonalities = 1;
	srand(time(NULL));
}

CPersonalityFactory::~CPersonalityFactory (void) 
{
	if (pfac != nullptr)
		delete pfac;
}

CPersonality *CPersonalityFactory::MakePersonality (CCloudEnemy *host, int type)
{
	if (type < 0)
	{//Change the type to a random number and proceed
		type = rand() % numPersonalities;
	}
	
	//Now make the personality
	//An erroneous type value will returrn the test personality, which will just spin in circles.
	switch (type)
	{
	case 0:
		return new CPersonalityVanguard(host);
	default:
		return new CTestPersonality(host);
	}
}