
// Includes
#include "WindMotion.h"

// Looks at these objects
#include "after/terrain/VoxelTerrain.h"
#include "physical/physics/CPhysics.h"
#include "after/entities/world/environment/weather/CWeatherSimulator.h"
#include "after/entities/world/environment/weather/CCloudMaster.h"

// Global instance
CWindMotion WindMotion;

// Constructor
CWindMotion::CWindMotion ( void )
	: CBaseWindTester()
{
	//pActiveSim = NULL;
}
// Get active simulation instance
bool CWindMotion::GetSimulationInstance ( void )
{
	if ( pActiveSim == NULL )
	{
		pActiveSim = CWeatherSimulator::Get();
		if ( pActiveSim == NULL )
			return false;
		else
			return true;
	}
	else
	{
		return true;
	}
}

// Not yet implemented!
ftype CWindMotion::GetHumidity		( const Vector3d& pos ) { throw Core::NotYetImplementedException(); return 0; }
ftype CWindMotion::GetHumidityFast	( const Vector3d& pos ) { throw Core::NotYetImplementedException(); return 0; }
ftype CWindMotion::GetTemperature  ( const Vector3d& pos ) { throw Core::NotYetImplementedException(); return 0; }

// Get the interpolated flow field for the current value
Vector3d	CWindMotion::GetFlowField	( const Vector3d& pos )
{
	Vector3d result ( 0,0,0 );

	if ( GetSimulationInstance() )
	{
		// First need to convert pos to cellspace
		// This is similar to converting to boobspace
		// But the origin is offset, ie (0,0,0) -> (size.x/2, size.y/2, size.z/4)
		Vector3d cellpos = (pos / 64.0f) + Vector3d( (ftype)pActiveSim->size.x/2, (ftype)pActiveSim->size.y/2, (ftype)pActiveSim->size.z/4 );
		cellpos -= Vector3d( (ftype)pActiveSim->root_position.x,(ftype)pActiveSim->root_position.y,(ftype)pActiveSim->root_position.z );

		// Now, frac(coord) = 0.5 corresponds to the center of the cell
		// If below 0.5 in a direction, then we need to sample down in that direction
		// If above 0.5 in a direction, then we need to sample up in that direction
		// So, eight samples
		sWeatherSimCell samples [8];
		short x, y, z;
		x = short( cellpos.x );
		y = short( cellpos.y );
		z = short( cellpos.z );
		if ( cellpos.x < 0.5f )
		{
			x -= 1;
		}
		if ( cellpos.y < 0.5f )
		{
			y -= 1;
		}
		if ( cellpos.z < 0.5f )
		{
			z -= 1;
		}

		// Check for the sample area is in range
		if (( x > 0 )&&( x < pActiveSim->size.x-1 )&&( y > 0 )&&( y < pActiveSim->size.y-1 )&&( z > 0 )&&( z < pActiveSim->size.z-1 ))
		{
			samples[7] = pActiveSim->get( pActiveSim->data, x+1,y+1,z+1 );
			samples[6] = pActiveSim->get( pActiveSim->data, x,y+1,z+1 );

			samples[5] = pActiveSim->get( pActiveSim->data, x+1,y,z+1 );
			samples[4] = pActiveSim->get( pActiveSim->data, x,y,z+1 );

			samples[3] = pActiveSim->get( pActiveSim->data, x+1,y+1,z );
			samples[2] = pActiveSim->get( pActiveSim->data, x,y+1,z );

			samples[1] = pActiveSim->get( pActiveSim->data, x+1,y,z );
			samples[0] = pActiveSim->get( pActiveSim->data, x,y,z );

			// Now do trilinear interpolation
			// need to get the interpolation coordinates
			cellpos += Vector3d( 0.5f,0.5f,0.5f );
			ftype dummy;
			cellpos.x = modf( cellpos.x, &dummy );
			cellpos.y = modf( cellpos.y, &dummy );
			cellpos.z = modf( cellpos.z, &dummy );

			// perform the interpolation on x
			Vector3d tempVal [4];
			tempVal[0] = ( samples[0].dir + (samples[1].dir-samples[0].dir)*cellpos.x );
			tempVal[1] = ( samples[2].dir + (samples[3].dir-samples[2].dir)*cellpos.x );
			tempVal[2] = ( samples[4].dir + (samples[5].dir-samples[4].dir)*cellpos.x );
			tempVal[3] = ( samples[6].dir + (samples[7].dir-samples[6].dir)*cellpos.x );
			// perform the interpolation on y
			tempVal[0] = ( tempVal[0] + (tempVal[1]-tempVal[0])*cellpos.y );
			tempVal[2] = ( tempVal[2] + (tempVal[3]-tempVal[2])*cellpos.y );
			// perform the interpolation on z
			result = ( tempVal[0] + (tempVal[2]-tempVal[0])*cellpos.z );

			// That's the value, whoop
			//cout << result << endl;
		}
		else
		{
			//cout << "No range" << endl;
		}
	}

	// First we check the terrain
	/*if (( !CVoxelTerrain::terrainList.empty() )&&( CVoxelTerrain::terrainList[0] != NULL ))
	{
		BlockInfo blockInfo;
		CVoxelTerrain::terrainList[0]->GetBlockInfoAtPosition( pos, blockInfo );

		// If the block is water, then position is in water
		if ( blockInfo.block == EB_WATER )
		{
			return true;
		}
	}


	// Then we ask the volume manager for the list of volumes

	// If the list isn't empty
		// Loop through all the water volumes for this point


	// If we still can't find anything that we're in, then we're not in water!
	return false;*/
	
	return result;
}


