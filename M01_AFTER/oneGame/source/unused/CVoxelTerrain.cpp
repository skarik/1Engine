
// Includes
#include "CVoxelTerrain.h"
#include "CGameSettings.h"
#include "CTime.h"

#include "CTerrainIO.h"
#include "CTerraGenDefault.h"
#include "CTerraGenDebug.h"

using namespace std;

// Static variable declares
const RangeVector	CVoxelTerrain::boobSize		= RangeVector( 32,32,32 ); //b-cup
const ftype				CVoxelTerrain::blockSize	= 2.0f;
vector<CVoxelTerrain*>	CVoxelTerrain::terrainList;

// Class definitions
CVoxelTerrain::CVoxelTerrain ( void )
	: CGameObject() , CRenderableObject()
{
	// Debug message
	cout << "Initalizing TerraSys MARK " << TERRA_SYSTEM_VERSION << endl;

	// Terrain Properties
	bSpreadTerrain = true;
	iTerrainSeed = CGameSettings::Active()->i_cl_DefaultSeed;

	// Terrain Loader Settings
	iTerrainSize = CGameSettings::Active()->i_cl_ter_Range;
	bLoadLowDetail = false;
	// Loader Flags and Properties
	bRootUpdated = false;
	// Create the loader object
	io = new CTerrainIO( this );

	// Terrain Memory Management	
	cout << " Allocating memory...";
	if ( !InitTerraMemory() )
	{
		MessageBox( NULL, "Could not allocate enough memory for terrain. Reduce settings.", "Fatal Error.", 0 );
		exit( 0 );
	}
	// Reserve space for load list
	vBoobLoadList.reserve( 1500 );
	vBoobSaveList.reserve( 500 );
	terraList.reserve( cub(iTerrainSize) );
	cout << "done" << endl;

	// Debug Info
	bDrawDebugBoobs = false;

	// Error flags
	bFailedBadAlloc = false;
	
	// Add this sucker to list
	terrainList.push_back( this );

	// Create the terrain
	bool bTerrainExists = TerraFileExists();
	int size = iTerrainSize;
	// Terrain Creation
	{
		CBoob* temp_x;
		CBoob* temp_y;
		CBoob* temp_z;


		temp_x = NULL;
		temp_y = NULL;
		temp_z = GenerateBoob();
		root = temp_z;
		for ( int k = 0; k < size; k += 1 )
		{
			if ( k != 0 )
			{
				LinkCube( temp_z, GenerateBoob(), BOTTOM );
				temp_z = temp_z->bottom;
			}
			//else
			//	temp_z = root = GenerateBoob();
			for ( int j = 0; j < size; j += 1 )
			{
				if ( j != 0 )
				{
					LinkCube( temp_y, GenerateBoob(), BACK );
					temp_y = temp_y->back;
				}
				else 
					temp_y = temp_z;
				for ( int i = 0; i < size; i += 1 )
				{
					if ( i != 0 )
					{
						LinkCube( temp_x, GenerateBoob(), RIGHT );
						temp_x = temp_x->right;
					}
					else
						temp_x = temp_y;
				}
			}	
		}

		for ( int k = 1; k <= size/2; k += 1 )
		{
			root = root->bottom->right->back;
		}
	}
	ForceRecursiveUpdate( root, root_position );
	// End Terrain Creation

	// Terrain Generation
	if ( CGameSettings::Active()->GetWorldSaveFile() == "debug" ) {
		generator = new CTerraGenDebug( this, iTerrainSeed );
	}
	else {
		generator = new CTerraGenDefault( this, iTerrainSeed );
	}
	{
		//iGenerationMode	= GENERATE_DUAL_SAVE;
		//iGenerationMode = GENERATE_TO_MEMORY;
		//iGenerationMode = GENERATE_TO_DISK;
		generator->SetGenerationMode( CTerrainGenerator::GENERATE_TO_MEMORY );
		
		if (( !bTerrainExists )&&( false ))
		{
			Debug::Console->PrintMessage( "Generating terrain.\n" );
			//GenerateTerrain( root, root_position );
			Debug::Console->PrintMessage( " Done.\n" );
		}
		else
		{
			Debug::Console->PrintMessage( "Loading terrain.\n" );
			LoadTerrain( root, root_position );
			ResetBlock7Flag( root );
			Debug::Console->PrintMessage( " Done.\n" );
		}
	}
	// End Terrain Generation

	// Add all the terrain to the loading list
	ResetBlock7Flag( root );
	ForceRecursiveUpdate( root, root_position );
	ResetBlock7Flag( root );

	// Initialize stepping variables
	stepNum	= 0;
	iWaterSimulation	= 0;
	fWaterSimulation	= 0;
	fWireSimulation		= 0;
	bWireQueueUpdate	= false;

	bQueueSimulation	= false;

	// Initialize area step variables
	vCameraPos			= Vector3d( 0,0,0 );
	bUpdateCameraPos	= false;

	// Initialize load variables
	bInitialLoad		= true;
	bInitialLoadDone	= false;

	// Initialize render settings
	InitializeRenderProperties();

	// Init the LOD
	cout << " Initializing LOD system...";
	LODInit();
	cout << "like a pirate" << endl;

	// Grab threading info
	bMinimizeThreads	= CGameSettings::Active()->b_cl_MinimizeTerrainThreads;
	bThreadLoadlist		= true;
	bThreadSimulation	= !bMinimizeThreads;
	iStallCount			= 0;
	// And now start the update thread
	cout << " Starting threads...";
	StartSideThread();
	// And now start the generation thread
	StartGenerationThread();
	cout << "freaking legit" << endl;
}

