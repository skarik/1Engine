// CVoxelTerrainGeneration2.cpp
// Version 4 and above of terrain generation implementation.
// Version 8 and above of generation request ordering.

//
#include "CVoxelTerrain.h"
#include "CTerrainGenerator.h"
#include "Math.h"
#include "CCamera.h"
//


/// == Freaking Pubs ==
char	CVoxelTerrain::TerraGen_pub_GetType	( Vector3d const& blockPosition )
{
	return generator->TerraGen_pub_GetType( blockPosition );
}
char	CVoxelTerrain::TerraGen_pub_GetBiome( Vector3d const& blockPosition )
{
	return generator->TerraGen_pub_GetBiome( blockPosition ); 
}
ftype	CVoxelTerrain::TerraGen_pub_GetElevation( Vector3d const& blockPosition )
{
	return generator->TerraGen_pub_GetElevation( blockPosition ); 
}

// == Terrain Generation List Editors ==
// This locks the generation thread, then adds or pushes down a request for terrain loading
void CVoxelTerrain::AddGenerationRequest ( const RangeVector & newRequestPosition, const bool newRequestCheckfile )
{
	// Lock the list while checking list
	mutex::scoped_lock local_lock( mtGenerationListGaurd );

	// Check for the request
	bool exists = false;
	vector<sGenerationRequest>::iterator found_iterator;
	for ( vector<sGenerationRequest>::iterator it = vGenerationRequests.begin(); it != vGenerationRequests.end(); ++it )
	{
		if ( it->position == newRequestPosition )
		{
			exists = true;
			found_iterator = it;
		}
	}

	// Add the request if it doesn't exist
	if ( !exists )
	{
		sGenerationRequest newRequest;
		newRequest.position = newRequestPosition;
		newRequest.checkfile = newRequestCheckfile;
		vGenerationRequests.push_back( newRequest );
	}
	else
	{
		// Move the request down if it does
		vGenerationRequests.erase( found_iterator );

		sGenerationRequest newRequest;
		newRequest.position = newRequestPosition;
		newRequest.checkfile = (newRequestCheckfile&&newRequest.checkfile);
		vGenerationRequests.push_back( newRequest );
	}
}

// Ordering struct
struct CVoxelTerrain::tGenerationListComparator
{
	RangeVector root_pos;
	Vector3d	camera_face_dir;
	bool operator() ( CVoxelTerrain::sGenerationRequest request_i, CVoxelTerrain::sGenerationRequest request_j )
	{
		// return true if i<j
		// i is smaller if needs to be loaded first
		// if i needs to be loaded first, return true

		int weighti = 0, weightj = 0;

		// Calculate delta to root
		RangeVector deltai ( request_i.position.x-root_pos.x, request_i.position.y-root_pos.y, request_i.position.z-root_pos.z );
		RangeVector deltaj ( request_j.position.x-root_pos.x, request_j.position.y-root_pos.y, request_j.position.z-root_pos.z );

		// Bigger weight, the less likely.
		weighti += Math.Sqr<rangeint>(deltai.x)+Math.Sqr<rangeint>(deltai.y)+Math.Sqr<rangeint>(deltai.z*2);
		weightj += Math.Sqr<rangeint>(deltaj.x)+Math.Sqr<rangeint>(deltaj.y)+Math.Sqr<rangeint>(deltaj.z*2);

		// With big weights, add directional
		/*if ( weighti > 4 ) {
			weighti += (int)( 0.2f * weighti * (1-camera_face_dir.dot(Vector3d(deltai.x,deltai.y,deltai.z).normalize())));
		}
		if ( weightj > 4 ) {
			weightj += (int)( 0.2f * weightj * (1-camera_face_dir.dot(Vector3d(deltaj.x,deltaj.y,deltaj.z).normalize())));
		}*/

		// Return true if i<j, thus i more likely to load first
		return weighti < weightj;
	}
};

// This locks the generation thread, then reorders requests.
void CVoxelTerrain::PruneGenerationRequests ( void )
{
	// Lock the list while editing list
	mutex::scoped_lock local_lock( mtGenerationListGaurd );

	//
	tGenerationListComparator LoadingListComparator;
	LoadingListComparator.root_pos = root_position;
	/*if ( CCamera::activeCamera ) {
		camera_face_dir = CCamera::activeCamera->transform.Forward();
	}*/
	sort( vGenerationRequests.begin(), vGenerationRequests.end(), LoadingListComparator );
}
