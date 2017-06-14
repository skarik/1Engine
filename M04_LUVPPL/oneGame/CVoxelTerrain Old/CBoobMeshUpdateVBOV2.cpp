
#include "CBoob.h"
#include "CToBeSeen.h"
#include "CVoxelTerrain.h"

#include <Common/Serialize/Util/hkStructureLayout.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Physics/Utilities/Serialize/hkpPhysicsData.h>

// Static Collision update
hkpStaticCompoundShape*		CBoobMesh::m_staticCompoundShape = NULL;
vector<sBoobCollisionRef>	CBoobMesh::m_collisionReferences;

bool CBoobMesh::PrepareRegen ( void )
{
	vertices =	new (std::nothrow) CTerrainVertex	[maxVertexCount];
	if ( vertices == NULL )
	{
		return false;
	}
	//quads =		new CModelQuad		[16384];
	quads =		new CModelQuad		[32768];
	if ( quads == NULL )
	{
		delete [] vertices;
		vertices = NULL;
		return false;
	}
	//if ( !physvertices )
	//	physvertices = new Vector3d		[32768];
	//if ( !phystris )
	//	phystris=	new CModelTriangle	[32768]; 

	vertexCount = 0;
	faceCount	= 0;

	// Ready for regeneration
	iVBOUpdateState = 0;

	return true;
}

void CBoobMesh::CleanupRegen ( void )
{
	delete [] vertices;
	vertices = NULL;
	delete [] quads;
	quads = NULL;
	//if ( pShape != NULL )
	//	Physics::FreeShape( pShape );
}

void CBoobMesh::UpdateRegen ( void )
{
	switch ( iVBOUpdateState )
	{
	case 0:	// First state: prepare the regen
		UpdateVBOMesh();
		iVBOUpdateState = 1;

		// If the mesh is too small for a valid collision mesh, don't continue
		if (( vertexCount < 4 )||( faceCount < 3 ))
		{
			// Clean up data
			CleanupRegen();

			// No action. (TODO: Remove existing havok collision object)
			needUpdateOnVBO = false;
			iVBOUpdateState = 0;
		}
		else
		{
			// Clean up data
			CleanupRegen();

			// ===Generate Collision===
			if ( pOwner->current_resolution == 1 )
			{
				// Create like 100 to 200 references to start with jic
				/*if ( m_collisionReferences.size() == 0 )
				{
					cout << "Adding collision references...";
					for ( int countss = 0; countss < 125; ++countss )
					{

						sBoobCollisionRef newRef;
						newRef.b_inUse = false;
						newRef.m_shape = NULL;
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
							//m_collisionReferences[iMyCollisionRef].m_shape = m_staticCompoundShape;
							newRef.m_shape = m_staticCompoundShape;
						}
						m_collisionReferences.push_back( newRef );

					}
					cout << "done." << endl;
				}*/
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
				// Otherwise, create the collision mesh.
				iVBOUpdateState = 2;
				BeginUpdateCollisionMesh();
			
				// Create rigidbody object
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
					info.m_motionType = physMotion::MOTION_KEYFRAMED;	// Set the motion to static
					pCollision = Physics::CreateRigidBody( &info, false );	// Create a rigidbody and assign it to the body variable.
					pCollision->setMotionType( physMotion::MOTION_FIXED );
					pCollision->setUserData( hkLong(CVoxelTerrain::terrainList[0]->GetId()) );	// Set the terrain as the owner
					pCollision->setPosition( hkVector4( pOwner->position.x-32.0f, pOwner->position.y-32.0f, pOwner->position.z-32.0f ) );
				}
			}
			else
			{
				// Remove collision if is a low LOD
				if ( pCollision != NULL )
				{
					Physics::FreeRigidBody( pCollision );
					pCollision = NULL;
				}
				if ( iMyCollisionRef != unsigned(-1) )
				{
					m_collisionReferences[iMyCollisionRef].b_inUse = false;
					iMyCollisionRef = unsigned(-1);
				}
			}
		}

		iVBOUpdateState = 3;
	break;
	case 3:
		needUpdateOnVBO = false;
		iVBOUpdateState = 0;
	break;
	}
}

