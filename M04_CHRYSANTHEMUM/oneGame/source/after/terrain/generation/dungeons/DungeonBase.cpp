

#include "DungeonBase.h"
#include "DungeonFactory.h"

#include "after/terrain/edit/csg/SidebufferVolumeEditor.h"
#include "after/types/terrain/BlockType.h"

Terrain::CWorldGen_Terran*	Terrain::DungeonBase::generator = NULL;

Terrain::DungeonBase::DungeonBase ( CWorldGen_Terran* gen ) 
	: m_generator(gen)
{
	m_system.hallway_affection_map	= NULL;
	m_system.room_affection_map		= NULL;
	m_system.affected_areas			= NULL;
}

Terrain::DungeonBase::~DungeonBase ( void )
{
	if ( m_system.hallway_affection_map ) {
		for ( uint i = 0; i < m_system.hallway_affection_map->size(); ++i ) {
			if ( m_system.hallway_affection_map->at(i) ) {
				delete m_system.hallway_affection_map->at(i);
			}
			m_system.hallway_affection_map->at(i) = NULL;
		}
		delete m_system.hallway_affection_map;
		m_system.hallway_affection_map = NULL;
	}
	if ( m_system.room_affection_map ) {
		for ( uint i = 0; i < m_system.room_affection_map->size(); ++i ) {
			if ( m_system.room_affection_map->at(i) ) {
				delete m_system.room_affection_map->at(i);
			}
			m_system.room_affection_map->at(i) = NULL;
		}
		delete m_system.room_affection_map;
		m_system.room_affection_map	= NULL;
	}
	if ( m_system.affected_areas ) {
		delete m_system.affected_areas;
		m_system.affected_areas = NULL;
	}
}

// Initialize
//  When called, generates values for the generation table. The values are then
//  used to determine if the dungeon can be used. If the values aren't good enough,
//  the dungeon is discarded.
//  Takes a seed position as an argument.
void	Terrain::DungeonBase::Initialize ( const RangeVector & seedposition )
{
	exit(13);
}
// Generate
//  Runs the dungeon's generation algorithm. Hallways and rooms are created with
//  this function.
void	Terrain::DungeonBase::Generate ( void )
{
	exit(13);
}

// Excavate
//  Transforms the blocks into an actual dungeon after Generate has been called.
void	Terrain::DungeonBase::Excavate ( const quickAccessData& qd )//( CBoob* pBoob, const RangeVector& position )
{
	//gen_pboob = pBoob;
	gen_index		= qd.indexer;
	gen_accessor	= qd.accessor;
	gen_editor		= qd.editor;

	// Generate the dungeon's affection map if needed
	GenerateAffectionMap();

	// Excavate the hallways
	for ( uint i = 0; i < m_system.hallways.size(); ++i )
	{
		std::vector<RangeVector>::iterator findResult = find( m_system.hallway_affection_map->at(i)->begin(), m_system.hallway_affection_map->at(i)->end(), gen_index );
		if ( findResult != m_system.hallway_affection_map->at(i)->end() )
		{
			ExcavateHallway( m_system.hallways[i] );
		}
	}

	// Excavate the rooms
	for ( uint i = 0; i < m_system.rooms.size(); ++i )
	{
		std::vector<RangeVector>::iterator findResult = find( m_system.room_affection_map->at(i)->begin(), m_system.room_affection_map->at(i)->end(), gen_index );
		if ( findResult != m_system.room_affection_map->at(i)->end() )
		{
			ExcavateRoom( m_system.rooms[i] );
		}
	}
}

inline RangeVector BoobSpaceToIndex ( const Vector3d_d& positionReference )
{
	RangeVector result;
	result.x = rangeint(floor(positionReference.x));
	result.y = rangeint(floor(positionReference.y));
	result.z = rangeint(floor(positionReference.z));
	return result;
}

// Generates the affection list and the affection map.
void	Terrain::DungeonBase::GenerateAffectionMap ( void )
{
	// Compute hallway affection map
	if ( !m_system.hallway_affection_map ) {
		m_system.hallway_affection_map = new std::vector<std::vector<RangeVector>*>;

		RangeVector basePosition, secondaryPosition, tertiaryPosition;
		for ( uint i = 0; i < m_system.hallways.size(); ++i )
		{
			m_system.hallway_affection_map->push_back( new std::vector<RangeVector> );

			Vector3d_d offset = m_system.hallways[i].ray.dir.normal() * m_system.hallways[i].height * 0.03125f;

			basePosition = BoobSpaceToIndex( m_system.hallways[i].ray.pos - offset );
			m_system.hallway_affection_map->at(i)->push_back( basePosition );

			secondaryPosition = BoobSpaceToIndex( m_system.hallways[i].ray.pos + m_system.hallways[i].ray.dir + offset );
			if ( secondaryPosition != basePosition ) {
				m_system.hallway_affection_map->at(i)->push_back( secondaryPosition );
			}

			tertiaryPosition = BoobSpaceToIndex( m_system.hallways[i].ray.pos + m_system.hallways[i].ray.dir*0.5f );
			if (( tertiaryPosition != secondaryPosition )&&( tertiaryPosition != basePosition )) {
				m_system.hallway_affection_map->at(i)->push_back( tertiaryPosition );
			}
		}
	}
}

