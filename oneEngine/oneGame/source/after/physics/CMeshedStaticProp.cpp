
#include "CMeshedStaticProp.h"

#include "physical/physics.h"
#include "physical/physics/CPhysics.h"
#include "physical/physics/motion/physRigidbody.h"

CMeshedStaticProp::CMeshedStaticProp ( void )
	: m_rigidbody(NULL), m_scale( 1.0f )
{
	;
}
CMeshedStaticProp::~CMeshedStaticProp ( void )
{
	DeleteRigidbody();
}

// UpdateScale () : Changes the scale that the UpdateCollider loads the mesh at
void CMeshedStaticProp::UpdateScale ( const float n_newScale )
{
	m_scale = n_newScale;
}
// UpdateCollider (TerrainVertex) : Creates Havok geometry from terrain model
void CMeshedStaticProp::UpdateCollider ( CTerrainVertex* vertices, CModelTriangle* triangles, uint vertex_count, uint triangle_count )
{
	hkGeometry geometry; 
	geometry.m_triangles.clear();
	geometry.m_vertices.clear();

	geometry.m_vertices.setSize( vertex_count );
	geometry.m_triangles.setSize( triangle_count );

	for ( uint vert = 0; vert < vertex_count; ++vert )
	{
		geometry.m_vertices[vert].setComponent<0>( vertices[vert].x*m_scale );
		geometry.m_vertices[vert].setComponent<1>( vertices[vert].y*m_scale );
		geometry.m_vertices[vert].setComponent<2>( vertices[vert].z*m_scale );
	}
	for ( uint tri = 0; tri < triangle_count; ++tri )
	{
		geometry.m_triangles[tri].m_a = triangles[tri].vert[0];
		geometry.m_triangles[tri].m_b = triangles[tri].vert[1];
		geometry.m_triangles[tri].m_c = triangles[tri].vert[2];
	}

	UpdateColliderCommon( geometry );
}
// UpdateCollider (ModelVertex) : Creates Havok geometry from normal model
void CMeshedStaticProp::UpdateCollider ( CModelVertex* vertices, CModelTriangle* triangles, uint vertex_count, uint triangle_count )
{
	hkGeometry geometry; 
	geometry.m_triangles.clear();
	geometry.m_vertices.clear();

	geometry.m_vertices.setSize( vertex_count );
	geometry.m_triangles.setSize( triangle_count );

	for ( uint vert = 0; vert < vertex_count; ++vert )
	{
		geometry.m_vertices[vert].setComponent<0>( vertices[vert].x*m_scale );
		geometry.m_vertices[vert].setComponent<1>( vertices[vert].y*m_scale );
		geometry.m_vertices[vert].setComponent<2>( vertices[vert].z*m_scale );
	}
	for ( uint tri = 0; tri < triangle_count; ++tri )
	{
		geometry.m_triangles[tri].m_a = triangles[tri].vert[0];
		geometry.m_triangles[tri].m_b = triangles[tri].vert[1];
		geometry.m_triangles[tri].m_c = triangles[tri].vert[2];
	}

	UpdateColliderCommon( geometry );
}
// UpdateColliderCommon () : Creates/Updates the rigidbody
void CMeshedStaticProp::UpdateColliderCommon ( hkGeometry& n_geometryToUse )
{
	if ( n_geometryToUse.m_vertices.getSize() > 0 )
	{
		// Geometry has been created
		hkpDefaultBvCompressedMeshShapeCinfo cInfo( &n_geometryToUse );
		physShape* currentShape = new hkpBvCompressedMeshShape( cInfo );

		if ( m_rigidbody != NULL )
		{
			// Set to movable mode
			m_rigidbody->setMotionType( physMotion::MOTION_KEYFRAMED );
			m_rigidbody->setShape( currentShape );
			// Set to fixed mode
			m_rigidbody->setMotionType( physMotion::MOTION_FIXED );
		}
		else
		{
			physRigidBodyInfo info;
			info.m_shape = currentShape;							// Set the collision shape to the collider's
			info.m_motionType = physMotion::MOTION_KEYFRAMED;		// Set the motion to static
			// Create a rigidbody and assign it to the body variable.
			m_rigidbody = new physRigidBody(&info,false);//Physics::CreateRigidBody( &info, false );
			m_rigidbody->setPosition( Vector3d( 0,0,0 ) );
			// Set body as fixed
			m_rigidbody->setMotionType( physMotion::MOTION_FIXED );
		}
		Physics::FreeShape(currentShape);
	}
	else
	{
		DeleteRigidbody();
	}
}
// DeleteRigidbody () : If exists, deletes the rigidbody
void CMeshedStaticProp::DeleteRigidbody ( void )
{
	if ( m_rigidbody )
	{
		physRigidBody* rigidbodyToDelete = m_rigidbody;
		m_rigidbody = NULL; // Set to null first thing to prevent read access on other threads during deletion
		//Physics::FreeRigidBody( rigidbodyToDelete );
		delete_safe( rigidbodyToDelete );
	}
}

void CMeshedStaticProp::SetOwner ( CGameBehavior* n_newBehavior )
{
	if ( m_rigidbody )
	{
		// Set to movable mode
		m_rigidbody->setMotionType( physMotion::MOTION_KEYFRAMED );
		m_rigidbody->setUserData( hkLong(n_newBehavior->GetId()) );
		// Set to fixed mode
		m_rigidbody->setMotionType( physMotion::MOTION_FIXED );
	}
}

void CMeshedStaticProp::SetPosition ( const Vector3d& n_newPosition )
{
	if ( m_rigidbody )
	{
		// Set to movable mode
		m_rigidbody->setMotionType( physMotion::MOTION_KEYFRAMED );
		m_rigidbody->setPosition( n_newPosition );
		// Set to fixed mode
		m_rigidbody->setMotionType( physMotion::MOTION_FIXED );
	}
}
