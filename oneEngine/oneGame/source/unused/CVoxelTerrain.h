

#ifndef _C_VOXEL_TERRAIN_
#define _C_VOXEL_TERRAIN_

// Includes
#include "CGameObject.h"
#include "CRenderableObject.h"
#include "Vector3d.h"
#include "Ray.h"
#include "RaycastHit.h"

#include "CBoob.h"
#include "CMetaboob.h"
#include "CLODBoobMesh.h"

//#include "Perlin.h"
#include "CCamera.h"
#include "CInput.h"
//#include "CRenderState.h"
#include "Cubic.h"
#include "Frustum.h"

#include "CDebugConsole.h"
#include "CBinaryFile.h"
class CWeaponItem;

#include "RangeVector.h"
#include "BlockInfo.h"

#include <bitset>
#include <math.h>
#include <algorithm>

#include "boost\thread.hpp"

#include "Events.h"

// Usings
using std::bitset;
//using boost::thread;
//using boost::mutex;

// Generator prototype
class CTerrainGenerator;
// IO prototype
class CTerrainIO;

// Class Definition prototype
class CVoxelTerrain;

// Class Definition
class CVoxelTerrain : public CGameObject, public CRenderableObject
{
	ClassName( "CVoxelTerrain" );
	BaseClass( "CVoxelTerrain" );

			typedef boost::thread	thread;
			typedef boost::mutex	mutex;
public:
			typedef Terrain::terra_t terra_t;
			typedef Terrain::terra_t_lod terra_t_lod;

						CVoxelTerrain ( void );
						~CVoxelTerrain ( void );

	void				Update ( void );
	void				PostUpdate ( void );

	static inline		CVoxelTerrain* GetActive ( void );
public:
		static vector<CVoxelTerrain*>	terrainList;	// Active terrain list

public:
	// Set Terrain Updates
	void				ForceFullUpdate ( void );
	
	// This toggles whether or not to move the root.
	// Turning off root update can be especially useful for cutscenes where
	// the camera moves, and you don't want any lag coming fromterrain I/O.
	void				SetRootUpdate ( bool );
	// This moves the root manually
	void				SetRootTarget ( const Vector3d & );

	int					GetLoadingListSize ( void );
	int					GetSavingListSize ( void );

	int					GetGenerationListSize ( void );

	bool				GetDoneLoading ( void );		// Check for finish initial loading
	float				GetLoadingPercent ( void );

	// Check terrain properties
	bool				GetActiveCollision ( Vector3d const& );
	int					GetTerrainRange ( void ) { return iTerrainSize; }

	// Grab and edit terrain data
	CBoob*				GetBoobAtPosition ( Vector3d const& );
	subblock16*			GetSubblock16AtPosition ( Vector3d const& );
	subblock8*			GetSubblock8AtPosition ( Vector3d const& );
	terra_t				GetBlockAtPosition ( Vector3d const& );
	bool				GetBlockInfoAtPosition ( Vector3d const&, BlockInfo & );

	bool				IsBlockOnEdge ( BlockInfo const&, EFaceDir );
	bool				IsBlockOnEdge ( char, char, short, EFaceDir );

	bool				SetBlockAtPosition ( Vector3d const&, ushort );
	bool				SetBlock( BlockInfo const&, ushort );

	CWeaponItem*		ItemizeBlockAtPosition ( Vector3d const& );
	CWeaponItem*		ItemizeBlock( BlockInfo const& );

	void				DestroyBlock ( BlockInfo const& );
	void				CompressBlock( BlockInfo const& );

	CBoob*				LockBoob ( const RangeVector& );	// Locks and loads an area for real-time editing
	bool				UnlockBoob ( CBoob* );

	const RangeVector&	GetRootPosition ( void ) const;

	vector<CBoob*>&		GetLockTerraList ( void );
	void				ReleaseLockTerraList ( void );

public:
	// Events
	CbpEvent	eventLoadSector;	// On load sector event
	CbpEvent	eventUnloadSector;	// Pre-unload sector event

	// ==Raycasting==
	bool				Raycast ( Ray const&, ftype, BlockInfo *, RaycastHit * );
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

	void				Raycast32 ( CBoob * );
	void				Raycast16 ( char );
	void				Raycast08 ( char );
	void				Raycast01 ( Vector3d const& );

