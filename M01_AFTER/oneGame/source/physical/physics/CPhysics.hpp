
//==Physics Worlds==
FORCE_INLINE PHYS_API void Physics::ShiftWorld ( Vector3d vShift )
{
	hkVector4 effectiveShift;
	hkVector4 requestedShift ( vShift.x, vShift.y, vShift.z );
	World()->shiftBroadPhase( requestedShift, effectiveShift, hkpWorld::SHIFT_BROADPHASE_UPDATE_ENTITY_AABBS );

	Active()->vWorldCenter += vShift;
}
FORCE_INLINE PHYS_API void Physics::GetWorldCenter ( Vector3d& vWorldPosition )
{
	vWorldPosition = Active()->vWorldCenter;
}
FORCE_INLINE PHYS_API Vector3d Physics::GetWorldGravity ( void )
{
	hkVector4 gravity = Physics::World()->getGravity();
	return Vector3d( gravity.getComponent<0>(), gravity.getComponent<1>(), gravity.getComponent<2>() );
}


//=========================================//
// World Update
//=========================================//
FORCE_INLINE PHYS_API void Physics::UpdateSimulationTarget ( float deltaTime )
{
#ifdef _HAVOK_VISUAL_DEBUGGER_
	// Debugger may lag a frame, but it works
	Physics::VDB()->step();
#endif
	Physics::World()->setFrameTimeMarker( deltaTime );
	Physics::World()->advanceTime();
}
FORCE_INLINE PHYS_API bool Physics::SimulationAtTarget ( void )
{
	return Physics::World()->isSimulationAtMarker();
}
FORCE_INLINE PHYS_API bool Physics::Simulate ( float simulationTime )
{
	hkpStepResult result = Physics::World()->stepDeltaTime( simulationTime );
	return (result == HK_STEP_RESULT_SUCCESS);
}
FORCE_INLINE PHYS_API bool Physics::SimulationAtSubstep ( void )
{
	return Physics::World()->isSimulationAtPsi();
}


