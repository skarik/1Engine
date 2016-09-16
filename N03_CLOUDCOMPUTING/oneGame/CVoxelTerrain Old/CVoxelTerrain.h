

#ifndef _C_VOXEL_TERRAIN_
#define _C_VOXEL_TERRAIN_

// Includes
#include "CBoob.h"
#include "CGameObject.h"
#include "CRenderableObject.h"
#include "Vector3d.h"
#include "Ray.h"
#include "RaycastHit.h"

#include "Perlin.h"
#include "CCamera.h"
#include "CInput.h"
#include "CToBeSeen.h"
#include "CCubic.h"
#include "CFrustum.h"

#include "glMaterial.h"

#include "CDebugConsole.h"

#include "CBinaryFile.h"

#include "CWeaponItem.h"

#include <bitset>
#include <math.h>
#include <algorithm>

// Boost Threads for awesomeness
#include "boost\thread.hpp"

// Namespace
using std::bitset;

using boost::thread;
using boost::mutex;

#include "LongIntPosition.h"
#include "BlockInfo.h"

struct sVTQueuedLoad
{
	CBoob*			pBoob;
	LongIntPosition	position;
	bool			regenSurround;
	bool			regenMesh;
	bool			regenWater;
};
struct sVTQueuedSave
{
	CBoob*			pBoob;
	LongIntPosition	position;
};

// Generator prototype
class CTerrainGenerator;

// Class Definition prototype
class CVoxelTerrain;

// Class Definition for render assistance
class CVoxelTerrainTranslucentRenderer : public CRenderableObject
{
public:
	explicit CVoxelTerrainTranslucentRenderer ( CVoxelTerrain* );
	~CVoxelTerrainTranslucentRenderer ( void );

	bool Render ( const char pass );
private:
	//==Terrain texture==
	glMaterial* myMat;
	
	//==Parent terrain==
	CVoxelTerrain* pMaster;
	friend CVoxelTerrain;
};

// Class Definition
class CVoxelTerrain : public CGameObject, public CRenderableObject
{
public:
	CVoxelTerrain ( void );
	~CVoxelTerrain ( void );

	void Update ( void );
	void PostUpdate ( void );

	bool PreRender ( const char pass );
	bool Render ( const char pass );

	static CVoxelTerrain* GetActive ( void )
	{
		if ( !terrainList.empty() )
			return terrainList[0];
		else
			return NULL;
	}

	short iploadstate;

public:
	static vector<CVoxelTerrain*> terrainList;

public:
	// Set Terrain Updates
	void	ForceFullUpdate ( void );
	
	// This toggles whether or not to move the root.
	// Turning off root update can be especially useful for cutscenes where
	// the camera moves, and you don't want any lag coming fromterrain I/O.
	void	SetRootUpdate ( bool );

	// Get Terrain Updates
	int		GetLoadingListSize ( void );
	int		GetSavingListSize ( void );

	// This returns if the terrain has finished its initial load, which includes the regen.
	bool	GetDoneLoading ( void );
	float	GetLoadingPercent ( void );

	// Check terrain stuff
	bool		GetActiveCollision ( Vector3d const& );
	int			GetTerrainRange ( void ) { return iTerrainSize; }

	// Grab terrain stuff
	CBoob*		GetBoobAtPosition ( Vector3d const& );
	subblock16*	GetSubblock16AtPosition ( Vector3d const& );
	subblock8*	GetSubblock8AtPosition ( Vector3d const& );
	char		GetBlockAtPosition ( Vector3d const& );
	bool		GetBlockInfoAtPosition ( Vector3d const&, BlockInfo & );

	bool		GetBlockOnEdge ( BlockInfo const&, EFaceDir );
	bool		GetBlockOnEdge ( char, char, short, EFaceDir );

	bool		SetBlockAtPosition ( Vector3d const&, char );
	bool		SetBlock( BlockInfo const&, char );

	CWeaponItem* ItemizeBlockAtPosition ( Vector3d const& );
	CWeaponItem* ItemizeBlock( BlockInfo const& );

	const LongIntPosition&	GetRootPosition ( void );

	// ==Raycasting==
	bool		Raycast ( Ray const&, ftype, BlockInfo *, RaycastHit * );
	// ==Raycasting subroutines==
private:
	Ray			rcst_ray;
	ftype		rcst_maxdist;
	BlockInfo*	rcst_pOutBlockInfo;
	RaycastHit*	rcst_pOutHitInfo;
	bool		rcst_bMadeCollision;
	ftype		rcst_curdist;
	ftype		rcst_mindist;
	CBoob*		rcst_pCurrentBoob;
	subblock16*	rcst_pCurrentSubblock;
	Vector3d	rcst_vCurSubpos;

