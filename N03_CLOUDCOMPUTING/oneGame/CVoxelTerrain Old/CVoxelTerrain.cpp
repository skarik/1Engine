
// Includes
#include "CVoxelTerrain.h"
#include "CGameSettings.h"
#include "CTime.h"

#include "CTerraGenDefault.h"

// Static variable declares
const LongIntPosition	CVoxelTerrain::boobSize		= LongIntPosition( 32,32,32 ); //b-cup
const ftype				CVoxelTerrain::blockSize	= 2.0f;
vector<CVoxelTerrain*>	CVoxelTerrain::terrainList;

// Class definitions
CVoxelTerrain::CVoxelTerrain ( void )
	: CGameObject() , CRenderableObject()
{
	// Terrain loader info
	iTerrainSize = 5;
	bLoadLowDetail = true;

	//bHasSamplers = false;
	bRootUpdated = false;

	// Debug Info
	bDrawDebugBoobs = false;

	// Terrain Properties
	bSpreadTerrain = true;
	iTerrainSeed = ActiveGameSettings->i_cl_DefaultSeed;

	// Error flags
	bFailedBadAlloc = false;
	
	// Add this sucker to list
	terrainList.push_back( this );

	int size;
	bool bTerrainExists = TerraFileExists();
	//bool bTerrainExists = false;
	/*if ( !bTerrainExists )
	{
		// GENERATE NEW WORLD
		size = iTerrainSize;// 23;
	}
	else
	{
		size = iTerrainSize;
	}*/
	//if ( bLoadLowDetail )
	//	size = iTerrainSize*2;
	//else
		size = iTerrainSize;

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
	// End Terrain Creation

	// Terrain Generation
	generator = new CTerraGenDefault( this, iTerrainSeed );

	{
		/*noise		= NULL;
		noise_hf	= NULL;
		noise_hhf	= NULL;
		noise_lf	= NULL;
		noise_biome	= NULL;
		noise_terra = NULL;

		pSamplerTerrainType	= NULL;
		pSamplerBiomeType	= NULL;

		noise_pub		= NULL;
		noise_pub_hf	= NULL;
		noise_pub_hhf	= NULL;
		noise_pub_lf	= NULL;
		noise_pub_biome	= NULL;
		noise_pub_terra = NULL;*/

		//iGenerationMode	= GENERATE_DUAL_SAVE;
		//iGenerationMode = GENERATE_TO_MEMORY;
		//iGenerationMode = GENERATE_TO_DISK;
		generator->SetGenerationMode( CTerrainGenerator::GENERATE_TO_MEMORY );
		
		if (( !bTerrainExists )&&( false ))
		{
			DebugConsole::PrintMessage( "Generating terrain.\n" );
			//GenerateTerrain( root, root_position );
			DebugConsole::PrintMessage( " Done.\n" );
		}
		else
		{
			DebugConsole::PrintMessage( "Loading terrain.\n" );
			LoadTerrain( root, root_position );
			ResetBlock7Flag( root );
			DebugConsole::PrintMessage( " Done.\n" );
		}

		// Update LOD's afterwards
		//UpdateLODs ( root, 1, root_position );
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

	// == Renderer Initialization ==
	my2ndRenderer	= new CVoxelTerrainTranslucentRenderer ( this );

	// Load a texture
	myMat = new glMaterial;
	myMat->loadTexture( ".res\\textures\\terraTexture.jpg" );
	myMat->useColors = true;
	//myMat->useLighting = false;
	myMat->setShader( new glShader( ".res\\shaders\\world\\terrainDefault.glsl" ) );
	//myMat->iFaceMode = glMaterial::FM_FRONTANDBACK;

	// Set the material as our base material
	this->SetMaterial( myMat );

	// And now start the update thread
	StartSideThread();
	// And now start the generation thread
	StartGenerationThread();
}

CVoxelTerrain::~CVoxelTerrain ( void )
{
	// Stop the side thread
	EndSideThread();
	// Stop the generation thread
	EndGenerationThread();

	// Remove terrain from list
	vector<CVoxelTerrain*>::iterator index;
	index = find( terrainList.begin(), terrainList.end(), this );
	if ( index != terrainList.end() )
	{
		terrainList.erase( index );
	}

	delete my2ndRenderer;
	// Free VBO
	//glDeleteBuffersARB( 1, &iVBOverts );
	//glDeleteBuffersARB( 1, &iVBOfaces );
	// Delete the data...sorta, gotta get it working PROPER (MEM LEAK ATM) TODO
	//delete [] vertices;
	//delete [] quads;
	//delete root;
	// SAVE
	SaveActiveTerrain( root, root_position );
	ResetBlock7Flag( root );

	FreeTerrain( root );
	root = NULL;

	myMat->releaseOwnership();
	//delete myMat; TODO

	delete generator;

	
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

// ==================================== FreeBoob ======================================
// Takes a input boob and frees all objects and data associated with it.
// This includes any pointers the Boob owns, including foliage and components.
// This also frees the terrain data.
// ====================================================================================
// After this function is called, the data is no longer accessible.
void CVoxelTerrain::FreeBoob ( CBoob* pBoob )
{
	// Delete the foliage in the list
	for ( CBoob::FoliageIterator it = pBoob->v_foliage.begin(); it != pBoob->v_foliage.end(); it++ )
	{
		delete (*it);
	}
	// Delete the components in the list
	for ( CBoob::ComponentIterator it = pBoob->v_component.begin(); it != pBoob->v_component.end(); it++ )
	{
		delete (*it);
	}

	// Delete the information stored
	for ( char i = 0; i < 8; i += 1 )
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
	}

	// Remove it from the terralist
	mtTerraListGuard.lock();
	vector<CBoob*>::iterator target = find( terraList.begin(), terraList.end(), pBoob );
	terraList.erase( target );
	mtTerraListGuard.unlock();

	// Delete the boob
	delete pBoob;
}

// ====================================== GETTERS AND SETTERS ======================================
// Toggle root update
void CVoxelTerrain::SetRootUpdate ( bool newMode )
{
	bUpdateCameraPos = newMode;
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

	// Update camera position
	if ( bUpdateCameraPos )
		vCameraPos = CCamera::pActiveCamera->transform.position;

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
	}
	CenterMoveFreeTree32( root, root_position ); // Unload that shit

	if ( bLoadLowDetail )
		UpdateLODs ( root, 1, root_position );

	// == LOADING ==
	// If there was a loading error, reset the entire loading list.
	if ( bFailedBadAlloc )
	{
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
	// Load the trees too
	CreateQueuedFoliage();

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
	if ( fWaterSimulation > 0.01f )
	{
		iWaterSimulation++;
		while ( fWaterSimulation > 0.01f )
			fWaterSimulation -= 0.01f;
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

	if ( CInput::keydown[VK_F7] )
	{
		bDrawDebugBoobs = !bDrawDebugBoobs;
	}
	
	// == DEBUG DRAWING ==
	static bool bDrawDebugCaves = false;
	if ( CInput::keydown[VK_F12] )
	{
		bDrawDebugCaves = !bDrawDebugCaves;
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
void CVoxelTerrain::PostUpdate ( void )
{
	if ( mtGaurd.try_lock() )
	{
		GenerateVBO(); // Todo: move this
		mtGaurd.unlock();
	}
}

// Updates the root node based on camera position and current root position
void CVoxelTerrain::UpdateRoot ( void )
{
	// Boolean to update LOD's if root has been updated
	bRootUpdated = false;

	// Check if the current camera position is in bounds
	Vector3d pos = vCameraPos * ( 1.0f/64.0f );

	// Stuff!
	LongIntPosition oldPos = root_position;

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
		pBoob->visible = false;
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

void CVoxelTerrain::UpdateLODs ( CBoob* pBoob, char currentResolution, LongIntPosition const& position )
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
			LongIntPosition vPosDiference( abs(position.x-root_position.x), abs(position.y-root_position.y), abs(position.z-root_position.z) );
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
					if ( pBoob->hasData )
					{
						//SaveBoobData( pBoob, position );
						PushSavingList( pBoob, position );
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
						// Set has data to false, meaning it needs a load.
						pBoob->hasData = false;
					}

					// Push to update since it needs to be regenerated
					PushLoadingList( pBoob, position, false );
					pBoob->current_resolution = newResolution;
				}
				else if ( newResolution == 1 ) // If the new resolution is 1, it needs to be loaded.
				{
					if ( !pBoob->hasData )
					{
						// Push to update if needs to be regenerated
						PushLoadingList( pBoob, position, false );
						pBoob->current_resolution = newResolution;
					}
				}
				
			}

			// Call resolution set for other guys
			if ( position.x >= root_position.x )
				UpdateLODs( pBoob->front, newResolution, LongIntPosition( position.x+1, position.y, position.z ) );
			if ( position.y >= root_position.y )
				UpdateLODs( pBoob->left, newResolution, LongIntPosition( position.x, position.y+1, position.z ) );
			if ( position.z >= root_position.z )
				UpdateLODs( pBoob->top, newResolution, LongIntPosition( position.x, position.y, position.z+1 ) );
			if ( position.x <= root_position.x )
				UpdateLODs( pBoob->back, newResolution, LongIntPosition( position.x-1, position.y, position.z ) );
			if ( position.y <= root_position.y )
				UpdateLODs( pBoob->right, newResolution, LongIntPosition( position.x, position.y-1, position.z ) );
			if ( position.z <= root_position.z )
				UpdateLODs( pBoob->bottom, newResolution, LongIntPosition( position.x, position.y, position.z-1 ) );
		}
	}
}

