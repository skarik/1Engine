
#include "CCloudMaster.h"
#include "CWeatherCloud.h"
#include <algorithm>

CCloudMaster* CCloudMaster::pActive = NULL;

void CCloudMaster::Init ( void )
{
	pActive = new CCloudMaster;
}
void CCloudMaster::Free ( void )
{
	if ( pActive )
	{
		CWeatherCloud* targetCloud;
		/*for ( vector<CWeatherCloud*>::iterator cloud = pActive->cloudList.begin(); cloud != pActive->cloudList.end(); ++cloud ) {
			targetCloud = (*cloud);
			delete targetCloud;
		}*/
		while ( !pActive->cloudList.empty() ) {
			targetCloud = pActive->cloudList.back();
			delete targetCloud;
			pActive->cloudList.pop_back();
		}
		delete pActive;
	}
	pActive = NULL;
}

void CCloudMaster::AddCloud ( CWeatherCloud* cloud )
{
	cloudList.push_back( cloud );
}
void CCloudMaster::RemoveCloud ( CWeatherCloud* cloud )
{
	std::vector<CWeatherCloud*>::iterator it;
	it = find( cloudList.begin(), cloudList.end(), cloud );
	if ( it != cloudList.end() )
		cloudList.erase( it );
	else
		std::cout << "BAD CLOUD!" << std::endl << "FIX IT!" << std::endl;
}

void CCloudMaster::UpdateClouds ( void )
{
	for ( std::vector<CWeatherCloud*>::iterator cloud = cloudList.begin(); cloud != cloudList.end(); ++cloud )
	{
		(*cloud)->Update();
	}
	for ( std::vector<CWeatherCloud*>::iterator cloud = cloudList.begin(); cloud != cloudList.end(); ++cloud )
	{
		if ( !(*cloud)->skip )
		{
			// Take the current cloud, compare it with all other clouds
			for ( std::vector<CWeatherCloud*>::iterator cloud_ot = cloudList.begin(); cloud_ot != cloudList.end(); ++cloud_ot )
			{
				if (( (*cloud_ot) != (*cloud) )&&( !(*cloud_ot)->skip ))
				{
					// check for collide
					Vector3d dist = ((*cloud)->position)-((*cloud_ot)->position);
					Vector3d sizedif = (((*cloud)->size)+((*cloud_ot)->size)) * 0.5f;

					if ( dist.sqrMagnitude() < sizedif.sqrMagnitude() )
					{
						CWeatherCloud* bigger;
						CWeatherCloud* smaller;
						// take the bigger one
						if ( (*cloud)->size.sqrMagnitude() > (*cloud_ot)->size.sqrMagnitude() )
						{
							bigger = *cloud;
							smaller = *cloud_ot;
						}
						else
						{
							bigger = *cloud_ot;
							smaller = *cloud;
						}
						// enlargen it
						bigger->size += smaller->size * 0.8f;
						bigger->position.x = sqrt( bigger->position.x * smaller->position.x );
						bigger->position.y = sqrt( bigger->position.y * smaller->position.y );
						bigger->position.z = sqrt( bigger->position.z * smaller->position.z );

						// destroy smaller one
						smaller->skip = true;
						//smaller->DeleteObjectDelayed( smaller, 0.01f );
					}
				}
			}
		}
	}
	for ( std::vector<CWeatherCloud*>::iterator cloud = cloudList.begin(); cloud != cloudList.end(); )
	{
		if ( (*cloud)->skip )
		{
			delete (*cloud);
			cloud = cloudList.begin();
		}
		else
		{
			cloud++;
		}
	}
}

// Get the cloud density
ftype CCloudMaster::GetCloudDensity ( const Vector3d & position )
{
	Vector3d distance;
	ftype	acculmulator = 0;
	ftype	add_value = 0;
	ftype	max_value = 0.01f;
	for ( std::vector<CWeatherCloud*>::iterator cloud = cloudList.begin(); cloud != cloudList.end(); ++cloud )
	{
		distance = position - (*cloud)->position;
		add_value = distance.sqrMagnitude();
		if ( add_value < sqr( 320 ) )
		{
			add_value = (*cloud)->size.sqrMagnitude()/add_value;
			acculmulator += std::min<ftype>( add_value, 1 );
			max_value += 0.6f;
		}
	}
	//cout << (( acculmulator/max_value )-0.2f)*1.25f << endl;
	return (( acculmulator/max_value )-0.2f)*1.25f;
}