	Maths::Cubic		GetAreaBoundingBox ( Vector3d const&, ftype );
	Maths::Cubic		CreateBoundingBox  ( Vector3d const&, ftype );
public:
	// ==BoxColliding==
	bool				BoxCollides ( Maths::Cubic const& );
	// ==BoxColliding subroutines==
private:
	Line				boxc_line;
	bool				boxc_bMadeCollision;

	void				BoxCollide32 ( CBoob * );
	void				BoxCollide16 ( char );
	void				BoxCollide08 ( char );
	void				BoxCollide01 ( Vector3d const& );

public:
	// ==Terrain Generation==
	char				TerraGen_pub_GetType	( Vector3d const& );
	char				TerraGen_pub_GetBiome	( Vector3d const& );
	ftype				TerraGen_pub_GetElevation ( Vector3d const& );

	// Foliage Def
	struct TerraFoiliage;
	// Prop def
	struct TerraProp;

	//=== PRIVATE VARIABLES and ROUTINES ===
private:
	// Queued I/O Structs
	struct sVTQueuedLoad;
	struct sVTQueuedSave;
	// IO sorter
	struct tLoadingListComparator;
	struct tGenerationListComparator;

	//==Terrain Memory Management==
		// The memory block for the entire terrain
		uint32_t*	pTerraData;		
		// Stores flags for data usage. 0 indicates corresponding spot, 1 indicates the spot is in use.
		char*		pTerraUsage;	
		// Max size of the data. Will vary based on user settings
		uint32_t	iMaxBlocks;

	bool				InitTerraMemory ( void );
	void				FreeTerraMemory ( void );

	void				NewDataBlock  ( subblock16** block );
	void				FreeDataBlock ( subblock16* block );

	terra_t_lod*		GetLODDataBlock ( uint32_t level );

	//==Terrain Rendering==
		glMaterial*	myMat;
		//friend CVoxelTerrainTranslucentRenderer;
		//CVoxelTerrainTranslucentRenderer* my2ndRenderer;
		glMaterial*	myWaterMat;

		glMaterial* myLODMat;

	//==Terrain Generation==
			friend		CTerrainGenerator;
		CTerrainGenerator*		generator;

		vector<TerraFoiliage>	vFoliageQueue;
		vector<TerraProp>		vComponentQueue;

	void				TerraGen_NewFoliage ( CBoob *, Vector3d const&, string const& );
	void				TerraGen_NewComponent ( CBoob *, const BlockInfo& block, const Vector3d&, const string& );
	void				TerraGen_NewGrass ( CBoob *, Vector3d const&, unsigned short );

	int					TerraGen_GetFoliageCount ( CBoob *, const string& );
	int					TerraGen_GetComponentCount ( CBoob *, const string& );
	int					TerraGen_GetGrassCount ( CBoob *, unsigned short );

	// These are still used in terrain generation
	unsigned short cLastCheckedBlock;
	bool				SideVisible ( CBoob * pBoob, char const index16, char const index8, terra_t * data, int const i, int const width, EFaceDir const dir, unsigned char ofs );
	bool				InSideVisible ( CBoob * pBoob, char const index16, char const index8, char * data, int const i, int const width, EFaceDir const dir, unsigned char ofs );

		long int	iTerrainSeed;

		// Whether or not to generate terrain
		bool		bSpreadTerrain;

	//==Terrain Dynamic Loading Information==
		bool		bRootUpdated;
		int			iTerrainSize;

		bool		bDrawDebugBoobs;

		// Whether or not to load low-resolution terrain
		bool		bLoadLowDetail;
		// Whether or not the last mesh update failed with an std::bad_alloc
		bool		bFailedBadAlloc;

	//==Terrain Loading State==
		bool		bInitialLoad;
		bool		bInitialLoadDone;

	//==Terrain Threading==
		// Option for threads
		bool		bMinimizeThreads;
		bool		bThreadLoadlist;
		bool		bThreadSimulation;

		// Loading and Simulation Threading
		bool		bContinueThread;
		int			iStallCount;

		mutex		mtGaurd;
		thread		mtThread;

	struct sCallableThread;
			friend		sCallableThread;

	void				StartSideThread ( void );
	void				EndSideThread ( void );
	void				WorkSideThread ( void );

	// Generation Threading
		bool		bContinueGenerationThread;
		int			iGenerationStallCount;
		int			iGenerationCreateCount;
		int			iGenerationPregenSpread;