//==Collision Shapes==
// Creation of shapes for collision
FORCE_INLINE PHYS_API physShape*	Physics::CreateBoxShape ( Vector3d vHalfExtents )
{
	//hkVector4 halfExtent( vHalfExtents.x, vHalfExtents.y, vHalfExtents.z );
	hkVector4 halfExtent;
	halfExtent.set( vHalfExtents.x, vHalfExtents.y, vHalfExtents.z );
	hkpBoxShape* boxShape = new hkpBoxShape( halfExtent, 0.01f );
	//hkpSphereShape* boxShape = new hkpSphereShape( vHalfExtents.x ); // creates a sphere shape with a radius of 2
	return boxShape;
}
FORCE_INLINE PHYS_API physShape*	Physics::CreateBoxShape ( Vector3d vHalfExtents, Vector3d vCenterOffset )
{
	hkVector4 halfExtent;
	halfExtent.set( vHalfExtents.x, vHalfExtents.y, vHalfExtents.z );
	hkpBoxShape* boxShape = new hkpBoxShape( halfExtent, 0.01f );

	// Create a translated version of the child shape
	hkVector4 translation;
	translation.set( vCenterOffset.x, vCenterOffset.y, vCenterOffset.z );
	hkpConvexTranslateShape* translatedBoxShape = new hkpConvexTranslateShape( boxShape, translation );

	return translatedBoxShape;
}
// Create a capsule shape
FORCE_INLINE PHYS_API physShape* Physics::CreateCapsuleShape ( Vector3d vStart, Vector3d vEnd, float fRadius )
{
	hkVector4 start( vStart.x, vStart.y, vStart.z );
	hkVector4 end  ( vEnd.x, vEnd.y, vEnd.z );
	hkpCapsuleShape* capsuleShape = new hkpCapsuleShape( start, end, fRadius ); // creates a capsule with a axis between "start" and "end", and the specified "radius"
	
	return capsuleShape;
}
FORCE_INLINE PHYS_API physShape* Physics::CreateSphereShape ( float fRadius )
{
	hkpSphereShape* sphereShape = new hkpSphereShape( fRadius );
	return sphereShape;
}
// Create a cylinder shape
FORCE_INLINE PHYS_API physShape* Physics::CreateCylinderShape ( Vector3d vStart, Vector3d vEnd, float fRadius, float fConvexRadius )
{
	hkVector4 start( vStart.x, vStart.y, vStart.z );
	hkVector4 end  ( vEnd.x, vEnd.y, vEnd.z );
	hkpCylinderShape* cylinderShape = new hkpCylinderShape( start, end, fRadius, fConvexRadius ); // creates a cylinder with a axis between "start" and "end", and the specified "radius"
	
	return cylinderShape;
}
// Creates a mesh shape using a set of physics vertices.
FORCE_INLINE PHYS_API physShape*	Physics::CreateConvMeshShape ( CPhysicsData* pMesh )
{
	//pWorld->markForWrite();
	//pWorld->markForRead();
	//
	//hkpBvCompressedMeshShape* meshShape;
	hkpConvexVerticesShape* meshShape;
	/*hkGeometry geometry;
	
	// Change the size of the geometry object.
	geometry.m_vertices.setSize( pMesh->vertexNum );
	geometry.m_triangles.setSize( pMesh->triangleNum );
	
	// bla bla bla loop through stuff, set it 1 for 1, really really nice stuff.
	for ( unsigned int i = 0; i < pMesh->vertexNum; i += 1 )
	{
		geometry.m_vertices[i] = hkVector4( pMesh->vertices[i].x, pMesh->vertices[i].y, pMesh->vertices[i].z, 1 );
	}
	for ( unsigned int i = 0; i < pMesh->triangleNum; i += 1 )
	{
		geometry.m_triangles[i].m_a = pMesh->triangles[i].vert[0];
		geometry.m_triangles[i].m_b = pMesh->triangles[i].vert[1];
		geometry.m_triangles[i].m_c = pMesh->triangles[i].vert[2];
		geometry.m_triangles[i].m_material = 0;
	}*/
	hkArray<hkVector4> vertices;
	for ( unsigned int i = 0; i < pMesh->vertexNum; i += 1 )
	{
		vertices.pushBack( hkVector4( pMesh->vertices[i].x, pMesh->vertices[i].y, pMesh->vertices[i].z ) );
	}
	
	// Create the shape info with the geometry.
	//hkpDefaultBvCompressedMeshShapeCinfo cInfo( &geometry );
	hkpConvexVerticesShape::BuildConfig config;
	config.m_convexRadius = 0.01f;

	//meshShape = new hkpBvCompressedMeshShape ( cInfo );
	//meshShape = new hkpConvexVerticesShape( hkStridedVertices( pMesh->vertices, pMesh->vertexNum ), config );
	meshShape = new hkpConvexVerticesShape( hkStridedVertices( vertices ), config );

	//pWorld->unmarkForWrite();
	//pWorld->unmarkForRead();
	// Return the new shape
	return ((physShape*)meshShape);
}
// Creates a mesh shape using a set of physics vertices.
FORCE_INLINE PHYS_API physShape*	Physics::CreateMeshShape ( CPhysicsData* pMesh )
{
	//
	hkpBvCompressedMeshShape* meshShape;
	hkGeometry geometry;
	
	// Change the size of the geometry object.
	geometry.m_vertices.setSize( pMesh->vertexNum );
	geometry.m_triangles.setSize( pMesh->triangleNum );
	
	// bla bla bla loop through stuff, set it 1 for 1, really really nice stuff.
	for ( unsigned int i = 0; i < pMesh->vertexNum; i += 1 )
	{
		geometry.m_vertices[i] = hkVector4( pMesh->vertices[i].x, pMesh->vertices[i].y, pMesh->vertices[i].z, 1 );
	}
	for ( unsigned int i = 0; i < pMesh->triangleNum; i += 1 )
	{
		geometry.m_triangles[i].m_a = pMesh->triangles[i].vert[0];
		geometry.m_triangles[i].m_b = pMesh->triangles[i].vert[1];
		geometry.m_triangles[i].m_c = pMesh->triangles[i].vert[2];
		geometry.m_triangles[i].m_material = 0;
	}
	
	// Create the shape info with the geometry.
	hkpDefaultBvCompressedMeshShapeCinfo cInfo( &geometry );

	meshShape = new hkpBvCompressedMeshShape ( cInfo );

	// Return the new shape
	return ((physShape*)meshShape);
}
FORCE_INLINE PHYS_API physShape* Physics::CreateMeshShape ( CTerrainVertex* pVertices, CModelQuad* pQuads, unsigned short vertexCount, unsigned short faceCount )
{
	// BLEH
	hkpBvCompressedMeshShape* meshShape;
	hkGeometry geometry;
	
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
	hkpDefaultBvCompressedMeshShapeCinfo cInfo( &geometry );
	cInfo.m_weldingType = hkpWeldingUtility::WELDING_TYPE_NONE;
	cInfo.m_maxConvexShapeError = 0;
	cInfo.m_vertexWeldingTolerance = 0;

	meshShape = new hkpBvCompressedMeshShape ( cInfo );

	// Return the new shape
	return ((physShape*)meshShape);
}
// Creates a mesh shape using a set of model vertices.
FORCE_INLINE PHYS_API physShape*	Physics::CreateMeshShape ( CModelData const* pMesh )
{
	//
	hkpBvCompressedMeshShape* meshShape;
	hkGeometry geometry;
	
	// Change the size of the geometry object.
	geometry.m_vertices.setSize( pMesh->vertexNum );
	geometry.m_triangles.setSize( pMesh->triangleNum );
	
	// bla bla bla loop through stuff, set it 1 for 1, really really nice stuff.
	for ( unsigned int i = 0; i < pMesh->vertexNum; i += 1 )
	{
		geometry.m_vertices[i] = hkVector4( pMesh->vertices[i].x, pMesh->vertices[i].y, pMesh->vertices[i].z, 1 );
	}
	for ( unsigned int i = 0; i < pMesh->triangleNum; i += 1 )
	{
		geometry.m_triangles[i].m_a = pMesh->triangles[i].vert[0];
		geometry.m_triangles[i].m_b = pMesh->triangles[i].vert[1];
		geometry.m_triangles[i].m_c = pMesh->triangles[i].vert[2];
		geometry.m_triangles[i].m_material = 0;
	}
	
	// Create the shape info with the geometry.
	hkpDefaultBvCompressedMeshShapeCinfo cInfo( &geometry );

	meshShape = new hkpBvCompressedMeshShape ( cInfo );

	// Return the new shape
	return ((physShape*)meshShape);
}
// Creates a faster, but RAM eating mesh shape from the given data. Should be used for larger objects that change often
FORCE_INLINE PHYS_API physShape* Physics::CreateFastMeshShape ( Vector3d* pVertices, CModelTriangle* pTris, unsigned short vertexCount, unsigned short faceCount )
{
	// BLEH
	hkpExtendedMeshShape* meshShape = new hkpExtendedMeshShape ();
	//hkGeometry* newGeo = new hkGeometry();
	//hkGeometry& geometry = *newGeo;
	/*hkGeometry* geometry = new hkGeometry;
	
	// Change the size of the geometry object.
	geometry->m_vertices.setSize( vertexCount );
	geometry->m_triangles.setSize( faceCount*2 );
	
	// bla bla bla loop through stuff, set it 1 for 1, really really nice stuff.
	for ( unsigned int i = 0; i < vertexCount; i += 1 )
	{
		geometry->m_vertices[i] = hkVector4( pVertices[i].x, pVertices[i].y, pVertices[i].z, 1 );
	}
	for ( unsigned int i = 0; i < faceCount; i += 1 )
	{
		geometry->m_triangles[i*2].m_a = pQuads[i].vert[0];
		geometry->m_triangles[i*2].m_b = pQuads[i].vert[1];
		geometry->m_triangles[i*2].m_c = pQuads[i].vert[2];
		geometry->m_triangles[i*2].m_material = 0;
		geometry->m_triangles[i*2+1].m_a = pQuads[i].vert[2];
		geometry->m_triangles[i*2+1].m_b = pQuads[i].vert[3];
		geometry->m_triangles[i*2+1].m_c = pQuads[i].vert[0];
		geometry->m_triangles[i*2+1].m_material = 0;
	}
	*/
	// Create a triangle subpart
	hkpExtendedMeshShape::TrianglesSubpart part;

	// set vertices
	/*part.m_numVertices = vertexCount;//geometry->m_vertices.getSize();
	part.m_vertexBase = &(geometry->m_vertices.begin()[0](0));
	part.m_vertexStriding = sizeof(hkVector4);

	// set triangles
	part.m_indexBase = geometry->m_triangles.begin();
	part.m_numTriangleShapes = faceCount*2;//geometry->m_triangles.getSize();
	part.m_indexStriding = sizeof(hkGeometry::Triangle);
	part.m_stridingType = hkpExtendedMeshShape::INDICES_INT32;*/

	// set vertices
	part.m_numVertices = vertexCount;
	part.m_vertexBase = &(pVertices[0].x);
	part.m_vertexStriding = sizeof(Vector3d);

	// set triangles
	part.m_indexBase = &(pTris[0].vert[0]);
	part.m_numTriangleShapes = faceCount;
	part.m_indexStriding = sizeof(CModelTriangle);
	part.m_stridingType = hkpExtendedMeshShape::INDICES_INT32;

	// Add triangles to the mesh
	meshShape->addTrianglesSubpart( part );

	//return ((physShape*)meshShape);

	// Create a MOPP for the mesh
	hkpMoppCompilerInput* cmi = new hkpMoppCompilerInput();
	hkpMoppCode* code = hkpMoppUtility::buildCode(meshShape, *cmi);
	// Create the MOPP with the mesh
    hkpMoppBvTreeShape* moppBvShape = new hkpMoppBvTreeShape( meshShape, code );

	meshShape->removeReference();
	code->removeReference();
	
	// Return the new shape
	return ((physShape*)moppBvShape);


	/*hkpBoxShape* boxShape = new hkpBoxShape( hkVector4( 256,256,256 ), 0.0f );
	hkpBvShape* bvShape = new hkpBvShape( boxShape, meshShape );

	return ((physShape*)bvShape);*/
}