// Updates the center of the loaded terrain based on root position and loads/unloads terrain based on what's needed.
void CVoxelTerrain::PerformCenterMovement ( void )
{
	CenterMoveTree32 ( root, root, root_position, FRONT );
	//GenerateTerrain( root, root_position ); //TODO switch with a loader checker
	//CenterMoveFreeTree32( root, root_position );

}
static vector<CBoob*> deleteList;

// 
//  void CVoxelTerrain::CenterMoveTree32 ( CBoob * pBoob, CBoob * pBoobPrev, LongIntPosition const& position, EFaceDir goDir )
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
void CVoxelTerrain::CenterMoveTree32 ( CBoob * pBoob, CBoob * pBoobPrev, LongIntPosition const& position, EFaceDir goDir )
{
	// Recursively check the outliers first, then work way in, because it likely will be unneeded on the inlyers.
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[6] == false )
		{
			pBoob->bitinfo[6] = true;

			// Move to the edges FIRST.
			if ( position.x >= root_position.x )
				CenterMoveTree32( pBoob->front,pBoob, LongIntPosition( position.x+1, position.y, position.z ), FRONT );
			if ( position.y >= root_position.y )
				CenterMoveTree32( pBoob->left,pBoob, LongIntPosition( position.x, position.y+1, position.z ), LEFT );
			if ( position.z >= root_position.z )
				CenterMoveTree32( pBoob->top,pBoob, LongIntPosition( position.x, position.y, position.z+1 ), TOP );
			if ( position.x <= root_position.x )
				CenterMoveTree32( pBoob->back,pBoob, LongIntPosition( position.x-1, position.y, position.z ), BACK );
			if ( position.y <= root_position.y )
				CenterMoveTree32( pBoob->right,pBoob, LongIntPosition( position.x, position.y-1, position.z ), RIGHT );
			if ( position.z <= root_position.z )
				CenterMoveTree32( pBoob->bottom,pBoob, LongIntPosition( position.x, position.y, position.z-1 ), BOTTOM );

			//pBoob->bitinfo[6] = false;

			// No LOD on far boobs
			if ( !bLoadLowDetail )
			{
				// Now, as we go back to the center of the calling, check if the boob is out of drawing bounds
				if ( ( abs(position.x - root_position.x) > iTerrainSize/2 )||( abs(position.y - root_position.y) > iTerrainSize/2 )||( abs(position.z - root_position.z) > iTerrainSize/2 ) )
				{
					int index = IsInLoadingList ( pBoob );
					// Check if it's in the loading list.
					if ( index < 0 )
					{
						// If it's not in the list, it's safe to save
						//SaveBoobData( pBoob, position ); // Save it as it's being unloaded.
						PushSavingList( pBoob, position );
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
				PushLoadingList( pNewBoob, position, true );
			}
		}
	}
}
// Check for memory overflow issues within Havok, causing Havok functions to return null.
void CVoxelTerrain::CenterMoveFreeTree32 ( CBoob * pBoob, LongIntPosition const& position )
{
	while ( !deleteList.empty() )
	{
		pBoob = deleteList.back();
		//if ( pBoob->pMesh->needUpdateOnVBO )
		//	return;

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

// Tree visibilty culling, based on frustum culling.
void CVoxelTerrain::CullTree32 ( CBoob * pBoob, LongIntPosition const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[2] == false )
		{
			// Set drawn flag
			pBoob->bitinfo[2] = true;

			pBoob->position = Vector3d( ftype(position.x*64+32),ftype(position.y*64+32),ftype(position.z*64+32) );
			pBoob->li_position = position;
			char result;
			result = CCamera::pActiveCamera->SphereIsVisible( pBoob->position, 55 );
			// 32

			if ( bDrawDebugBoobs )
			{
				CCubic debugCube ( Vector3d(ftype(position.x*64),ftype(position.y*64),ftype(position.z*64)), Vector3d( 64,64,64 ) );
				debugCube.DrawDebug();
			}

			// Check current boob
			if ( result == 2 )
			{
				//if ( !pBoob->bitinfo[0] )
				//	needUpdateOnVBO = true;
				pBoob->bitinfo[0] = true;
				for ( char i = 0; i < 8; i += 1 )
				{
					pBoob->visible[i] = true;
					for ( char j = 0; j < 8; j += 1 )
					{
						pBoob->data[i].visible[j] = true;
					}
				}
			}
			else if ( result == 1 )
			{
				pBoob->bitinfo[0] = true;
				/*for ( char i = 0; i < 8; i += 1 )
				{
					if ( pBoob->visible[i] )
					{
						CullTree16( pBoob, &(pBoob->data[i]), i, position );
					}
				}*/
				for ( char i = 0; i < 8; i += 1 )
				{
					pBoob->visible[i] = true;
					for ( char j = 0; j < 8; j += 1 )
					{
						pBoob->data[i].visible[j] = true;
					}
				}
			}
			else if ( result == 0 )
			{
				//if ( pBoob->bitinfo[0] )
				//	needUpdateOnVBO = true;
				// Boob completely invisible
				pBoob->bitinfo[0] = false;
			}

			// Cull other boobs
			CullTree32( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			CullTree32( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			CullTree32( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			CullTree32( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
			CullTree32( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			CullTree32( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );
		}
	}
}

void CVoxelTerrain::CullTree16 ( CBoob * pBoob, subblock16 * block, char const index, LongIntPosition const& position )
{
	// Draw current block
	for ( char i = 0; i < 8; i += 1 )
	{
		char result;
		result = CCamera::pActiveCamera->SphereIsVisible(
			Vector3d(
				ftype(position.x*64 + ((index%2))*32 + 16),
				ftype(position.y*64 + ((index/2)%2)*32 + 16),
				ftype(position.z*64 + ((index/4))*32 + 16)
				), 27 //16
			);
		if ( result == 2 )
		{
			//if ( !pBoob->visible[index] )
			//	needUpdateOnVBO = true;
			pBoob->visible[index] = true;
			for ( char j = 0; j < 8; j += 1 )
			{
				block->visible[j] = true;
			}
		}
		else if ( result == 1 )
		{
			pBoob->visible[index] = false;
			CullTree8( pBoob, &(block->data[i]), index, i, position );
		}
		else if ( result == 0 )
		{
			//if ( pBoob->visible[index] )
			//	needUpdateOnVBO = true;
			pBoob->visible[index] = false;
		}

		/*if ( pBoob->visible[i] )
		{
			CullTree8( pBoob, &(block->data[i]), index, i, position );
		}*/
	}
}
void CVoxelTerrain::CullTree8 ( CBoob * pBoob, subblock8 * block, char const index, char const subindex, LongIntPosition const& position )
{

}

// ============= GenerateBoob ===============
// Creates a new boob and initializes data
CBoob* CVoxelTerrain::GenerateBoob ( void )
{
	bool startVisible = false;
	CBoob* pNewBoob;
	try
	{
		pNewBoob = new CBoob;
		// and create its stuff
		pNewBoob->pMesh = new CBoobMesh;
		pNewBoob->pMesh->pOwner = pNewBoob;
		pNewBoob->pGrass = new CTerraGrass( &pNewBoob->v_grass, pNewBoob );
		//pNewBoob->pGrass = NULL;
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
	pNewBoob->hasData = false;
	pNewBoob->hasBlockData = false;
	for ( char i = 0; i < 8; i += 1 )
	{
		pNewBoob->visible[i] = startVisible;
		for ( char j = 0; j < 8; j += 1 )
		{
			pNewBoob->data[i].visible[j] = startVisible;

			pNewBoob->data[i].data[j].data	= new unsigned char [512];
			pNewBoob->data[i].data[j].light	= new unsigned char [512];
			pNewBoob->data[i].data[j].water	= new unsigned char [512];
			pNewBoob->data[i].data[j].temp	= new unsigned char [512];
			for ( int k = 0; k < 512; k += 1 )
			{
				pNewBoob->data[i].data[j].data[k]	= 0;
				pNewBoob->data[i].data[j].light[k]	= 255;
				pNewBoob->data[i].data[j].water[k]	= 0;
				pNewBoob->data[i].data[j].temp[k]	= 0;
			}
		}
	}

	pNewBoob->version = 5;
	pNewBoob->biome = 0;
	pNewBoob->terrain = 0;
	pNewBoob->li_position = LongIntPosition( 0,0,0 );

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
		//char* bull = ba.what();
		cout << "CVoxelTerrain.cpp (" << __LINE__ << "): Caught exception: " << ba.what() << endl;
		MessageBox ( NULL,"The gosh darn fucking memory.","FACK FACK FACK.",MB_OK | MB_ICONINFORMATION );
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
	pNewBoob->hasData = false;
	pNewBoob->hasBlockData = false;
	for ( char i = 0; i < 8; i += 1 )
	{
		pNewBoob->visible[i] = startVisible;
		for ( char j = 0; j < 8; j += 1 )
		{
			pNewBoob->data[i].visible[j] = startVisible;

			pNewBoob->data[i].data[j].data	= new unsigned char [512];
			pNewBoob->data[i].data[j].light	= new unsigned char [512];
			pNewBoob->data[i].data[j].water	= new unsigned char [512];
			pNewBoob->data[i].data[j].temp	= new unsigned char [512];
			for ( int k = 0; k < 512; k += 1 )
			{
				pNewBoob->data[i].data[j].data[k]	= 0;
				pNewBoob->data[i].data[j].light[k]	= 255;
				pNewBoob->data[i].data[j].water[k]	= 0;
				pNewBoob->data[i].data[j].temp[k]	= 0;
			}
		}
	}

	pNewBoob->version = 5;
	pNewBoob->biome = 0;
	pNewBoob->terrain = 0;
	pNewBoob->li_position = LongIntPosition( 0,0,0 );

	// Push it to the terrain array
	mtTerraListGuard.lock();
	terraList.push_back( pNewBoob );
	mtTerraListGuard.unlock();

	return pNewBoob;
}

void CVoxelTerrain::GetBlockXYZ( unsigned int &x,unsigned int &y,unsigned int &z,unsigned int const& i ,unsigned int const& j ,unsigned int const& k  )
{
	x = ((i%2) * 16) + ((j%2) * 8) + (k%8);
	y = (((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8);
	z = ((i/4) * 16) + ((j/4) * 8)  + (k/64);
}

void CVoxelTerrain::GenerateCube ( void )
{
	Vector3d vertexData[8];
	vertexData[0].x = 0;
	vertexData[0].y = 0;
	vertexData[0].z = 0;

	vertexData[1].x = 2;
	vertexData[1].y = 0;
	vertexData[1].z = 0;

	vertexData[2].x = 2;
	vertexData[2].y = 2;
	vertexData[2].z = 0;

	vertexData[3].x = 0;
	vertexData[3].y = 2;
	vertexData[3].z = 0;

	vertexData[4].x = 0;
	vertexData[4].y = 0;
	vertexData[4].z = 2;

	vertexData[5].x = 2;
	vertexData[5].y = 0;
	vertexData[5].z = 2;

	vertexData[6].x = 2;
	vertexData[6].y = 2;
	vertexData[6].z = 2;

	vertexData[7].x = 0;
	vertexData[7].y = 2;
	vertexData[7].z = 2;

	glNewList( iCubeList, GL_COMPILE );
	glBegin( GL_QUADS );
		// Bottom
		glColor3f(1.0f,0.0f,0.0f);
		glNormal3f( 0,0,-1 );
		glVertex3f( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		glVertex3f( vertexData[2].x,vertexData[2].y,vertexData[2].z );
		glVertex3f( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		glVertex3f( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		// Top
		glColor3f(0.0f,1.0f,0.0f);//
		glNormal3f( 0,0,1 );
		glVertex3f( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		glVertex3f( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		glVertex3f( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		glVertex3f( vertexData[7].x,vertexData[7].y,vertexData[7].z );
		// Front
		glColor3f(0.0f,0.0f,1.0f);
		glNormal3f( 1,0,0 );
		glVertex3f( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		glVertex3f( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		glVertex3f( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		glVertex3f( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		// Back
		glColor3f(1.0f,1.0f,0.0f);
		glNormal3f( -1,0,0 );
		glVertex3f( vertexData[7].x,vertexData[7].y,vertexData[7].z );
		glVertex3f( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		glVertex3f( vertexData[2].x,vertexData[2].y,vertexData[2].z );
		glVertex3f( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		// Left
		glColor3f(1.0f,0.0f,1.0f);//
		glNormal3f( 0,1,0 );
		glVertex3f( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		glVertex3f( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		glVertex3f( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		glVertex3f( vertexData[7].x,vertexData[7].y,vertexData[7].z );
		// Right
		glColor3f(0.0f,1.0f,1.0f);
		glNormal3f( 0,-1,0 );
		glVertex3f( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		glVertex3f( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		glVertex3f( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		glVertex3f( vertexData[2].x,vertexData[2].y,vertexData[2].z );
	glEnd();
	glEndList();

	glNewList( iFaceList+BOTTOM-1, GL_COMPILE );
	glBegin( GL_QUADS );
		// Bottom 0
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		glVertex3f( vertexData[2].x,vertexData[2].y,vertexData[2].z );
		glVertex3f( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		glVertex3f( vertexData[0].x,vertexData[0].y,vertexData[0].z );
	glEnd();
	glEndList();

	glNewList( iFaceList+TOP-1, GL_COMPILE );
	glBegin( GL_QUADS );
		// Top 1 
		glColor3f(0.0f,1.0f,0.0f);//
		glVertex3f( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		glVertex3f( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		glVertex3f( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		glVertex3f( vertexData[7].x,vertexData[7].y,vertexData[7].z );
	glEnd();
	glEndList();

	glNewList( iFaceList+FRONT-1, GL_COMPILE );
	glBegin( GL_QUADS );
		// Front 2
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		glVertex3f( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		glVertex3f( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		glVertex3f( vertexData[1].x,vertexData[1].y,vertexData[1].z );
	glEnd();
	glEndList();

	glNewList( iFaceList+BACK-1, GL_COMPILE );
	glBegin( GL_QUADS );
		// Back 3 
		glColor3f(1.0f,1.0f,0.0f);
		glVertex3f( vertexData[7].x,vertexData[7].y,vertexData[7].z );
		glVertex3f( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		glVertex3f( vertexData[2].x,vertexData[2].y,vertexData[2].z );
		glVertex3f( vertexData[3].x,vertexData[3].y,vertexData[3].z );
	glEnd();
	glEndList();

	glNewList( iFaceList+LEFT-1, GL_COMPILE );
	glBegin( GL_QUADS );
		// Left 4
		glColor3f(1.0f,0.0f,1.0f);//
		glVertex3f( vertexData[3].x,vertexData[3].y,vertexData[3].z );
		glVertex3f( vertexData[0].x,vertexData[0].y,vertexData[0].z );
		glVertex3f( vertexData[4].x,vertexData[4].y,vertexData[4].z );
		glVertex3f( vertexData[7].x,vertexData[7].y,vertexData[7].z );
	glEnd();
	glEndList();

	glNewList( iFaceList+RIGHT-1, GL_COMPILE );
	glBegin( GL_QUADS );
		// Right 5
		glColor3f(0.0f,1.0f,1.0f);
		glVertex3f( vertexData[6].x,vertexData[6].y,vertexData[6].z );
		glVertex3f( vertexData[5].x,vertexData[5].y,vertexData[5].z );
		glVertex3f( vertexData[1].x,vertexData[1].y,vertexData[1].z );
		glVertex3f( vertexData[2].x,vertexData[2].y,vertexData[2].z );
	glEnd();
	glEndList();
}


void CVoxelTerrain::RenderTree32 ( CBoob * pBoob, LongIntPosition const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[1] == false )
		{
			// Set drawn flag
			pBoob->bitinfo[1] = true;

			// If visible...
			if ( pBoob->bitinfo[0] == true )
			{
				// Draw current boob
				for ( char i = 0; i < 8; i += 1 )
				{
					if ( pBoob->visible[i] )
					{
						RenderTree16( pBoob, &(pBoob->data[i]), i, position );
					}
				}
			}

			// Draw other boobs
			RenderTree32( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			RenderTree32( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			RenderTree32( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			RenderTree32( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
			RenderTree32( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			RenderTree32( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );
		}
	}
}
void CVoxelTerrain::RenderTree16 ( CBoob * pBoob, subblock16 * block, char const& index, LongIntPosition const& position )
{
	// Draw current block
	for ( char i = 0; i < 8; i += 1 )
	{
		if ( block->visible[i] )
		{
			RenderTree8( pBoob, &(block->data[i]), index, i, position );
		}
	}
}
void CVoxelTerrain::RenderTree8 ( CBoob * pBoob, subblock8 * block, char const& index, char const& subindex, LongIntPosition const& position )
{
	// Draw current block
	Vector3d baseOffset = Vector3d(
		position.x*32.0f+((index%2)*16)+((subindex%2)*8),
		position.y*32.0f+(((index/2)%2)*16)+(((subindex/2)%2)*8),
		position.z*32.0f+((index/4)*16)+((subindex/4)*8) );

	Vector3d pos;

	// Draw current block
	char res = pBoob->current_resolution;
	char i, j, k;
	for ( k = 0; k < 8; k += res )
	{
		for ( j = 0; j < 8; j += res )
		{
			for ( i = 0; i < 8; i += res )
			{
				if ( block->data[i+j*8+k*64] > 0 )
				{
					if ( BlockVisibleEx( (char*)(block->data), i+j*8+k*64, 8 ) )
					{
						pos.x = baseOffset.x + i;
						pos.y = baseOffset.y + j;
						pos.z = baseOffset.z + k;
						RenderBlock( pos*2.0f, res );
					}
				}
			}
		}
	}
}

void CVoxelTerrain::RenderBoob ( CBoob * pBoob, LongIntPosition const& position  )
{
	if ( pBoob != NULL )
	{
		/*RenderBoob( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
		RenderBoob( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
		RenderBoob( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
		RenderBoob( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
		RenderBoob( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
		RenderBoob( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );*/
		// FIX THIS
		if ( position.z >= 0 )
			RenderBoob( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
		if ( position.x >= 0 )
			RenderBoob( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
		if ( position.y >= 0 )
			RenderBoob( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
		if ( position.z <= 0 )
			RenderBoob( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
		if ( position.x <= 0 )
			RenderBoob( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
		if ( position.y <= 0 )
			RenderBoob( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );

		if ( renderMethod == 0 )
		{
			for ( unsigned int i = 0; i < 32768; i += 1 )
			{
				/*if ( pBoob->data[i] )
				{
					RenderBlockExp( Vector3d(
						(ftype)(( position.x*boobSize.x + (int)(i%32) )*2),
						(ftype)(( position.y*boobSize.y + (int)((i/32)%32) )*2),
						(ftype)(( position.z*boobSize.z + (int)(((i/32)/32)%32) ))*2),
						pBoob, i
						);
				}*/
			}
		}
		else
		{
			for ( unsigned int i = 0; i < 32768; i += 1 )
			{
				/*if (( pBoob->data[i] > 0 )&&( BlockVisible( (char*)pBoob->data, i ) ))
				{
					RenderBlock( Vector3d(
						(ftype)(( position.x*boobSize.x + (int)(i%32) )*2),
						(ftype)(( position.y*boobSize.y + (int)((i/32)%32) )*2),
						(ftype)(( position.z*boobSize.z + (int)(((i/32)/32)%32) ))*2)
						);
				}*/
			}
		}
	}
}

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
bool CVoxelTerrain::BlockVisibleEx ( CBoob * pBoob, subblock8 * block, char const& index, char const& subindex, char * data, unsigned int const& i, int const& width )
{
	static char check_data [6];
	if ((i%width != 0))
	{
		check_data[0] = data[i-1];
	}
	else
	{
		if ( pBoob->front )
		{

		}
		else
		{
			return false;
		}
	}
	/*if (( i%width == 0 )||( i%width == width-1 ))
		return true;
	if (( (i/width)%width == 0 )||( (i/width)%width == width-1 ))
		return true;
	if (( (i/width/width)%width == 0 )||( (i/width/width)%width == width-1 ))
		return true;
	if (( data[i-1] == 0 )||( data[i+1] == 0 ))
		return true;
	if (( data[i+width] == 0 )||( data[i-width] == 0 ))
		return true;
	if (( data[i+width*width] == 0 )||( data[i-width*width] == 0 ))*/
		return true;
	return false;
}

void CVoxelTerrain::RenderBlock ( Vector3d const& position )
{
	glLoadIdentity();	
	glTranslatef( position.x, position.y, position.z );
	glCallList( iCubeList );
	/*glCallList( iFaceList );
	glCallList( iFaceList+1 );
	glCallList( iFaceList+2 );
	glCallList( iFaceList+3 );
	glCallList( iFaceList+4 );
	glCallList( iFaceList+5 );*/
}
void CVoxelTerrain::RenderBlock ( Vector3d const& position, char const& scale )
{
	glLoadIdentity();
	glTranslatef( position.x, position.y, position.z );
	glScalef( scale,scale,scale );
	glCallList( iCubeList );
	/*glCallList( iFaceList );
	glCallList( iFaceList+1 );
	glCallList( iFaceList+2 );
	glCallList( iFaceList+3 );
	glCallList( iFaceList+4 );
	glCallList( iFaceList+5 );*/
}

void CVoxelTerrain::RenderBlockExp ( Vector3d const& position, CBoob * pBoob, unsigned int const& i )
{
	//if ( !CCamera::pActiveCamera->PointIsVisible( position ) )
	//	return;

	char * data = (char*)pBoob->data;

	if (( i%32 == 0 )||( i%32 == 31 ))
		return RenderBlock( position );
	if (( (i/32)%32 == 0 )||( (i/32)%32 == 31 ))
		return RenderBlock( position );
	if (( (i/32/32)%32 == 0 )||( (i/32/32)%32 == 31 ))
		return RenderBlock( position );
	if (( i%32 == 0 )||( i%32 == 31 ))
		return;
	if (( (i/32)%32 == 0 )||( (i/32)%32 == 31 ))
		return;
	if (( (i/32/32)%32 == 0 )||( (i/32/32)%32 == 31 ))
		return;
	glLoadIdentity();	
	glTranslatef( position.x, position.y, position.z );
	/*if (( data[i-1024] != 0 )&&( vCameraDir.z < 0 ))
		glCallList( iFaceList+1 );
	if (( data[i+1024] != 0 )&&( vCameraDir.z > 0 ))
		glCallList( iFaceList );
	if (( data[i-32] != 0 )&&( vCameraDir.y < 0 ))
		glCallList( iFaceList+3 );
	if (( data[i+32] != 0 )&&( vCameraDir.y > 0 ))
		glCallList( iFaceList+2 );
	if (( data[i-1] != 0 )&&( vCameraDir.x < 0 ))
		glCallList( iFaceList+5 );
	if (( data[i+1] != 0 )&&( vCameraDir.x > 0 ))
		glCallList( iFaceList+4 );*/
	if (( data[i+1024] == 0 ))
		glCallList( iFaceList+TOP-1 );
	if (( data[i-1024] == 0 ))
		glCallList( iFaceList+BOTTOM-1 );
	if (( data[i+32] == 0 ))
		glCallList( iFaceList+BACK-1 );
	if (( data[i-32] == 0 ))
		glCallList( iFaceList+FRONT-1 );
	if (( data[i+1] == 0 ))
		glCallList( iFaceList+RIGHT-1 );
	if (( data[i-1] == 0 ))
		glCallList( iFaceList+LEFT-1 );
}

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