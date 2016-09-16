
#ifndef _WORLD_CULTURES_H_
#define _WORLD_CULTURES_H_

#include "core/types/types.h"

namespace World
{
	//  eCultureType
	// World culture enumeration.
	// These are all the possible cultures to use in the game.
	enum eCultureType {
		CULTURE_EUROPEAN	= 0,	// Southwestern European
		CULTURE_ASIAN		= 1,	// Eastern Asian
		CULTURE_ARCANA		= 2,	// Eastern Europe

		CULTURE_DESERT		= 3,	// North Africa to Middle East
		CULTURE_ISLANDER	= 4,	// Tahitian
		CULTURE_NORDIC		= 5,	// North European
	};

	//  GetDominantCulture
	// Given an array of culture values, returns the most influential culture.
	eCultureType GetDominantCulture ( const ftype* cultureList, const int listLength );
};

#endif _WORLD_CULTURES_H_