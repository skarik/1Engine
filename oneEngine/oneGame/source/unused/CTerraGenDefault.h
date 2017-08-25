
#ifndef _C_TERRA_GEN_DEFAULT_H_
#define _C_TERRA_GEN_DEFAULT_H_

// Includes
#include "CTerrainGenerator.h"
#include "TerrainInstanceTypes.h"

class CRegionManager;
class CTownManager;
class CDungeonBase;

#if __cplusplus > 199711L
	#include <unordered_map>
	using std::unordered_map;
#else
	#include <unordered_map>
	using std::tr1::unordered_map;
#endif


class CTerraGenDefault : public CTerrainGenerator
{
public:
	explicit CTerraGenDefault( CVoxelTerrain*, long int );
	~CTerraGenDefault ( void );

	virtual char	TerraGen_pub_GetType	( Vector3d const& );
	virtual char	TerraGen_pub_GetBiome	( Vector3d const& );
	virtual ftype	TerraGen_pub_GetElevation ( Vector3d const& );

	// == Threaded Query for Subsystems ==
	virtual ftype	Trd_Noise_HF ( const ftype, const ftype, const ftype=0 );
	virtual ftype	Trd_Noise_Sample ( const ftype, const ftype, const ftype=0 );

public:
	// DO NOT ACCESS ANYTHING BELOW THIS POINT WITHOUT PROPER KNOWLEDGE //

	// Simulation
	virtual void Simulate ( void );

	// Generation
	virtual char	TerraGen_priv_GetType	( Vector3d const& );
	virtual char	TerraGen_priv_GetBiome	( Vector3d const& );
	ftype	TerraGen_1p_GetElevation ( Vector3d const& );
	ftype	TerraGen_1p_GetElevation ( RangeVector const& );

	virtual void GenerateTerrain ( CBoob *, RangeVector const& );

	virtual void GenerateTerrainSector ( CBoob *, RangeVector const& );
	virtual void GenerateTerrainSectorSecondPass ( CBoob *, RangeVector const& );
	virtual void GenerateTerrainSectorThirdPass ( CBoob *, RangeVector const& );

	// Subpasses

	// Pass 1
	void TerraGenLandscapePass ( CBoob *, RangeVector const& );
	void TerraGenStonePass ( CBoob *, RangeVector const& );
	void TerraGenUnderdirtPass ( CBoob *, RangeVector const& );
	//void TerraGenBridgePass ( CBoob *, RangeVector const& ); //Is now in caves
	//void TerraGenCavePass ( CBoob *, RangeVector const& ); //Is now in dungeons, bridges removed
	void TerraGenRiverPass ( CBoob *, const RangeVector & );
	void TerraGenDungeonPass ( CBoob *, const RangeVector & );

	// Pass 1.5
	void TerraGenPatternPass ( CBoob *, const RangeVector & );

	// Pass 2
	void TerraGenInstancePass ( CBoob *, const RangeVector & );

	// Pass 3
	void TerraGenBiomePass ( CBoob *, RangeVector const& );
	void TerraGenLiquidPass ( CBoob *, RangeVector const& );