	void		Raycast32 ( CBoob * );
	void		Raycast16 ( char );
	void		Raycast08 ( char );
	void		Raycast01 ( Vector3d const& );

	CCubic		GetAreaBoundingBox ( Vector3d const&, ftype );
	CCubic		CreateBoundingBox  ( Vector3d const&, ftype );
public:
	// ==BoxColliding==
	bool		BoxCollides ( CCubic const& );
	// ==BoxColliding subroutines==
private:
	Line		boxc_line;
	bool		boxc_bMadeCollision;

	void		BoxCollide32 ( CBoob * );
	void		BoxCollide16 ( char );
	void		BoxCollide08 ( char );
	void		BoxCollide01 ( Vector3d const& );

public:
	// ==Terrain Generation==
	char		TerraGen_pub_GetType	( Vector3d const& );
	char		TerraGen_pub_GetBiome	( Vector3d const& );
	ftype		TerraGen_pub_GetElevation ( Vector3d const& );

	// Foliage Def
	struct TerraFoiliage
	{
		unsigned short	foliage_index;
		Vector3d	position;
		//uint64_t	bitmask;
		string		userdata;
		CBoob*		myBoob;
	};

	//=== PRIVATE VARIABLES and ROUTINES ===
private:
	/*char		TerraGen_priv_GetType	( Vector3d const& );
	char		TerraGen_priv_GetBiome	( Vector3d const& );*/

	//==Terrain texture==
	glMaterial* myMat;
	friend CVoxelTerrainTranslucentRenderer;
	CVoxelTerrainTranslucentRenderer* my2ndRenderer;

	//==Terrain Generation==
	friend CTerrainGenerator;
	CTerrainGenerator*	generator;

	vector<TerraFoiliage>	vFoliageQueue;

	void TerraGen_NewFoliage ( CBoob *, Vector3d const&, string const& );
	void TerraGen_NewComponent ( CBoob *, Vector3d const&, string const& );
	void TerraGen_NewGrass ( CBoob *, Vector3d const&, unsigned short );

	// These are still used in terrain generation
	unsigned char cLastCheckedBlock;
	bool SideVisible ( CBoob * pBoob, char const index16, char const index8, char * data, int const i, int const width, EFaceDir const dir, unsigned char ofs, Vector3d const& dot );
	bool InSideVisible ( CBoob * pBoob, char const index16, char const index8, char * data, int const i, int const width, EFaceDir const dir, unsigned char ofs );

