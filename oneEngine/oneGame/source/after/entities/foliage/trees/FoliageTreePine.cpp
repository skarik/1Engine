
#include "FoliageTreePine.h"
#include "core/math/Math.h"
#include "core/math/Vector3d.h"
#include "engine/physics/raycast/Raycaster.h"

using std::vector;
using std::list;

FoliageTreePine::FoliageTreePine ( void )
	: CTreeBase()
{

}

void FoliageTreePine::GenerateTreeData ( void )
{
	vector<bool>		pointCloudSplittable;
	vector<Vector3d>	pointCloud;
	Vector3d position = transform.position;
	float height	  =	27.0f;
	// First, generate the target point cloud at the top
	for ( unsigned int i = 0; i < 18; i++ )
	{
		Vector3d newVect (0,0,0);

		newVect.x = noise->Get3D( (position.x+position.y+position.z)/32.0f - i * 0.08f, position.x/32.0f, i * 0.13f );
		newVect.y = noise->Get3D( position.y/32.0f, i * 0.13f, (position.x+position.y+position.z)/32.0f - i * 0.08f );
		newVect.z = 0;
		if ( newVect.sqrMagnitude() > 1 ) {
			newVect.normalize();
		}
		newVect *= 15.0f;

		newVect.z = noise->Get3D( i * 0.13f, (position.x+position.y+position.z)/32.0f - i * 0.08f, position.z/32.0f );
		newVect.z *= 1.5f;

		ftype sampler = noise->Get3D( (position.x+position.y+position.z)*0.21f + i * 0.27f, position.z/17.0f, 22.13f + i * 0.43f );
		if ( sampler < 0.05f ) {
			newVect.z += height*0.4f;
		}
		else if ( sampler < 0.35f ) {
			newVect.z += height*0.7f;
		}
		else {
			newVect.z += height*0.9f;
		}

		pointCloud.push_back( newVect );
		pointCloudSplittable.push_back( true );
	}

	// Now, starting with a single ray in the queue
	Ray start;
	start.dir = Vector3d( 0,0,2 );
	start.pos = Vector3d( 0,0,0 );

	TreePart* startPart = new TreePart;
	startPart->parent = NULL;
	startPart->shape = start;
	startPart->userData = -1;
	list<TreePart*> rayList;
	rayList.push_front( startPart );

	// Set the travel direction branch split threshold
	float splitThreshold = 0.65f;

	// Set the initial trunk radius
	float branchRadius = 1.0f;

	// Count iterations
	int iterationCount = 0;

	// Continue iterating while the queue is not empty
	while ( !rayList.empty() )
	{
		iterationCount += 1;

		// Get the current ray
		TreePart* currentPart = rayList.back();
		rayList.pop_back();
		Ray current = currentPart->shape;

		// Set the current part's size
		currentPart->size = branchRadius + Math.Sqr<ftype>( std::max<int>( 4 - iterationCount, -1 )*0.2f ) * std::min<ftype>( 1.0f, std::max<ftype>( 0, (8-iterationCount)*0.5f ) );
		currentPart->strength = branchRadius;

		// Decrement the radius for this bit
		branchRadius *= 0.97f;

		// Add current branch to part list
		part_list.push_back( currentPart );
		
		// Move to the next position
		current.pos += current.dir;

		// Increase the split threshold
		splitThreshold = std::min<ftype>( 0.82f,splitThreshold+0.04f );

		// Go through all the points in the cloud and compute attraction
		Vector3d newDir = current.dir * 5.0f;
		if ( currentPart->userData == -1 ) {
			newDir *= 10.0f;
		}
		bool branchFree = true;
		for ( unsigned int i = 0; i < pointCloud.size(); i++ )
		{
			Vector3d toDir = pointCloud[i]-current.pos;
			float distSqr = toDir.sqrMagnitude();
			Vector3d attraction = (toDir / distSqr)*20.0f;

			// Add attraction to new dir, with some weights based on partial known targets
			if ( currentPart->userData == -1 ) {
				if ( attraction.z > 0.1 ) {
				//if ( pointCloudSplittable[i] ) {
					newDir += attraction;
				//}
				/*else {
					newDir += attraction*0.6f;
				}*/
				}
			}
			else if ( currentPart->userData == (signed)i ) {
				newDir += attraction;
			}
			else {
				newDir += attraction*0.08f;
			}
		}
		// Slow down vertical movement near the target
		if ( currentPart->userData != -1 ) {
			newDir.z *= std::max<ftype>(0.1f, std::min<ftype>( 1, (fabs(pointCloud[currentPart->userData].z-current.pos.z)/height)*6.0f ) );
		}
		// Normalize the dir
		newDir = newDir.normal() * 2.0f;
		// Split off
		for ( unsigned int i = 0; i < pointCloud.size(); i++ )
		{
			Vector3d toDir = pointCloud[i]-current.pos;
			float distSqr = toDir.sqrMagnitude();
			Vector3d attraction = (toDir / distSqr)*20.0f;

			Vector3d nextDir = newDir + attraction;

			// Check here to see if the branch should end
			if ( currentPart->userData != -1 && distSqr < 9 ) // Stop if too close to the target point
			{
				branchFree = false;
			}

			// If we can split towards the current point
			if ( pointCloudSplittable[i] )
			{
				// If the attraction and the current travel direction are too different, then split off
				//if ( attraction.normal().dot( nextDir.normal() ) < splitThreshold )
				ftype splitValue = attraction.normal().dot( newDir.normal() );
				if (( currentPart->userData != -1 && splitValue > 0.7f && splitValue < splitThreshold )||
					( currentPart->userData == -1 && splitValue < -0.1f && attraction.z < 0 ))
				{
					// Then we can't split towards this point anymore
					pointCloudSplittable[i] = false;
					// And we add a new travel dir
					Ray newRay = current;
					newRay.dir = nextDir.normal()*1.8f; // do not normalize to get the 'magnet' trees

					TreePart* newPart = new TreePart;
					newPart->shape = newRay;
					newPart->parent = currentPart;
					newPart->userData = i;
					rayList.push_front( newPart );
				}
			}
		}
		
		// Stops branch if too high or too far away from center
		//if ( current.pos.sqrMagnitude() > Math.Sqr( height*1.1f + 2.3f ) ) 
		if ( (currentPart->size < 0.1f) || (current.pos.z > height*1.1f + 1.0f) || (Vector2d(current.pos.x,current.pos.y).sqrMagnitude() > Math.Sqr( 4.9f )) ) 
		{
			branchFree = false;
		}

		// If the branch should not end, continue
		if ( branchFree )
		{
			// Go through all the points in the rayList and compute repulsion
			for ( list<TreePart*>::iterator it = rayList.begin(); it != rayList.end(); it++ )
			{
				Vector3d toDir = ((*it)->shape.pos+(*it)->shape.dir)-(current.pos+current.dir);
				float distSqr = toDir.sqrMagnitude(); // If there's no attraction from the leaves, this becomes zero, causing a divZERO exception
				Vector3d repulsion = (toDir / distSqr)*0.4f;
				newDir -= repulsion;
			}
			// Normalize the dir
			newDir = newDir.normal() * 1.8f;

			// Take the new dir
			current.dir = newDir;

			// Add the ray to the list
			TreePart* newPart = new TreePart;
			newPart->shape = current;
			newPart->parent = currentPart;
			newPart->userData = currentPart->userData;
			rayList.push_front( newPart );

			// Random chance to add a leaf on a branch
			if ( branchRadius < 0.38f )
			{
				if ( noise->Get( position.x*14.6f + newDir.x, newDir.y*newDir.z*32.0f + part_list.size()*0.7f ) > 0.2f )
				{
					// Add a leaf at the mid of the branch
					leaf_cloud.push_back( current.pos + current.dir*0.5f );
					leaf_branch.push_back( currentPart );
				}
			}
		}
		else
		{
			// Add a leaf at the end of the branch
			leaf_cloud.push_back( current.pos + current.dir );
			leaf_branch.push_back( currentPart );
		}
	}
}