void CBoobMesh::UpdateVBOMesh ( void )
{
	//==Begin OpenGL vertex buffer==

	if ( iVBOverts != 0 )
		glDeleteBuffersARB( 1, &iVBOverts );
	if ( iVBOfaces != 0 )
		glDeleteBuffersARB( 1, &iVBOfaces );
	
	glGenBuffersARB( 1, &iVBOverts );
	glGenBuffersARB( 1, &iVBOfaces );

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, iVBOverts);         // for vertex coordinates
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, iVBOfaces); // for face vertex indexes

	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(CTerrainVertex)*vertexCount, NULL, GL_DYNAMIC_DRAW_ARB );
	glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(CModelQuad)*faceCount, NULL, GL_DYNAMIC_DRAW_ARB );

	glBufferSubDataARB( GL_ARRAY_BUFFER_ARB, 0,vertexCount*sizeof(CTerrainVertex), vertices );
	glBufferSubDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0, faceCount*sizeof(CModelQuad), quads );

	/*glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(CTerrainVertex)*vertexCount, vertices, GL_DYNAMIC_DRAW_ARB );
	glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(CModelQuad)*faceCount, quads, GL_DYNAMIC_DRAW_ARB );*/

	/*glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(CTerrainVertex)*vertexCount, NULL, GL_DYNAMIC_DRAW_ARB );
	glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(CModelQuad)*faceCount, NULL, GL_DYNAMIC_DRAW_ARB );

	memcpy( glMapBufferARB( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB ), vertices, sizeof(CTerrainVertex)*vertexCount );
	memcpy( glMapBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB ), quads, sizeof(CModelQuad)*faceCount );

	glUnmapBufferARB( GL_ARRAY_BUFFER_ARB );
	glUnmapBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB );*/

	// bind with 0, so, switch back to normal pointer operation
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	//==End OpenGL vertex buffer==
}