		mutex		mtGenerationListGaurd;
		thread		mtGenerationThread;

	struct sGenerationThread;
			friend		sGenerationThread;

	void				StartGenerationThread ( void );
	void				EndGenerationThread ( void );
	void				WorkGenerationThread ( void );

	void				PruneGenerationRequests ( void );

	struct sGenerationRequest;
	vector<sGenerationRequest>	vGenerationRequests;

	void				AddGenerationRequest ( const RangeVector &, const bool=false );

	//==Culling Variables==
	/*GLuint iCubeList;
	GLuint iFaceList;
	Vector3d vCameraDir;
	char renderMethod;*/
		char		stepNum;

	//==Position Update Information==
		Vector3d	vCameraPos;
		Vector3d	vCameraDir;
		bool		bUpdateCameraPos;

	//==Boob Information==
		CBoob *			root;
		RangeVector		root_position;
		static const	RangeVector boobSize;
		static const	ftype blockSize;
	
	//==Terrain List==
		vector<CBoob*>	terraList;
		mutex			mtTerraListGuard;

	//==New Boob Generation==
	// Creates a new boob object with default values
	CBoob*				GenerateBoob ( void );
	CMetaboob*			GenerateMetaboob ( void );
	// Link a cube in the direction
	//  Last argument is a direction, values defined EFaceDir
	void				LinkCube ( CBoob *, CBoob *, char const& );
	// Adds the entire terrain to the update list
	void				ForceRecursiveUpdate ( CBoob *, RangeVector const& );
	// Adds the entire terrain to the update list using terraList
	void				ForceFullNonRecursiveUpdate ( void );

	//==Boob Updating==
	// Updates the center of the terrain based on player position
	void				UpdateRoot ( void );
	// Updates the LOD when the terrain is moved
	void				UpdateLODs ( CBoob *, char, RangeVector const& );

	// Updates the center of the loaded terrain based on root position and loads/unloads terrain based on what's needed.
	void				PerformCenterMovement ( void ); // Partially part of new boob generation
	void				CenterMoveTree32 ( CBoob *,CBoob *, RangeVector const&, EFaceDir );
	void				CenterMoveFreeTree32 ( CBoob *, RangeVector const& );

	// ==Boob IO==
public:
	short iploadstate;
private:
	// Loading list. Routines for moving the loading cycle to the side instead of in the bottle neck
	void				PopLoadingList ( void );
	int					IsInLoadingList ( CBoob* );
	void				RemoveFromLoadingList ( int );
	void				PushLoadingList ( sVTQueuedLoad );
	void				PushLoadingList ( CBoob*, RangeVector const&, bool recursive=false );
	void				PushLoadingListWater ( CBoob*, RangeVector const&, bool recursive=false );
	void				ReorderLoadingList ( CBoob* );

	void				PruneLoadingList ( void );
	void				ClearLoadingList ( void );

	vector<sVTQueuedLoad> vBoobLoadList;
		mutex			mtLoadingListGaurd;
		unsigned int	iPopListCount;

	// Saving list. Routines for moving the saving cycle to the side.
	void				PopSavingList ( void );
	int					IsInSavingList ( CBoob* );
	void				RemoveFromSavingList ( int );
	void				PushSavingList ( sVTQueuedSave );
	void				PushSavingList ( CBoob*, RangeVector const& );
	void				ReorderSavingList ( CBoob* );

		mutex		mtSavingListGaurd;
	vector<sVTQueuedSave> vBoobSaveList;
	
	// Loading Checks. Check for first loading finish.
	bool				CheckLoadedAndVisible ( CBoob* );

	// Check if boob exists
		CBoob*		pCurrentTargetBoob;
	bool				BoobExists ( CBoob* );
	bool				BoobExistsRecursive ( CBoob* );

	//==LOD Generation and Handling==
		mutex		mtLevelOfDetail;
		bool		bLODWantsRegen;
		unsigned short	iCurrentLoadTarget;
		unsigned short	iCurrentSkipDirection;
		CLODBoobMesh*	pLOD_Level1Mesh;
		RangeVector	lod_root_draw_position;
		bool		bLODBeginLoad;
		bool		bLODCanDraw_L1;

