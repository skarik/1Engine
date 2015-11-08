
#include "FoliageBushGreen.h"
#include "core/math/Vector3d.h"
#include "engine/physics/raycast/Raycaster.h"

//Where the hell is list?
using std::list;
using std::vector;

FoliageBushGreen::FoliageBushGreen ( void )
	: CTreeBase()
{

}

void FoliageBushGreen::GenerateTreeData ( void )
{
	vector<bool>		pointCloudSplittable;
	vector<Vector3d>	pointCloud;
	Vector3d position = transform.position;
	float height	  =	0.1f;
	// First, generate the target point cloud at the top
	for ( unsigned int i = 0; i < 4; ++i )
	{
		Vector3d newVect (0,0,0);
		newVect.x = noise->Get3D( (position.x+position.y+position.z)/32.0f - i * 0.38f, position.x/32.0f, i * 0.43f );
		newVect.y = noise->Get3D( position.y/32.0f, i * 0.43f, (position.x+position.y+position.z)/32.0f - i * 0.38f );
		newVect.z = noise->Get3D( i * 0.43f, (position.x+position.y+position.z)/32.0f - i * 0.38f, position.z/32.0f );
		newVect *= 14.0f;
		newVect.z *= 0.01f;
		newVect.z += height;
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
	float splitThreshold = 0.6f;

	// Set the initial trunk radius
	float branchRadius = 0.3f;

	// Continue iterating while the queue is not empty
	while ( !rayList.empty() )
	{
		// Get the current ray
		TreePart* currentPart = rayList.back();
		rayList.pop_back();
		Ray current = currentPart->shape;

		// Set the current part's size
		currentPart->size = branchRadius;
		currentPart->strength = branchRadius;

		// Decrement the radius for this bit
		branchRadius *= 0.77f;

		// Add current branch to part list
		part_list.push_back( currentPart );
		
		// Move to the next position
		current.pos += current.dir;

		// Increase the split threshold
		splitThreshold = std::min<ftype>( 0.9f,splitThreshold+0.1f );

		// Go through all the points in the cloud and compute attraction
		Vector3d newDir = current.dir;
		bool branchFree = true;
		for ( unsigned int i = 0; i < pointCloud.size(); i++ )
		{
			Vector3d toDir = pointCloud[i]-current.pos;
			float distSqr = toDir.sqrMagnitude();
			Vector3d attraction = (toDir / distSqr)*20.0f;
			
			newDir += attraction;

			// Check here to see if the branch should end
			if ( distSqr < sqr( 1.2f ) )
			{
				branchFree = false;
			}
			else if ( current.pos.sqrMagnitude() > sqr( height*1.1f + 1.5f ) )
			{
				branchFree = false;
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
					newRay.dir = newDir.normal()*1.2f; // do not normalize to get the 'magnet' trees
					//rayList.push_front( newRay );

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
				float distSqr = toDir.sqrMagnitude(); 
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