// Begins the updating of the collision mesh
void CBoobMesh::BeginUpdateCollisionMesh ( void )
{
	/*
	// Original traditional way
	{
		// BLEH
		//hkpBvCompressedMeshShape* meshShape;
		//hkGeometry geometry;
		//pVertices ;
		CTerrainVertex* pVertices = vertices;
		CModelQuad* pQuads = quads;
		//unsigned short vertexCount = vertexCount;, unsigned short faceCount
		
		// Change the size of the geometry object.
		geometry.m_vertices.setSize( vertexCount );
		geometry.m_triangles.setSize( faceCount*2 );
		
		// bla bla bla loop through stuff, set it 1 for 1, really really nice stuff.
		for ( unsigned int i = 0; i < vertexCount; i += 1 )
		{
			geometry.m_vertices[i] = hkVector4( pVertices[i].x, pVertices[i].y, pVertices[i].z, 1 );
		}
		for ( unsigned int i = 0; i < faceCount; i += 1 )
		{
			geometry.m_triangles[i*2].m_a = pQuads[i].vert[0];
			geometry.m_triangles[i*2].m_b = pQuads[i].vert[1];
			geometry.m_triangles[i*2].m_c = pQuads[i].vert[2];
			geometry.m_triangles[i*2].m_material = 0;
			geometry.m_triangles[i*2+1].m_a = pQuads[i].vert[2];
			geometry.m_triangles[i*2+1].m_b = pQuads[i].vert[3];
			geometry.m_triangles[i*2+1].m_c = pQuads[i].vert[0];
			geometry.m_triangles[i*2+1].m_material = 0;
		}
		
		// Create the shape info with the geometry.
		//hkpDefaultBvCompressedMeshShapeCinfo cInfo( &geometry );
		//cInfo( &geometry );
		cInfo.m_geometry = &geometry;
		cInfo.m_weldingType = hkpWeldingUtility::WELDING_TYPE_NONE;
		cInfo.m_maxConvexShapeError = 0;
		cInfo.m_vertexWeldingTolerance = 0;

		 // this goes to thread

		// Return the new shape
		//return ((physShape*)meshShape);

		meshShape = new hkpBvCompressedMeshShape ( cInfo );

		// Create the new thread
		sCBoobMeshCollisionRegen x;
		x.pTargetMesh = this;
		//currentRegenThread = thread( x );
	}
	*/

	// Shitty Framerate, extremely fast update
	/*{
		if ( m_staticCompoundShape == NULL )
		{
			m_staticCompoundShape = new hkpStaticCompoundShape();
			{
				// create box
				hkVector4 halfs; halfs.set(1.0f,1.0f,1.0f);
				

				// create compressed mesh
				hkpBvCompressedMeshShape* pCompressedMesh;
				hkpDefaultBvCompressedMeshShapeCinfo cinfo;

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
							cinfo.addConvexShape( box, transform );
						}
					}
					// remove reference to box
					box->removeReference();
				}
				// todo add user data
				cinfo.m_userDataMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_8_BIT;
				// create mesh
				pCompressedMesh = new hkpBvCompressedMeshShape ( cinfo );
				// remove reference to box
				//box->removeReference();

				// add mesh
				m_staticCompoundShape->addInstance( pCompressedMesh, hkQsTransform::getIdentity() );
			}
			m_staticCompoundShape->bake();
			//meshShape = m_staticCompoundShape;
			//pShape = m_staticCompoundShape;
		}

		pShape = m_staticCompoundShape;
		
		//cout << m_staticCompoundShape->getNumChildShapes() << endl;

		hkpShapeKey currentKey = m_staticCompoundShape->getFirstKey();
		m_staticCompoundShape->enableAllInstancesAndShapeKeys(); // Reset
		for ( char i = 0; i < 8; i++ )
		{
			for ( char j = 0; j < 8; j++ )
			{
				for ( short k = 0; k < 512; k++ )
				{
					if ( currentKey == HK_INVALID_SHAPE_KEY )
					{
						cout << "Error in collision mesh regen: Invalid key" << endl;
						continue;
					}
					
					if ( pOwner->data[i].data[j].data[k] == EB_NONE ) 
						m_staticCompoundShape->setShapeKeyEnabled( currentKey, false );

					currentKey = m_staticCompoundShape->getNextKey( currentKey );
				}
			}
		}
	}*/

	// Shitty Framerate, extremely fast update
	{
		const char*	sBlockFilename = ".game\\magic_DO_NOT_TOUCH.hkt";

		m_staticCompoundShape = m_collisionReferences[iMyCollisionRef].m_shape;

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

		m_staticCompoundShape->enableAllInstancesAndShapeKeys(); // Reset
		for ( char i = 0; i < 8; i++ )
		{
			for ( char j = 0; j < 8; j++ )
			{
				for ( short k = 0; k < 512; k++ )
				{
					if (( pOwner->data[i].data[j].data[k] == EB_NONE )||( pOwner->data[i].data[j].data[k] == EB_WATER ))
						m_staticCompoundShape->setInstanceEnabled( (i*8+j)*512 + k, false );
				}
			}
		}

		pShape = m_staticCompoundShape;
	}

	// Slow. Just, slow.
	/*{
		// create box
		hkVector4 halfs; halfs.set(1.0f,1.0f,1.0f);
		

		// create compressed mesh
		hkpBvCompressedMeshShape* pCompressedMesh;
		hkpDefaultBvCompressedMeshShapeCinfo cinfo;

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
						blockPosition.x = (((i%2) * 16) + ((j%2) * 8) + (k%8))*2.0f;
						blockPosition.y = ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))*2.0f;
						blockPosition.z = (((i/4) * 16) + ((j/4) * 8)  + (k/64))*2.0f;
						hkVector4 translation; translation.set( blockPosition.x, blockPosition.y, blockPosition.z );
						transform.setTranslation( translation );
					}
					cinfo.addConvexShape( box, transform );
				}
			}
			// remove reference to box
			box->removeReference();
		}
		// todo add user data
		cinfo.m_userDataMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_8_BIT;
		// create mesh
		pCompressedMesh = new hkpBvCompressedMeshShape ( cinfo );
		// remove reference to box
		//box->removeReference();

		// add mesh
		//m_staticCompoundShape->addInstance( pCompressedMesh, hkQsTransform::getIdentity() );

		meshShape = pCompressedMesh;
	}*/
}

// Collision Regenerator
void sCBoobMeshCollisionRegen::operator() ( void )
{
	CPhysics::pWorld->lock();

	// Generate the mesh shape
	//pTargetMesh->meshShape = new hkpBvCompressedMeshShape ( pTargetMesh->cInfo );

	CPhysics::pWorld->unlock();
}