	/*enum eGenerationMode {
		GENERATE_TO_MEMORY,
		GENERATE_TO_DISK,
		GENERATE_DUAL_SAVE
	};
	eGenerationMode	iGenerationMode;

	void GenerateTerrain ( CBoob *, LongIntPosition const& );
	void GenerateSingleTerrain ( CBoob *, LongIntPosition const& );
	void GenerateTerrainSector ( CBoob *, LongIntPosition const& );

	// =First pass: terrain generation=
	void TerraGenLandscapePass ( CBoob *, LongIntPosition const& );

	ftype TerraGen_1p_GetElevation ( LongIntPosition const& );
	ftype TerraGen_1p_GetElevation ( Vector3d const& );

	// Terrain types
	char TerraGen_1p_Default ( CBoob *, Vector3d const& );
	char TerraGen_1p_Flatlands ( CBoob *, Vector3d const& );
	char TerraGen_1p_Outlands ( CBoob *, Vector3d const& );
	char TerraGen_1p_Mountains ( CBoob *, Vector3d const& );
	char TerraGen_1p_Hilllands ( CBoob *, Vector3d const& );
	char TerraGen_1p_Islands ( CBoob *, Vector3d const& );
	char TerraGen_1p_Ocean ( CBoob *, Vector3d const& );
	char TerraGen_1p_Spires ( CBoob *, Vector3d const& );
	char TerraGen_1p_Desert ( CBoob *, Vector3d const& );
	char TerraGen_1p_Badlands ( CBoob *, Vector3d const& );
	char TerraGen_1p_TheEdge ( CBoob *, Vector3d const& );

	// After creating terrain, switch to stone
	void TerraGenStonePass ( CBoob *, LongIntPosition const& );
	void TerraGenUnderdirtPass ( CBoob *, LongIntPosition const& );

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
		ECaveType type;
	};
	struct TerraCave
	{
		LongIntPosition	startPosition;
		vector<LongIntPosition> affectedAreas;
		vector<vector<LongIntPosition>*> affectionMap;
		vector<TerraTunnel>	partTree;
	};
	vector<TerraCave*> vCaveSystems;
	Vector3d vCurrentCaveCenter;

	void TerraGenCavePass ( CBoob *, LongIntPosition const& );
	void TerraGen_Caves_CreateSystems ( LongIntPosition const& );
	void TerraGen_Caves_StartSystem ( LongIntPosition const& );
	void TerraGen_Caves_Generate ( TerraCave*, const unsigned int, const unsigned int seed = 0 );
	void TerraGen_Caves_ExcavateCaves ( CBoob *, LongIntPosition const& );
	void TerraGen_Caves_ExcavateCave ( CBoob *, Ray, LongIntPosition const& );

	// Bridge Generation
	vector<TerraCave*> vBridgeSystems;
	
	void TerraGenBridgePass ( CBoob *, LongIntPosition const& );
	void TerraGen_Bridges_CreateSystems ( LongIntPosition const& );
	void TerraGen_Bridges_StartSystem ( LongIntPosition const& );
	void TerraGen_Bridges_Generate ( TerraCave*, const unsigned int, const unsigned int seed = 0 );
	void TerraGen_Bridges_ExcavateCaves ( CBoob *, LongIntPosition const& );
	void TerraGen_Bridges_ExcavateCave ( CBoob *, Ray, LongIntPosition const& );

	// =====
	// Conversion of a coordinate in boob space to a boob index
	inline LongIntPosition BoobSpaceToIndex ( Vector3d const& pos )
	{
		LongIntPosition result;
		result.x = int(floor(pos.x));
		result.y = int(floor(pos.y));
		result.z = int(floor(pos.z));
		return result;
	}

	// =Second pass: instanced terrain generation=
	void GenerateTerrainSectorSecondPass ( CBoob *, LongIntPosition const& );

	// =====
	// Instances just change things based on where they are. They're not so much of an add/subtract as it is a placing.
	// They have an option to downtrace on creation, or just create wherever their position says.
	// There is a hasdowntraced bool to check if something has already hit the ground.
	// Massive is a bool to control the order of generation.

	struct TerraInstance
	{
		Vector3d position;
		vector<LongIntPosition> affectedAreas;
		LongIntPosition	startPosition;
		bool downtrace;
		bool hasdowntraced;
		bool ignore;
		bool massive;
		EInstanceType type;
	};
	vector<TerraInstance*> vInstanceSystems;

	void TerraGenInstancePass ( CBoob *, const LongIntPosition & );
	void TerraGen_Instances_Create ( CBoob *, const LongIntPosition & );
	EInstanceType TerraGen_Instances_Choose ( CBoob *, const LongIntPosition &, const Vector3d & ); 
	bool TerraGen_Instances_Downtrace ( TerraInstance *, CBoob *, const LongIntPosition & );
	bool TerraGen_Instances_Downtrace_Fallback ( TerraInstance *, CBoob *, const LongIntPosition & );
	bool TerraGen_Instances_Uptrace_Fallback ( TerraInstance *, CBoob *, const LongIntPosition & );
	void TerraGen_Instances_Generate ( CBoob *, const LongIntPosition & );
	void TerraGen_Instances_Generate_Work ( TerraInstance *, CBoob *, const LongIntPosition & );

	char TerraGen_Instances_RuinSpike		( TerraInstance *, const Vector3d &, char );
	char TerraGen_Instances_RuinPillar		( TerraInstance *, const Vector3d &, char );
	char TerraGen_Instances_Ravine			( TerraInstance *, const Vector3d &, char );
	char TerraGen_Instances_Desert_Rocks0	( TerraInstance *, const Vector3d &, char );
	char TerraGen_Instances_Desert_RockWtr	( TerraInstance *, const Vector3d &, char );
	char TerraGen_Instances_Desert_Oasis	( TerraInstance *, const Vector3d &, char );
	char TerraGen_Instances_Crystal_Spike	( TerraInstance *, const Vector3d &, char );

	// =Third pass: biome+prop generation=
	void GenerateTerrainSectorThirdPass ( CBoob *, LongIntPosition const& );

	void TerraGenBiomePass ( CBoob *, LongIntPosition const& );

	// Biome Gen
	struct TerraFoiliage
	{
		unsigned short	foliage_index;
		Vector3d	position;
		//uint64_t	bitmask;
		string		userdata;
		CBoob*		myBoob;
	};
	vector<TerraFoiliage>	vFoliageQueue;

	void TerraGen_NewFoliage ( CBoob *, Vector3d const&, string const& );
	void TerraGen_NewComponent ( CBoob *, Vector3d const&, string const& );
	void TerraGen_NewGrass ( CBoob *, Vector3d const&, unsigned short );

	// Biome types
	char TerraGen_3p_Default		( CBoob *, Vector3d const&, char, bool );
	char TerraGen_3p_Grassland		( CBoob *, Vector3d const&, char, bool );
	char TerraGen_3p_GrassDesert	( CBoob *, Vector3d const&, char, bool );

	void TerraGenGrassPass ( CBoob *, LongIntPosition const& );
	void TerraGenTreePass ( CBoob *, LongIntPosition const& );

	void GenerateTerrainSectorCleanup ( CBoob * );


	// =Noise Functions=
	Perlin* noise;
	Perlin* noise_hf;
	Perlin* noise_hhf;
	Perlin* noise_lf;
	Perlin* noise_biome;
	Perlin* noise_terra;
	*/
	long int iTerrainSeed;

