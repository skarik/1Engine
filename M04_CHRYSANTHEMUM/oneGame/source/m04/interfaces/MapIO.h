//		class M04::MapIO
// Given a set of objects or virtual objects as well as a file, can perform saving and loading
//

#ifndef _M04_MAP_IO_H_
#define _M04_MAP_IO_H_

#include <cstdio>
#include "core/types/types.h"

//=========================================//
// Prototypes
//=========================================//

namespace Engine2D
{
	class TileMap;
}
namespace M04
{
	class MapInformation;
}

//=========================================//
// Definition
//=========================================//

namespace M04
{
	//=========================================//
	// IO Structures
	//=========================================//

	struct mapio_header_t
	{
		char		signature [4];
		uint32_t	vmajor;
		uint32_t	vminor;
	};
	static_assert(sizeof(mapio_header_t)==12,"mapio_header_t: misalignment issue");

	struct mapio_section_start_t
	{
		char		signature [4];
		char		name [120];
		uint32_t	size;

		// Set the default topper
		mapio_section_start_t ( void )
		{
			strncpy( signature, "$TOP", 4 );
		}
	};
	static_assert(sizeof(mapio_section_start_t)==128,"mapio_section_start_t: misalignment issue");

	//=========================================//
	// MapIO Class 
	//=========================================//

	class MapIO
	{
	public:
		FILE*				m_file;
		
		Engine2D::TileMap*		m_tilemap;
		M04::MapInformation*	m_mapinfo;
		bool					m_io_areas;
		bool					m_io_objects_editor;
		bool					m_io_objects_game;

	public:

		//		Save ( )
		// Saves data to the file, using anything that's not null
		void	Save ( void );
		
		//		Load ( )
		// Loads data from the file, filling anything that's not null
		void	Load ( void );

	public:
		MapIO ( void )
			: m_file(NULL), m_tilemap(NULL),
			m_io_areas(false), m_io_objects_editor(false), m_io_objects_game(false)
		{
			;
		}
	};
};

#endif//_M04_MAP_IO_H_