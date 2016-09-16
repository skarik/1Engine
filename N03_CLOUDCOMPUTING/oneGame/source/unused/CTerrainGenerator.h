// class CTerrainGenerator
// Baseline class for terrain generation.

#ifndef _C_TERRAIN_GENERATOR_H_
#define _C_TERRAIN_GENERATOR_H_

#include "CVoxelTerrain.h"
#include "BaseNoise.h"

class CTerrainGenerator
{
public:
	typedef Terrain::terra_t terra_t;

	explicit CTerrainGenerator ( CVoxelTerrain*, long int );
	~CTerrainGenerator( void );

	enum eGenerationMode {
		GENERATE_TO_MEMORY,
		GENERATE_TO_DISK,
		GENERATE_DUAL_SAVE
	};
	eGenerationMode	iGenerationMode;
	void SetGenerationMode ( eGenerationMode newMode ) { iGenerationMode = newMode; };
	int	 GetGenerationMode ( void ) { return iGenerationMode; };

	// == Set Properties ==
	void SetSeed ( long int &iSeed )	{ iTerrainSeed = iSeed; };

	// == Public Query ==
	virtual char	TerraGen_pub_GetType	( Vector3d const& )=0;
	virtual char	TerraGen_pub_GetBiome	( Vector3d const& )=0;
	virtual ftype	TerraGen_pub_GetElevation ( Vector3d const& )=0;

	// == Threaded Query for Subsystems ==
	virtual ftype	Trd_Noise_HF ( const ftype, const ftype, const ftype=0 )=0;
	virtual ftype	Trd_Noise_Sample ( const ftype, const ftype, const ftype=0 )=0;

	// == Space Conversion ==
	// Conversion of a coordinate in boob space to a boob index
	inline RangeVector BoobSpaceToIndex ( const Vector3d & pos )
	{
		RangeVector result;
		result.x = rangeint(floor(pos.x));
		result.y = rangeint(floor(pos.y));
		result.z = rangeint(floor(pos.z));
		return result;
	}
	// Coversion of a coordinate in boob space to world space
	inline Vector3d BoobSpaceToWorld ( const Vector3d & pos )
	{
		Vector3d result (
			pos.x * 64.0f,
			pos.y * 64.0f,
			pos.z * 64.0f );
		return result;
	}
	// Conversion of a coordinate in boob space to world space
	inline Vector3d RangeVectorToWorld ( const RangeVector & index )
	{
		Vector3d result (
			index.x * 64.0f,
			index.y * 64.0f,
			index.z * 64.0f );
		return result;
	}
	
	// == Generation Tools ==
	void	SB_Ray ( CBoob* pBoob, const RangeVector& position, const Ray& inRay, const terra_t block );
	void	SB_Line ( CBoob* pBoob, const RangeVector& position, const Ray& inRay, const ftype inRadius, const terra_t block );
	void	SB_Cube ( CBoob* pBoob, const RangeVector& position, const Maths::Cubic& inCubic, const terra_t block );

	void	SB_RoadLine ( CBoob* pBoob, const RangeVector& position, const Ray& inRay, const ftype inWidth, const ftype inHeight, const terra_t block );

	void	SB_PasteVXG ( CBoob* pBoob, const RangeVector& position, const Vector3d& terraposition, const RangeVector& size, const terra_t* rawvxg );
	void	SB_PasteVXGLoop ( CBoob* pBoob, const RangeVector& position, const Vector3d& terraposition, const RangeVector& size, const RangeVector& target, const terra_t* rawvxg );
	void	SB_PasteVXGOffset ( CBoob* pBoob, const RangeVector& position, const Vector3d& terraposition, const RangeVector& size, const RangeVector& target, const RangeVector& baseoffset, const terra_t* rawvxg, bool overwriteEmpty = true );

	// Typedef foliage to get proper threaded generation
	typedef CVoxelTerrain::TerraFoiliage TerraFoiliage;
protected:
	virtual char	TerraGen_priv_GetType	( Vector3d const& )=0;
	virtual char	TerraGen_priv_GetBiome	( Vector3d const& )=0;

	virtual void GenerateTerrain ( CBoob *, RangeVector const& );
	virtual void GenerateSingleTerrain ( CBoob *, RangeVector const& );
	virtual void GenerateTerrainSector ( CBoob *, RangeVector const& )=0;
	virtual void GenerateTerrainSectorSecondPass ( CBoob *, RangeVector const& )=0;
	virtual void GenerateTerrainSectorThirdPass ( CBoob *, RangeVector const& )=0;

