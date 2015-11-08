
// Terrain::SectorGameData is just storage for object types to be stored with the terrain

#ifndef _TERRAIN_GAME_SECTOR_H_
#define _TERRAIN_GAME_SECTOR_H_

#include <vector>

#include "Vector3d.h"

class CTerrainProp;
class CFoliage;
struct sTerraGrass;
class CTerraGrass;

namespace Terrain
{
	// Foliage Def
	struct sTerraFoiliage
	{
		unsigned short	foliage_index;
		Vector3d		position;
		char			userdata [48];
	};
	// Component Def
	struct sTerraProp
	{
		unsigned short	component_index;
		Vector3d		position;
		Vector3d		rotation;
		Vector3d		scaling;
		uint64_t		userdata;
		// Indexing information
		uint16_t		block_pos_x;
		uint16_t		block_pos_y;
		uint16_t		block_pos_z;
	};

	struct SectorGameData {
		bool	m_loaded_big;
		bool	m_loaded_small;

		std::vector<CTerrainProp*>	m_components;
		std::vector<CFoliage*>			m_foliage;
		std::vector<sTerraGrass>		m_grass;

		std::vector<sTerraFoiliage>		m_foliage_queue;
		std::vector<sTerraProp>			m_component_queue;
	};

	struct SectorGameDataState {
		CTerraGrass*	m_rendered_grass;

		SectorGameDataState ( void )
			: m_rendered_grass(NULL)
		{
			;
		}
	};
};

#endif//_TERRAIN_GAME_SECTOR_H_