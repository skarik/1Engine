
#include "CBoob.h"
#include "CBoobCollision.h"
#include "CVoxelTerrain.h"
#include "CGameSettings.h"

#include <Common/Serialize/Util/hkStructureLayout.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Physics/Utilities/Serialize/hkpPhysicsData.h>

// Static Collision update
hkpStaticCompoundShape*		CBoobCollision::m_staticCompoundShape = NULL;
vector<sBoobCollisionRef>	CBoobCollision::m_collisionReferences;


// Constructor + Destructor
CBoobCollision::CBoobCollision ( void )
{
	pShape		= NULL;
	pCollision	= NULL;

	iMyCollisionRef = unsigned(-1);

	meshShape = NULL;

	needUpdateOnCollider = false;
	for ( int i = 0; i < 8; ++i ) {
		update[i] = 0;
		m_collisions[i] = NULL;
		m_bodies[i] = NULL;
	}
}


CBoobCollision::~CBoobCollision ( void )
{
	// Free the collision
	if ( pCollision != NULL )
		Physics::FreeRigidBody( pCollision );
	if ( iMyCollisionRef != unsigned(-1) )
		m_collisionReferences[iMyCollisionRef].b_inUse = false;
	if ( meshShape ) {
		Physics::FreeShape( meshShape );
	}
	for ( int i = 0; i < 8; ++i ) {
		if ( m_bodies[i] ) {
			Physics::FreeRigidBody( m_bodies[i] );
		}
		if ( m_collisions[i] ) {
			Physics::FreeShape( m_collisions[i] );
		}
	}
}

// Update of collision ( fairly fast if the collision object is already cached )
void CBoobCollision::Update ( void )
{
	//if ( pOwner->current_resolution == 1 )	// On normal LoD, enable the collision
	if ( needUpdateOnCollider )
	{
		if ( pOwner->hasPropData && pOwner->hasBlockData )
		{
			/*InitiallizeCollisionReferences();
			// If we have no reference, look for a reference that's not being used
			if ( iMyCollisionRef == unsigned(-1) )
			{
				unsigned int i = 0;
				for ( i = 0; i < m_collisionReferences.size(); i++ )
				{
					if ( m_collisionReferences[i].b_inUse == false )
					{
						iMyCollisionRef = i;
						m_collisionReferences[i].b_inUse = true;
						break;
					}
				}
				if ( i >= m_collisionReferences.size() )
				{
					sBoobCollisionRef newRef;
					newRef.b_inUse = true;
					newRef.m_shape = NULL;
					m_collisionReferences.push_back( newRef );
					iMyCollisionRef = m_collisionReferences.size()-1;
				}
			}
			// Create or update the collision object.
			CreateCollision();
		
			// Create or update rigidbody object
			if ( pCollision != NULL )
			{
				pCollision->setMotionType( physMotion::MOTION_KEYFRAMED );
				pCollision->setShape( pShape );
				pCollision->setMotionType( physMotion::MOTION_FIXED );
			}
			else
			{
				physRigidBodyInfo info;
				info.m_shape = pShape;									// Set the collision shape to the collider's
				info.m_motionType = physMotion::MOTION_KEYFRAMED;		// Set the motion to static
				pCollision = Physics::CreateRigidBody( &info, false );	// Create a rigidbody and assign it to the body variable.
				pCollision->setMotionType( physMotion::MOTION_FIXED );
				if ( !CVoxelTerrain::terrainList.empty() ) {
					pCollision->setUserData( hkLong(CVoxelTerrain::terrainList[0]->GetId()) );	// Set the terrain as the owner
				}
				pCollision->setPosition( hkVector4( pOwner->position.x-32.0f, pOwner->position.y-32.0f, pOwner->position.z-32.0f ) );
			}

			needUpdateOnCollider = false;*/
			// Loop through each segment
			for ( uint i = 0; i < 8; ++i )
			{
				if ( update[i] )
				{
					// Create or update the collision shape
					CreateCollisionShape( i );

					// Now, create or update the rigidbody object
					if ( m_collisions[i] != NULL )
					{
						if ( m_bodies[i] != NULL )
						{
							m_bodies[i]->setMotionType( physMotion::MOTION_KEYFRAMED );
							m_bodies[i]->setShape( m_collisions[i] );
							m_bodies[i]->setMotionType( physMotion::MOTION_FIXED );
						}
						else
						{
							physRigidBodyInfo info;
							info.m_shape = m_collisions[i];									// Set the collision shape to the collider's
							info.m_motionType = physMotion::MOTION_KEYFRAMED;		// Set the motion to static
							m_bodies[i] = Physics::CreateRigidBody( &info, false );	// Create a rigidbody and assign it to the body variable.
							m_bodies[i]->setMotionType( physMotion::MOTION_FIXED );
							if ( !CVoxelTerrain::terrainList.empty() ) {
								m_bodies[i]->setUserData( hkLong(CVoxelTerrain::terrainList[0]->GetId()) );	// Set the terrain as the owner
							}
							m_bodies[i]->setPosition( hkVector4( pOwner->position.x-32.0f, pOwner->position.y-32.0f, pOwner->position.z-32.0f ) );
						}
						Physics::FreeShape(m_collisions[i]);
						m_collisions[i] = NULL;
					}
					else
					{	// If no collision, then remove the rigidbody
						if ( m_bodies[i] != NULL )
						{
							Physics::FreeRigidBody( m_bodies[i] );
							m_bodies[i] = NULL;
						}
					}
					update[i] = false;
				}
				// End check
			} // End for loop
			needUpdateOnCollider = false;
		}
		else
		{
			// Remove collision if is a low LOD
			/*if ( pCollision != NULL )
			{
				Physics::FreeRigidBody( pCollision );
				pCollision = NULL;
			}
			if ( iMyCollisionRef != unsigned(-1) )
			{
				m_collisionReferences[iMyCollisionRef].b_inUse = false;
				iMyCollisionRef = unsigned(-1);
			}
			needUpdateOnCollider = false;*/
		}
	}
	else
	{
		static int failRefreshCount = 0;
		failRefreshCount += 1;
		if ( failRefreshCount > (rand()%60 + 60) ) {
			failRefreshCount = 0;
			needUpdateOnCollider = true;
		}
	}
}

