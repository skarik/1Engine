
#include "after/terrain/VoxelTerrain.h"
#include "CVoxelMesher.h"

#include "core/math/Math.h"
#include "core-ext/math/MathQEF.h"

#include "core-ext/profiler/CTimeProfiler.h"

#include "after/types/terrain/BlockType.h"

// 
bool CVoxelMesher::m_useDualContouring = true;

CVoxelMesher::CVoxelMesher ( CVoxelTerrain* n_terrain )
	: m_terrain(n_terrain),
	m_split_border(), m_pushdown(false)
{
	/*m_min = Vector3d( -32,-32,-32 );
	m_max = Vector3d( 32, 32, 32 );
	m_step = CTerrain::BlockSize;*/

	//m_split_border = {false,false,false,false,false,false};
}

// SetSourceData ( ) : Set the target buffer and size to build the mesh from.
void CVoxelMesher::SetSourceData ( Terrain::SidebufferAccessor* i_buffer )
{
	m_buffer = i_buffer;
}

// CreateMesh ( ) : Build the mesh from the data given in SetSourceData.
void CVoxelMesher::CreateMesh ( CTerrainVertex* io_vertices, CModelTriangle* io_triangles, uint& o_vertex_count, uint& o_triangle_count )
{
	// Set up initial mesh settings
	vertexCount = 0;
	triangleCount = 0;
	vertices = io_vertices;
	triangles = io_triangles;

	// Grab size of the buffer for fast reference
	WorldVector size = m_buffer->GetSize();

	// Generate the mesh using Marching Cubes -> Dual Contouring
	if ( m_useDualContouring )
	{
		cubes = new Cube[size.x*size.y*size.z];

		CubeIndexer indexer;
		indexer.sizex = size.x;
		indexer.sizey = size.y;
		indexer.sizez = size.z;

		for ( uint z = 0; z < size.z-1; ++z )
		{
			for ( uint y = 0; y < size.y-1; ++y )
			{
				for ( uint x = 0; x < size.x-1; ++x )
				{
					m_pushdown = false;

					indexer.position = Vector3d( x,y,z );
					indexer.x = x;
					indexer.y = y;
					indexer.z = z;
					indexer.index = x + y*size.x + x*size.x*size.y;
					Vertex_DC( indexer );
				}
			}
		}

		// Generate triangles using dual contouring
		for ( int ix = 0; ix < size.x-2; ++ix )
		{
			for ( int iy = 0; iy < size.y-2; ++iy )
			{
				for ( int iz = 0; iz < size.z-2; ++iz )
				{
					indexer.x = ix;
					indexer.y = iy;
					indexer.z = iz;
					Triangulate_DC( indexer );
				}
			}
		}
	}
	// Generate the mesh using Transvoxel Marching Cubes (Terathon Technology)
	else
	{
		for ( uint z = 0; z < size.z-1; ++z )
		{
			for ( uint y = 0; y < size.y-1; ++y )
			{
				for ( uint x = 0; x < size.x-1; ++x )
				{
					Triangulate_TVMC( x,y,z );
#					ifdef _ENGINE_DEBUG
					if ( vertexCount > 1024*32*8 || triangleCount > 1024*16*8 )  {
						throw std::out_of_range( "Too much geometry in mesh generation" );
					}
#					endif
				}
			}
		}
	}

	/*const ftype n_halfStep = m_step * 0.5;

	// Generate the mesh using Transvoxel Marching Cubes (Terathon Technology)
	if ( !useDualContouring )
	{
		// Need to pass into the mesher the 3x3x3 LoD map
		for ( Real x = m_min.x; x <= m_max.x-n_halfStep; x += m_step )
		{
			for ( Real y = m_min.y; y <= m_max.y-n_halfStep; y += m_step )
			{
				for ( Real z = m_min.z; z <= m_max.z-n_halfStep; z += m_step )
				{
					if (( m_split_border[0] && x < m_min.x+n_halfStep )||
						( m_split_border[1] && x > m_max.x-m_step )||
						( m_split_border[2] && y < m_min.y+n_halfStep )||
						( m_split_border[3] && y > m_max.y-m_step )||
						( m_split_border[4] && z < m_min.z+n_halfStep )||
						( m_split_border[5] && z > m_max.z-m_step ))
					{
						Triangulate_TVMC_BORDER( x,y,z,0 );
					}
					else
					{
						Triangulate_TVMC( x,y,z );
					}
#					ifdef _ENGINE_DEBUG
					if ( vertexCount > 1024*32*8 || triangleCount > 1024*16*8 )  {
						throw std::out_of_range( "Too much geometry in mesh generation" );
					}
#					endif
				}
			}
		}
	}
	// Generate the mesh using Marching Cubes -> Dual Contouring
	else
	{
		// Overshoot for now.
		int sizeX = int((m_max.x-m_min.x)/m_step + 0.5) + 1;
		int sizeY = int((m_max.y-m_min.y)/m_step + 0.5) + 1;
		int sizeZ = int((m_max.z-m_min.z)/m_step + 0.5) + 1;
		int biasX = 0;
		int biasY = 0;
		int biasZ = 0;

		if ( m_split_border[0] ) {
			biasX += 1;
			sizeX += 1;
		}
		if ( m_split_border[2] ) {
			biasY += 1;
			sizeY += 1;
		}
		if ( m_split_border[4] ) {
			biasZ += 1;
			sizeZ += 1;
		}
		if ( m_split_border[1] ) {
			sizeX += 1;
		}
		if ( m_split_border[3] ) {
			sizeY += 1;
		}
		if ( m_split_border[5] ) {
			sizeZ += 1;
		}

		cubes = new Cube[sizeX*sizeY*sizeZ];

		CubeIndexer indexer;
		indexer.sizex = sizeX;
		indexer.sizey = sizeY;
		indexer.sizez = sizeZ;

		// Generate cubes w/ marching cubes
		for ( int ix = 0; ix < sizeX; ++ix )
		{
			for ( int iy = 0; iy < sizeY; ++iy )
			{
				for ( int iz = 0; iz < sizeZ; ++iz )
				{
					m_pushdown = false;
					if ( m_split_border[0] && ix == 0 ) m_pushdown = true;
					if ( m_split_border[2] && iy == 0 ) m_pushdown = true;
					if ( m_split_border[4] && iz == 0 ) m_pushdown = true;
					if ( m_split_border[1] && ix == sizeX-1 ) m_pushdown = true;
					if ( m_split_border[3] && iy == sizeY-1 ) m_pushdown = true;
					if ( m_split_border[5] && iz == sizeZ-1 ) m_pushdown = true;

					indexer.position = Vector3d( m_min.x, m_min.y, m_min.z ) + (Vector3d(ix-biasX,iy-biasY,iz-biasZ)*m_step);
					indexer.x = ix;
					indexer.y = iy;
					indexer.z = iz;
					indexer.index = ix + iy*sizeX + iz*sizeX*sizeY;
					Vertex_DC( indexer );
					//int bitmask = 0;
					//if ( m_split_border[0] && ix==0 ) bitmask |= 0x01;
					//if ( m_split_border[2] && iy==0 ) bitmask |= 0x02;
					//if ( m_split_border[4] && iz==0 ) bitmask |= 0x04;
					//if ( m_split_border[1] && ix==sizeX-1 ) bitmask |= 0x08;
					//if ( m_split_border[3] && iy==sizeY-1 ) bitmask |= 0x10;
					//if ( m_split_border[5] && iz==sizeZ-1 ) bitmask |= 0x20;
					//if ( ix%2 != 0 ) bitmask |= 0x40;
					//if ( iy%2 != 0 ) bitmask |= 0x80;
					//if ( iz%2 != 0 ) bitmask |= 0x100;
					//Vertex_DC_BORDER( indexer, bitmask );
				}
			}
		}
		// Generate triangles using dual contouring
		for ( int ix = 0; ix < sizeX-1; ++ix )
		{
			for ( int iy = 0; iy < sizeY-1; ++iy )
			{
				for ( int iz = 0; iz < sizeZ-1; ++iz )
				{
					//indexer.position = Vector3d( minX, minX, minZ ) + (Vector3d(ix,iy,iz)*COctreeTerrain::blockSize);
					indexer.x = ix;
					indexer.y = iy;
					indexer.z = iz;
					//indexer.index = ix + iy*sizeX + iz*sizeX*sizeY;
					Triangulate_DC( indexer );
				}
			}
		}
		//
		delete [] cubes;
	}*/

	o_vertex_count = vertexCount;
	o_triangle_count = triangleCount;
}