CVoxelTerrain::~CVoxelTerrain ( void )
{
	// Stop the side thread
	EndSideThread();
	// Stop the generation thread
	EndGenerationThread();

	// Free the LODs
	LODFree();

	// Remove terrain from list
	vector<CVoxelTerrain*>::iterator index;
	index = find( terrainList.begin(), terrainList.end(), this );
	if ( index != terrainList.end() )
	{
		terrainList.erase( index );
	}

	// Save the Terrain data to file
	SaveActiveTerrain( root, root_position );
	ResetBlock7Flag( root );

	FreeTerrain( root );
	root = NULL;

	// Free renderer
	delete bgRenderer;
	bgRenderer = NULL;

	myMat->removeReference();
	//delete myMat; TODO

	delete generator;
	delete io;

	// Free terrain memory LAST, since not sure what objects are still usin' it.
	FreeTerraMemory();
}
// Recursive Destructor Assistant
void CVoxelTerrain::FreeTerrain ( CBoob* pBoob )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			pBoob->bitinfo[7] = true;

			if ( pBoob->left )
				FreeTerrain( pBoob->left );
			if ( pBoob->right )
				FreeTerrain( pBoob->right );
			if ( pBoob->top )
				FreeTerrain( pBoob->top );
			if ( pBoob->bottom )
				FreeTerrain( pBoob->bottom );
			if ( pBoob->front )
				FreeTerrain( pBoob->front );
			if ( pBoob->back )
				FreeTerrain( pBoob->back );

			if ( pBoob->left )
				pBoob->left->right = NULL;
			if ( pBoob->right )
				pBoob->right->left = NULL;
			if ( pBoob->top )
				pBoob->top->bottom = NULL;
			if ( pBoob->bottom )
				pBoob->bottom->top = NULL;
			if ( pBoob->front )
				pBoob->front->back = NULL;
			if ( pBoob->back )
				pBoob->back->front = NULL;

			// Free the boob data
			FreeBoob( pBoob );
		}
	}
}

// ====================================== GETTERS AND SETTERS ======================================
// Toggle root update
void CVoxelTerrain::SetRootUpdate ( bool newMode )
{
	bUpdateCameraPos = newMode;
}
// Set root target
void CVoxelTerrain::SetRootTarget ( const Vector3d & newPos )
{
	vCameraPos = newPos;
}

// Get primary loading
bool CVoxelTerrain::GetDoneLoading ( void )
{
	return bInitialLoadDone;
}


// ====================================== UPDATING ======================================
// Update Terrain
void CVoxelTerrain::Update ( void )
{
	// Lock the terrain while updating
	mutex::scoped_lock local_lock( mtGaurd );

	// In minimal thread mode, check if any of the threads needs to be restarted
	if ( bMinimizeThreads || (true) ) { // Set to true because there seems to be a bug in the terrain generator that keeps crashing
		// Check if loader/simulation thread not running
		if ( mtThread.timed_join(boost::posix_time::millisec(0)) )
		{
			// Check if needs to run
			if ( !vBoobLoadList.empty() ) {
				StartSideThread();
			}
		}
		// Check if generation thread not running
		if ( mtGenerationThread.timed_join(boost::posix_time::millisec(0)) )
		{
			// Check if needs to run
			if ( !vGenerationRequests.empty() ) {
				StartGenerationThread();
			}
		}
	}

	// Update camera position
	if ( bUpdateCameraPos ) {
		vCameraPos = CCamera::activeCamera->transform.position;
		vCameraDir = CCamera::activeCamera->transform.Forward();
	}

	// Check initial load
	if ( bInitialLoad || !bInitialLoadDone )
	{
		ResetBlock7Flag( root );
		if ( CheckLoadedAndVisible( root ) )
		{
			bInitialLoad = false;
			bInitialLoadDone = true;
		}
		ResetBlock7Flag( root );
		if ( bInitialLoadDone )
		{
			ForceFullUpdate();
			ResetBlock7Flag( root );
		}
	}

	//cout << "lls: " << GetLoadingListSize() << " " << CheckLoadedAndVisible(root) << endl;

	// Update the root node based on camera position
	UpdateRoot();
	// Do the chunk loading/unloading
	if ( bRootUpdated )
	{
		PerformCenterMovement(); // Adds to the loading list and removes things from the loading list
		// Update the LOD listing
		//if ( bLoadLowDetail )
		//	UpdateLODs ( root, 1, root_position );
		if ( !bInitialLoadDone ) {
			ClearLoadingList();
			ForceFullUpdate();
		}
	}
	CenterMoveFreeTree32( root, root_position ); // Unload that shit

	if ( bLoadLowDetail ) {
		// Single-world LOD's (Sea and Storm)
		UpdateLODs ( root, 1, root_position );
	}
	else {
		// MCC Style LOD's
		if ( CGameSettings::Active()->b_cl_ter_ShadowOfTheCollussusRenderStyle ) {
			bLODBeginLoad = true;
		}
		else {
			bLODBeginLoad = false;
			bLODCanDraw_L1 = false;
		}
	}

	// == LOADING ==
	// If there was a loading error, reset the entire loading list.
	if ( bFailedBadAlloc )
	{
		mutex::scoped_lock( mtLoadingListGaurd );
		vBoobLoadList.clear();
		ResetBlock7Flag( root );
		ForceRecursiveUpdate( root, root_position );
		ResetBlock7Flag( root );
		stepNum = -8;

		// TODO: reset all mesh states
		bFailedBadAlloc = false;
		//if ( stepNum > 0 )
		//	stepNum = -8;
	}
	/*if ( stepNum >= 0 )
	{
		bFailedBadAlloc = false;
	}*/
	// Load the trees
	CreateQueuedFoliage();
	// Load the props
	CreateQueuedComponents();

	// Pop the loading list every so many steps
	const int stepMod = 3;
	stepNum++;
	if (( stepNum > 0 )&&( stepNum % stepMod == 0 ))
		bQueueLoadingListPop = true;
		//PopLoadingList();
	if ( stepNum > stepMod*3 )
		stepNum -= stepMod*2;

	// == SIMULATION ==
	// Work on water simulation counters
	fWaterSimulation += Time::deltaTime;
	if ( fWaterSimulation > 0.1f )
	{
		iWaterSimulation++;
		while ( fWaterSimulation > 0.1f )
			fWaterSimulation -= 0.1f;
	}
	if ( iWaterSimulation > 100 )
		iWaterSimulation -= 100;
	/*if ( iWaterSimulation % 50 == 0 )
		bWaterQueueUpdate = true;*/
	// Work on circuit simulation counters
	if ( fWireSimulation > 0.06f )
	{
		bWireQueueUpdate = true;
		fWireSimulation = 0;
	}
	else
	{
		bWireQueueUpdate = false;
		fWireSimulation += Time::deltaTime;
	}
	// Perform recursive simulation
	/*ResetBlock7Flag( root );
	UpdateSimulation( root, root_position );
	ResetBlock7Flag( root );*/
	bQueueSimulation = true;

	// == VISUALS ==
	// Do culling
	//CullTree32( root, root_position );
	//PerformOcclusion();

	if ( Input::Keydown( Keys.F7 ) ) 	///if ( CInput::keydown[VK_F7] )
	{
		bDrawDebugBoobs = !bDrawDebugBoobs;
	}
	
	// == DEBUG DRAWING ==
	static bool bDrawDebugCaves = false;
	if ( Input::Keydown( Keys.F12 ) )  //if ( CInput::keydown[VK_F12] )
	{
		//bDrawDebugCaves = !bDrawDebugCaves;
	}
	// Draw some debug shit
	if ( bDrawDebugCaves )
	{
		/*for ( vector<TerraCave*>::iterator it = vCaveSystems.begin(); it != vCaveSystems.end(); it++ )
		{
			for ( vector<TerraTunnel>::iterator itt = (*it)->partTree.begin(); itt != (*it)->partTree.end(); itt++ )
			{
				DebugD::DrawLine( itt->ray.pos * 64.0f, (itt->ray.pos+itt->ray.dir) * 64.0f );
			}
		}*/
		/*for ( vector<TerraCave*>::iterator it = vBridgeSystems.begin(); it != vBridgeSystems.end(); it++ )
		{
			for ( vector<TerraTunnel>::iterator itt = (*it)->partTree.begin(); itt != (*it)->partTree.end(); itt++ )
			{
				DebugD::DrawLine( itt->ray.pos * 64.0f, (itt->ray.pos+itt->ray.dir) * 64.0f );
			}
		}*/
	}
}