// Freeing shapes
FORCE_INLINE PHYS_API void Physics::FreeShape ( physShape* pShape )
{
	pShape->removeReference();
	//delete pShape;
}

//==Rigidbodies==
// Creation of rigidbodies
FORCE_INLINE PHYS_API hkpRigidBody*	Physics::CreateRigidBody ( physRigidBodyInfo* pBodyInfo, bool bIsDynamic )
{
	World()->markForWrite();
	//pWorld->markForRead();

	//threadPool->waitForCompletion();

	hkpMassProperties massProperties;
	if ( pBodyInfo == NULL )
		exit( 0 );
	if ( bIsDynamic )
	{
		if ( pBodyInfo->m_motionType != hkpMotion::MOTION_FIXED )
			hkpInertiaTensorComputer::computeShapeVolumeMassProperties( pBodyInfo->m_shape, pBodyInfo->m_mass, massProperties );
		
		pBodyInfo->setMassProperties( massProperties );
	}

	hkpRigidBody* pRigidBody = new hkpRigidBody( *pBodyInfo );
	World()->addEntity( pRigidBody );
	// World now owns the pointer to the rigid body
	//pRigidBody->removeReference();

	World()->unmarkForWrite();
	//pWorld->unmarkForRead();

	return pRigidBody;
}	
// Destruction of rigidbodies
FORCE_INLINE PHYS_API void Physics::FreeRigidBody ( hkpRigidBody* pRigidBody )
{
	pRigidBody->removeReference();
	World()->removeEntity( pRigidBody );
	// TODO: remove reference?
	//pRigidBody->removeReference();
	//delete pRigidBody;
}
// Setting their info
FORCE_INLINE PHYS_API void Physics::SetRigidBodyTransform ( hkpRigidBody* pRigidBody, CTransform* pSourceTransform )
{
	hkTransform tempTransform ( pRigidBody->getTransform() );

	Quaternion m_tempQuat = pSourceTransform->rotation.getQuaternion();
	hkQuaternion tempQuat ( m_tempQuat.x, m_tempQuat.y, m_tempQuat.z, m_tempQuat.w );
	tempTransform.setRotation( tempQuat );

	hkVector4 tempVect ( pSourceTransform->position.x, pSourceTransform->position.y, pSourceTransform->position.z );
	tempTransform.setTranslation( tempVect );

	pRigidBody->setTransform( tempTransform );
}
// Grabbing their info
FORCE_INLINE PHYS_API void Physics::GetRigidBodyTransform ( hkpRigidBody* pRigidBody, CTransform* pTargetTransform )
{
	if ( !pRigidBody->isActive() )
		return;

	// rotation
	Matrix4x4 tempMatrix;
	pRigidBody->getTransform().get4x4ColumnMajor( tempMatrix.pData );
	//tempMatrix = tempMatrix.transpose();
	//pTargetTransform->rotation = tempMatrix.getEulerAngles();
	//hkVector4 tempQuat = pRigidBody->getRotation().m_vec;
	//pTargetTransform->rotation.setRotation( Quaternion( tempQuat.m_quad.v[0], tempQuat.m_quad.v[1], tempQuat.m_quad.v[2], tempQuat.m_quad.v[3] ) );
	
	//pTargetTransform->rotation.setRotation( -tempMatrix.getEulerAngles() ); //works
	pTargetTransform->rotation.setRotation( (!tempMatrix).getEulerAngles() );
	//pTargetTransform->rotation.SwitchBasis();

	// translation
	hkVector4 tempVect = pRigidBody->getTransform().getTranslation();
	//pTargetTransform->position = Vector3d( tempVect, tempVect.y, tempVect.z );
	tempVect.store3( &(pTargetTransform->position.x) );

	// now, get offset of the object by the center of mass
	Vector3d temp3Vect;
	pRigidBody->getCenterOfMassLocal().store3( &(temp3Vect.x) );
	tempMatrix = Matrix4x4();
	tempMatrix.setRotation( pTargetTransform->rotation );
	//tempMatrix.setRotation( pTargetTransform->
	temp3Vect = tempMatrix*temp3Vect;
	
	// offset the object
	pTargetTransform->position += temp3Vect;

}
// Grabbing their info (position only)
FORCE_INLINE PHYS_API void Physics::GetRigidBodyTranslation( hkpRigidBody* pRigidBody, CTransform* pTargetTransform )
{
	if ( !pRigidBody->isActive() )
		return;
	// translation
	hkVector4 tempVect = pRigidBody->getTransform().getTranslation();
	tempVect.store3( &(pTargetTransform->position.x) );
}

