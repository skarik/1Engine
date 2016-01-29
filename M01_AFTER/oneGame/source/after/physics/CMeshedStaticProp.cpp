
#include "CMeshedStaticProp.h"

#include "physical/physics.h"
#include "physical/physics/CPhysics.h"
#include "physical/physics/motion/physRigidbody.h"
#include "physical/physics/shapes/physStaticCompressedMesh.h"

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
	if ( vertex_count > 0 )
	{
		physStaticCompressedMesh* currentShape = new physStaticCompressedMesh();
		currentShape->Initialize( vertices, triangles, vertex_count, triangle_count, m_scale );
		UpdateColliderCommon( currentShape );
		delete_safe(currentShape);
	}
	else 
	{
		UpdateColliderCommon( NULL );
	}
}
// UpdateCollider (ModelVertex) : Creates Havok geometry from normal model
void CMeshedStaticProp::UpdateCollider ( CModelVertex* vertices, CModelTriangle* triangles, uint vertex_count, uint triangle_count )
{
	if ( vertex_count > 0 )
	{
		physStaticCompressedMesh* currentShape = new physStaticCompressedMesh();
		currentShape->Initialize( vertices, triangles, vertex_count, triangle_count, m_scale );
		UpdateColliderCommon( currentShape );
		delete_safe(currentShape);
	}
	else 
	{
		UpdateColliderCommon( NULL );
	}
}
// UpdateColliderCommon () : Creates/Updates the rigidbody
void CMeshedStaticProp::UpdateColliderCommon ( physShape* n_shapeToUse )
{
	if ( n_shapeToUse != NULL )
	{
		// Geometry has been created
		//hkpDefaultBvCompressedMeshShapeCinfo cInfo( &n_geometryToUse );
		//physShape* currentShape = new hkpBvCompressedMeshShape( cInfo );
		//physStaticCompressedMesh* currentShape = new physStaticCompressedMesh();
		//currentShape->Initialize(&n_geometryToUse);

		if ( m_rigidbody != NULL )
		{
			// Set to movable mode
			m_rigidbody->setMotionType( physMotion::MOTION_KEYFRAMED );
			m_rigidbody->setShape( n_shapeToUse->getShape() );
			// Set to fixed mode
			m_rigidbody->setMotionType( physMotion::MOTION_FIXED );
		}
		else
		{
			physRigidBodyInfo info;
			info.m_shape = n_shapeToUse;							// Set the collision shape to the collider's
			info.m_motionType = physMotion::MOTION_KEYFRAMED;		// Set the motion to static
			// Create a rigidbody and assign it to the body variable.
			m_rigidbody = new physRigidBody(&info,false);//Physics::CreateRigidBody( &info, false );
			m_rigidbody->setPosition( Vector3d( 0,0,0 ) );
			// Set body as fixed
			m_rigidbody->setMotionType( physMotion::MOTION_FIXED );
		}
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