// Updates VBO if needed
#include "CTimeProfiler.h"
void CVoxelTerrain::PostUpdate ( void )
{
	TimeProfiler.BeginTimeProfile( "ter_postupdate" );
	if ( mtGaurd.try_lock() )
	{
	TimeProfiler.BeginTimeProfile( "ter_mesh_update" );
		UpdateTerrainMesh(); // Todo: move this
	TimeProfiler.EndTimeProfile( "ter_mesh_update" );
	TimeProfiler.BeginTimeProfile( "ter_collision_update" );
		UpdateTerrainCollision();
	TimeProfiler.EndTimeProfile( "ter_collision_update" );
		mtGaurd.unlock();
	}
	if ( mtLevelOfDetail.try_lock() )
	{
	TimeProfiler.BeginTimeProfile( "ter_lod_update" );
		LOD_Level1_UpdateMesh();
	TimeProfiler.EndTimeProfile( "ter_lod_update" );
		mtLevelOfDetail.unlock();
	}
	ResetBlock7Flag( root );
	TimeProfiler.EndTimeProfile( "ter_postupdate" );
}

// Updates the root node based on camera position and current root position
void CVoxelTerrain::UpdateRoot ( void )
{
	// Boolean to update LOD's if root has been updated
	bRootUpdated = false;

	// Check if the current camera position is in bounds
	Vector3d pos = vCameraPos * ( 1.0f/64.0f );

	// Stuff!
	RangeVector oldPos = root_position;

	// Check for forward movement
	if ( pos.x > (root_position.x+1)+0.1f )
	{
		if ( root->front != NULL )
		{
			root = root->front;
			root_position.x += 1;
			bRootUpdated = true;
		}
		//else
		//	break;
	}
	// Check for back movement
	if ( pos.x < root_position.x-0.1f )
	{
		if ( root->back != NULL )
		{
			root = root->back;
			root_position.x -= 1;
			bRootUpdated = true;
		}
		//else
		//	break;
	}
	// Check for left movement
	if ( pos.y > (root_position.y+1)+0.1f )
	{
		if ( root->left != NULL )
		{
			root = root->left;
			root_position.y += 1;
			bRootUpdated = true;
		}
		//else
		//	break;
	}
	// Check for right movement
	if ( pos.y < root_position.y-0.1f )
	{
		if ( root->right != NULL )
		{
			root = root->right;
			root_position.y -= 1;
			bRootUpdated = true;
		}
		//else
		//	break;
	}
	// Check for top movement
	if ( pos.z > (root_position.z+1)+0.1f )
	{
		if ( root->top != NULL )
		{
			root = root->top;
			root_position.z += 1;
			bRootUpdated = true;
		}
		//else
		//	break;
	}
	// Check for down movement
	if ( pos.z < root_position.z-0.1f )
	{
		if ( root->bottom != NULL )
		{
			root = root->bottom;
			root_position.z -= 1;
			bRootUpdated = true;
		}
		//else
		//	break;
	}

	// Update the LOD's if the root has changed
	if ( bRootUpdated )
	{
		// SHIFT BROADPHASEEE
		Physics::ShiftWorld( Vector3d(
			(ftype)(root_position.x-oldPos.x)*64,
			(ftype)(root_position.y-oldPos.y)*64,
			(ftype)(root_position.z-oldPos.z)*64) );
		// Update the LOD listing
		/*if ( bLoadLowDetail )
			UpdateLODs ( root, 1, root_position );*/
		LOD_ShiftData( 1, RangeVector(
			oldPos.x-root_position.x,
			oldPos.y-root_position.y,
			oldPos.z-root_position.z) );
	}
}