const int blocklut [7][4]= {
	{0,0,0,0},
	{4,5,6,7},
	{0,3,2,1},
	{1,2,6,5},
	{0,4,7,3},
	{3,7,6,2},
	{0,1,5,4}
};

void CBoobCollision::CreateCollisionShape ( const uint b16index )
{
	if ( m_collisions[b16index] ) {
		Physics::FreeShape(m_collisions[b16index]);
		m_collisions[b16index] = NULL;
	}

	hkGeometry geometry; 
	geometry.m_triangles.clear();
	geometry.m_vertices.clear();

	// Now, create the shape, looping through the area to make a collision
	int i;
	const ftype bsize = 2.0f;
	Vector3d pos;
	for ( uchar b8index = 0; b8index < 8; ++b8index )
	{
		for ( ushort bdindex = 0; bdindex < 512; ++bdindex )
		{
			//.data[b8index].data[b1index].block
			const subblock16& mblock16 = pOwner->data[b16index];

			pos.x = (32.0f * (b16index%2))		+ (16.0f * (b8index%2))		+ (2.0f * (bdindex%8));
			pos.y = (32.0f * ((b16index/2)%2))	+ (16.0f * ((b8index/2)%2)) + (2.0f * ((bdindex/8)%8));
			pos.z = (32.0f * (b16index/4))		+ (16.0f * (b8index/4))		+ (2.0f * (bdindex/64));

			// Create the face block type list
			terra_t terraFaceGrid [7];
			bool faceGrid [7];

			terraFaceGrid[0] = mblock16.data[b8index].data[bdindex];
			faceGrid[0] = CBoobMesh::IsCollidable(terraFaceGrid[0].block);

			// If current block is not collidable, skip
			if ( !faceGrid[0] ) {
				continue;
			}

			for ( i = 1; i < 7; ++i )
				terraFaceGrid[i] = CBoobMesh::GetBlockValue( pOwner, b16index, b8index, (terra_t*)mblock16.data[b8index].data,bdindex,8, (EFaceDir)(i), 1 );

			for ( i = 1; i < 7; ++i )
				faceGrid[i] = CBoobMesh::IsCollidable(terraFaceGrid[i].block);

			// Create the cube point list
			Vector3d gridvList[8];
			for ( i = 0; i < 4; ++i )
				gridvList[i] = pos;
			gridvList[1].x += bsize;
			gridvList[2].x += bsize;
			gridvList[2].y += bsize;
			gridvList[3].y += bsize;
			for ( i = 4; i < 8; ++i )
			{
				gridvList[i] = gridvList[i-4];
				gridvList[i].z += bsize;
			}

			// Create the face
			for ( i = 1; i < 7; ++i )
			{
				// If block isn't collidable, make a face for collision
				if ( !faceGrid[i] ) {
					int currentPos = geometry.m_vertices.getSize();
					for ( uint k = 0; k < 4; ++k ) {
						Vector3d target = gridvList[blocklut[i][k]];
						geometry.m_vertices.pushBack( hkVector4(target.x,target.y,target.z) );
					}
					// Add triangles
					hkGeometry::Triangle newtri;
					newtri.m_a = currentPos+0;
					newtri.m_b = currentPos+1;
					newtri.m_c = currentPos+2;
					geometry.m_triangles.pushBack( newtri );
					newtri.m_a = currentPos+2;
					newtri.m_b = currentPos+3;
					newtri.m_c = currentPos+0;
					geometry.m_triangles.pushBack( newtri );
				}
			}
			// end
		}
	}

	// Geometry has been created
	hkpDefaultBvCompressedMeshShapeCinfo cInfo( &geometry );
	m_collisions[b16index] = new hkpBvCompressedMeshShape( cInfo );
}