	/*Perlin* noise_pub;
	Perlin* noise_pub_hf;
	Perlin* noise_pub_hhf;
	Perlin* noise_pub_lf;
	Perlin* noise_pub_biome;
	Perlin* noise_pub_terra;
	*/

	// Sampler Information
	/*void TerraGenGetSamplers ( void );
	unsigned char* pSamplerTerrainType;
	unsigned char* pSamplerBiomeType;
	ftype	fCurrentElevation;
	bool	bHasSamplers;*/

	// Whether or not to generate terrain
	bool	bSpreadTerrain;

	//==Terrain Dynamic Loading Information==
	bool	bRootUpdated;
	int		iTerrainSize;

	bool	bDrawDebugBoobs;

	// Whether or not to load low-resolution terrain
	bool	bLoadLowDetail;
	// Whether or not the last mesh update failed with an std::bad_alloc
	bool	bFailedBadAlloc;

	//==Terrain Loading State==
	bool	bInitialLoad;
	bool	bInitialLoadDone;

	//==Terrain Threading==
	// Loading and Simulation Threading
	bool	bContinueThread;

	mutex	mtGaurd;
	thread	mtThread;

	struct sCallableThread
	{
		CVoxelTerrain* pMyTerrain;
		void operator() ( void );
	};
	friend sCallableThread;

	void StartSideThread ( void );
	void EndSideThread ( void );
	void WorkSideThread ( void );

	// Generation Threading
	bool	bContinueGenerationThread;
	
	mutex	mtGenerationListGaurd;
	thread	mtGenerationThread;

	struct sGenerationThread
	{
		CVoxelTerrain* pMyTerrain;
		void operator() ( void );
	};
	friend sGenerationThread;

	void StartGenerationThread ( void );
	void EndGenerationThread ( void );
	void WorkGenerationThread ( void );

	struct sGenerationRequest
	{
		LongIntPosition	position;
		bool			checkfile;
	};
	vector<sGenerationRequest>	vGenerationRequests;

	void AddGenerationRequest ( const LongIntPosition &, const bool=false );

	//==Culling stuff==
	GLuint iCubeList;
	GLuint iFaceList;
	Vector3d vCameraDir;
	char renderMethod;
	char stepNum;

	//==Position Update Information==
	Vector3d vCameraPos;
	bool	bUpdateCameraPos;

	//==Boob Information==
	CBoob * root;
	LongIntPosition root_position;
	static const LongIntPosition boobSize;
	static const ftype blockSize;
	
	//==Terrain List==
	vector<CBoob*> terraList;
	mutex mtTerraListGuard;

	//==Terrain Vertex Data== (is this still used?)
	GLuint iVBOverts;
	GLuint iVBOfaces;
	unsigned int vertexCount;
	unsigned int faceCount;
	bool needUpdateOnVBO;

	unsigned short iVBOType;

	CTerrainVertex* vertices;
	CModelQuad* quads;

	//==New Boob Generation==
	// Creates a new boob object with default values
	CBoob* GenerateBoob ( void );
	CMetaboob*	GenerateMetaboob ( void );
	// Link a cube in the direction
	//  Last argument is a direction, values defined EFaceDir
	void LinkCube ( CBoob *, CBoob *, char const& );
	// Adds the entire terrain to the update list
	void ForceRecursiveUpdate ( CBoob *, LongIntPosition const& );

