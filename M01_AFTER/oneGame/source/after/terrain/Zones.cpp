
#include "Zones.h"
//#include "CVoxelTerrain.h"
#include "after/types/terrain/TerrainTypes.h"
#include "after/types/terrain/BiomeTypes.h"

// Global instance
CZoneChecker Zones;


//
// Returns:
//		int - counted difference between the terrain types
//		A value of 4 is typically the "stop" zone. Anything higher is a full stop.
//
int		CZoneChecker::TerraCompare		( const char srcTerra, const char dstTerra )
{
	using namespace Terrain;

	if ( srcTerra == dstTerra ) {
		return 0;
	}

	switch ( srcTerra )
	{
	case TER_ISLANDS:
	case TER_SPIRES:
		switch ( dstTerra )
		{
			case TER_OCEAN:		return 4;
			case TER_ISLANDS:
			case TER_SPIRES:	return 1;
			case TER_FLATLANDS:	return 2;
			case TER_HILLLANDS:	return 3;
			case TER_MOUNTAINS:	return 4;
			default:			return 5;
		}
		break;
	case TER_FLATLANDS:
		switch ( dstTerra )
		{
			case TER_OCEAN:		return 4;
			case TER_ISLANDS:	return 1;
			case TER_SPIRES:	return 2;
			case TER_HILLLANDS:	return 2;
			case TER_MOUNTAINS:	return 4;
			default:			return 5;
		}
		break;
	case TER_HILLLANDS:
		switch ( dstTerra )
		{
			case TER_ISLANDS:	return 2;
			case TER_SPIRES:	return 3;
			case TER_FLATLANDS: return 1;
			case TER_MOUNTAINS:	return 4;
			default:			return 5;
		}
		break;
	case TER_MOUNTAINS:
		switch ( dstTerra )
		{
			case TER_FLATLANDS: return 4;
			case TER_HILLLANDS:	return 3;
			default:			return 5;
		}
		break;
	case TER_DESERT:
		switch ( dstTerra )
		{
			case TER_BADLANDS:	return 3;
			case TER_ISLANDS:	return 4;
			case TER_SPIRES:	return 4;
			case TER_FLATLANDS: return 4;
			default:			return 5;
		}
		break;
	case TER_BADLANDS:
		switch ( dstTerra )
		{
			case TER_DESERT:	return 3;
			case TER_OUTLANDS:	return 4;
			case TER_HILLLANDS: return 3;
			default:			return 5;
		}
		break;
	case TER_OUTLANDS:
		switch ( dstTerra )
		{
			case TER_BADLANDS:	return 4;
			case TER_MOUNTAINS: return 4;
			default:			return 5;
		}
		break;
	case TER_GLACIER:
		switch ( dstTerra )
		{
			case TER_FLATLANDS:
			case TER_HILLLANDS:	return 4;
			case TER_ISLANDS:	return 3;
			case TER_SPIRES:	return 3;
			case TER_OCEAN:		return 4;
			default:			return 5;
		}
		break;
	case TER_OCEAN:
		return 5;
	}

	return 5;
}
//
// Returns:
//		int - counted difference between the terrain types
//		A value of 4 is typically the "stop" zone. Anything higher is a full stop.
//
int		CZoneChecker::BiomeCompare		( const char srcBiome, const char dstBiome )
{
	using namespace Terrain;

	if ( srcBiome == dstBiome ) {
		return 0;
	}

	switch ( srcBiome )
	{
	case BIO_S_TUNDRA:
		switch ( dstBiome ) {
			case BIO_TUNDRA:
			case BIO_TAIGA:
			case BIO_COLD_DESERT:	return 4;
			default:				return 5;
		}
		break;
	case BIO_TUNDRA:
	case BIO_TAIGA:
	case BIO_COLD_DESERT:
		switch ( dstBiome ) {
			case BIO_TUNDRA:
			case BIO_TAIGA:
			case BIO_COLD_DESERT:	return 0;
			case BIO_SAVANNA:
			case BIO_EVERGREEN:		return 3;
			case BIO_GRASSLAND:
			case BIO_TEMPERATE:		return 4;
			case BIO_DESERT:
			case BIO_GRASS_DESERT:	return 4;
			default:				return 5;
		}
		break;
	case BIO_SAVANNA:
	case BIO_EVERGREEN:
		switch ( dstBiome ) {
			case BIO_TUNDRA:
			case BIO_TAIGA:
			case BIO_COLD_DESERT:	return 3;
			case BIO_SAVANNA:
			case BIO_EVERGREEN:		return 0;
			case BIO_GRASSLAND:
			case BIO_TEMPERATE:		return 2;
			case BIO_TROPICAL:
			case BIO_RAINFOREST:	return 3;
			case BIO_MOIST:
			case BIO_SWAMP:			return 4;
			case BIO_DESERT:
			case BIO_GRASS_DESERT:	return 3;
			default:				return 5;
		}
		break;
	case BIO_GRASSLAND:
	case BIO_TEMPERATE:
		switch ( dstBiome ) {
			case BIO_TUNDRA:
			case BIO_TAIGA:
			case BIO_COLD_DESERT:	return 3;
			case BIO_SAVANNA:
			case BIO_EVERGREEN:		return 2;
			case BIO_GRASSLAND:
			case BIO_TEMPERATE:		return 0;
			case BIO_TROPICAL:
			case BIO_RAINFOREST:	return 2;
			case BIO_MOIST:
			case BIO_SWAMP:			return 3;
			case BIO_DESERT:
			case BIO_GRASS_DESERT:	return 3;
			default:				return 5;
		}
		break;
	case BIO_TROPICAL:
	case BIO_RAINFOREST:
		switch ( dstBiome ) {
			case BIO_TUNDRA:
			case BIO_TAIGA:
			case BIO_COLD_DESERT:	return 4;
			case BIO_SAVANNA:
			case BIO_EVERGREEN:		return 3;
			case BIO_GRASSLAND:
			case BIO_TEMPERATE:		return 2;
			case BIO_TROPICAL:
			case BIO_RAINFOREST:	return 0;
			case BIO_MOIST:
			case BIO_SWAMP:			return 2;
			case BIO_DESERT:
			case BIO_GRASS_DESERT:	return 4;
			default:				return 5;
		}
		break;
	case BIO_MOIST:
	case BIO_SWAMP:
		switch ( dstBiome ) {
			case BIO_TUNDRA:
			case BIO_TAIGA:
			case BIO_COLD_DESERT:	return 4;
			case BIO_SAVANNA:
			case BIO_EVERGREEN:		return 3;
			case BIO_GRASSLAND:
			case BIO_TEMPERATE:		return 2;
			case BIO_TROPICAL:
			case BIO_RAINFOREST:	return 2;
			case BIO_MOIST:
			case BIO_SWAMP:			return 0;
			case BIO_DESERT:
			case BIO_GRASS_DESERT:	return 4;
			default:				return 5;
		}
		break;
	case BIO_DESERT:
	case BIO_GRASS_DESERT:
		switch ( dstBiome ) {
			case BIO_TUNDRA:
			case BIO_TAIGA:
			case BIO_COLD_DESERT:	return 5;
			case BIO_SAVANNA:
			case BIO_EVERGREEN:		return 4;
			case BIO_GRASSLAND:
			case BIO_TEMPERATE:		return 2;
			case BIO_TROPICAL:
			case BIO_RAINFOREST:	return 3;
			case BIO_MOIST:
			case BIO_SWAMP:			return 4;
			case BIO_DESERT:
			case BIO_GRASS_DESERT:	return 0;
			default:				return 5;
		}
		break;
	case BIO_OUTLANDS:
	case BIO_WET_OUTLANDS:
		switch ( dstBiome ) {
			case BIO_OUTLANDS:
			case BIO_WET_OUTLANDS:	return 0;
			default:				return 5;
		}
		break;
	}

	return 5;
}