void CVoxelTerrain::ResetBlockDrawn ( CBoob * pBoob )
{
	/*if ( pBoob != NULL )
	{
		if (( pBoob->bitinfo[1] == true )||( pBoob->bitinfo[2] == true ))
		{
			pBoob->visible = false;
			pBoob->bitinfo[1] = false; // AlreadyDrawn flag
			pBoob->bitinfo[2] = false; // AlreadyChecked flag
			pBoob->bitinfo[4] = false; // CheckedRegen flag
			pBoob->bitinfo[5] = false; // VBO Generation flag
			pBoob->bitinfo[6] = false; // Movement check flag
			ResetBlockDrawn( pBoob->front );
			ResetBlockDrawn( pBoob->back );
			ResetBlockDrawn( pBoob->left );
			ResetBlockDrawn( pBoob->right );
			ResetBlockDrawn( pBoob->bottom );
			ResetBlockDrawn( pBoob->top );
		}
	}*/
	mtTerraListGuard.lock();
	for ( vector<CBoob*>::iterator it = terraList.begin(); it != terraList.end(); ++it )
	{
		pBoob = *it;
		//pBoob->visible = false;
		pBoob->bitinfo[1] = false; // AlreadyDrawn flag
		pBoob->bitinfo[2] = false; // AlreadyChecked flag
		pBoob->bitinfo[4] = false; // CheckedRegen flag
		pBoob->bitinfo[5] = false; // VBO Generation flag
		pBoob->bitinfo[6] = false; // Movement check flag
	}
	mtTerraListGuard.unlock();
}
void CVoxelTerrain::ResetBlock7Flag ( CBoob * pBoob )
{
	/*if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] )
		{
			pBoob->bitinfo[7] = false; // Movement check flag
			ResetBlock7Flag( pBoob->front );
			ResetBlock7Flag( pBoob->back );
			ResetBlock7Flag( pBoob->left );
			ResetBlock7Flag( pBoob->right );
			ResetBlock7Flag( pBoob->bottom );
			ResetBlock7Flag( pBoob->top );
		}
	}*/
	mtTerraListGuard.lock();
	for ( vector<CBoob*>::iterator it = terraList.begin(); it != terraList.end(); ++it )
	{
		(*it)->bitinfo[7] = false;
	}
	mtTerraListGuard.unlock();
}

// PRIVATE::

void CVoxelTerrain::UpdateLODs ( CBoob* pBoob, char currentResolution, RangeVector const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[4] == false )
		{
			pBoob->bitinfo[4] = true;


			const static unsigned char maxRes = 8;
			unsigned char newResolution = 1;

			//==Calculate the new LOD==
			// Base it on max boob distance on a single axis
			RangeVector vPosDiference( abs(position.x-root_position.x), abs(position.y-root_position.y), abs(position.z-root_position.z) );
			rangeint iBoobDistance = max( max( vPosDiference.x, vPosDiference.y ), vPosDiference.z );

			// Only set low LOD if the boob is beyond normal loading range
			if ( iBoobDistance > iTerrainSize/2 )
			{
				for ( char i = 0; i < ((char)(iBoobDistance-iTerrainSize/2)); ++i )
				{
					newResolution *= 2;
					if ( newResolution > maxRes )
					{
						newResolution = maxRes;
						break;
					}
				}
			}
			
			//==Set the resolution==
			if ( pBoob->current_resolution != newResolution )
			{
				// If the current resolution is 1
				if ( pBoob->current_resolution == 1 )
				{
					// We're basically going to 'unload' it. Meaning, we need to save it.

					// So, we need to save the data if it's valid
					if ( pBoob->hasPropData && pBoob->hasBlockData )
					{
						SaveBoobData( pBoob, position ); // Save it as it's being unloaded.
						//PushSavingList( pBoob, position );
					}

					// And then we have to delete the boob values
					{
						// Delete the foliage in the list
						for ( CBoob::FoliageIterator it = pBoob->v_foliage.begin(); it != pBoob->v_foliage.end(); it++ )
						{
							delete (*it);
						}
						pBoob->v_foliage.clear();
						// Delete the components in the list
						for ( CBoob::ComponentIterator it = pBoob->v_component.begin(); it != pBoob->v_component.end(); it++ )
						{
							delete (*it);
						}
						pBoob->v_component.clear();
						// Delete the grass in the list
						pBoob->v_grass.clear();

						// Since we've deleted the prop data, set corresponding flag
						pBoob->hasPropData = false;
						// Set to not needing prop data
						pBoob->loadPropData = false;
					}

					// Push to update since it needs to be regenerated
					PushLoadingList( pBoob, position, false );
					pBoob->current_resolution = newResolution;
				}
				else if ( newResolution == 1 ) // If the new resolution is 1, it needs to be loaded.
				{
					pBoob->loadPropData = true;
					if ( !pBoob->hasPropData )
					{
						// Push to update if needs to be regenerated
						PushLoadingList( pBoob, position, false );
						pBoob->current_resolution = 1;
					}
				}
				
			}

			// Call resolution set for other guys
			if ( position.x >= root_position.x )
				UpdateLODs( pBoob->front, newResolution, RangeVector( position.x+1, position.y, position.z ) );
			if ( position.y >= root_position.y )
				UpdateLODs( pBoob->left, newResolution, RangeVector( position.x, position.y+1, position.z ) );
			if ( position.z >= root_position.z )
				UpdateLODs( pBoob->top, newResolution, RangeVector( position.x, position.y, position.z+1 ) );
			if ( position.x <= root_position.x )
				UpdateLODs( pBoob->back, newResolution, RangeVector( position.x-1, position.y, position.z ) );
			if ( position.y <= root_position.y )
				UpdateLODs( pBoob->right, newResolution, RangeVector( position.x, position.y-1, position.z ) );
			if ( position.z <= root_position.z )
				UpdateLODs( pBoob->bottom, newResolution, RangeVector( position.x, position.y, position.z-1 ) );
		}
	}
}

