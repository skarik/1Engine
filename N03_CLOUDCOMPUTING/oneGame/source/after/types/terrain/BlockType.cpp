
#include "BlockType.h"
#include "engine/physics/material/physMaterial.h"
#include <string.h>

namespace Terrain
{
	char cDefaultBlockHardness [1024];
	char blockName [1024][64];

	int _SetDefaultBlockHardness ( void )
	{
		for ( unsigned short i = 0; i < 1024; i++ )
		{
			cDefaultBlockHardness[i] = 32;
		}
		cDefaultBlockHardness[EB_DIRT]		= 4;
		cDefaultBlockHardness[EB_GRASS]		= 5;
		cDefaultBlockHardness[EB_SAND]		= 4;
		cDefaultBlockHardness[EB_CLAY]		= 6;
		cDefaultBlockHardness[EB_WOOD]		= 6;
		cDefaultBlockHardness[EB_WIRE]		= 2;
		cDefaultBlockHardness[EB_MUD]		= 4;
		cDefaultBlockHardness[EB_GRAVEL]	= 5;

		cDefaultBlockHardness[EB_WATER]		= 2;
		cDefaultBlockHardness[EB_ICE]		= 3;

		cDefaultBlockHardness[EB_STONE]		= 20;
		cDefaultBlockHardness[EB_STONEBRICK]= 16;

		cDefaultBlockHardness[EB_CRYSTAL]	= 12;
		cDefaultBlockHardness[EB_XPLO_CRYSTAL]	= 12;

		cDefaultBlockHardness[EB_ANC_ELECTRIC]	= 0;
		cDefaultBlockHardness[EB_ANC_GRAVITY_JOKE]	= 0;
		cDefaultBlockHardness[EB_ANC_UNOBTANIUM]	= 0;
		cDefaultBlockHardness[EB_ANC_WIRE]	= 0;

		cDefaultBlockHardness[EB_DEADSTONE]			= 22;
		cDefaultBlockHardness[EB_CURSED_DEADSTONE]	= 20;

		cDefaultBlockHardness[EB_SNOW]		= 3;
		cDefaultBlockHardness[EB_TOP_SNOW]	= 3;
		
		cDefaultBlockHardness[EB_HEMATITE]	= 12;
		cDefaultBlockHardness[EB_MAGNETITE]	= 12;
		cDefaultBlockHardness[EB_GOETHITE]	= 12;

		return 0;
	}

	int _SetDefaultBlockNames ( void )
	{
		for ( unsigned short i = 0; i < 1024; i++ )
		{
			strcpy( blockName[i], "undefined" );
		}
		strcpy( blockName[EB_NONE],		"Terrain Block" );
		strcpy( blockName[EB_DIRT],		"Dirt" );
		strcpy( blockName[EB_GRASS],	"Grass Block" );
		strcpy( blockName[EB_STONE],	"Stone" );
		strcpy( blockName[EB_SAND],		"Sand" );
		strcpy( blockName[EB_WATER],	"Water Block" );
		strcpy( blockName[EB_WOOD],		"Wood Planks" );
		strcpy( blockName[EB_CLAY],		"Clay" );
		strcpy( blockName[EB_MUD],		"Mud" );
		strcpy( blockName[EB_GRAVEL],	"Gravel" );
		strcpy( blockName[EB_WIRE],		"Wire (Block)" );
		strcpy( blockName[EB_STONEBRICK],	"Stone Brick" );
		strcpy( blockName[EB_WATERSPRING],	"Water Block" );
		strcpy( blockName[EB_STEAMPIPE],	"Steampipe" );
		strcpy( blockName[EB_ICE],		"Ice" );
		strcpy( blockName[EB_CRYSTAL],	"Quartz Ore" );
		strcpy( blockName[EB_XPLO_CRYSTAL],	"Tetrylite Ore" );
		strcpy( blockName[EB_SANDSTONE],	"Sandstone" );

		strcpy( blockName[EB_ANC_ELECTRIC],	"anc" );
		strcpy( blockName[EB_ANC_WIRE],		"anc" );
		strcpy( blockName[EB_ANC_UNOBTANIUM],	"anc" );
		strcpy( blockName[EB_ANC_GRAVITY_JOKE],	"anc" );

		strcpy( blockName[EB_ASH],		"Ash" );
		strcpy( blockName[EB_DEADSTONE],		"Deadstone" );
		strcpy( blockName[EB_CURSED_DEADSTONE],	"Cursed Deadstone" );

		strcpy( blockName[EB_SNOW],		"Snow" );
		strcpy( blockName[EB_TOP_SNOW],	"Snow" );

		strcpy( blockName[EB_HEMATITE],		"Hematite Iron Ore" );
		strcpy( blockName[EB_MAGNETITE],	"Magnetite Iron Ore" );
		strcpy( blockName[EB_GOETHITE],		"Goethite Iron Ore" );
		
		return 0;
	}

	const physMaterial& MaterialOf ( const short iBlockType )
	{
		switch ( iBlockType )
		{
		case EB_DIRT:			return *PhysMats::Get(PhysMats::MAT_Dirt);

		case EB_GRASS:			return *PhysMats::Get(PhysMats::MAT_Grass);

		case EB_MUD:			return *PhysMats::Get(PhysMats::MAT_Mud);

		case EB_GRAVEL:	
		case EB_ROAD_GRAVEL:	return *PhysMats::Get(PhysMats::MAT_Gravel);

		case EB_STONE:			
		case EB_STONEBRICK:		
		case EB_DEADSTONE:
		case EB_CURSED_DEADSTONE: 
								return *PhysMats::Get(PhysMats::MAT_Rock);

		case EB_SAND:
		case EB_SANDSTONE:
		case EB_ASH:			return *PhysMats::Get(PhysMats::MAT_Sand);

		case EB_SNOW:
		case EB_TOP_SNOW:		return *PhysMats::Get(PhysMats::MAT_Snow);

		case EB_CRYSTAL:
		case EB_XPLO_CRYSTAL:
		case EB_GOETHITE:		return *PhysMats::Get(PhysMats::MAT_Crystal);

		case EB_WOOD:			return *PhysMats::Get(PhysMats::MAT_Wood);

		case EB_ICE:			return *PhysMats::Get(PhysMats::MAT_Ice);

		case EB_HEMATITE:
		case EB_MAGNETITE:
		case EB_IRONBRICK:		return *PhysMats::Get(PhysMats::MAT_Metal);

		default:				return *PhysMats::Get(PhysMats::MAT_Default);
		};
	}
};

int _setup0 = Terrain::_SetDefaultBlockHardness();
int _setup1 = Terrain::_SetDefaultBlockNames();