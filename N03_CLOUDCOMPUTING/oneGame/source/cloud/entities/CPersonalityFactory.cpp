#include "CPersonalityFactory.h"

#include "Personalities/CTestPersonality.h"

CPersonalityFactory::CPersonalityFactory (void) {}

CPersonalityFactory::~CPersonalityFactory (void) {}

CPersonality *CPersonalityFactory::MakePersonality (int type)
{
	//For now, only have the test personality. Will have more
	return new CTestPersonality();
}