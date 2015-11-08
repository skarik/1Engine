
#include "core/math/Math.h"
#include "core/math/Vector3d.h"
#include "engine/physics/raycast/Raycaster.h"
#include "FoliageCactusDefault.h"

using std::vector;
using std::list;
//Again, where is list?

FoliageCactusDefault::FoliageCactusDefault ( void )
	: CTreeBase()
{
	mTexCoordR = 0.75f;
}

void FoliageCactusDefault::GenerateTreeData ( void )
{
	vector<Vector3d>	pointCloud;
	vector<bool>		pointCloudSplittable;
	Vector3d position = transform.position;
	float				height;

	// Generate the height
	height	= 9.4f+3.7f*noise->Get3D( position.x+position.y * 41.7f, position.z * 12.4f, position.x*position.z*0.24f+3.2f );

	// Generate the point cloud.
	uint point_count = (uint)( 3.4f + 2*noise->Get3D( position.x,position.y,position.z ) );
	// 2 to 4 points between 1/3 way up and 2/3 way up
	for ( uint i = 0; i < point_count; ++i ) {
		Vector3d newPoint;
		newPoint.z = height*(0.5f+(noise->Get3D(position.x*5.4f-13.2f+i*12.9f,position.y*5.6f*i-23.2f,position.z-i*32.3f)/3));
		newPoint.x = noise->Get3D( (position.y+i)*1.3f,position.z*2.43f,(position.x-i)*1.67f ) * 4.5f;
		newPoint.y = noise->Get3D( (position.y+i)*1.2f,position.x*1.62f,(position.z-i)*2.46f ) * 4.5f;
		pointCloud.push_back( newPoint );
		pointCloudSplittable.push_back( true );
	}


	// Add initial ray in the queue
	Ray start;
	start.dir = Vector3d( 0,0,1.1f );
	start.pos = Vector3d( 0,0,0 );

	TreePart* startPart = new TreePart;
	startPart->parent = NULL;
	startPart->shape = start;
	startPart->userData = -1;
	list<TreePart*> rayList;
	rayList.push_front( startPart );

	// Set the initial trunk radius
	float branchRadius = 0.7f;
	float t;

	// While ray list isn't empty, go upwards.
	while ( !rayList.empty() )
	{
		// Get the current ray
		TreePart* currentPart = rayList.back();
		rayList.pop_back();
		Ray current = currentPart->shape;

		// Set the current part's size
		currentPart->size = branchRadius;
		currentPart->strength = branchRadius;

		// Set the radius for this
		t = std::max<ftype>(0,(height-current.pos.z)/height);
		branchRadius = std::max<ftype>( (ftype)( sin(t*(PI/2)) / sin(t*(PI/2)+0.03f) )*0.7f, 0.1f );
		
		// Move to the next position
		current.pos += current.dir;

		// If above height, make tip
		if ( current.pos.z >= height ) {
			currentPart->size *= 0.5f;
		}

		bool added_arm = false;
		for ( uint i = 0; i < point_count; ++i )
		{
			// If detect that current spot is to go after point cloud,
			if ( pointCloudSplittable[i] && (current.pos.z > pointCloud[i].z-height*0.4f) )
			{
				TreePart* newPart;

				pointCloudSplittable[i] = false;

				if ( !added_arm ) {
					// add self to ray list again
					newPart = new TreePart;
					newPart->shape = currentPart->shape;
					newPart->parent = currentPart->parent;
					newPart->userData = -1;
					rayList.push_front( newPart );
				}

				// add entire cactus arm
				bool target_reached = false;

				// Do first ray
				current.dir.z *= fabs( noise->Get3D(current.pos.x*126.2f,current.pos.z*212.3f,current.pos.y*173.3f) )+0.1f;
				current.dir.x += pointCloud[i].x-current.pos.x;
				current.dir.y += pointCloud[i].y-current.pos.y;
				current.dir.normalize();
				current.dir *= 1.1f;
				// Add it!
				newPart = new TreePart;
				newPart->shape = current;
				newPart->parent = currentPart;
				currentPart = newPart;
				newPart->userData = i;
				newPart->size = branchRadius;
				newPart->strength = newPart->size;
				part_list.push_back( newPart );

				while ( !target_reached )
				{
					// Move branch
					current.pos += current.dir;
					// Lerp dirs
					current.dir.z = Math.Lerp( 0.25f, current.dir.z, pointCloud[i].z - current.pos.z );
					current.dir.x = (pointCloud[i].x - current.pos.x)*0.5f;
					current.dir.y = (pointCloud[i].y - current.pos.y)*0.5f;
					// Normalize + set length of dir
					current.dir.normalize();
					current.dir *= 1.1f;

					// New branch
					newPart = new TreePart;
					newPart->shape = current;
					newPart->parent = currentPart;
					currentPart = newPart;
					newPart->userData = i;
					t = std::max<ftype>(0,(pointCloud[i].z-current.pos.z)/pointCloud[i].z);
					newPart->size = branchRadius * std::max<ftype>( (ftype)( sin(t*(PI/2)) / sin(t*(PI/2)+0.10f) )*1.1f, 0.1f );
					newPart->strength = newPart->size;
					// Add branch to the part list
					part_list.push_back( newPart );

					// Check for target reached
					if ( current.pos.z > pointCloud[i].z-0.2f ) {
						target_reached = true;
					}
				}

				// Set that added arm
				added_arm = true;
			}
		}

		// Go upwards to height.
		if ( !added_arm )
		{
			// Add current branch to part list
			part_list.push_back( currentPart );

			// Set direction
			current.dir.z += 2.1f;
			current.dir.x += noise->Get3D( position.x+current.pos.y*0.03f, position.y+current.pos.z*0.03f, position.z+current.pos.x*0.03f );
			current.dir.y += noise->Get3D( position.x-current.pos.z*0.03f, position.y-current.pos.x*0.03f, position.z-current.pos.y*0.03f );
			current.dir = current.dir.normal() * 1.2f;

			// Push to list if made it
			if ( current.pos.z < height )
			{
				// Add to ray list if not tall enough
				TreePart* newPart = new TreePart;
				newPart->shape = current;
				newPart->parent = currentPart;
				newPart->userData = -1;
				rayList.push_front( newPart );
			}
		}
	}


	// At the end, clear the leaf cloud.
	pointCloud.clear();
	// Clear the leaf-associated lists, just in case
	leaf_cloud.clear();
	leaf_branch.clear();
}