
#include "CSpeechTrie.h"
#include <algorithm>


CSpeechTrie::CSpeechTrie ( void )
{
	rootKeys.fill(NULL);
}
CSpeechTrie::~CSpeechTrie ( void )
{
	for ( unsigned int i = 0; i < rootKeys.size(); ++i )
	{
		if ( rootKeys[i] != NULL )
		{

		}
	}
}


void CSpeechTrie::SetKey ( const NPC::eGeneralSpeechType speech, const eCharacterGender gender, const eCharacterRace race, const char* str )
{
	speechKey_t* speechResult = NULL;
	for ( unsigned int i = 0; i < rootKeys.size(); ++i )
	{
		if ( rootKeys[i] && rootKeys[i]->value == speech )
		{
			speechResult = rootKeys[i];
			break;
		}
	}
	if ( speechResult == nullptr )
	{
		// Add speech result to the tree
		for ( unsigned int i = 0; i < rootKeys.size(); ++i )
		{
			if ( rootKeys[i] != NULL )
			{
				continue;
			}
			else
			{
				rootKeys[i] = new speechKey_t();
				rootKeys[i]->value = speech;
				rootKeys[i]->children.fill(NULL);
				speechResult = rootKeys[i];
				break;
			}
		}
	}

	std::array<genderKey_t*,2>& genderKeys = speechResult->children;
	genderKey_t* genderResult = NULL;
	for ( unsigned int i = 0; i < genderKeys.size(); ++i )
	{
		if ( genderKeys[i] && genderKeys[i]->value == gender )
		{
			genderResult = genderKeys[i];
			break;
		}
	}
	if ( genderResult == NULL )
	{
		// Add gender result to the tree
		for ( unsigned int i = 0; i < genderKeys.size(); ++i )
		{
			if ( genderKeys[i] != NULL )
			{
				continue;
			}
			else
			{
				genderKeys[i] = new genderKey_t();
				genderKeys[i]->children.fill(NULL);
				genderKeys[i]->value = gender;
				genderResult = genderKeys[i];
				break;
			}
		}
	}

	std::array<raceKey_t*,7>& raceKeys = genderResult->children;
	raceKey_t* raceResult = NULL;
	for ( unsigned int i = 0; i < raceKeys.size(); ++i )
	{
		if ( raceKeys[i] && raceKeys[i]->value == race )
		{
			raceResult = raceKeys[i];
			break;
		}
	}
	if ( raceResult == NULL )
	{
		// Add race result to the tree
		for ( unsigned int i = 0; i < raceKeys.size(); ++i )
		{
			if ( raceKeys[i] != NULL )
			{
				continue;
			}
			else
			{
				raceKeys[i] = new raceKey_t();
				raceKeys[i]->value = race;
				raceKeys[i]->child = new char [strlen(str)+1];
				strcpy( raceKeys[i]->child, str );
				break;
			}
		}
	}

}


char* CSpeechTrie::GetKey ( const NPC::eGeneralSpeechType speech, const eCharacterGender gender, const eCharacterRace race ) const
{
	// Look for speech key
	auto speechResult = rootKeys.end();
	for ( auto it = rootKeys.begin(); it != rootKeys.end(); ++it )
	{
		if ( (*it) && (*it)->value == speech )
		{
			speechResult = it;
			break;
		}
	}
	if ( speechResult != rootKeys.end() )
	{
		// Look for gender key
		std::array<genderKey_t*,2>& genderKeys = (*speechResult)->children;
		auto genderResult = genderKeys.end();
		for ( auto it = genderKeys.begin(); it != genderKeys.end(); ++it )
		{
			if ( (*it) && (*it)->value == gender )
			{
				genderResult = it;
				break;
			}
		}
		if ( genderResult != genderKeys.end() )
		{
			// Look for race key
			std::array<raceKey_t*,7>& raceKeys = (*genderResult)->children;
			auto raceResult = raceKeys.end();
			for ( auto it = raceKeys.begin(); it != raceKeys.end(); ++it )
			{
				if ( (*it) && (*it)->value == race )
				{
					raceResult = it;
					break;
				}
			}
			if ( raceResult != raceKeys.end() )
			{
				// At last level, return string
				return (*raceResult)->child;
			}
		}
	}

	return NULL;
}