	// Pass 1
	// Terrain types
	ushort TerraGen_1p_Default ( CBoob *, Vector3d const& );
	ushort TerraGen_1p_Flatlands ( CBoob *, Vector3d const& );
	ushort TerraGen_1p_Outlands ( CBoob *, Vector3d const& );
	ushort TerraGen_1p_Mountains ( CBoob *, Vector3d const& );
	ushort TerraGen_1p_Hilllands ( CBoob *, Vector3d const& );
	ushort TerraGen_1p_Islands ( CBoob *, Vector3d const& );
	ushort TerraGen_1p_Ocean ( CBoob *, Vector3d const& );
	ushort TerraGen_1p_Spires ( CBoob *, Vector3d const& );
	ushort TerraGen_1p_Desert ( CBoob *, Vector3d const& );
	ushort TerraGen_1p_Badlands ( CBoob *, Vector3d const& );
	ushort TerraGen_1p_TheEdge ( CBoob *, Vector3d const& );
	// Cave generation
	//void TerraGen_Caves_CreateSystems ( RangeVector const& );
	//void TerraGen_Caves_CreateCaves ( CBoob *, RangeVector const& );
	//void TerraGen_Caves_StartSystem ( RangeVector const&, ECaveType const& );
	//void TerraGen_Caves_Generate ( TerraCave*, const unsigned int, const unsigned int seed = 0 );
	/*void TerraGen_Caves_ExcavateCaves ( CBoob *, RangeVector const& );
	void TerraGen_Caves_ExcavateCave ( CBoob *, Ray, RangeVector const& );*/
	// Bridge generation
	//void TerraGen_Bridges_CreateSystems ( RangeVector const& );
	//void TerraGen_Bridges_StartSystem ( RangeVector const& );
	//void TerraGen_Bridges_Generate ( TerraCave*, const unsigned int, const unsigned int seed = 0 );
	/*void TerraGen_Bridges_ExcavateCaves ( CBoob *, RangeVector const& );
	void TerraGen_Bridges_ExcavateCave ( CBoob *, Ray, RangeVector const& );*/
	// River generation
	void TerraGen_Rivers_Exclusify ( RangeVector const& );
	void TerraGen_Rivers_Generate ( const rangeint, const rangeint );
	void TerraGen_Rivers_Load ( RangeVector const& );
	struct TerraRiver;
	bool TerraGen_Rivers_Read ( TerraRiver&, CBinaryFile& );
	void TerraGen_Rivers_Write ( const TerraRiver&, CBinaryFile& );
	void TerraGen_Rivers_Excavate ( CBoob *, RangeVector const& );
	// Dungeon Generation
	void TerraGen_Dungeons_Generate ( RangeVector const& );
	void TerraGen_Dungeons_Excavate ( CBoob *, RangeVector const& );

	//list<RangeVector>		dungeonChecklist;
	//unordered_map<RangeVector,int8_t>  dungeonChecklist;
	vector<CDungeonBase*>	dungeonSamplerList;
	//vector<CDungeonBase*>	dungeonList;
	//unordered_map<CDungeonBase*,int8_t> dungeonList;
	unordered_map<RangeVector,CDungeonBase*> dungeonList;
	vector<CDungeonBase*>	dungeonLocalList;


	friend CDungeonBase;