// Excavates a room with the given type and at the given position
void	Terrain::DungeonBase::ExcavateRoom ( const Dungeon::Room & )
{

}
// Excavates a hallway with the given type and at the given position
void	Terrain::DungeonBase::ExcavateHallway ( const Dungeon::Hallway & hallway )
{
	/*char i,j;
	short k;
	Vector3d blockPosition, a_to_p, a_to_b;
	ftype atb2, atp_dot_atb, t;
	Vector3d pos;

	for ( i = 0; i < 8; i += 1 )
	{
		for ( j = 0; j < 8; j += 1 )
		{
			for ( k = 0; k < 512; k += 1 )
			{
				blockPosition.x = gen_index.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
				blockPosition.y = gen_index.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
				blockPosition.z = gen_index.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

				a_to_p = blockPosition-(hallway.ray.pos);
				a_to_b = hallway.ray.dir;

				atb2 = a_to_b.sqrMagnitude();
				atp_dot_atb = a_to_p.dot( a_to_b );

				t = min<ftype>( 1.0f, max<ftype>( 0.0f, atp_dot_atb / atb2  ) );

				pos = hallway.ray.pos + a_to_b*t;

				// If the distance is close enough, then bam, excavate
				if ( (pos-blockPosition).magnitude() <= (hallway.height/32.0f) )
				{
					gen_pboob->data[i].data[j].data[k].block = EB_NONE;
				}
			}
		}
	}*/
	Terrain::terra_b emptyBlock;
	emptyBlock.raw = 0;
	emptyBlock.block = Terrain::EB_NONE;

	//generator->SB_Line( gen_pboob, gen_index, hallway.ray, hallway.height/32.0f, emptyBlock );
	gen_editor->Sub_Sphere( hallway.ray.pos * 64.0f, hallway.height );
	gen_editor->Sub_Sphere( (hallway.ray.pos+hallway.ray.dir) * 64.0f, hallway.height );
	gen_editor->Mak_Line( hallway.ray.pos * 64.0f, hallway.ray.dir * 64.0f, hallway.height, Terrain::EB_NONE );
}


// Save/load system
bool	Terrain::DungeonBase::save ( CBinaryFile & file, const uint ver )
{
	FILE* fp = file.GetFILE();
	file.WriteShort( index );

	// Write the version
	file.WriteUInt32( ver );

	// Write the generation table
	fwrite( (void*)&m_gentable, sizeof( Dungeon::GenerationTable ), 1, fp );
	// Write the gameplay table
	fwrite( (void*)&m_proptable, sizeof( Dungeon::GameplayTable ), 1, fp );

	// Write dungeon properties
	fwrite( (void*)&m_centerindex, sizeof( RangeVector ), 1, fp );

	// Write the system table properties
	fwrite( (void*)&m_system.start_position, sizeof( RangeVector ), 1, fp );
	fwrite( (void*)&m_system.center_position, sizeof( Vector3d_d ), 1, fp );
	fwrite( (void*)&m_system.halfbox_size, sizeof( RangeVector ), 1, fp );

	// Write the size of the system tables
	file.WriteUInt32( m_system.hallways.size() );
	file.WriteUInt32( m_system.rooms.size() );
	// Write the hallway table
	for ( uint32_t i = 0; i < m_system.hallways.size(); ++i ) {
		fwrite( (void*)&(m_system.hallways[i]), sizeof( Dungeon::Hallway ), 1, fp );
	}
	// Write the room table
	for ( uint32_t i = 0; i < m_system.rooms.size(); ++i ) {
		fwrite( (void*)&(m_system.rooms[i]), sizeof( Dungeon::Room ), 1, fp );
	}

	return true;
}
bool	Terrain::DungeonBase::load ( CBinaryFile & file, const uint ver )
{
	FILE* fp = file.GetFILE();

	// Check for the proper version
	if ( file.ReadUInt32() != ver ) {
		// Mismatched dungeon file. Should delete the file.
		return false;
	}

	// Read the generation table
	fread( (void*)&m_gentable, sizeof( Dungeon::GenerationTable ), 1, fp );
	// Read the gameplay table
	fread( (void*)&m_proptable, sizeof( Dungeon::GameplayTable ), 1, fp );

	// Read dungeon properties
	fread( (void*)&m_centerindex, sizeof( RangeVector ), 1, fp );

	// Read the system table properties
	fread( (void*)&m_system.start_position, sizeof( RangeVector ), 1, fp );
	fread( (void*)&m_system.center_position, sizeof( Vector3d_d ), 1, fp );
	fread( (void*)&m_system.halfbox_size, sizeof( RangeVector ), 1, fp );

	// Read the system table sizes
	uint32_t hallway_count = file.ReadUInt32();
	uint32_t room_count = file.ReadUInt32();
	// Read in the hallway table
	for ( uint32_t i = 0; i < hallway_count; ++i ) {
		Dungeon::Hallway tempHallway;
		fread( (void*)&tempHallway, sizeof( Dungeon::Hallway ), 1, fp );
		m_system.hallways.push_back( tempHallway );
	}
	// Read in the room table
	for ( uint32_t i = 0; i < room_count; ++i ) {
		Dungeon::Room tempRoom;
		fread( (void*)&tempRoom, sizeof( Dungeon::Room ), 1, fp );
		m_system.rooms.push_back( tempRoom );
	}

	return true;
}
Terrain::DungeonBase* Terrain::DungeonBase::load ( CBinaryFile & file, CWorldGen_Terran* gen )
{
	short index = file.ReadShort();

	DungeonBase* newDungeon = Terrain::Dungeon::Factory.dungeon_insts[index](gen);
	if ( !newDungeon->load( file, TERRA_SYSTEM_VERSION ) ) {
		delete newDungeon;
		newDungeon = NULL;
	}
	return newDungeon;
}