//==Phantoms==
// Create a phantom using a collider
FORCE_INLINE PHYS_API physCollisionVolume* Physics::CreateAABBPhantom ( hkAabb* pInfo, unsigned int iOwnerID )
{
	// Create the new phantom
	hkpAabbPhantom* pPhantom = new hkpAabbPhantom ( *pInfo );
	pPhantom->setUserData( iOwnerID ); // Set the phantom's owner object.
	// Add the phantom to the world
	World()->addPhantom( pPhantom );

	// Return the new object
	return pPhantom;
}
// Create a phantom using a collider
FORCE_INLINE PHYS_API physCollisionVolume* Physics::CreateShapePhantom ( physShape* pShape, CTransform* pSourceTransform, unsigned int iOwnerID )
{
	// Get the transform
	hkTransform tempTransform ( hkRotation(),
		hkVector4(pSourceTransform->position.x,pSourceTransform->position.y,pSourceTransform->position.z)
		);
	// Create the new phantom
	hkpSimpleShapePhantom* pPhantom = new hkpSimpleShapePhantom ( pShape,tempTransform );
	pPhantom->setUserData( iOwnerID ); // Set the phantom's owner object.
	// Add the phantom to the world
	World()->addPhantom( pPhantom );

	// Return the new object
	return pPhantom;
}
// Remove phantom
FORCE_INLINE PHYS_API void Physics::FreePhantom ( physCollisionVolume* pCollisionVolume )
{
	World()->removePhantom( pCollisionVolume );
	// TODO: remove reference?
}
// Checking for phantom collisions contacts
FORCE_INLINE PHYS_API void Physics::CheckPhantomContacts ( physCollisionVolume* pCollisionVolume )
{
	hkpFlagCdBodyPairCollector collisionAccumulation;

	((hkpShapePhantom*)pCollisionVolume)->getPenetrations( collisionAccumulation );
}
// Creation of a trigger phantom
FORCE_INLINE PHYS_API hkpRigidBody* Physics::CreateTriggerVolume ( physRigidBodyInfo* pBodyInfo, physShape* pShape, hkpPhantomCallbackShape* pCbPhantom )
{
	// Create a new callback shape
	//hkArPhantomCallbackShape* myPhantomShape = new hkArPhantomCallbackShape();
	// Create a compound shape with the designated shape
	hkpBvShape* bvShape = new hkpBvShape( pShape, pCbPhantom );
	pCbPhantom->removeReference();

	pBodyInfo->m_shape = bvShape;

	hkpRigidBody* pRigidBody = new hkpRigidBody( *pBodyInfo );
	//Physics::World()->addEntity( pRigidBody );
	Physics::AddEntity( pRigidBody );

	bvShape->removeReference();

	return pRigidBody;
}