	RangeVector			LODIndexToRangeVector ( unsigned short iLODIndex );
	// Initializes the LODs
	void				LODInit ( void );
	// Frees the LODs
	void				LODFree ( void );
	// Performs loading of target
	void				LOD_Level1_UpdateTarget ( void );
	// Actually opens the file and loads target
	//	returns false if couldn't load
	bool				LOD_Level1_LoadTarget ( bool& changeOccurred );
	// Update the LOD mesh
	void				LOD_Level1_UpdateMesh ( void );
	// Shift the LOD data
	void				LOD_ShiftData ( int levelIndex, RangeVector const& shiftOffset );

	//==Culling==
	// Performs frustum culling recursively. Still needs a little bit of work.
	void				CullTree32 ( CBoob *, RangeVector const& );
	void				CullTree16 ( CBoob *, subblock16 *, char const, RangeVector const& );
	void				CullTree8 ( CBoob *, subblock8 *, char const, char const, RangeVector const& );
	// Occlusion culling. Still needs to be implemented.
	void				PerformOcclusion ( void );
	void				OccludeTree ( CBoob *, RangeVector const& );

	// Frees the terrain objects recursively
	void				FreeTerrain ( CBoob* );
	void				FreeBoob ( CBoob* );
	//void FreeMetaboob ( CMetaboob* );

	//==Saving and Loading==
			friend		CTerrainIO;
		CTerrainIO*	io;
	//void WriteBoobToFile ( CBoob *, CBinaryFile * );
	//void WriteMetaboobToFile ( CMetaboob *, CBinaryFile * );
	//void ReadBoobFromFile ( CBoob *, CBinaryFile * );

	void				GetBoobData ( CBoob *, RangeVector const&, bool generateTerrain );
	void				SaveBoobData ( CBoob *, RangeVector const& );
	void				SaveMetaboobData ( CMetaboob *, RangeVector const& );

	void				LoadTerrain ( CBoob *, RangeVector const& );
	void				SaveActiveTerrain ( CBoob *, RangeVector const& );

	bool				TerraFileExists ( void );

		bool		bQueueLoadingListPop;

	//==Foliage Management==
	unsigned short		GetFoliageType ( CFoliage* pFoliage );
	unsigned short		GetFoliageType ( string const& stName );
	CFoliage*			CreateFoliage ( unsigned short iType, Vector3d const& vInPos, const char * iInData );
	void				CreateQueuedFoliage ( void );
	//==Component Management==
	unsigned short		GetComponentType ( CTerrainProp* pComponent );
	CTerrainProp*	CreateComponent ( unsigned short iType, Vector3d const& vInPos, Vector3d const& vInRot, Vector3d const& vInScale, uint64_t const iInData, CBoob*, char const, char const, short const );
	void				CreateQueuedComponents ( void );

	//==User Block Reset==
	void				ResetBlock7Flag ( CBoob * );

public:
	//==Renderable Object Interface==
	bool				PreRender ( const char pass );
	bool				Render ( const char pass );
	unsigned char		GetPassNumber ( void );
	glMaterial*			GetPassMaterial ( const char pass );

	glMaterial*			GetBlockMaterial ( void );
private:
	//==Drawing/Rendering==
	// Sets up the needed materials for rendering
	void				InitializeRenderProperties ( void );
	// Resets the boob's drawing state. Needs to be done before drawing
	void				ResetBlockDrawn ( CBoob * );
	// Render the sub-terrain meshes
	void				RenderOcttree ( CBoob *, RangeVector const& );
	// Render the terrain's water meshes
	void				RenderWaterOcttree ( CBoob *, RangeVector const& );
	// Render the lod's meshes
	void				RenderLODL1 ( void );


	class BackgroundRenderer : public CRenderableObject
	{
	public:
		BackgroundRenderer ( CVoxelTerrain* target );
		~BackgroundRenderer ( void );

		bool			Render ( const char pass );
		unsigned char	GetPassNumber ( void );
		glMaterial*		GetPassMaterial ( const char pass );
	private:
		CVoxelTerrain*	m_terra;
	}*	bgRenderer;