void CBoobCollision::CreateCollision ( void )
{
	// Okay Framerate, extremely fast update
	{
		const char*	sBlockFilename = ".game\\magic_DO_NOT_TOUCH.hkt";

		m_staticCompoundShape = m_collisionReferences[iMyCollisionRef].m_shape;	// KEY FUCKING LINE RIGHT HERE IF YOU'RE CONFUSED ABOUT THE STATIC
		// see, m_staticCompoundShape is ACTUALLY a temporary variable!

		if ( m_staticCompoundShape == NULL ) // Check if shape is invalid
		{
			CBinaryFile tempFile;
			// Check if collision file needs to be generated.
			if ( !tempFile.Exists(sBlockFilename) )
			{
				m_staticCompoundShape = new hkpStaticCompoundShape();
				{
					// create box
					hkVector4 halfs; halfs.set(1.0f,1.0f,1.0f);

					// fill it with the boxes
					for ( char i = 0; i < 8; i++ )
					{
						// create a box
						hkpConvexShape* box = new hkpBoxShape(halfs);
						// add to the collision mesh
						for ( char j = 0; j < 8; j++ )
						{
							for ( short k = 0; k < 512; k++ )
							{
								hkQsTransform transform;
								{
									transform.setIdentity();
									Vector3d blockPosition;
									blockPosition.x = (((i%2) * 16) + ((j%2) * 8) + (k%8))*2.0f +1.0f;
									blockPosition.y = ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))*2.0f +1.0f;
									blockPosition.z = (((i/4) * 16) + ((j/4) * 8)  + (k/64))*2.0f +1.0f;
									hkVector4 translation; translation.set( blockPosition.x, blockPosition.y, blockPosition.z );
									transform.setTranslation( translation );
								}
								//cinfo.addConvexShape( box, transform );
								m_staticCompoundShape->addInstance( box, transform );
							}
						}
						// remove reference to box
						box->removeReference();
					}
				}
				m_staticCompoundShape->bake();
				m_collisionReferences[iMyCollisionRef].m_shape = m_staticCompoundShape;
				// save file
				{
					hkpBvTreeShape* ashape = m_staticCompoundShape;
					hkSerializeUtil::save( ashape, hkpBvTreeShapeClass, hkOstream(sBlockFilename).getStreamWriter() );
				}
			}
			else // If down here, then the collision file has been generated, and we should load it.
			{
				hkResource* loadedData = hkSerializeUtil::load(sBlockFilename);
				m_staticCompoundShape = loadedData->getContents<hkpStaticCompoundShape>();
				
				m_collisionReferences[iMyCollisionRef].m_shape = m_staticCompoundShape;
			}
		}

		// Loop through all the shapes and enable collision if solid
		m_staticCompoundShape->enableAllInstancesAndShapeKeys(); // Reset
		for ( char i = 0; i < 8; i++ )
		{
			for ( char j = 0; j < 8; j++ )
			{
				for ( short k = 0; k < 512; k++ )
				{
					if (( pOwner->data[i].data[j].data[k].block == EB_NONE )
						||( pOwner->data[i].data[j].data[k].block == EB_WATER )
						||( pOwner->data[i].data[j].data[k].block == EB_TOP_SNOW ))
						m_staticCompoundShape->setInstanceEnabled( (i*8+j)*512 + k, false );
				}
			}
		}

		pShape = m_staticCompoundShape;
	}
}