Real CVoxelMesher::BlockGetSpecular ( const ushort block )
{
	using namespace Terrain;
	switch ( block&0xFF )
	{
	case EB_MUD:
		return 0.6f;
	case EB_CRYSTAL:
	case EB_XPLO_CRYSTAL:
		return 0.8f;
	case EB_SNOW:
	case EB_TOP_SNOW:
		return 0.3f;
	case EB_DEADSTONE:
	case EB_CURSED_DEADSTONE:
		return 0.3f;
	case EB_ICE:
		return 0.9f;
	case EB_HEMATITE:
		return 0.6f;
	}
	return 0;
}
Real CVoxelMesher::BlockGetSaturation ( const ushort block )
{
	using namespace Terrain;
	switch ( block&0xFF )
	{
	case EB_MUD:
		return 0.5f;
	case EB_XPLO_CRYSTAL:
		return 0.3f;
	case EB_ASH:
		return 0.1f;
	case EB_SNOW:
	case EB_TOP_SNOW:
		return 0.0f;
	case EB_ICE:
		return 0.8f;
	}
	return 1;
}
Real CVoxelMesher::BlockGetGlow ( const ushort block )
{
	using namespace Terrain;
	switch ( block&0xFF )
	{
	case EB_CURSED_DEADSTONE:
		return 1.0f;
	}
	return 0;
}
Real CVoxelMesher::BlockGetBrightness ( const ushort block )
{
	using namespace Terrain;
	blkflg_t flags;
	flags.raw = (uchar)(block >> 8);
	switch ( block&0xFF )
	{
	//case EB_GRASS:
	//	return 1.05f;
	case EB_MUD:
		return 0.9f;
	case EB_ASH:
		return 0.6f;
	case EB_TILLED_DIRT:
		if ( flags.nutrients == 1 ) {
			return 0.8f;
		}
		return 1.0f;
	}
	return 1;
}
Real CVoxelMesher::BlockGetHue ( const ushort block )
{
	using namespace Terrain;
	switch ( block&0xFF )
	{
	//case EB_GRASS:
	//	return -0.22f;
	default:
		return 0;
	}
}