// Get the interpolated flow field for the current value
Vector3d	CWindMotion::GetFlowFieldFast( const Vector3d& pos )
{
	Vector3d result ( 0,0,0 );

	if ( GetSimulationInstance() )
	{
		// First need to convert pos to cellspace
		// This is similar to converting to boobspace
		// But the origin is offset, ie (0,0,0) -> (size.x/2, size.y/2, size.z/4)
		Vector3d cellpos = (pos / 64.0f) + Vector3d( (ftype)pActiveSim->size.x/2, (ftype)pActiveSim->size.y/2, (ftype)pActiveSim->size.z/4 );
		cellpos -= Vector3d( (ftype)pActiveSim->root_position.x,(ftype)pActiveSim->root_position.y,(ftype)pActiveSim->root_position.z );

		// Since 0.5 corresponds to the center of the cell, all one
		// needs to do is cast the values to an integer
		short x, y, z;
		x = short( cellpos.x );
		y = short( cellpos.y );
		z = short( cellpos.z );

		// Check for the sample area is in range
		if (( x > 0 )&&( x < pActiveSim->size.x-1 )&&( y > 0 )&&( y < pActiveSim->size.y-1 )&&( z > 0 )&&( z < pActiveSim->size.z-1 ))
		{
			sWeatherSimCell simCell = pActiveSim->get( pActiveSim->data, x,y,z );
			result = simCell.dir;
		}
	}

	return result;
}

ftype		CWindMotion::GetTemperatureFast ( const Vector3d& pos )
{
	if ( GetSimulationInstance() )
	{
		// First need to convert pos to cellspace
		// This is similar to converting to boobspace
		// But the origin is offset, ie (0,0,0) -> (size.x/2, size.y/2, size.z/4)
		Vector3d cellpos = (pos / 64.0f) + Vector3d( (ftype)pActiveSim->size.x/2, (ftype)pActiveSim->size.y/2, (ftype)pActiveSim->size.z/4 );
		cellpos -= Vector3d( (ftype)pActiveSim->root_position.x,(ftype)pActiveSim->root_position.y,(ftype)pActiveSim->root_position.z );

		// Since 0.5 corresponds to the center of the cell, all one
		// needs to do is cast the values to an integer
		short x, y, z;
		x = short( cellpos.x );
		y = short( cellpos.y );
		z = short( cellpos.z );

		// Check for the sample area is in range
		if (( x > 0 )&&( x < pActiveSim->size.x-1 )&&( y > 0 )&&( y < pActiveSim->size.y-1 )&&( z > 0 )&&( z < pActiveSim->size.z-1 ))
		{
			sWeatherSimCell simCell = pActiveSim->get( pActiveSim->data, x,y,z );
			return simCell.temperature;
		}
	}

	return 1.0f;
}

ftype		CWindMotion::GetCloudDensity ( const Vector3d& pos )
{
	if ( CCloudMaster::pActive != NULL ) {
		return CCloudMaster::pActive->GetCloudDensity( pos );
	}
	return 0;
}

// Get the weather for the cell at the given position
uchar	CWindMotion::GetWeather ( const Vector3d& pos )
{
	if ( GetSimulationInstance() )
	{
		// First need to convert pos to cellspace
		// This is similar to converting to boobspace
		// But the origin is offset, ie (0,0,0) -> (size.x/2, size.y/2, size.z/4)
		Vector3d cellpos = (pos / 64.0f) + Vector3d( (ftype)pActiveSim->size.x/2, (ftype)pActiveSim->size.y/2, (ftype)pActiveSim->size.z/4 );
		cellpos -= Vector3d( (ftype)pActiveSim->root_position.x,(ftype)pActiveSim->root_position.y,(ftype)pActiveSim->root_position.z );

		// Since 0.5 corresponds to the center of the cell, all one
		// needs to do is cast the values to an integer
		short x, y, z;
		x = short( cellpos.x );
		y = short( cellpos.y );
		z = short( cellpos.z );

		// Check for the sample area is in range
		if (( x > 0 )&&( x < pActiveSim->size.x-1 )&&( y > 0 )&&( y < pActiveSim->size.y-1 )&&( z > 0 )&&( z < pActiveSim->size.z-1 ))
		{
			sWeatherSimCell simCell = pActiveSim->get( pActiveSim->data, x,y,z );
			return simCell.weather;
		}
	}

	return Terrain::WTH_NONE;
}


// Sets the weather at the given position. Forces the weather system to add bullshit values.
bool CWindMotion::SetWeather ( const Vector3d& pos, const uchar weather )
{
	if ( GetSimulationInstance() )
	{
		// First need to convert pos to cellspace
		// This is similar to converting to boobspace
		// But the origin is offset, ie (0,0,0) -> (size.x/2, size.y/2, size.z/4)
		Vector3d cellpos = (pos / 64.0f) + Vector3d( (ftype)pActiveSim->size.x/2, (ftype)pActiveSim->size.y/2, (ftype)pActiveSim->size.z/4 );
		cellpos -= Vector3d( (ftype)pActiveSim->root_position.x,(ftype)pActiveSim->root_position.y,(ftype)pActiveSim->root_position.z );

		// Since 0.5 corresponds to the center of the cell, all one
		// needs to do is cast the values to an integer
		short x, y, z;
		x = short( cellpos.x );
		y = short( cellpos.y );
		z = short( cellpos.z );

		// Check for the sample area is in range
		if (( x > 0 )&&( x < pActiveSim->size.x-1 )&&( y > 0 )&&( y < pActiveSim->size.y-1 )&&( z > 0 )&&( z < pActiveSim->size.z-1 ))
		{
			sWeatherSimCell& simCell = pActiveSim->get( pActiveSim->data, x,y,z );
			simCell.weather = weather;
			simCell.humidity = 4;
			simCell.pressure = 0.5;

			//simCell.
			// TODO: Create bullshit values

			return true;
		}
	}
	return false;
}