	//==Boob Updating==
	// Updates the center of the terrain based on player position
	void UpdateRoot ( void );
	// Updates the LOD when the terrain is moved
	void UpdateLODs ( CBoob *, char, LongIntPosition const& );

	// Updates the center of the loaded terrain based on root position and loads/unloads terrain based on what's needed.
	void PerformCenterMovement ( void ); // Partially part of new boob generation
	void CenterMoveTree32 ( CBoob *,CBoob *, LongIntPosition const&, EFaceDir );
	void CenterMoveFreeTree32 ( CBoob *, LongIntPosition const& );

	// Loading list. Routines for moving the loading cycle to the side instead of in the bottle neck
	void PopLoadingList ( void );
	int IsInLoadingList ( CBoob* );
	void RemoveFromLoadingList ( int );
	void PushLoadingList ( sVTQueuedLoad );
	void PushLoadingList ( CBoob*, LongIntPosition const&, bool recursive=false );
	void PushLoadingListWater ( CBoob*, LongIntPosition const&, bool recursive=false );
	void ReorderLoadingList ( CBoob* );

	vector<sVTQueuedLoad> vBoobLoadList;
	mutex	mtLoadingListGaurd;
	unsigned int iPopListCount;

	// Saving list. Routines for moving the saving cycle to the side.
	void PopSavingList ( void );
	int IsInSavingList ( CBoob* );
	void RemoveFromSavingList ( int );
	void PushSavingList ( sVTQueuedSave );
	void PushSavingList ( CBoob*, LongIntPosition const& );
	void ReorderSavingList ( CBoob* );

	mutex	mtSavingListGaurd;
	vector<sVTQueuedSave> vBoobSaveList;
	
	// Loading Checks. Check for first loading finish.
	bool CheckLoadedAndVisible ( CBoob* );

	// Check if boob exists
	CBoob* pCurrentTargetBoob;
	bool BoobExists ( CBoob* );
	bool BoobExistsRecursive ( CBoob* );

	//==Culling==
	// Performs frustum culling recursively. Still needs a little bit of work.
	void CullTree32 ( CBoob *, LongIntPosition const& );
	void CullTree16 ( CBoob *, subblock16 *, char const, LongIntPosition const& );
	void CullTree8 ( CBoob *, subblock8 *, char const, char const, LongIntPosition const& );
	// Occlusion culling. Still needs to be implemented.
	void PerformOcclusion ( void );
	void OccludeTree ( CBoob *, LongIntPosition const& );

	// Frees the terrain objects recursively
	void FreeTerrain ( CBoob* );
	void FreeBoob ( CBoob* );
	void FreeMetaboob ( CMetaboob* );

	//==Saving and Loading==
	void WriteBoobToFile ( CBoob *, CBinaryFile * );
	void WriteMetaboobToFile ( CMetaboob *, CBinaryFile * );
	void ReadBoobFromFile ( CBoob *, CBinaryFile * );

	void GetBoobData ( CBoob *, LongIntPosition const&, bool generateTerrain );
	void SaveBoobData ( CBoob *, LongIntPosition const& );
	void SaveMetaboobData ( CMetaboob *, LongIntPosition const& );

	void LoadTerrain ( CBoob *, LongIntPosition const& );
	void SaveActiveTerrain ( CBoob *, LongIntPosition const& );

	bool TerraFileExists ( void );

	bool	bQueueLoadingListPop;

	//==Foliage Management==
	unsigned short GetFoliageType ( CFoliage* pFoliage );
	unsigned short GetFoliageType ( string const& stName );
	CFoliage* CreateFoliage ( unsigned short iType, Vector3d const& vInPos, const string & iInData );
	void	CreateQueuedFoliage ( void );
	//==Component Management==
	unsigned short GetComponentType ( CTerraComponent* pComponent );
	CTerraComponent* CreateComponent ( unsigned short iType, Vector3d const& vInPos, Vector3d const& vInRot, Vector3d const& vInScale, uint64_t const iInData, CBoob*, char const, char const, short const );

	//==User Block Reset==
	void ResetBlock7Flag ( CBoob * );

	//==Drawing and VBO Regeneration==
	// Resets the boob's drawing state. Needs to be done before drawing
	void ResetBlockDrawn ( CBoob * );
	// Render the actual terrain
	void RenderOcttree ( CBoob *, LongIntPosition const& );
	// Render the actual terrain
	void RenderWaterOcttree ( CBoob *, LongIntPosition const& );