Real CVoxelMesher::BlockGetTexW ( const ushort block )
{
	using namespace Terrain;
	switch ( block&0xFF )
	{
	case EB_GRASS:
		return 0;
	case EB_STONE:
		return 1;
	case EB_DIRT:
	case EB_MUD:
		return 2;
	case EB_SAND:
	case EB_SNOW:
		return 3;
	case EB_CLAY:
		return 4;
	case EB_TILLED_DIRT:
		return 5;
	case EB_STONEBRICK:
		return 6;
	case EB_WOOD:
		return 9;
	case EB_CRYSTAL:
	case EB_ICE:
		return 8;
	case EB_GRAVEL:
	case EB_ROAD_GRAVEL:
		return 10;
	case EB_DEADSTONE:
	case EB_CURSED_DEADSTONE:
		return 11;
	case EB_RIGDESTONE:
		return 12;
	case EB_HEMATITE:
		return 14;
	case EB_NONE:
		return 15;
	default:
		return 11;
	}
}

Real CVoxelMesher::BlockGetTexScale ( const ushort block )
{
	using namespace Terrain;
	switch ( block&0xFF )
	{
	case EB_GRASS:
		return 0.25f;
	case EB_WOOD:
		return 0.333f;
	case EB_DIRT:
	case EB_MUD:
	case EB_SAND:
		return 0.125f;
	case EB_RIGDESTONE:
		return 0.070f;
	default:
		return 0.25f;
	}
}