
#include "CBoob.h"
#include "CRenderState.h"
#include "CVoxelTerrain.h"

CTerrainVertex* CBoobMesh::vertices = NULL;
CModelQuad*		CBoobMesh::quads	= NULL;

bool CBoobMesh::PrepareRegen ( void )
{
	/*if ( vertices == NULL )
		vertices =	new (std::nothrow) CTerrainVertex	[maxVertexCount];
	if ( vertices == NULL )
	{
		return false;
	}
	if ( quads == NULL )
		quads =		new CModelQuad		[32768];
	if ( quads == NULL )
	{
		delete [] vertices;
		vertices = NULL;
		return false;
	}*/

	vertexCount = 0;
	faceCount	= 0;

	iVBOUpdateState = 0;	// Ready for regeneration

	return true;
}
bool CBoobMesh::PrepareWaterRegen ( void )
{
	/*if ( vertices == NULL )
		vertices =	new (std::nothrow) CTerrainVertex	[maxVertexCount];
	if ( vertices == NULL )
	{
		return false;
	}
	if ( quads == NULL )
		quads =		new CModelQuad		[32768];
	if ( quads == NULL )
	{
		delete [] vertices;
		vertices = false;
		return false;
	}*/

	vertexCountWater = 0;
	faceCountWater	= 0;

	return true;
}

void CBoobMesh::CleanupRegen ( void )
{
	/*delete [] vertices;
	vertices = NULL;
	delete [] quads;
	quads = NULL;*/
}
void CBoobMesh::CleanupWaterRegen ( void )
{
	//;
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
	
	if ( iVBOverts == 0 )
		glGenBuffers( 1, &iVBOverts );
	if ( iVBOfaces == 0 )
		glGenBuffers( 1, &iVBOfaces );

	glBindBuffer(GL_ARRAY_BUFFER, iVBOverts);         // for vertex coordinates
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iVBOfaces); // for face vertex indexes

	glBufferData( GL_ARRAY_BUFFER, sizeof(CTerrainVertex)*vertexCount, NULL, GL_DYNAMIC_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(CModelQuad)*faceCount, NULL, GL_DYNAMIC_DRAW );

	glBufferSubData( GL_ARRAY_BUFFER, 0,vertexCount*sizeof(CTerrainVertex), vertices );
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, faceCount*sizeof(CModelQuad), quads );

	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//==End OpenGL vertex buffer==
}
void CBoobMesh::UpdateWaterVBOMesh ( void )
{
	//==Begin OpenGL vertex buffer==
	
	if ( iWaterVBOverts == 0 )
		glGenBuffers( 1, &iWaterVBOverts );
	if ( iWaterVBOfaces == 0 )
		glGenBuffers( 1, &iWaterVBOfaces );

	glBindBuffer(GL_ARRAY_BUFFER, iWaterVBOverts);         // for vertex coordinates
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iWaterVBOfaces); // for face vertex indexes

	glBufferData( GL_ARRAY_BUFFER, sizeof(CTerrainVertex)*vertexCountWater, NULL, GL_STREAM_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(CModelQuad)*faceCountWater, NULL, GL_STREAM_DRAW );

	glBufferSubData( GL_ARRAY_BUFFER, 0,vertexCountWater*sizeof(CTerrainVertex), vertices );
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, faceCountWater*sizeof(CModelQuad), quads );

	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//==End OpenGL vertex buffer==
}



