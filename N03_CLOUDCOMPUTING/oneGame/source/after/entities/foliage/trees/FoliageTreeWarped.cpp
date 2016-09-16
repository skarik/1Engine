
#include "FoliageTreeWarped.h"
#include "core/math/Math.h"
#include "core/math/Vector3d.h"
#include "engine/physics/raycast/Raycaster.h"

using std::list;
using std::vector;

FoliageTreeWarped::FoliageTreeWarped ( void )
	: CTreeBase()
{

}

void FoliageTreeWarped::GenerateTreeData ( void )
{
	vector<bool>		pointCloudSplittable;
	vector<Vector3d>	pointCloud;
	Vector3d position = transform.position;
	float height	  =	21.0f;
	// First, generate the target point cloud
	for ( unsigned int i = 0; i < 14; ++i )
	{
		Vector3d newVect (0,0,0);
		newVect.x = noise->Get3D( (position.x+position.y+position.z)/32.0f - i * 0.38f, position.x/32.0f, i * 0.43f );
		newVect.y = noise->Get3D( position.y/32.0f, i * 0.43f, (position.x+position.y+position.z)/32.0f - i * 0.38f );
		newVect.z = noise->Get3D( i * 0.43f, (position.x+position.y+position.z)/32.0f - i * 0.38f, position.z/32.0f )+0.5f;
		newVect.x *= 1-newVect.z;
		newVect.y *= 1-newVect.z;
		newVect *= 13.0f;
		newVect.z += height * 0.5f;
		pointCloud.push_back( newVect );
		pointCloudSplittable.push_back( true );
	}

	// Now, starting with a single ray in the queue
	Ray start;
	start.dir = Vector3d( 0,0,1.3f );
	start.pos = Vector3d( 0,0,0 );

	TreePart* startPart = new TreePart;
	startPart->parent = NULL;
	startPart->shape = start;
	startPart->userData = -1;
	list<TreePart*> rayList;
	rayList.push_front( startPart );

	// Set the travel direction branch split threshold
	float splitThreshold = 0.1f;

	// Set the initial trunk radius
	float branchRadius = 1.2f;

	// Count iterations
	int iterationCount = 0;

	// Continue iterating while the queue is not empty
	while ( !rayList.empty() )
	{
		// Get the current ray
		TreePart* currentPart = rayList.back();
		rayList.pop_back();
		Ray current = currentPart->shape;

		// Set the current part's size
		currentPart->size = branchRadius + Math.Sqr<ftype>( std::max<int>( 4 - iterationCount, -1 )*0.2f ) * std::min<ftype>( 1.0f, std::max<ftype>( 0, (8-iterationCount)*0.5f ) );
		currentPart->strength = branchRadius;

		// Decrement the radius for this bit
		branchRadius *= 0.94f;

		// Add current branch to part list
		part_list.push_back( currentPart );
		
		// Move to the next position
		current.pos += current.dir;

		// Increase the split threshold
		splitThreshold = std::min<ftype>( 0.9f,splitThreshold+0.04f );

		// Go through all the points in the cloud and compute attraction
		Vector3d newDir = current.dir;
		bool branchFree = true;
		for ( unsigned int i = 0; i < pointCloud.size(); i++ )
		{
			Vector3d toDir = pointCloud[i]-current.pos;
			float distSqr = toDir.sqrMagnitude();
			Vector3d attraction = (toDir / distSqr)*20.0f;

			// If the current branch is going up, then there's only attraction going up
			if ( current.dir.z > 0 )
			{
				if ( toDir.z > 0 )
					newDir += attraction;
			}
			else // Opposite for a branch going down
			{
				if ( toDir.z < 0 )
					newDir += attraction;
			}
		}
			
			// However, there's only attraction if we're higher than the point
			/*if ( toDir.z < 0 )
				newDir += attraction;
			else
				newDir += attraction * 0.03f;

			// Check here to see if the branch should end
			if (( distSqr < sqr( 1.2f ) )&&( Vector3d( current.pos.x, current.pos.y, 0 ).sqrMagnitude() > sqr( 1.2f ) )&&( current.dir.z < 0.5f )) // Stop if too close to the point
			{
				branchFree = false;
			}
			else if ( current.pos.sqrMagnitude() > sqr( height*1.1f + 1.5f ) ) // Stops if too high
			{
				branchFree = false;
			}*/
		// Check here to see if the branch should end
		if ( current.pos.sqrMagnitude() > sqr( height*1.1f + 1.5f ) ) // Stops if too high
		{
			branchFree = false;
		}

		// Now, a second pass for the branches off the main vertical.
		Vector3d tempDir = newDir;
		for ( unsigned int i = 0; i < pointCloud.size(); i++ )
		{
			Vector3d toDir = pointCloud[i]-current.pos;
			float distSqr = toDir.sqrMagnitude();
			Vector3d attraction = (toDir / distSqr)*20.0f;

			// Now, a second pass.
			if ( current.dir.z > 0.1f ) // Upwards branch
			{
				// Only add branchs below to see if the branch should split
				if ( toDir.z < 0 )
				{
					tempDir += attraction*1.2f;
				}
			}
			else // Downwards branch
			{
				if ( distSqr < sqr( 1.2f ) ) // Stop if too close to the point
				{
					branchFree = false;
				}
				// Add attraction normally
				tempDir += attraction*0.8f;
			}

			// If we can split towards the current point
			if ( pointCloudSplittable[i] )
			{
				// If the attraction and the current travel direction are too different, then split off
				if ( attraction.normal().dot( newDir.normal() ) < splitThreshold )
				{
					// Then we can't split towards this point anymore
					pointCloudSplittable[i] = false;
					// And we add a new travel dir
					Ray newRay = current;
					newRay.dir = tempDir.normal()*1.2f;

					TreePart* newPart = new TreePart;
					newPart->shape = newRay;
					newPart->parent = currentPart;
					newPart->userData = i;
					rayList.push_front( newPart );
				}
			}
		}
		// Normalize the dir
		newDir = newDir.normal() * 2.0f;

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
			newDir = newDir.normal() * 1.2f;

			// Take the new dir
			current.dir = newDir;

			// Add the ray to the list
			TreePart* newPart = new TreePart;
			newPart->shape = current;
			newPart->parent = currentPart;
			newPart->userData = -1;
			rayList.push_front( newPart );

			// Random chance to add a leaf on a branch
			if ( branchRadius < 0.38f )
			{
				if ( noise->Get( position.x*14.6f + newDir.x, newDir.y*newDir.z*32.0f + part_list.size()*0.7f ) > 0.4f )
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