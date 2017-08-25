
#include "Cultures.h"

namespace World
{
	//  GetDominantCulture
	// Given an array of culture values, returns the most influential culture.
	eCultureType GetDominantCulture ( const ftype* cultureList, const int listLength )
	{
		ftype			largestCultureStrength = 0;
		eCultureType	largestCulture = CULTURE_EUROPEAN;
		for ( uint i = 0; i < 3; ++i )
		{
			if ( cultureList[i] > largestCultureStrength ) {
				largestCultureStrength = cultureList[i];
				largestCulture = (eCultureType)i;
			}
		}
		return largestCulture;
	}
}