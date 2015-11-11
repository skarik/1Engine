
#include "core/math/Math.h"
#include "core/math/Math3d.h"
#include "core/math/noise/SimplexNoise.h"
#include "core/math/random/Random.h"

#include "CWorldGen_Terran.h"

using namespace std;
using namespace Terrain;

#include "renderer/logic/model/CModel.h"
CWorldGen_Terran::CWorldGen_Terran ( const int continentCount )
	: CWorldGenerator(), m_continentCount(continentCount)
{
	noise = NULL;
	m_worldSize = Vector2d_d( 0,0 );
	m_worldDepth = 0;

	m_model = new CModel( "models/test/boxes.FBX" );
	((CModel*)m_model)->SetVisibility(false);
}	
	
CWorldGen_Terran::~CWorldGen_Terran( void )
{
	Cleanup();

	delete ((CModel*)m_model);
	m_model = NULL;
}

void CWorldGen_Terran::Initialize ( void )
{
	cout << "Initializing terrain with seed " << Seed() << endl;
	{
		Real calculatedSeedValue = sqrt(double(Seed()))*0.2 + Seed()*0.01;
		Real seedMod = std::max<Real>( fmod( Seed(), 250000 ), 1 );
		calculatedSeedValue = fmod( calculatedSeedValue, seedMod );
		cout << "  used float seed is " << calculatedSeedValue << endl;
		noise = new SimplexNoise( 2, 1, 1, calculatedSeedValue );
		cout << "Sample values: " << noise->Get(0,0) << " " << noise->Get(0,1) << " " << noise->Get(2,2) << " " << noise->Get(-4,-4) << endl;
	}
	// Set the world size
	m_worldSize = Vector2d_d( 1600000, 1600000 );
	m_worldDepth = 800000;

	// Create continents
	Init_CreateContinents();

	// Create the noise sample buffers
	m_buf_general.CreateBuffer( noise, 3.1f, 3.1f );
	m_buf_general_noise.CreateBuffer( noise, 11.2f, 11.3f );

	// Clear dungeon flag
	m_dungeonIO_flag.clear();
}
void CWorldGen_Terran::Cleanup ( void )
{
	// Delete noise
	delete noise;
	noise = NULL;

	// Delete continents
	m_continentPositions.clear();
}

bool CWorldGen_Terran::GetInland ( const Vector2d_d& worldposition )
{
	return (GetLandmassity( worldposition, true ) >= 0);
}
Vector2d_d CWorldGen_Terran::GetWorldSize ( void )
{
	return m_worldSize;
}
RangeVector CWorldGen_Terran::GetWorldSectorSize ( void ) 
{
	return RangeVector( m_worldSize.x/64, m_worldSize.y/64, m_worldDepth/64 );
}

// Gameplay generation queries
Vector3d_d CWorldGen_Terran::GetSpawnPoint ( const int n_continent, const int n_seed_direction, const int n_index )
{
	return Vector3d_d( 0,0,GetLandmassity(Vector2d_d(0,0),false) + 3 );
	// Input n_continent is the selection of continent based on size. Thus, continents need to be sorted.
	struct ContinentInfo {
		Vector2d_d position;
		Vector2d_d size;
	};
	struct ContinentInfo_Comparator {
		bool operator() (ContinentInfo i,ContinentInfo j) {
			return i.size.sqrMagnitude() < j.size.sqrMagnitude();
		}
	} comparator;
	vector<ContinentInfo> continents;
	for ( uint i = 0; i < m_continentSizes.size(); ++i ) {
		ContinentInfo newContinent;
		newContinent.position	= m_continentPositions[i];
		newContinent.size		= m_continentSizes[i];
		continents.push_back( newContinent );
	}
	std::sort( continents.begin(), continents.end(), comparator );

	// Choose the continent to work with
	ContinentInfo& m_continent = continents[n_continent];

	// Starting with the a seed direction, move along the continent looking for a safe spot.
	Real_d offsetAngle = n_seed_direction * 0.1047;
	Real_d distanceOffset = 150000+130000; // Weighted (sort of) towards the coast
	int failures = 0;
	int successes = 0;
	bool inland = false;

	Vector2d_d samplePosition;
	do
	{
		// Calculate sample position based on circular movement
		samplePosition.x = m_continent.position.x + cos( offsetAngle ) * distanceOffset;
		samplePosition.y = m_continent.position.y + sin( offsetAngle ) * distanceOffset;

		// Increment failure count to force noise offset
		failures += 1;

		// Check if this position is inland
		inland = GetInland( samplePosition );

		if ( !inland ) { // If not inland, bring it inwards
			distanceOffset -= 500;
		}
		else {
			// If it is inland, eventually add safty checks. For now, just count it as a success.
			successes += 1;
		}

		// Offset the point
		offsetAngle		+= noise->Get3D( failures*0.1f, samplePosition.x*0.0002f, samplePosition.y*0.0002f ) * 0.5;
		distanceOffset	+= noise->Get3D( failures*0.2f, samplePosition.y*0.0001f, samplePosition.x*0.0001f ) * 1000;
		if ( distanceOffset > 150000+130000 ) {
			distanceOffset  = 150000+130000;
		}
	}
	while ( (!inland) || (successes < n_index) );  // Repeat the search method n_index more times.

	// samplePosition is the current found value.
	Real_d landmassity = GetLandmassity( samplePosition, false );
	return Vector3d_d( samplePosition.x, samplePosition.y, landmassity );
}