// Updates the center of the loaded terrain based on root position and loads/unloads terrain based on what's needed.
void CVoxelTerrain::PerformCenterMovement ( void )
{
	CenterMoveTree32 ( root, root, root_position, FRONT );
	//CenterMoveFreeTree32( root, root_position );

}
static vector<CBoob*> deleteList;

// 
//  void CVoxelTerrain::CenterMoveTree32 ( CBoob * pBoob, CBoob * pBoobPrev, RangeVector const& position, EFaceDir goDir )
//
//	Description:
//		This function is the main client for the main loader + unloader queue.
//		At the edge of renderable terrain, it checks if boob creation or deletion is necessary, and
//		sends a request to the proper queue.
//
//	Notes:
//		This function has a memory leak. It is a LARGE memory leak.
//		(3/24/2012): DOES IT STILL HAVE THE LEAK? PLEASE CHECK.
//		(6/03/2012): I don't think this function has a leak.
//
void CVoxelTerrain::CenterMoveTree32 ( CBoob * pBoob, CBoob * pBoobPrev, RangeVector const& position, EFaceDir goDir )
{
	// Recursively check the outliers first, then work way in, because it likely will be unneeded on the inlyers.
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[6] == false )
		{
			pBoob->bitinfo[6] = true;

			// Move to the edges FIRST.
			if ( position.x >= root_position.x )
				CenterMoveTree32( pBoob->front,pBoob, RangeVector( position.x+1, position.y, position.z ), FRONT );
			if ( position.y >= root_position.y )
				CenterMoveTree32( pBoob->left,pBoob, RangeVector( position.x, position.y+1, position.z ), LEFT );
			if ( position.z >= root_position.z )
				CenterMoveTree32( pBoob->top,pBoob, RangeVector( position.x, position.y, position.z+1 ), TOP );
			if ( position.x <= root_position.x )
				CenterMoveTree32( pBoob->back,pBoob, RangeVector( position.x-1, position.y, position.z ), BACK );
			if ( position.y <= root_position.y )
				CenterMoveTree32( pBoob->right,pBoob, RangeVector( position.x, position.y-1, position.z ), RIGHT );
			if ( position.z <= root_position.z )
				CenterMoveTree32( pBoob->bottom,pBoob, RangeVector( position.x, position.y, position.z-1 ), BOTTOM );

			//pBoob->bitinfo[6] = false;

			// No LOD on far boobs
			if ( !bLoadLowDetail )
			{
				// Now, as we go back to the center of the calling, check if the boob is out of drawing bounds
				if ( ( abs(position.x - root_position.x) > iTerrainSize/2 )||( abs(position.y - root_position.y) > iTerrainSize/2 )||( abs(position.z - root_position.z) > iTerrainSize/2 ) )
				{
					// Save the boob if it has both block and prop data
					if ( pBoob->hasPropData && pBoob->hasBlockData )
					{
						SaveBoobData( pBoob, position ); // Save it as it's being unloaded.
					}
					// Check the boob to unload in the loading list
					int index = IsInLoadingList ( pBoob );
					if ( index >= 0 )
					{
						// If it's in the list, remove it.
						RemoveFromLoadingList( index );
					}

					// Check if the current boob is already in the delete list
					bool exists = false;
					for ( std::vector<CBoob*>::iterator it = deleteList.begin(); it != deleteList.end(); it++ )
					{
						if ( (*it) == pBoob )
						{
							exists = true;
						}
					}
					// Add it to the delete list if it's not in it
					if ( !exists )
					{
						deleteList.push_back( pBoob );
					}
					else
					{
						cout << "Warning: double pushback on address " << pBoob << endl;
					}
				}
			}
			else // LOD on far boobs
			{
				// Now, as we go back to the center of the calling, check if the boob is out of drawing bounds
				if ( ( abs(position.x - root_position.x) > iTerrainSize )||( abs(position.y - root_position.y) > iTerrainSize )||( abs(position.z - root_position.z) > iTerrainSize ) )
				{
					int index = IsInLoadingList ( pBoob );
					// Check if it's in the loading list.
					if ( index < 0 )
					{
						
					}
					else
					{
						// If it's in the list, remove it.
						RemoveFromLoadingList( index );
					}
					// Check if the current boob is already in the delete list
					bool exists = false;
					for ( std::vector<CBoob*>::iterator it = deleteList.begin(); it != deleteList.end(); it++ )
					{
						if ( (*it) == pBoob )
						{
							exists = true;
						}
					}
					// Add it to the delete list if it's not in it
					if ( !exists )
					{
						deleteList.push_back( pBoob );
					}
					else
					{
						cout << "Warning: double pushback on address " << pBoob << endl;
					}
				}
			}
			// End Edge Removal Check
		}
	}
	else // Once we're at the edges, check if we need to make new objects.
	{
		// Change the limit based on the LOD stuff
		int iTerrainLimit = iTerrainSize/2;
		if ( bLoadLowDetail )
			iTerrainLimit = iTerrainSize;
		// Begin checks
		if (( goDir == FRONT )||( goDir == BACK ))
		{
			if ( abs(position.x - root_position.x) <= iTerrainLimit )
			{
				// Create a new boob.
				CBoob* pNewBoob = GenerateBoob();
				//std::cout << (void*)pNewBoob << ": ";
				if ( position.x > root_position.x )
					LinkCube( pBoobPrev, pNewBoob, FRONT );
				else
					LinkCube( pBoobPrev, pNewBoob, BACK );
				pNewBoob->bitinfo[6] = true;
				pNewBoob->bitinfo[3] = false;
				pNewBoob->li_position = position;
				//GetBoobData( pNewBoob, position, true );
				PushLoadingList( pNewBoob, position, true );
			}
		}
		else if (( goDir == LEFT )||( goDir == RIGHT ))
		{
			if ( abs(position.y - root_position.y) <= iTerrainLimit )
			{
				// Create a new boob.
				CBoob* pNewBoob = GenerateBoob();
				if ( position.y > root_position.y )
					LinkCube( pBoobPrev, pNewBoob, LEFT );
				else
					LinkCube( pBoobPrev, pNewBoob, RIGHT );
				pNewBoob->bitinfo[6] = true;
				pNewBoob->bitinfo[3] = false;
				pNewBoob->li_position = position;
				PushLoadingList( pNewBoob, position, true );
			}
		}
		else if (( goDir == TOP )||( goDir == BOTTOM ))
		{
			if ( abs(position.z - root_position.z) <= iTerrainLimit )
			{
				// Create a new boob.
				CBoob* pNewBoob = GenerateBoob();
				if ( position.z > root_position.z )
					LinkCube( pBoobPrev, pNewBoob, TOP );
				else
					LinkCube( pBoobPrev, pNewBoob, BOTTOM );
				pNewBoob->bitinfo[6] = true;
				pNewBoob->bitinfo[3] = false;
				pNewBoob->li_position = position;
				PushLoadingList( pNewBoob, position, true );
			}
		}
	}
}
// Check for memory overflow issues within Havok, causing Havok functions to return null.
void CVoxelTerrain::CenterMoveFreeTree32 ( CBoob * pBoob, RangeVector const& position )
{
	while ( !deleteList.empty() )
	{
		pBoob = deleteList.back();
		// Notify unload
		eventUnloadSector.notifyObservers( pBoob );
		// Pop from delete list
		deleteList.pop_back();

		// Remove all references to this boob
		if ( pBoob->left )
			pBoob->left->right = NULL;
		if ( pBoob->right )
			pBoob->right->left = NULL;
		if ( pBoob->top )
			pBoob->top->bottom = NULL;
		if ( pBoob->bottom )
			pBoob->bottom->top = NULL;
		if ( pBoob->front )
			pBoob->front->back = NULL;
		if ( pBoob->back )
			pBoob->back->front = NULL;

		// Free the boob data
		FreeBoob( pBoob );
	}
}