	// Pass 2
	// Instance generation
	void TerraGen_Instances_Global_Create ( void );
	void TerraGen_Instances_Create ( CBoob *, const RangeVector & );
	EInstanceType TerraGen_Instances_Choose ( CBoob *, const RangeVector &, const Vector3d & ); 
	bool TerraGen_Instances_Downtrace ( TerraInstance *, CBoob *, const RangeVector & );
	bool TerraGen_Instances_Downtrace_Fallback ( TerraInstance *, CBoob *, const RangeVector & );
	bool TerraGen_Instances_Uptrace_Fallback ( TerraInstance *, CBoob *, const RangeVector & );
	void TerraGen_Instances_Generate ( CBoob *, const RangeVector & );
	void TerraGen_Instances_Generate_Work ( TerraInstance *, CBoob *, const RangeVector & );
	// Instance types
	ushort TerraGen_Instances_RuinSpike		( TerraInstance *, const Vector3d &, ushort ); // make classes or structs?
	ushort TerraGen_Instances_RuinPillar	( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Ravine		( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Path_Gravel	( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Desert_Rocks0	( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Desert_RockWtr( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Desert_Oasis	( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Crystal_Spike	( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Vein_Crystal	( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Vein_Clay		( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Boulder_Small ( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Deadlands_Crater ( TerraInstance *, const Vector3d &, ushort );

	ushort TerraGen_Instances_Ore_Vein	( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Ore_Splotch	( TerraInstance *, const Vector3d &, ushort );
	ushort TerraGen_Instances_Ore_Splatter	( TerraInstance *, const Vector3d &, ushort );

	ushort TerraGen_Instances_Clearing		( TerraInstance *, const Vector3d &, ushort );
	

	// Pass 3
	// Biome types
	ushort TerraGen_3p_Default		( CBoob *, Vector3d const&, ushort, bool );
	ushort TerraGen_3p_Grassland	( CBoob *, Vector3d const&, ushort, bool );
	ushort TerraGen_3p_Swamp		( CBoob *, Vector3d const&, ushort, bool );
	ushort TerraGen_3p_GrassDesert	( CBoob *, Vector3d const&, ushort, bool );
	ushort TerraGen_3p_Taiga		( CBoob *, Vector3d const&, ushort, bool, bool );
	ushort TerraGen_3p_Tundra		( CBoob *, Vector3d const&, ushort, bool, bool );
	ushort TerraGen_3p_SuperTundra	( CBoob *, Vector3d const&, ushort, bool, bool );
	// Biome helpers
	bool	TerraGen_3p_CanMakeTree ( const Vector3d & blockPosition, ftype density );

public:
	// =====
	// Noise objects
	BaseNoise* noise;
	BaseNoise* noise_hf;
	BaseNoise* noise_hhf;
	BaseNoise* noise_lf;
	BaseNoise* noise_biome;
	BaseNoise* noise_terra;

	BaseNoise* noise_pub;
	BaseNoise* noise_pub_hf;
	BaseNoise* noise_pub_hhf;
	BaseNoise* noise_pub_lf;
	BaseNoise* noise_pub_biome;
	BaseNoise* noise_pub_terra;

	// =====
	// Samplers
	void TerraGenGetSamplers ( void );
	unsigned char* pSamplerTerrainType;
	unsigned char* pSamplerBiomeType;
	Vector3d vMainSamplerOffset;
	ftype	fCurrentElevation;
	char	iCurrentTerraType;
	ftype	fCurrentRockiness;
	ftype	fCurrentUnpredictability;
	bool	bHasSamplers;
	BlockInfo currentBlock;

	// =====
	// Caves excavate material, while Bridges are additive structures.
	// They both can be used together with similar seeding values to generate interesting natural structures.
	// Memory wise, their storage is the nearly exact same, with no differences in their shape definitions.
	// Cave/dungeon generation
	//Vector3d vCurrentCaveCenter;

	// =====
	// Patterns are seeded objects that change block types. The have the ability to change their area over time.
	// Especially, patterns have the ability to "regenerate." On an input, they can edit an already generated area.
	// They are used for towns, castles, and other non-natural formations with patternlike elements.
	// Patterns are important enough that they are saved to the hard disk.
	CRegionManager*	regionManager;
		friend	CRegionManager;
	CTownManager*	townManager;
		friend	CTownManager;

	// =====
	// Instances just change things based on where they are. They're not so much of an add/subtract as it is a placing.
	// They have an option to downtrace on creation, or just create wherever their position says.
	// There is a hasdowntraced bool to check if something has already hit the ground.
	// Massive is a bool to control the order of generation.

	// =====
	// Rivers are cool. River Song is pretty cool too. River Tam is also cool.
	struct TerraRiver
	{
		uint32_t	index;
		RangeVector startVector;
		vector<Ray> riverSegments;
		vector<RangeVector> riverSegmentSources;
		vector<RangeVector> areas;
	};
	RangeVector		vCurrentFeatureIndex;
	vector<TerraRiver>	activeRivers;
	uint32_t	iFeatureCount;

	// Rivers and features use the same counter (iFeatureCount)
	// As unique features are not too numerous (between 100 and 200 at most), all world features are kept in memory
	// and saved into a single file (features.state)
	struct TerraFeature
	{
		uint32_t	index;
		uint16_t	type;
		uint16_t	padding;
		Vector3d	centerposition;
		ftype		generationRadius;
		RangeVector	generationPosition;
	};
	vector<TerraFeature>	featureList;

};

#endif