void CBoobCollision::InitiallizeCollisionReferences ( void )
{
	
}

unsigned int CBoobCollision::GetTargetReferenceCount ( void )
{
	return cub(CGameSettings::Active()->i_cl_ter_Range);
}
unsigned int CBoobCollision::GetCurrentReferenceCount ( void )
{
	return m_collisionReferences.size();
}
void CBoobCollision::PreloadCollision ( void )
{
	//CPhysics::pWorld->lock();
	//CPhysics::pWorld->markForWrite();

	/*const char*	sBlockFilename = ".game\\magic_DO_NOT_TOUCH.hkt";
	if ( m_collisionReferences.size() < GetTargetReferenceCount() )
	{
		int iCollisionRef = m_collisionReferences.size();

		sBoobCollisionRef newRef;
		newRef.b_inUse = false;
		newRef.m_shape = NULL;
		m_collisionReferences.push_back( newRef );

		m_staticCompoundShape = m_collisionReferences[iCollisionRef].m_shape;

		// First generate the collider
		if ( m_staticCompoundShape == NULL )
		{
			CBinaryFile tempFile;
			// Check if collision file needs to be generated.
			if ( !tempFile.Exists(sBlockFilename) )
			{
				m_staticCompoundShape = new hkpStaticCompoundShape();
				{
					// create box
					hkVector4 halfs; halfs.set(1.0f,1.0f,1.0f);

					// fill it with the boxes
					for ( char i = 0; i < 8; i++ )
					{
						// create a box
						hkpConvexShape* box = new hkpBoxShape(halfs);
						// add to the collision mesh
						for ( char j = 0; j < 8; j++ )
						{
							for ( short k = 0; k < 512; k++ )
							{
								hkQsTransform transform;
								{
									transform.setIdentity();
									Vector3d blockPosition;
									blockPosition.x = (((i%2) * 16) + ((j%2) * 8) + (k%8))*2.0f +1.0f;
									blockPosition.y = ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))*2.0f +1.0f;
									blockPosition.z = (((i/4) * 16) + ((j/4) * 8)  + (k/64))*2.0f +1.0f;
									hkVector4 translation; translation.set( blockPosition.x, blockPosition.y, blockPosition.z );
									transform.setTranslation( translation );
								}
								//cinfo.addConvexShape( box, transform );
								m_staticCompoundShape->addInstance( box, transform );
							}
						}
						// remove reference to box
						box->removeReference();
					}
				}
				m_staticCompoundShape->bake();
				m_collisionReferences[iCollisionRef].m_shape = m_staticCompoundShape;
				// save file
				{
					hkpBvTreeShape* ashape = m_staticCompoundShape;
					hkSerializeUtil::save( ashape, hkpBvTreeShapeClass, hkOstream(sBlockFilename).getStreamWriter() );
				}
			}
			else // If down here, then the collision file has been generated, and we should load it.
			{
				hkResource* loadedData = hkSerializeUtil::load(sBlockFilename);
				m_staticCompoundShape = loadedData->getContents<hkpStaticCompoundShape>();	// This is what takes a while
				
				m_collisionReferences[iCollisionRef].m_shape = m_staticCompoundShape;		// Save it to the list and carry on
			}
		}
		else
		{
			return;
		}

		m_staticCompoundShape = NULL;
	}*/

	//CPhysics::pWorld->unlock();
	//CPhysics::pWorld->unmarkForWrite();
}