// ============= GenerateBoob ===============
// Creates a new boob and initializes data
CBoob* CVoxelTerrain::GenerateBoob ( void )
{
	bool startVisible = false;
	CBoob* pNewBoob;
	try
	{
		pNewBoob = new CBoob ();
		// and create its stuff
		pNewBoob->pMesh = new CBoobMesh;
		pNewBoob->pMesh->pOwner = pNewBoob;
		pNewBoob->pCollision = new CBoobCollision;
		pNewBoob->pCollision->pOwner = pNewBoob;
		pNewBoob->pGrass = new CTerraGrass( &pNewBoob->v_grass, pNewBoob );
		
	}
	catch ( bad_alloc& ba )
	{
		//char* bull = ba.what();
		cout << "CVoxelTerrain.cpp (" << __LINE__ << "): Caught exception: " << ba.what() << endl;
		MessageBox ( NULL,"FUCKING MEMORY COULD NOT BE FUCKING ALLOCATED.","FACK FACK FACK MEMORY FACK",MB_OK | MB_ICONINFORMATION );
	}
	//CBoob* pNewBoob = new CBoob;
	pNewBoob->front = NULL;
	pNewBoob->back = NULL;
	pNewBoob->top = NULL;
	pNewBoob->bottom = NULL;
	pNewBoob->left = NULL;
	pNewBoob->right = NULL;

	pNewBoob->current_resolution = 1;

	/*for ( unsigned int i = 0; i < 32768; i += 1 )
	{
		pNewBoob->data[i] = 0;
	}*/
	pNewBoob->bitinfo[0] = startVisible; // Visible flag
	pNewBoob->bitinfo[1] = false; // AlreadyDrawn flag
	pNewBoob->bitinfo[2] = false; // AlreadyChecked flag
	pNewBoob->bitinfo[3] = false; // Generation flag
	pNewBoob->bitinfo[5] = false; // VBO Generation flag
	pNewBoob->bitinfo[6] = false; // Movement check flag
	pNewBoob->bitinfo[7] = false; // Deletion flag
	// Set the block data to all non-solid air
	pNewBoob->hasPropData = false;
	pNewBoob->hasBlockData = false;
	pNewBoob->loadPropData = true;
	for ( char i = 0; i < 8; i += 1 )
	{
		pNewBoob->solid[i] = false;
	}
	// Get a data block
	NewDataBlock( &(pNewBoob->data) );
	// Zero the memory
	memset( pNewBoob->data, 0, 32*32*32*4 );

	pNewBoob->version = TERRA_SYSTEM_VERSION;
	pNewBoob->biome = 0;
	pNewBoob->terrain = 0;
	pNewBoob->li_position = RangeVector( 0,0,0 );

	// Er, set lighting to full
	for ( uint32_t i = 0; i < 32768; ++i )
		((terra_t*)(pNewBoob->data))[i].light = 7;

	// Push it to the terrain array
	mtTerraListGuard.lock();
	terraList.push_back( pNewBoob );
	mtTerraListGuard.unlock();

	return pNewBoob;
}
// Creates a new metaboob and initializes data
CMetaboob* CVoxelTerrain::GenerateMetaboob ( void )
{
	bool startVisible = false;
	CMetaboob* pNewBoob;
	try
	{
		pNewBoob = new CMetaboob;
	}
	catch ( bad_alloc& ba )
	{
		cout << "CVoxelTerrain.cpp (" << __LINE__ << "): Caught exception: " << ba.what() << endl;
		MessageBox ( NULL,"The gosh darn fucking memory.","FACK FACK FACK.",MB_OK | MB_ICONINFORMATION );
	}
	pNewBoob->pMesh			= NULL;
	pNewBoob->pCollision	= NULL;
	pNewBoob->pGrass		= NULL;

	//CBoob* pNewBoob = new CBoob;
	pNewBoob->front = NULL;
	pNewBoob->back = NULL;
	pNewBoob->top = NULL;
	pNewBoob->bottom = NULL;
	pNewBoob->left = NULL;
	pNewBoob->right = NULL;

	pNewBoob->current_resolution = 1;

	pNewBoob->bitinfo[0] = startVisible; // Visible flag
	pNewBoob->bitinfo[1] = false; // AlreadyDrawn flag
	pNewBoob->bitinfo[2] = false; // AlreadyChecked flag
	pNewBoob->bitinfo[3] = false; // Generation flag
	pNewBoob->bitinfo[5] = false; // VBO Generation flag
	pNewBoob->bitinfo[6] = false; // Movement check flag
	pNewBoob->bitinfo[7] = false; // Deletion flag
	// Set the block data to all non-solid air
	pNewBoob->hasPropData = false;
	pNewBoob->hasBlockData = false;
	for ( char i = 0; i < 8; i += 1 )
	{
		pNewBoob->solid[i] = false;
	}
	// Get a data block
	NewDataBlock( &(pNewBoob->data) );
	// Zero the memory
	memset( pNewBoob->data, 0, 32*32*32*4 );

	pNewBoob->version = TERRA_SYSTEM_VERSION;
	pNewBoob->biome = 0;
	pNewBoob->terrain = 0;
	pNewBoob->li_position = RangeVector( 0,0,0 );

	// Er, set lighting to full
	for ( uint32_t i = 0; i < 32768; ++i )
		((terra_t*)(pNewBoob->data))[i].light = 7;

	// Push it to the terrain array
	//mtTerraListGuard.lock();
	//terraList.push_back( pNewBoob );
	//mtTerraListGuard.unlock();

	return pNewBoob;
}