	// Generate the VBO. Called before render to check if need to update
	void GenerateVBO ( void );
	// Regenerates the boob VBO's. Also regenerates the boob collision meshes.
	void GenerateTree32 ( CBoob *, LongIntPosition const& );
	void GenerateTree16 ( CBoob *, subblock16 *, char const, LongIntPosition const& );
	void GenerateTree8 ( CBoob *, subblock8 *, char const, char const, LongIntPosition const& );
	// Regenerates the boob Water VBO's.
	void GenerateWater16 ( CBoob *, subblock16 *, char const, LongIntPosition const& );
	void GenerateWater8 ( CBoob *, subblock8 *, char const, char const, LongIntPosition const& );
	// Regenerates the boob lighting. Should be done before VBO Generation
//	void LightTree32 ( CBoob *, LongIntPosition const& );
//	void LightTree16 ( CBoob *, subblock16 *, char const, LongIntPosition const& );
	void LightTree8 ( CBoob *, subblock8 *, char const, char const );
	inline bool LightTrace ( CBoob *, subblock8 *, char const, char const, short const, Vector3d );
	inline bool BlockHasNeighbors ( CBoob* , char const index16, char const index8, char * data, int const i, int const width );
	bool TraverseTree ( CBoob ** pCurrentBoob, char& index16, char& index8, short& i, EFaceDir direction );

	//==System Simulation==
	bool	bQueueSimulation;
	unsigned char iWaterSimulation;
	bool	bWaterUpdated;
	float	fWaterSimulation;
	float	fWireSimulation;
	bool	bWireQueueUpdate;
	void UpdateSimulation ( CBoob *, LongIntPosition const& );

	void UpdateWater_Low32 ( CBoob *, LongIntPosition const& );
	void UpdateWater_Low16 ( CBoob *, subblock16 *, char const, LongIntPosition const& );
	void UpdateWater_Low8 ( CBoob *, subblock8 *, char const, char const, LongIntPosition const& );
	void UpdateWater_Low1 ( CBoob *, subblock8 *, char const, char const, short const, LongIntPosition const& );

	void UpdateWire_Reset32 ( CBoob *, LongIntPosition const& );

	void UpdateWire_Default32 ( CBoob *, LongIntPosition const& );
	void UpdateWire_Default16 ( CBoob *, subblock16 *, char const, LongIntPosition const& );
	void UpdateWire_Default8 ( CBoob *, subblock8 *, char const, char const, LongIntPosition const& );
	void UpdateWire_Default1 ( CBoob *, subblock8 *, char const, char const, short const, LongIntPosition const& );

	//==NEED MORE COMMENTS AND CLEANUP BELOW THIS POINT==

	// Yeah rendering. I don't even know. These aren't used.
	void RenderBoob ( CBoob *, LongIntPosition const& );
	void RenderBlock ( Vector3d const& );
	void RenderBlock ( Vector3d const&, char const& );
	void RenderBlockExp ( Vector3d const&, CBoob *, unsigned int const& );
	void GenerateCube ( void );

	// I doubt these are used either.
	bool BlockVisible ( char *, unsigned int const& );
	bool BlockVisibleEx ( char *, unsigned int const&, int const& );

	// I don't think this one is used either.
	void GetBlockXYZ( unsigned int &,unsigned int &,unsigned int &,unsigned int const&,unsigned int const&,unsigned int const& );

	// And I don't think these ones are used.
	void RenderTree32 ( CBoob *, LongIntPosition const& );
	void RenderTree16 ( CBoob *, subblock16 *, char const&, LongIntPosition const& );
	void RenderTree8 ( CBoob *, subblock8 *, char const&, char const&, LongIntPosition const& );

	//void GenerateCube ( CBoob *, char const, char const, int const, Vector3d const&, char const );

	bool BlockVisibleEx ( CBoob *, subblock8 *, char const&, char const&, char *, unsigned int const&, int const& );
	// Checks if a side is visible.
	//  SideVisible ( boob, index16, index8, index8's data, final index, width=1, EFaceDir );
	// This one seems broken though...remove later?
	bool SideVisible ( CBoob * pBoob, char const index16, char const index8, char * data, unsigned int const i, int const width, EFaceDir const dir ); //unused

	char GetCube ( CBoob*, LongIntPosition const& );
};

#endif