//=========================================//
// Tracing Collision Queries
//=========================================//
// Cast a ray
//PHYS_API static void Raycast ( Ray const& rDir, ftype fCastDist, RaycastHit * outHitInfo, uint32_t collisionFilter = 0, void* mismatch=NULL );
FORCE_INLINE PHYS_API void Physics::Raycast( const hkpWorldRayCastInput& input, hkpRayHitCollector& collector )
{
	Physics::World()->castRay ( input, collector );
}
// Cast a shape
//PHYS_API static void Linearcast ( Ray const& rDir, ftype fCastDist, physShape* pShape, RaycastHit* outHitInfo, const int hitInfoArrayCount, uint32_t collisionFilter = 0, void* mismatch=NULL );
FORCE_INLINE PHYS_API void Physics::Linearcast( const hkpCollidable* collA, const hkpLinearCastInput& input, hkpCdPointCollector& castCollector, hkpCdPointCollector* startCollector )
{
	Physics::World()->linearCast( collA, input, castCollector, startCollector );
}

//==Collision==
#include "physical/system/Layers.h"

// Get collision filter
FORCE_INLINE PHYS_API uint32_t Physics::GetCollisionFilter ( int layer, int subsystem, int nocollidewith )
{
	// Get the world's filter
	hkpGroupFilter* groupFilter = (hkpGroupFilter*)World()->getCollisionFilter();

	if ( layer == Layers::PHYS_BULLET_TRACE )
	{
		if ( nocollidewith == -1 ) {
			nocollidewith = 31;
		}
		if ( subsystem == -1 ) {
			subsystem = 0;
		}
		return groupFilter->calcFilterInfo( layer, Active()->systemGroups[Layers::PHYS_CHARACTER], subsystem, nocollidewith );
	}
	else
	{
		if ( nocollidewith == -1 ) {
			if ( layer == Layers::PHYS_CHARACTER ) {
				nocollidewith = 31;
			}	
			else {
				nocollidewith = 30;
			}
		}
		int targetGroup = Active()->systemGroups[layer];
		if ( subsystem == -1 ) {
			subsystem = 0;
			targetGroup = 0;
		}
		return groupFilter->calcFilterInfo( layer, targetGroup, subsystem, nocollidewith );
	}
}