// ==================================== FreeBoob ======================================
// Takes a input boob and frees all objects and data associated with it.
// This includes any pointers the Boob owns, including foliage and components.
// This also frees the terrain data.
// ====================================================================================
// After this function is called, the data is no longer accessible.
void CVoxelTerrain::FreeBoob ( CBoob* pBoob )
{
	// Remove it from the loading list
	{
		mutex::scoped_lock( mtLoadingListGaurd );
		for ( uint i = 0; i < vBoobLoadList.size(); ++i )
		{
			if ( vBoobLoadList[i].pBoob == pBoob ) {
				Debug::Console->PrintWarning( "A sector that has been deallocated was found in the load list! Removing...\n" );
				vBoobLoadList.erase( vBoobLoadList.begin() + i );
			}
		}
	}

	// Wait for unlock, then lock
	while ( pBoob->locked );
	pBoob->locked = true;

	// Remove it from the terralist
	{
		mtTerraListGuard.lock();
		vector<CBoob*>::iterator target = find( terraList.begin(), terraList.end(), pBoob );
		if ( target != terraList.end() ) {
			terraList.erase( target );
		}
		mtTerraListGuard.unlock();
	}

	// Delete the foliage in the list
	for ( CBoob::FoliageIterator it = pBoob->v_foliage.begin(); it != pBoob->v_foliage.end(); it++ )
	{
		delete (*it);
	}
	// Delete the components in the list
	/*for ( CBoob::ComponentIterator it = pBoob->v_component.begin(); it != pBoob->v_component.end(); it++ )
	{
		delete (*it);
	}*/
	while ( pBoob->v_component.size() > 0 ) {
		delete (pBoob->v_component.front()); // Must be done this way, since the delete destroys the list as well
	}

	// Delete the information stored
	/*for ( char i = 0; i < 8; i += 1 )
	{
		for ( char j = 0; j < 8; j += 1 )
		{
			delete [] pBoob->data[i].data[j].data;
			delete [] pBoob->data[i].data[j].light;
			delete [] pBoob->data[i].data[j].water;
			delete [] pBoob->data[i].data[j].temp;

			pBoob->data[i].data[j].data		= NULL;
			pBoob->data[i].data[j].light	= NULL;
			pBoob->data[i].data[j].water	= NULL;
			pBoob->data[i].data[j].temp		= NULL;
		}
	}*/
	// Get a data block
	FreeDataBlock( pBoob->data );

	// Delete the boob
	delete pBoob;
}

/*
void CVoxelTerrain::GetBlockXYZ( unsigned int &x,unsigned int &y,unsigned int &z,unsigned int const& i ,unsigned int const& j ,unsigned int const& k  )
{
	x = ((i%2) * 16) + ((j%2) * 8) + (k%8);
	y = (((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8);
	z = ((i/4) * 16) + ((j/4) * 8)  + (k/64);
}*/

/*
bool CVoxelTerrain::BlockVisible ( char * data, unsigned int const& i )
{
	if (( i%32 == 0 )||( i%32 == 31 ))
		return true;
	if (( (i/32)%32 == 0 )||( (i/32)%32 == 31 ))
		return true;
	if (( (i/32/32)%32 == 0 )||( (i/32/32)%32 == 31 ))
		return true;
	if (( data[i-1] == 0 )||( data[i+1] == 0 ))
		return true;
	if (( data[i+32] == 0 )||( data[i-32] == 0 ))
		return true;
	if (( data[i+1024] == 0 )||( data[i-1024] == 0 ))
		return true;
	return false;
}
bool CVoxelTerrain::BlockVisibleEx ( char * data, unsigned int const& i, int const& width )
{
	if (( i%width == 0 )||( i%width == width-1 ))
		return true;
	if (( (i/width)%width == 0 )||( (i/width)%width == width-1 ))
		return true;
	if (( (i/width/width)%width == 0 )||( (i/width/width)%width == width-1 ))
		return true;
	if (( data[i-1] == 0 )||( data[i+1] == 0 ))
		return true;
	if (( data[i+width] == 0 )||( data[i-width] == 0 ))
		return true;
	if (( data[i+width*width] == 0 )||( data[i-width*width] == 0 ))
		return true;
	return false;
}
*/

