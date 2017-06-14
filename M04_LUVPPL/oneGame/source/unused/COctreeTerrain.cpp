
// Includes
#include "COctreeTerrain.h"
#include "CGameSettings.h"
#include "CTime.h"

#include "CTerrainIO.h"
#include "CTerraGenDefault.h"
#include "CTerraGenDebug.h"

#include "COctreeRenderer.h"
#include "COctreeIO.h"
#include "CTerrainQueuedObjectLoader.h"

#include "world/generation/CPatternController.h"
#include "world/generation/CRegionController.h"

using std::cout;
using std::endl;

// Static variable declares
//const RangeVector	CVoxelTerrain::boobSize		= RangeVector( 32,32,32 ); //b-cup
const Real				COctreeTerrain::blockSize	= 2.0f;
vector<COctreeTerrain*>	COctreeTerrain::terrainList;

// Class definitions
COctreeTerrain::COctreeTerrain ( void )
	: CGameBehavior()
{
	// Debug message
	cout << "Initalizing TerraSys MARK " << TERRA_SYSTEM_VERSION << endl;
	// Mark system as not ready yet.
	m_systemReady = false;
	m_systemPaused = false;

	// Terrain Properties
	m_generateTerrain = true; //False for levels
	m_terrainSeed = CGameSettings::Active()->i_cl_DefaultSeed;

	// Terrain Loader Settings
	m_subdivisionTarget = uint32_t(blockSize);
	m_currentSubdivisionTarget = m_subdivisionTarget;
	CGameSettings::Active()->i_cl_ter_Range = 5;
	m_givenRange = CGameSettings::Active()->i_cl_ter_Range;
	m_treeStartResolution = 16 * (int)pow( 2, CGameSettings::Active()->i_cl_ter_Range );
	cout << " across a " << m_treeStartResolution*blockSize << "x" << m_treeStartResolution*blockSize << "x" << m_treeStartResolution*blockSize*0.5 << " ft area" << endl;
	cout << " current max resolution is " << m_treeStartResolution << " (in relation to zero)" << endl;
	cout << " sector memory size is " << sizeof(Terrain::Sector) << endl;

	// Terrain Memory Management	
	cout << " Allocating memory..." << endl;
	if ( !InitTerraMemory() )
	{
		char message [256];
		sprintf( message, "Could not allocate enough memory for terrain. Currently at density %d.", CGameSettings::Active()->i_cl_ter_Range );
		MessageBox( NULL, message, "Fatal Error.", 0 );
		exit( 0 );
	}
	// Setup sectors
	int tSectorStateSize = (m_treeStartResolution/32)*(m_treeStartResolution/32)*(m_treeStartResolution/64);
	cout << " allocating memory for game sector states. Number of sectors: " << tSectorStateSize << endl;
	cout << "   Estimated size: " << (tSectorStateSize*sizeof(SectorState))/1024 << " KB" << endl;
	m_state_sectors = new SectorState [tSectorStateSize];
	// Setup start position
	m_state_centerPosition = Vector3d_d(0,0,0);
	m_state_centerIndex = RangeVector(0,0,0);
	m_state_followTarget = Vector3d(0,0,0);
	m_systemRequestingShift = false;

	// Generate the root
	cout << " creating root..." << endl;
	NewDataBlock( &m_root );
	m_root->linkstyle = true;
	m_root->BeginWrite(); // Lock root for rest of the time
	//Terrain::write_lock lock(m_root->lock); 

	// Generate the sidebuffers
	cout << " allocating side-buffers..." << endl;
	NewDataBlock( &m_sidebuffer0 );
	SubdivideSideBuffer( m_sidebuffer0, 32 );
	m_sidebuffer0_open.clear();
	NewDataBlock( &m_sidebuffer1 );
	SubdivideSideBuffer( m_sidebuffer1, 32 );
	m_sidebuffer1_open.clear();
	cout << " currently using " << GetMemoryUsage()*100 << "% of memory" << endl;

	// Create the queued loader
	cout << " Creating queued loader system..." << endl;
	m_queueloader = new Terrain::CQueuedObjectLoader;

	// Create renderer
	cout << " Creating COctreeRenderer instance..." << endl;
	m_renderer = new COctreeRenderer( this );

	// Create the pattern and world simulators
	m_patterns = NULL;
	m_regions  = NULL;

	// Create IO tool
	//mIO = new Terrain::COctreeIO( CGameSettings::Active()->GetTerrainSaveDir().c_str(), this );
	mIO = NULL; // Set it to null, stopping any data IO until done

	// Add self to list
	terrainList.push_back( this );

	Job_StartThreads();

	// Mark system as now ready
	m_systemReady = true;
	// Mark root as open for usage
	m_root->EndWrite();
}