	void GenerateTerrainSectorCleanup ( CBoob * );

	virtual void Simulate ( void )=0;

	// Private terrain routines that need to be redirected to the terrain
	void TerraGen_NewFoliage ( CBoob * pBoob, Vector3d const& pos, string const& type )
	{
		terrain->TerraGen_NewFoliage( pBoob, pos, type );
	};
	void TerraGen_NewComponent ( CBoob * pBoob, const BlockInfo& block, const Vector3d& pos, const string& type )
	{
		terrain->TerraGen_NewComponent( pBoob, block, pos, type );
	};
	void TerraGen_NewGrass ( CBoob * pBoob, Vector3d const& pos, unsigned short type )
	{
		terrain->TerraGen_NewGrass( pBoob, pos, type );
	};
	int TerraGen_GetFoliageCount ( CBoob* pBoob, const string& type )
	{
		//return terrain->TerraGen_GetFoliageCount( pBoob, type );
		return 0;
	};
	int TerraGen_GetComponentCount ( CBoob* pBoob, const string& type )
	{
		return terrain->TerraGen_GetComponentCount( pBoob, type );
	};
	int TerraGen_GetGrassCount ( CBoob* pBoob, const string& type )
	{
		//return terrain->TerraGen_GetGrassCount( pBoob, type );
		return 0;
	};
	bool SideVisible ( CBoob * pBoob, char const index16, char const index8, terra_t * data, int const i, int const width, EFaceDir const dir, unsigned char ofs )
	{
		bool result = terrain->SideVisible( pBoob, index16, index8, data, i, width, dir, ofs );
		cLastCheckedBlock = terrain->cLastCheckedBlock;
		return result;
	};
	unsigned short cLastCheckedBlock;
	unsigned short cBottomCheckBlock;
	bool TraverseTree ( CBoob ** pCurrentBoob, char& index16, char& index8, short& i, EFaceDir direction )
	{
		return terrain->TraverseTree( pCurrentBoob, index16, index8, i, direction );
	};
protected:
	long int iTerrainSeed;
	bool	 bSpreadTerrain;

	CVoxelTerrain*	terrain;

	// =====
	// Caves excavate material, while Bridges are additive structures.
	// They both can be used together with similar seeding values to generate interesting natural structures.
	// Memory wise, their storage is the nearly exact same, with no differences in their shape definitions.
	// Cave/dungeon generation
	struct TerraTunnel
	{
		Ray	ray;
		ftype width;
		ftype height;
		ftype noise;
		short type;
	};
	struct TerraCave
	{
		RangeVector	startPosition;
		vector<RangeVector> affectedAreas;
		vector<vector<RangeVector>*> affectionMap;
		vector<TerraTunnel>	partTree;
		short type;
	};
	vector<TerraCave*> vCaveSystems;
	// Bridge Generation
	//vector<TerraCave*> vBridgeSystems;

	// =====
	// Patterns are seeded objects that change block types. The have the ability to change their area over time.
	// Especially, patterns have the ability to "regenerate." On an input, they can edit an already generated area.
	// They are used for towns, castles, and other non-natural formations with patternlike elements.
	// Patterns are important enough that they are saved to the hard disk.
	struct PatternInstance
	{
		
	};
	struct PatternLine
	{
		
	};
	struct TerraPattern
	{
		vector<RangeVector> affectedAreas;
	};
	vector<TerraPattern*> vPatternSystems;

	// =====
	// Instances just change things based on where they are. They're not so much of an add/subtract as it is a placing.
	// They have an option to downtrace on creation, or just create wherever their position says.
	// There is a hasdowntraced bool to check if something has already hit the ground.
	// Massive is a bool to control the order of generation.
	struct TerraInstance
	{
		Vector3d position;
		vector<RangeVector> affectedAreas;
		RangeVector	startPosition;
		bool downtrace;
		bool hasdowntraced;
		bool ignore;
		bool massive;
		// Instance to generation
		uint16_t type;
		// system: Generation mode (0 for per-block, 1 for per-sector)
		char mode; 

		TerraInstance ( void ) :
			downtrace(false), hasdowntraced(false), ignore(false),
			massive(false), type(-1), mode(0)
		{
			;
		}
	};
	vector<TerraInstance*> vInstanceSystems;

private:
	friend CVoxelTerrain;

};

#endif