/** 1 top 2 bottom
// 3 front 4 back
// 5 left 6 right
**/
void CVoxelTerrain::LinkCube ( CBoob * pOld, CBoob * pNew, char const& c )
{
	switch ( c )
	{
	case 1: // new top link
		pOld->top = pNew;
		pNew->bottom = pOld;
		if ( pOld->left )
			if ( pOld->left->top )
			{
				pOld->left->top->right = pNew;
				pNew->left = pOld->left->top;
			}
		if ( pOld->right )
			if ( pOld->right->top )
			{
				pOld->right->top->left = pNew;
				pNew->right = pOld->right->top;
			}
		if ( pOld->front )
			if ( pOld->front->top )
			{
				pOld->front->top->back = pNew;
				pNew->front = pOld->front->top;
			}
		if ( pOld->back )
			if ( pOld->back->top )
			{
				pOld->back->top->front = pNew;
				pNew->back = pOld->back->top;
			}
		break;
	case 2: // new bottom link
		pOld->bottom = pNew;
		pNew->top = pOld;
		if ( pOld->left )
			if ( pOld->left->bottom )
			{
				pOld->left->bottom->right = pNew;
				pNew->left = pOld->left->bottom;
			}
		if ( pOld->right )
			if ( pOld->right->bottom )
			{
				pOld->right->bottom->left = pNew;
				pNew->right = pOld->right->bottom;
			}
		if ( pOld->front )
			if ( pOld->front->bottom )
			{
				pOld->front->bottom->back = pNew;
				pNew->front = pOld->front->bottom;
			}
		if ( pOld->back )
			if ( pOld->back->bottom )
			{
				pOld->back->bottom->front = pNew;
				pNew->back = pOld->back->bottom;
			}
		break;
	case 3: // new front link
		pOld->front = pNew;
		pNew->back = pOld;
		if ( pOld->left )
			if ( pOld->left->front )
			{
				pOld->left->front->right = pNew;
				pNew->left = pOld->left->front;
			}
		if ( pOld->right )
			if ( pOld->right->front )
			{
				pOld->right->front->left = pNew;
				pNew->right = pOld->right->front;
			}
		if ( pOld->top )
			if ( pOld->top->front )
			{
				pOld->top->front->bottom = pNew;
				pNew->top = pOld->top->front;
			}
		if ( pOld->bottom )
			if ( pOld->bottom->front )
			{
				pOld->bottom->front->top = pNew;
				pNew->bottom = pOld->bottom->front;
			}
		break;
	case 4: // new back link
		pOld->back = pNew;
		pNew->front = pOld;
		if ( pOld->left )
			if ( pOld->left->back )
			{
				pOld->left->back->right = pNew;
				pNew->left = pOld->left->back;
			}
		if ( pOld->right )
			if ( pOld->right->back )
			{
				pOld->right->back->left = pNew;
				pNew->right = pOld->right->back;
			}
		if ( pOld->top )
			if ( pOld->top->back )
			{
				pOld->top->back->bottom = pNew;
				pNew->top = pOld->top->back;
			}
		if ( pOld->bottom )
			if ( pOld->bottom->back )
			{
				pOld->bottom->back->top = pNew;
				pNew->bottom = pOld->bottom->back;
			}
		break;
	case 5: // new left link
		pOld->left = pNew;
		pNew->right = pOld;
		if ( pOld->top )
			if ( pOld->top->left )
			{
				pOld->top->left->bottom = pNew;
				pNew->top = pOld->top->left;
			}
		if ( pOld->bottom )
			if ( pOld->bottom->left )
			{
				pOld->bottom->left->top = pNew;
				pNew->bottom = pOld->bottom->left;
			}
		if ( pOld->front )
			if ( pOld->front->left )
			{
				pOld->front->left->back = pNew;
				pNew->front = pOld->front->left;
			}
		if ( pOld->back )
			if ( pOld->back->left )
			{
				pOld->back->left->front = pNew;
				pNew->back = pOld->back->left;
			}
		break;
	case 6: // new right link
		pOld->right = pNew;
		pNew->left = pOld;
		if ( pOld->top )
			if ( pOld->top->right )
			{
				pOld->top->right->bottom = pNew;
				pNew->top = pOld->top->right;
			}
		if ( pOld->bottom )
			if ( pOld->bottom->right )
			{
				pOld->bottom->right->top = pNew;
				pNew->bottom = pOld->bottom->right;
			}
		if ( pOld->front )
			if ( pOld->front->right )
			{
				pOld->front->right->back = pNew;
				pNew->front = pOld->front->right;
			}
		if ( pOld->back )
			if ( pOld->back->right )
			{
				pOld->back->right->front = pNew;
				pNew->back = pOld->back->right;
			}
		break;
	}
}

bool CVoxelTerrain::BoobExists ( CBoob* pBoob )
{
	/*ResetBlock7Flag( root );
	pCurrentTargetBoob = pBoob;
	return BoobExistsRecursive( root );*/

	// Search for it in the terralist
	mtTerraListGuard.lock();
	vector<CBoob*>::iterator target = find( terraList.begin(), terraList.end(), pBoob );
	bool result = ( target != terraList.end() );
	mtTerraListGuard.unlock();
	return result;
}
bool CVoxelTerrain::BoobExistsRecursive ( CBoob* pBoob )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			pBoob->bitinfo[7] = true;
			if ( pBoob == pCurrentTargetBoob )
			{
				return true;
			}
			else
			{
				return (
					BoobExistsRecursive( pBoob->left )||
					BoobExistsRecursive( pBoob->right )||
					BoobExistsRecursive( pBoob->top )||
					BoobExistsRecursive( pBoob->bottom )||
					BoobExistsRecursive( pBoob->front )||
					BoobExistsRecursive( pBoob->back )
					);
			}
		}
	}
	return false;
}

vector<CBoob*>& CVoxelTerrain::GetLockTerraList ( void )
{
	mtTerraListGuard.lock();
	return terraList;
}
void CVoxelTerrain::ReleaseLockTerraList ( void )
{
	mtTerraListGuard.unlock();
}