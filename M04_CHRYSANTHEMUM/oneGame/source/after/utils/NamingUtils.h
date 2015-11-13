
#ifndef _NAMING_UTILS_H_
#define _NAMING_UTILS_H_

#include "core/containers/arstring.h"
#include "after/types/character/Attributes.h"
#include "after/types/world/Resources.h"

namespace NamingUtils
{
	enum eGeography
	{
		GEO_MOUNTAIN = 0,
		GEO_VALLEY,
		GEO_PLAINS,
		GEO_FOREST,
		GEO_SEASIDE,
		GEO_BORDER,

		GEO_HELL = 254,
		GEO_WHAT = 255
	};

	struct sNameInfo
	{
		eCharacterRace race;
		eGeography geo;
		World::eResourceType resource;
	};

	/*enum eCharacterRace
	{
		CRACE_DEFAULT = 0,
		CRACE_HUMAN,		
		CRACE_ELF,			
		CRACE_DWARF,		
		CRACE_KITTEN,		
		CRACE_FLUXXOR,		
		CRACE_MERCHANT,		

		CRACE_SAME		= 254,
		CRACE_RANDOM	= 255
	};*/

	/*enum eResources 
	{
		RES_WOOD = 0,
		RES_STONE,
		RES_FARMLAND,
		RES_LUXURY,

		RES_OHGOD = 254,
		RES_BIG_NOOO = 255
	};*/

	arstring<128>	CreateBaseTownName ( void );
	arstring<128>	CreateDetailedTownName ( sNameInfo base );

	arstring<128>	CreateHumanTownName ( sNameInfo base );
	arstring<128>	CreateGypsyTownName ( sNameInfo base );
	arstring<128>	CreateDwarfTownName ( sNameInfo base);
	arstring<128>	CreateMeowTownName ( sNameInfo base );
	arstring<128>	CreateFluxTownName ( sNameInfo base );

	arstring<128>	CreateCharacterName ( void );
};


#endif//_NAMING_UTILS_H_