
// Includes
#include "CWeatherCloud.h"
#include "CWeatherSimulator.h"
#include "after/physics/wind/WindMotion.h"
#include "core/time/time.h"
#include "CCloudMaster.h"

// Cloud constructor
CWeatherCloud::CWeatherCloud ( Vector3d const& vInPos, Vector3d const& vInSize )
{
	position= vInPos;
	size	= vInSize;
	velocity= Vector3d( 0,0,0 );

	skip = false;

	// Add to cloud list
	CCloudMaster::pActive->AddCloud( this );
}
// Weather cloud
CWeatherCloud::~CWeatherCloud ( void )
{
	// Remove from cloud list
	CCloudMaster::pActive->RemoveCloud( this );
}

#include "renderer/debug/CDebugDrawer.h"

void CWeatherCloud::Update ( void )
{
	//DebugD::DrawLine( position-size/2, position+size/2 );

	// killbonus calc
	ftype killbonus;
	if ( velocity.sqrMagnitude() > 1.0f )
		killbonus = 10.0f / velocity.sqrMagnitude();
	else
		killbonus = 10.0f;

	// Destroy over time
	size.z -= Time::deltaTime * size.z * 0.23f * killbonus;
	size.x -= Time::deltaTime * size.x * 0.04f * killbonus;
	size.y -= Time::deltaTime * size.y * 0.04f * killbonus;
	if ( size.sqrMagnitude() < 12.0f ) {
		//DeleteObjectDelayed( this, 0.01f );
		skip = true;
	}

	// Set a maximum size
	if ( size.z > 212.0f )
		size.z = 212.0f;
	if ( size.x > 160.0f )
		size.x = 160.0f;
	if ( size.y > 160.0f )
		size.y = 160.0f;

	// Add to velocity
	velocity += WindMotion.GetFlowField( position ) * Time::deltaTime * 36.0f;

	// Add drag?
	velocity -= velocity * Time::deltaTime * 0.15f;

	// Move position
	position += velocity * Time::deltaTime;
}