//// Begins the updating of the collision mesh
//void CBoobMesh::BeginUpdateCollisionMesh ( void )
//{
//	/*
//	// Original traditional way
//	{
//		// BLEH
//		//hkpBvCompressedMeshShape* meshShape;
//		//hkGeometry geometry;
//		//pVertices ;
//		CTerrainVertex* pVertices = vertices;
//		CModelQuad* pQuads = quads;
//		//unsigned short vertexCount = vertexCount;, unsigned short faceCount
//		
//		// Change the size of the geometry object.
//		geometry.m_vertices.setSize( vertexCount );
//		geometry.m_triangles.setSize( faceCount*2 );
//		
//		// bla bla bla loop through stuff, set it 1 for 1, really really nice stuff.
//		for ( unsigned int i = 0; i < vertexCount; i += 1 )
//		{
//			geometry.m_vertices[i] = hkVector4( pVertices[i].x, pVertices[i].y, pVertices[i].z, 1 );
//		}
//		for ( unsigned int i = 0; i < faceCount; i += 1 )
//		{
//			geometry.m_triangles[i*2].m_a = pQuads[i].vert[0];
//			geometry.m_triangles[i*2].m_b = pQuads[i].vert[1];
//			geometry.m_triangles[i*2].m_c = pQuads[i].vert[2];
//			geometry.m_triangles[i*2].m_material = 0;
//			geometry.m_triangles[i*2+1].m_a = pQuads[i].vert[2];
//			geometry.m_triangles[i*2+1].m_b = pQuads[i].vert[3];
//			geometry.m_triangles[i*2+1].m_c = pQuads[i].vert[0];
//			geometry.m_triangles[i*2+1].m_material = 0;
//		}
//		
//		// Create the shape info with the geometry.
//		//hkpDefaultBvCompressedMeshShapeCinfo cInfo( &geometry );
//		//cInfo( &geometry );
//		cInfo.m_geometry = &geometry;
//		cInfo.m_weldingType = hkpWeldingUtility::WELDING_TYPE_NONE;
//		cInfo.m_maxConvexShapeError = 0;
//		cInfo.m_vertexWeldingTolerance = 0;
//
//		 // this goes to thread
//
//		// Return the new shape
//		//return ((physShape*)meshShape);
//
//		meshShape = new hkpBvCompressedMeshShape ( cInfo );
//
//		// Create the new thread
//		sCBoobMeshCollisionRegen x;
//		x.pTargetMesh = this;
//		//currentRegenThread = thread( x );
//	}
//	*/
//
//	// Shitty Framerate, extremely fast update
//	/*{
//		if ( m_staticCompoundShape == NULL )
//		{
//			m_staticCompoundShape = new hkpStaticCompoundShape();
//			{
//				// create box
//				hkVector4 halfs; halfs.set(1.0f,1.0f,1.0f);
//				
//
//				// create compressed mesh
//				hkpBvCompressedMeshShape* pCompressedMesh;
//				hkpDefaultBvCompressedMeshShapeCinfo cinfo;
//
//				// fill it with the boxes
//				for ( char i = 0; i < 8; i++ )
//				{
//					// create a box
//					hkpConvexShape* box = new hkpBoxShape(halfs);
//					// add to the collision mesh
//					for ( char j = 0; j < 8; j++ )
//					{
//						for ( short k = 0; k < 512; k++ )
//						{
//							hkQsTransform transform;
//							{
//								transform.setIdentity();
//								Vector3d blockPosition;
//								blockPosition.x = (((i%2) * 16) + ((j%2) * 8) + (k%8))*2.0f +1.0f;
//								blockPosition.y = ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))*2.0f +1.0f;
//								blockPosition.z = (((i/4) * 16) + ((j/4) * 8)  + (k/64))*2.0f +1.0f;
//								hkVector4 translation; translation.set( blockPosition.x, blockPosition.y, blockPosition.z );
//								transform.setTranslation( translation );
//							}
//							cinfo.addConvexShape( box, transform );
//						}
//					}
//					// remove reference to box
//					box->removeReference();
//				}
//				// todo add user data
//				cinfo.m_userDataMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_8_BIT;
//				// create mesh
//				pCompressedMesh = new hkpBvCompressedMeshShape ( cinfo );
//				// remove reference to box
//				//box->removeReference();
//
//				// add mesh
//				m_staticCompoundShape->addInstance( pCompressedMesh, hkQsTransform::getIdentity() );
//			}
//			m_staticCompoundShape->bake();
//			//meshShape = m_staticCompoundShape;
//			//pShape = m_staticCompoundShape;
//		}
//
//		pShape = m_staticCompoundShape;
//		
//		//cout << m_staticCompoundShape->getNumChildShapes() << endl;
//
//		hkpShapeKey currentKey = m_staticCompoundShape->getFirstKey();
//		m_staticCompoundShape->enableAllInstancesAndShapeKeys(); // Reset
//		for ( char i = 0; i < 8; i++ )
//		{
//			for ( char j = 0; j < 8; j++ )
//			{
//				for ( short k = 0; k < 512; k++ )
//				{
//					if ( currentKey == HK_INVALID_SHAPE_KEY )
//					{
//						cout << "Error in collision mesh regen: Invalid key" << endl;
//						continue;
//					}
//					
//					if ( pOwner->data[i].data[j].data[k] == EB_NONE ) 
//						m_staticCompoundShape->setShapeKeyEnabled( currentKey, false );
//
//					currentKey = m_staticCompoundShape->getNextKey( currentKey );
//				}
//			}
//		}
//	}*/
//
//	// Shitty Framerate, extremely fast update
//	/*{
//		const char*	sBlockFilename = ".game\\magic_DO_NOT_TOUCH.hkt";
//
//		m_staticCompoundShape = m_collisionReferences[iMyCollisionRef].m_shape;
//
//		if ( m_staticCompoundShape == NULL ) // Check if shape is invalid
//		{
//			CBinaryFile tempFile;
//			// Check if collision file needs to be generated.
//			if ( !tempFile.Exists(sBlockFilename) )
//			{
//				m_staticCompoundShape = new hkpStaticCompoundShape();
//				{
//					// create box
//					hkVector4 halfs; halfs.set(1.0f,1.0f,1.0f);
//
//					// fill it with the boxes
//					for ( char i = 0; i < 8; i++ )
//					{
//						// create a box
//						hkpConvexShape* box = new hkpBoxShape(halfs);
//						// add to the collision mesh
//						for ( char j = 0; j < 8; j++ )
//						{
//							for ( short k = 0; k < 512; k++ )
//							{
//								hkQsTransform transform;
//								{
//									transform.setIdentity();
//									Vector3d blockPosition;
//									blockPosition.x = (((i%2) * 16) + ((j%2) * 8) + (k%8))*2.0f +1.0f;
//									blockPosition.y = ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))*2.0f +1.0f;
//									blockPosition.z = (((i/4) * 16) + ((j/4) * 8)  + (k/64))*2.0f +1.0f;
//									hkVector4 translation; translation.set( blockPosition.x, blockPosition.y, blockPosition.z );
//									transform.setTranslation( translation );
//								}
//								//cinfo.addConvexShape( box, transform );
//								m_staticCompoundShape->addInstance( box, transform );
//							}
//						}
//						// remove reference to box
//						box->removeReference();
//					}
//				}
//				m_staticCompoundShape->bake();
//				m_collisionReferences[iMyCollisionRef].m_shape = m_staticCompoundShape;
//				// save file
//				{
//					hkpBvTreeShape* ashape = m_staticCompoundShape;
//					hkSerializeUtil::save( ashape, hkpBvTreeShapeClass, hkOstream(sBlockFilename).getStreamWriter() );
//				}
//			}
//			else // If down here, then the collision file has been generated, and we should load it.
//			{
//				hkResource* loadedData = hkSerializeUtil::load(sBlockFilename);
//				m_staticCompoundShape = loadedData->getContents<hkpStaticCompoundShape>();
//				
//				m_collisionReferences[iMyCollisionRef].m_shape = m_staticCompoundShape;
//			}
//		}
//
//		m_staticCompoundShape->enableAllInstancesAndShapeKeys(); // Reset
//		for ( char i = 0; i < 8; i++ )
//		{
//			for ( char j = 0; j < 8; j++ )
//			{
//				for ( short k = 0; k < 512; k++ )
//				{
//					if (( pOwner->data[i].data[j].data[k].block == EB_NONE )||( pOwner->data[i].data[j].data[k].block == EB_WATER ))
//						m_staticCompoundShape->setInstanceEnabled( (i*8+j)*512 + k, false );
//				}
//			}
//		}
//
//		pShape = m_staticCompoundShape;
//	}*/
//
//	// Slow. Just, slow.
//	/*{
//		// create box
//		hkVector4 halfs; halfs.set(1.0f,1.0f,1.0f);
//		
//
//		// create compressed mesh
//		hkpBvCompressedMeshShape* pCompressedMesh;
//		hkpDefaultBvCompressedMeshShapeCinfo cinfo;
//
//		// fill it with the boxes
//		for ( char i = 0; i < 8; i++ )
//		{
//			// create a box
//			hkpConvexShape* box = new hkpBoxShape(halfs);
//			// add to the collision mesh
//			for ( char j = 0; j < 8; j++ )
//			{
//				for ( short k = 0; k < 512; k++ )
//				{
//					hkQsTransform transform;
//					{
//						transform.setIdentity();
//						Vector3d blockPosition;
//						blockPosition.x = (((i%2) * 16) + ((j%2) * 8) + (k%8))*2.0f;
//						blockPosition.y = ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))*2.0f;
//						blockPosition.z = (((i/4) * 16) + ((j/4) * 8)  + (k/64))*2.0f;
//						hkVector4 translation; translation.set( blockPosition.x, blockPosition.y, blockPosition.z );
//						transform.setTranslation( translation );
//					}
//					cinfo.addConvexShape( box, transform );
//				}
//			}
//			// remove reference to box
//			box->removeReference();
//		}
//		// todo add user data
//		cinfo.m_userDataMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_8_BIT;
//		// create mesh
//		pCompressedMesh = new hkpBvCompressedMeshShape ( cinfo );
//		// remove reference to box
//		//box->removeReference();
//
//		// add mesh
//		//m_staticCompoundShape->addInstance( pCompressedMesh, hkQsTransform::getIdentity() );
//
//		meshShape = pCompressedMesh;
//	}*/
//}
//
//// Collision Regenerator
///*void sCBoobMeshCollisionRegen::operator() ( void )
//{
//	CPhysics::pWorld->lock();
//
//	// Generate the mesh shape
//	//pTargetMesh->meshShape = new hkpBvCompressedMeshShape ( pTargetMesh->cInfo );
//
//	CPhysics::pWorld->unlock();
//}*/