// Get a collision collector
FORCE_INLINE PHYS_API hkpCollisionInput* Physics::GetCollisionCollector ( void )
{
	return (hkpCollisionInput*)World()->getCollisionInput();
}

// Get closest points to a collider
FORCE_INLINE PHYS_API void Physics::GetClosestPoints ( const hkpCollidable* collA, const hkpCollisionInput& input, hkpCdPointCollector& collector )
{
	World()->getClosestPoints( collA, input, collector );
}

//=========================================//
// Object handling
//=========================================//
FORCE_INLINE PHYS_API void Physics::AddEntity ( hkpEntity* entity )
{
	Physics::World()->addEntity(entity);
}
FORCE_INLINE PHYS_API void Physics::ForceEntityUpdate ( hkpEntity* entity )
{
	Physics::World()->updateCollisionFilterOnEntity( entity, HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS ); //todo
}

FORCE_INLINE PHYS_API void Physics::AddConstraint ( hkpConstraintInstance* constraint )
{
	Physics::World()->addConstraint( constraint );
}

FORCE_INLINE PHYS_API void Physics::AddPhantom ( hkpPhantom* phantom )
{
	Physics::World()->addPhantom( phantom );
}
FORCE_INLINE PHYS_API void Physics::AddListener ( hkpWorldPostSimulationListener* listener )
{
	Physics::World()->addWorldPostSimulationListener( listener );
}

//=========================================//
// Threading
//=========================================//
FORCE_INLINE PHYS_API void Physics::ThreadLock ( void )
{
	Physics::World()->markForWrite();
}
FORCE_INLINE PHYS_API void Physics::ThreadUnlock ( void )
{
	Physics::World()->unmarkForWrite();
}
FORCE_INLINE PHYS_API void Physics::ReadLock ( void )
{
	Physics::World()->markForRead();
}
FORCE_INLINE PHYS_API void Physics::ReadUnlock ( void )
{
	Physics::World()->unmarkForRead();
}