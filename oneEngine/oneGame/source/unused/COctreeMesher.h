
#ifndef _C_OCTREE_MESHER_H_
#define _C_OCTREE_MESHER_H_

#include "CModelData.h"

class COctreeTerrain;
class COctreeRenderer;

class COctreeMesher
{

public:
	explicit COctreeMesher ( COctreeTerrain*, COctreeRenderer* );

	void CreateMesh ( CTerrainVertex* io_vertices, CModelTriangle* io_triangles, uint& o_vertex_count, uint& o_triangle_count );

public:
	static bool m_useDualContouring;
	Vector3d m_min;
	Vector3d m_max;
	Real	m_step;
	
	bool	m_split_border [6];

private:
	// Marching cubes
	void Triangulate_MC ( Real x, Real y, Real z );
	void Triangulate_TVMC ( Real x, Real y, Real z );
	void Triangulate_TVMC_BORDER ( Real x, Real y, Real z, const uint border );

	// Dual contouring
	struct Cube;
	struct CubeIndexer;

	void Cube_DC ( Real x, Real y, Real z );
	void Vertex_DC ( const CubeIndexer& index );
	//void Vertex_DC_BORDER ( const CubeIndexer& index, const uint border );
	void Triangulate_DC ( const CubeIndexer& index );

	// Helper functions
	Real BlockGetSpecular ( const ushort block );
	Real BlockGetSaturation ( const ushort block );
	Real BlockGetGlow ( const ushort block );
	Real BlockGetBrightness ( const ushort block );
	Real BlockGetHue ( const ushort block );
	Real BlockGetTexW ( const ushort block );
	Real BlockGetTexScale ( const ushort block );

private:
	bool	m_pushdown;

	unsigned int vertexCount;
	unsigned int triangleCount;

	COctreeTerrain*		m_terrain;
	COctreeRenderer*	m_renderer;

	CTerrainVertex*	vertices;
	CModelTriangle* triangles;

	// Texture storage
	typedef union {
		struct {
			unsigned	fluid_type	: 2;	// 4 fluid types
			unsigned	nutrients	: 2;	// bit 0: has water, bit 1: ???
		};
		uchar raw;
	} blkflg_t;

	// Marching cubes
	static const int edgeTable[256];
	static const int triTable[256][16];

	// Transvoxel
	struct RegularCellData;
	struct TransitionCellData;
	struct TransitionSampleData;

	static const unsigned char regularCellClass[256];
	static const RegularCellData regularCellData[16];
	static const unsigned short regularVertexData[256][12];
	static const unsigned char transitionCellClass[512];
	static const TransitionCellData transitionCellData[56];
	static const unsigned char transitionCornerData[13];
	static const unsigned short transitionVertexData[512][12];

	static const TransitionSampleData transitionSampleData[6];

	// Dual Contouring
	struct Cube
	{
		int index;			// marching cubes cube index
		Vector3d position;	// vertex position
		Vector3d normal;	// vertex normal
		
		Real	blk_weight;	//[2];	// block weight
		ushort	blk_type;	//[2];	// block type
		uchar	blk_smooth;
		Real	blk_light_r;
		Real	blk_light_g;
		Real	blk_light_b;
		//char	blk_flags;
		//blkflg_t blk_flags;
	};
	Cube* cubes;
	struct CubeIndexer
	{
		int index;
		int x, y, z;
		int sizex, sizey, sizez;
		Vector3d position;
	};

};

#endif//_C_OCTREE_MESHER_H_