COctreeTerrain::~COctreeTerrain ( void )
{
	// Stop all jobs first
	Job_StopThreads();
	
	// Delete the renderer that works on the terrain's memory
	delete m_renderer;
	m_renderer	= NULL;
	// Delete the world properties
	delete m_patterns;
	m_patterns	= NULL;
	delete m_regions;
	m_regions	= NULL;

	// Delete the IO
	delete mIO;
	mIO	= NULL;

	// Free up the terrain's memory
	FreeTerraMemory();

	// Delete the sector information
	delete [] m_state_sectors;
}

void COctreeTerrain::TerrainUpdate ( void )
{
	//DebugGenerateNormals();
//	m_renderer->GenerateMesh();
}

void COctreeTerrain::Reinitialize ( void )
{
	//Terrain::write_lock writeLock( m_state_readlock );
	Terrain::write_lock writeLock( m_state_synchronizeLock );

	// Reset sectors
	State_ClearInformation();

	// Recreate IO tool
	delete mIO;
	mIO = new Terrain::COctreeIO( CGameSettings::Active()->GetTerrainSaveDir().c_str(), this );

	// Create the pattern and world simulators
	delete m_patterns;
	delete m_regions;
	m_patterns = new Terrain::CPatternController( this );
	m_regions  = new Terrain::CRegionController ( this );
}


// State accessors
void COctreeTerrain::SetSystemPaused ( const bool n_pauseSystem )
{
	if ( n_pauseSystem ) {
		Terrain::write_lock writeLock( m_state_readlock );
		m_systemPaused = true;
		if ( m_patterns ) {
			m_patterns->active = false;
		}
		if ( m_regions ) {
			m_regions->active = false;
		}
	}
	else {
		m_systemPaused = false;
		if ( m_patterns ) {
			m_patterns->active = true;
		}
		if ( m_regions ) {
			m_regions->active = true;
		}
	}
}
bool COctreeTerrain::GetSystemPaused ( void ) const
{
	return m_systemPaused;
}

void COctreeTerrain::SetStateFollowTarget ( const Vector3d& n_followTarget )
{
	//Terrain::write_lock writeLock( m_state_readlock );
	if ( !(VALID_FLOAT(n_followTarget.x)) || !(VALID_FLOAT(n_followTarget.y)) || !(VALID_FLOAT(n_followTarget.z)) )
	{ // TECHINCALLY INCORRECT. VALID_DOUBLE IS THE CORRECT CHECK.
		throw std::invalid_argument( "Out of range argument" );
	}
	m_state_followTarget = n_followTarget;
}


void COctreeTerrain::SetStateWorldOffset ( const Vector3d_d& n_followTarget )
{
	if ( !(VALID_FLOAT(n_followTarget.x)) || !(VALID_FLOAT(n_followTarget.y)) || !(VALID_FLOAT(n_followTarget.z)) )
	{ // TECHINCALLY INCORRECT. VALID_DOUBLE IS THE CORRECT CHECK.
		throw std::invalid_argument( "Out of range argument" );
	}
	m_state_followTarget = Vector3d(
		n_followTarget.x-m_state_centerPosition.x,
		n_followTarget.y-m_state_centerPosition.y,
		n_followTarget.z-m_state_centerPosition.z );
	Sectors_FollowTarget();
}
Vector3d_d COctreeTerrain::GetStateWorldCenterPosition ( void ) const
{
	return m_state_centerPosition;
}
RangeVector	COctreeTerrain::GetStateWorldCenterIndex ( void ) const
{
	return m_state_centerIndex;
}


// GetGenerator ( ) : returns the current generator being worked with
Terrain::CWorldGenerator* COctreeTerrain::GetGenerator ( void )
{
	return mGenerator;
}
// GetRenderer ( ) : return this terrain's renderer. This renderer can be edited.
// Typically used for disabling the renderer or editing it for special effects.
COctreeRenderer* COctreeTerrain::GetRenderer ( void )
{
	return m_renderer;
}
// GetPatternManager ( ) : return this terrain's pattern manager.
Terrain::CPatternController* COctreeTerrain::GetPatternManager ( void )
{
	return m_patterns;
}
// GetRegionManager ( ) : return this terrain's region manager.
Terrain::CRegionController* COctreeTerrain::GetRegionManager ( void )
{
	return m_regions;
}

// GetIO ( ) : return this terrain's IO handler.
const Terrain::COctreeIO* COctreeTerrain::GetIO ( void ) const
{
	return mIO;
}