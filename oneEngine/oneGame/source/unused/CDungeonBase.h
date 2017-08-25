
#ifndef _C_DUNGEON_BASE_H_
#define _C_DUNGEON_BASE_H_

#include "CBaseSerializer.h"
#include "CVoxelTerrain.h"
#include "CTerrainGenerator.h"

#include <memory>

namespace Dungeon
{
	struct GenerationTable
	{
		ftype rarity;
		ftype difficulty;	// from 0 to 1
		ftype itemquality;	// from 0 to 5
		short type;
		short element;
	};
	struct GameplayTable
	{
		short spawnstate;
		short cratespawnstate;
		short buffer0;
		short buffer1;
	};

	enum ElementType
	{
		ElementNeutral,
		ElementOcean,
		ElementRock,
		ElementFire,
		ElementWater,
		ElementForest
	};

	struct Hallway
	{
		Ray	ray;
		ftype width;
		ftype height;
		ftype noise;
		short type;
	};
	struct Room
	{
		Vector3d position;
		ftype width;
		ftype height;
		short type;
	};
	struct DungeonSystem
	{
		RangeVector	start_position;
		Vector3d	center_position;
		RangeVector	halfbox_size;

		vector<RangeVector>*		affected_areas; // Only used for generation

		vector<Hallway>	hallways;
		vector<vector<RangeVector>*>* hallway_affection_map;// Only used for generation

		vector<Room> rooms;
		vector<vector<RangeVector>*>* room_affection_map;	// Only used for generation
	};
	//vector<TerraCave*> vCaveSystems;
};

class CDungeonBase
{
public:
	explicit		CDungeonBase ( CTerrainGenerator* );
	virtual			~CDungeonBase ( void );

	// Initialize
	//  When called, generates values for the generation table. The values are then
	//  used to determine if the dungeon can be used. If the values aren't good enough,
	//  the dungeon is discarded.
	//  Takes a seed position as an argument.
	virtual void	Initialize ( const RangeVector & );
	// Generate
	//  Runs the dungeon's generation algorithm. Hallways and rooms are created with
	//  this function.
	virtual void	Generate ( void );
	// Excavate
	//  Transforms the blocks into an actual dungeon after Generate has been called.
	void			Excavate ( CBoob*, const RangeVector& );

	// Generation options
	void			SetElevation ( ftype ele ) { gen_elevationLine = ele; };

	// Serialization system
	//void			serialize ( Serializer &,  const uint ); // Needs to save generation and gameplay tables
	// Serialization system will not work for this type of object, especially without a proper vector serialization and deserialization method.
	// Instead, basic IO functions will suffice
	// Save/load system
	bool			save ( CBinaryFile &, const uint=TERRA_SYSTEM_VERSION );
	bool			load ( CBinaryFile &, const uint );
	static CDungeonBase* load ( CBinaryFile &, CTerrainGenerator* );

	RangeVector		m_centerindex;
	Dungeon::GenerationTable	m_gentable;
	Dungeon::GameplayTable		m_proptable;

	Dungeon::DungeonSystem		m_system;

	short			index;

	static CTerrainGenerator*	generator;	// Set on excavate

protected:
	CTerrainGenerator*	m_generator;

	ftype			gen_elevationLine;	// Set before generate

	CBoob*			gen_pboob;	// Set on excavate
	RangeVector		gen_index;	// Set on excavate

	// Generates the affection list and the affection map.
	void			GenerateAffectionMap ( void );
	// Excavates a room with the given type and at the given position
	virtual void	ExcavateRoom ( const Dungeon::Room & );
	// Excavates a hallway with the given type and at the given position
	virtual void	ExcavateHallway ( const Dungeon::Hallway & );
};

#endif//_C_DUNGEON_BASE_H_