	//==VBO Regeneration==
	// Generate the VBO. Called before render to check if need to update
	//void GenerateVBO ( void );
	void				UpdateTerrainMesh ( void );
	// Regenerates the boob VBO's. Also regenerates the boob collision meshes.
	void				GenerateTree32 ( CBoob *, RangeVector const& );
	void				GenerateTree16 ( CBoob *, subblock16 *, char const, RangeVector const& );
	void				GenerateTree8 ( CBoob *, subblock8 *, char const, char const, RangeVector const& );
	// Regenerates the boob Water VBO's.
	void				GenerateWater16 ( CBoob *, subblock16 *, char const, RangeVector const& );
	void				GenerateWater8 ( CBoob *, subblock8 *, char const, char const, RangeVector const& );
	// Regenerates the boob lighting. Should be done before VBO Generation
//	void LightTree32 ( CBoob *, RangeVector const& );
//	void LightTree16 ( CBoob *, subblock16 *, char const, RangeVector const& );
	void				LightTree8 ( CBoob *, subblock8 *, char const, char const );
	inline bool			LightTrace ( CBoob *, subblock8 *, char const, char const, short const, Vector3d );
	inline bool			LightTraceUpLong ( CBoob *, subblock8 *, char const, char const, short const );
	inline bool			BlockHasNeighbors ( CBoob* , char const index16, char const index8, char * data, int const i, int const width );
	bool				TraverseTree ( CBoob ** pCurrentBoob, char& index16, char& index8, short& i, EFaceDir direction );

	//==Collision Regeneration==
	// Generate the collision. Called before physics update to check if need to update
	void				UpdateTerrainCollision ( void );
	// Regenerates the boob collision
	void				UpdateCollision32 ( CBoob * );

	//==System Simulation==
		bool		bQueueSimulation;
		unsigned char	iWaterSimulation;
		bool		bWaterUpdated;
		float		fWaterSimulation;
		float		fWireSimulation;
		bool		bWireQueueUpdate;
	void				UpdateSimulation ( CBoob *, RangeVector const& );

	void				UpdateWater_Low32 ( CBoob *, RangeVector const& );
	void				UpdateWater_Low16 ( CBoob *, subblock16 *, char const, RangeVector const& );
	void				UpdateWater_Low8 ( CBoob *, subblock8 *, char const, char const, RangeVector const& );
	void				UpdateWater_Low1 ( CBoob *, subblock8 *, char const, char const, short const, RangeVector const& );

	void				UpdateWire_Reset32 ( CBoob *, RangeVector const& );

	void				UpdateWire_Default32 ( CBoob *, RangeVector const& );
	void				UpdateWire_Default16 ( CBoob *, subblock16 *, char const, RangeVector const& );
	void				UpdateWire_Default8 ( CBoob *, subblock8 *, char const, char const, RangeVector const& );
	void				UpdateWire_Default1 ( CBoob *, subblock8 *, char const, char const, short const, RangeVector const& );

	void				UpdateOre_Default32 ( CBoob * );

	//==NEED MORE COMMENTS AND CLEANUP BELOW THIS POINT==

	// Yeah rendering. I don't even know. These aren't used.
	/*void RenderBoob ( CBoob *, RangeVector const& );
	void RenderBlock ( Vector3d const& );
	void RenderBlock ( Vector3d const&, char const& );
	void RenderBlockExp ( Vector3d const&, CBoob *, unsigned int const& );
	void GenerateCube ( void );*/

	// I doubt these are used either.
	/*bool BlockVisible ( char *, unsigned int const& );
	bool BlockVisibleEx ( char *, unsigned int const&, int const& );*/

	// I don't think this one is used either.
	//void GetBlockXYZ( unsigned int &,unsigned int &,unsigned int &,unsigned int const&,unsigned int const&,unsigned int const& );

	// And I don't think these ones are used.
	/*void RenderTree32 ( CBoob *, RangeVector const& );
	void RenderTree16 ( CBoob *, subblock16 *, char const&, RangeVector const& );
	void RenderTree8 ( CBoob *, subblock8 *, char const&, char const&, RangeVector const& );*/

	//void GenerateCube ( CBoob *, char const, char const, int const, Vector3d const&, char const );

	bool				BlockVisibleEx	( CBoob *, subblock8 *, char const&, char const&, char *, unsigned int const&, int const& );
	// Checks if a side is visible.
	//  SideVisible ( boob, index16, index8, index8's data, final index, width=1, EFaceDir );
	// This one seems broken though...remove later?
	bool				SideVisible ( CBoob * pBoob, char const index16, char const index8, char * data, unsigned int const i, int const width, EFaceDir const dir ); //unused

	char				GetCube ( CBoob*, RangeVector const& );
};

#include "CVoxelTerrain.hpp"

#endif